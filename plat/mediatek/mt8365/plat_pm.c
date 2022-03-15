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
#include <plat/common/platform.h>

/* mediatek platform specific headers */
#include <platform_def.h>
#include <mtk_plat_common.h>
#include <mt_spm.h>
#include <mt_spm_dpidle.h>
#include <mt_spm_internal.h>
#include <mt_spm_sodi.h>
#include <sleep_def.h>
#include <mtspmc.h>
#include <mtk_mcdi.h>
#include "plat/mediatek/mt8365/include/scu.h"
#include <drivers/arm/gicv3.h>
#include <mt_gic_v3.h>

#include "plat_private.h"

#include <mtk_gic_v3_main.h>

#define USE_MCUPM_HP		1

#if USE_COHERENT_MEM
#define __coherent  __section("tzfw_coherent_mem")
#else
#define __coherent
#endif

#define STA_POWER_DOWN		0
#define STA_POWER_ON		1

/* Local power state for power domains in Run state. */
#define MTK_LOCAL_STATE_RUN	0
/* Local power state for retention. */
#define MTK_LOCAL_STATE_MCDI 2
/* Local power state for OFF/power-down. */
#define MTK_LOCAL_STATE_OFF	3

#define MTK_PWR_LVL0    0
#define MTK_PWR_LVL1    1
#define MTK_PWR_LVL2    2

enum mtk_suspend_mode {
        MTK_MCDI_MODE = 1,
        MTK_SODI_MODE,
        MTK_SODI3_MODE,
        MTK_DPIDLE_MODE,
        MTK_SUSPEND_MODE,
};

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
 * MTK_platform handler called when an affinity instance is about to be
 * suspended. The level and mpidr determine the affinity instance. The 'state'
 * arg. allows the platform to decide whether the cluster is being turned off
 * and take apt actions.
 *
 * CAUTION: This function is called with coherent stacks so that caches can be
 * turned off, flushed and coherency disabled. There is no guarantee that caches
 * will remain turned on across calls to this function as each affinity level is
 * dealt with. So do not write & read global variables across calls. It will be
 * wise to do flush a write to the global to prevent unpredictable results.
 ******************************************************************************/
static void plat_power_domain_suspend(const psci_power_state_t *state)
{
	/* Perform the common cpu specific operations */

	int cpu;
	int cluster;
	int linear_id;
	uint64_t mpidr;
	int mcdi_lvl0;
	int mcdi_lvl1;
	int lvl1_off;
	int lvl2_off;

	const plat_local_state_t *pds = state->pwr_domain_state;

	mpidr = read_mpidr();
	cpu = MPIDR_AFFLVL0_VAL(mpidr);
	cluster = MPIDR_AFFLVL1_VAL(mpidr);
	linear_id = (cluster << 2) + cpu;

	mcdi_lvl0 = pds[MPIDR_AFFLVL0] >= MTK_LOCAL_STATE_MCDI ? 1 : 0;
	mcdi_lvl1 = pds[MPIDR_AFFLVL1] >= MTK_LOCAL_STATE_MCDI ? 1 : 0;
	lvl1_off = pds[MPIDR_AFFLVL1] == MTK_LOCAL_STATE_OFF ? 1 : 0;
	lvl2_off = pds[MPIDR_AFFLVL2] == MTK_LOCAL_STATE_OFF ? 1 : 0;

	spark_disable(cluster, cpu);

        /* Prevent interrupts from spuriously waking up this cpu */
	gic_rdist_save();
       gic_cpuif_deactivate(0);

	if (lvl2_off) {
		spm_suspend();
	} else if (lvl1_off) {
		spm_dpidle();
	} else if (mcdi_lvl1) {
		mcdi_mcupm_standbywfi_irq_enable(linear_id);
		mcdi_mcupm_cluster_auto_off_enable(linear_id);
	} else if (mcdi_lvl0) {
		mcdi_mcupm_standbywfi_irq_enable(linear_id);
		mcdi_mcupm_cluster_auto_off_disable(linear_id);
	}

	/* Perform the common cluster specific operations */
	if (mcdi_lvl1 || lvl1_off || lvl2_off) {
	       /* Disable coherency if this cluster is to be turned off */
		disable_scu(mpidr);
	}

	/* Perform the common mcusys specific operations */
	if (lvl1_off) {
		gic_dist_save();
		gic_sgi_save_all();
	}
}

/*******************************************************************************
 * MTK_platform handler called when an affinity instance has just been powered
 * on after having been suspended earlier. The level and mpidr determine the
 * affinity instance.
 ******************************************************************************/
static void plat_power_domain_suspend_finish(const psci_power_state_t *state)
{
	int cpu;
	int cluster;
	uint64_t mpidr;
	int mcdi_lvl0;
	int mcdi_lvl1;
	int lvl1_off;
	int lvl2_off;

	const plat_local_state_t *pds = state->pwr_domain_state;

	mpidr = read_mpidr();
	cpu = MPIDR_AFFLVL0_VAL(mpidr);
	cluster = MPIDR_AFFLVL1_VAL(mpidr);

	mcdi_lvl0 = pds[MPIDR_AFFLVL0] >= MTK_LOCAL_STATE_MCDI ? 1 : 0;
	mcdi_lvl1 = pds[MPIDR_AFFLVL1] >= MTK_LOCAL_STATE_MCDI ? 1 : 0;
	lvl1_off = pds[MPIDR_AFFLVL1] == MTK_LOCAL_STATE_OFF ? 1 : 0;
	lvl2_off = pds[MPIDR_AFFLVL2] == MTK_LOCAL_STATE_OFF ? 1 : 0;

	/* Perform the common mcusys specific operations */
	if (lvl1_off)
		gic_sgi_restore_all();

	/* Perform the common cluster specific operations */
	if (mcdi_lvl1 || lvl1_off || lvl2_off) {
		if (lvl1_off || lvl2_off) {
			gic_setup();
			gic_dist_restore();

			dfd_resume();
		}

		/* Enable coherency if this cluster was off */
		enable_scu(mpidr);

		/* Enable mcusys emi wfifo */
		mmio_write_32(EMI_WFIFO, 0xf);
	}

	if (lvl2_off) {
		spm_suspend_finish();
	} else if (lvl1_off) {
		spm_dpidle_finish();
	}

	/* Enable the gic cpu interface */
	gic_cpuif_init();

	if (mcdi_lvl0 || mcdi_lvl1 || lvl2_off)
		gic_rdist_restore();
	else
		gic_rdist_restore_all();

	spark_enable(cluster, cpu);
}

