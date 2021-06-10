/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* common headers */
#include <stdbool.h>
#include <assert.h>
#include <arch.h>
#include <arch_helpers.h>
#include <lib/mmio.h>
#include <lib/psci/psci.h>
#include <bl31/bl31.h>
#include <errno.h>

/* mediatek platform specific headers */
#include <platform_def.h>
#include <mtk_plat_common.h>
#include <mt_spm.h>
#include <mt_spm_dpidle.h>
#include <mt_spm_sodi.h>
#include <mtspmc.h>
#include <mtk_mcdi.h>
#include <scu.h>
#include <drivers/arm/gicv3.h>
#include <mt_gic_v3.h>

#define USE_MCUPM_HP		1

#if USE_COHERENT_MEM
#define __coherent  __section("tzfw_coherent_mem")
#else
#define __coherent
#endif

#define STA_POWER_DOWN		0
#define STA_POWER_ON		1

uintptr_t mtk_suspend_footprint_addr __coherent;
uintptr_t mtk_suspend_timestamp_addr __coherent;
unsigned long sec_entrypoint;

int mt_cluster_ops(int cputop_mpx, int mode, int state)
{
	ERROR("%s(): Not support.\n", __func__);
	panic();

	return -1;
}

int mt_core_ops(int cpux, int mode, int state)
{
	ERROR("%s(): Not support.\n", __func__);
	panic();

	return -1;
}

/*******************************************************************************
 * MTK_platform handler called when an affinity instance is about to be turned
 * on. The level and mpidr determine the affinity instance.
 ******************************************************************************/
static int plat_power_domain_on(unsigned long mpidr)
{
	int cpu;
	int cluster;

	cpu = MPIDR_AFFLVL0_VAL(mpidr);
	cluster = MPIDR_AFFLVL1_VAL(mpidr);

	if (!spm_get_cluster_powerstate(cluster)) {
		spm_poweron_cluster(cluster);

#ifdef MTK_CM_MGR
		/* init_cpu_stall_counter(cluster); */
#endif /* MTK_CM_MGR */
	}

	/* init cpu reset arch as AARCH64 */
	mcucfg_init_archstate(cluster, cpu, 1);
	mcucfg_set_bootaddr(cluster, cpu, sec_entrypoint);

#if USE_MCUPM_HP
	mcupm_hp_on(cpu);
#else
	spm_poweron_cpu(cluster, cpu);
#endif

	return PSCI_E_SUCCESS;
}

/*******************************************************************************
 * MTK_platform handler called when an affinity instance has just been powered
 * on after being turned off earlier. The level and mpidr determine the affinity
 * instance. The 'state' arg. allows the platform to decide whether the cluster
 * was turned off prior to wakeup and do what's necessary to setup it up
 * correctly.
 ******************************************************************************/
static void plat_power_domain_on_finish(const psci_power_state_t *state)
{
	uint16_t mpidr = read_mpidr();
	int cpu = MPIDR_AFFLVL0_VAL(mpidr);
	int cluster = MPIDR_AFFLVL1_VAL(mpidr);

	/* Perform the common cluster specific operations */
	enable_scu(mpidr);

#if !USE_MCUPM_HP
	spm_disable_cpu_auto_off(cluster, cpu);
#endif

	spark_enable(cluster, cpu);

	/* Enable the gic cpu interface */
#if 0
	gic_cpuif_init();
	gic_rdist_restore();
#else
       gicv3_rdistif_on(cpu);
       gicv3_cpuif_enable(cpu);
       mt_gic_rdistif_init();
#endif
}

static void plat_power_domain_off(const psci_power_state_t *state)
{
	uint16_t mpidr = read_mpidr();
	int cpu = MPIDR_AFFLVL0_VAL(mpidr);
	int cluster = MPIDR_AFFLVL1_VAL(mpidr);

	spark_disable(cluster, cpu);

	//mt_gic_rdistif_save();
	gicv3_cpuif_disable(cpu);
	gicv3_rdistif_off(cpu);

#if !USE_MCUPM_HP
	spm_enable_cpu_auto_off(cluster, cpu);
#endif

#if 0
	if (afflvl != MPIDR_AFFLVL0) {
		/* Disable coherency if this cluster is to be turned off */
		plat_cci_disable();
		disable_scu(mpidr);

#if !USE_MCUPM_HP
		spm_enable_cluster_auto_off(cluster);
#endif
	}
#endif

#if USE_MCUPM_HP
	mcupm_hp_off(cpu);
#else
	spm_set_cpu_power_off(cluster, cpu);
#endif
}

static void __dead2 plat_system_reset(void)
{
	/* Write the System Configuration Control Register */
	INFO("MTK System Reset\n");

	/* disable irq and dual mode */
	mmio_write_32(MTK_WDT_MODE,
		(MTK_WDT_MODE_KEY | (mmio_read_32(MTK_WDT_MODE) &
		~(MTK_WDT_MODE_DUAL_MODE | MTK_WDT_MODE_IRQ))));

	/* enable extern */
	mmio_setbits_32(MTK_WDT_MODE, (MTK_WDT_MODE_KEY | MTK_WDT_MODE_EXTEN));

	/* trigger sw reset */
	mmio_setbits_32(MTK_WDT_SWRST, MTK_WDT_SWRST_KEY);

	wfi();
	ERROR("MTK System Reset: operation not handled.\n");
	panic();
}

/*******************************************************************************
 * Export the platform handlers to enable psci to invoke them
 ******************************************************************************/
static const plat_psci_ops_t plat_plat_pm_ops = {
	.pwr_domain_off			= plat_power_domain_off,
	.pwr_domain_on			= plat_power_domain_on,
	.pwr_domain_on_finish		= plat_power_domain_on_finish,
	.system_reset			= plat_system_reset,
};

/*******************************************************************************
 * Export the platform specific power ops & initialize the mtk_platform power
 * controller
 ******************************************************************************/
int plat_setup_psci_ops(uintptr_t secure_entrypoint,
			const plat_psci_ops_t **plat_ops)
{
	*plat_ops = &plat_plat_pm_ops;
	sec_entrypoint = secure_entrypoint;

	/*
	 * init the warm reset config for boot CPU
	 *  a. reset arch as AARCH64
	 *  b. reset addr as function bl31_warm_entrypoint()
	 */
	mcucfg_init_archstate(0, 0, 1);
	mcucfg_set_bootaddr(0, 0, (uintptr_t) bl31_warm_entrypoint);

#ifdef MTK_CM_MGR
	/* init cpu stall counter */
	init_cpu_stall_counter_all();
#endif /* MTK_CM_MGR */

	return 0;
}