static void plat_cpu_standby(plat_local_state_t cpu_state)
{
	u_register_t scr;

	scr = read_scr_el3();
	write_scr_el3(scr | SCR_IRQ_BIT | SCR_FIQ_BIT);

	isb();
	dsb();
	wfi();

	write_scr_el3(scr);
}

static int plat_validate_power_state(unsigned int power_state,
					psci_power_state_t *req_state)
{
	int pwr_lvl = psci_get_pstate_pwrlvl(power_state);
	int state_id = psci_get_pstate_id(power_state);
	int i;

	assert(req_state);

	if (pwr_lvl > PLAT_MAX_PWR_LVL)
		return PSCI_E_INVALID_PARAMS;

	for (i = 0; i < pwr_lvl; i++)
		req_state->pwr_domain_state[i] = MTK_LOCAL_STATE_OFF;

	if (pwr_lvl == MPIDR_AFFLVL2)
		req_state->pwr_domain_state[MTK_PWR_LVL2] = MTK_LOCAL_STATE_OFF;
	else if (state_id == MTK_DPIDLE_MODE &&
		 (pwr_lvl == MPIDR_AFFLVL1 && spm_can_dpidle_enter()))
		req_state->pwr_domain_state[MPIDR_AFFLVL1] = MTK_LOCAL_STATE_OFF;
	else
		req_state->pwr_domain_state[pwr_lvl] = MTK_LOCAL_STATE_MCDI;

	return PSCI_E_SUCCESS;
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
	int mcdi_lvl1;

	const plat_local_state_t *pds = state->pwr_domain_state;

	mcdi_lvl1 = pds[MPIDR_AFFLVL1] >= MTK_LOCAL_STATE_MCDI ? 1 : 0;

	/* Perform the common cluster specific operations */
	if (mcdi_lvl1) {
		enable_scu(mpidr);
	}

#if !USE_MCUPM_HP
	spm_disable_cpu_auto_off(cluster, cpu);
#endif

	spark_enable(cluster, cpu);

	/* Enable the gic cpu interface */
#if 1
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
	int mcdi_lvl1;
	const plat_local_state_t *pds = state->pwr_domain_state;

	mcdi_lvl1 = pds[MPIDR_AFFLVL1] >= MTK_LOCAL_STATE_MCDI ? 1 : 0;

	spark_disable(cluster, cpu);

	//mt_gic_rdistif_save();
	gic_rdist_save();
	gic_cpuif_deactivate(0);

#if !USE_MCUPM_HP
	spm_enable_cpu_auto_off(cluster, cpu);
#endif

#if 1
	if (mcdi_lvl1) {
	}
#endif

#if USE_MCUPM_HP
	mcupm_hp_off(cpu);
#else
	spm_set_cpu_power_off(cluster, cpu);
#endif
}

/*******************************************************************************
 * MTK handlers to shutdown/reboot the system
 ******************************************************************************/
static void __dead2 plat_mtk_system_off(void)
{
	INFO("MTK System Off\n");

	rtc_power_off_sequence();
	pmic_power_off();

	wfi();
	ERROR("MTK System Off: operation not handled.\n");
	panic();
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

static void plat_mtk_get_sys_suspend_power_state(psci_power_state_t *req_state)
{
	assert(PLAT_MAX_PWR_LVL >= 2);

	for (int i = MPIDR_AFFLVL0; i <= PLAT_MAX_PWR_LVL; i++)
		req_state->pwr_domain_state[i] = MTK_LOCAL_STATE_OFF;
}

/*******************************************************************************
 * Export the platform handlers to enable psci to invoke them
 ******************************************************************************/
static const plat_psci_ops_t plat_plat_pm_ops = {
	.cpu_standby			= plat_cpu_standby,
	.pwr_domain_off			= plat_power_domain_off,
	.pwr_domain_on			= plat_power_domain_on,
	.pwr_domain_on_finish		= plat_power_domain_on_finish,
	.pwr_domain_suspend		= plat_power_domain_suspend,
	.pwr_domain_suspend_finish	= plat_power_domain_suspend_finish,
	.system_off			= plat_mtk_system_off,
	.system_reset			= plat_system_reset,
	.validate_power_state		= plat_validate_power_state,
	.get_sys_suspend_power_state	= plat_mtk_get_sys_suspend_power_state,
};

static void ssusb_infra_workaround(void)
{
	uint64_t flags;

	flags = spm_get_pwr_ctrl_args(SPM_PWR_CTRL_SUSPEND, PWR_PCM_FLAGS, 0);
	flags |= SPM_FLAG_DIS_INFRA_PDN;
	spm_pwr_ctrl_args(SPM_PWR_CTRL_SUSPEND, PWR_PCM_FLAGS, flags);
}

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

	ssusb_infra_workaround();

	return 0;
}
