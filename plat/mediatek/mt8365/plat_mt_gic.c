/*
 * Copyright (c) 2016-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <platform_def.h>

#include <arch/aarch64/arch_helpers.h>
#include <common/interrupt_props.h>
#include <drivers/arm/gicv3.h>
#include <lib/mmio.h>
#include <plat/common/platform.h>
#include <mt_gic_v3.h>

void gicr_write_ipriorityr(uintptr_t base, unsigned int id, unsigned int val);

uintptr_t rdistif_base_addrs[PLATFORM_CORE_COUNT];

static unsigned int mt_mpidr_to_core_pos(u_register_t mpidr)
{
	return plat_core_pos_by_mpidr(mpidr);
}

static gicv3_driver_data_t mt_gic_data = {
	.gicd_base = MT_GIC_BASE,
	.gicr_base = MT_GIC_RDIST_BASE,
	.rdistif_num = PLATFORM_CORE_COUNT,
	.rdistif_base_addrs = rdistif_base_addrs,
	.mpidr_to_core_pos = mt_mpidr_to_core_pos,
};

static void clear_sec_pol_ctl_en(void)
{
	unsigned int i;

	for (i = 0; i <= NR_INT_POL_CTL - 1; i++) {
		mmio_write_32((SEC_POL_CTL_EN0 + (i * 4)), 0);
	}

	dsb();
}

void plat_mt_gic_driver_init(void)
{
	gicv3_driver_init(&mt_gic_data);
}


void mt_gic_rdistif_init(void)
{
	unsigned int proc_num;
	unsigned int index;
	uintptr_t gicr_base;

	proc_num = plat_my_core_pos();
	gicr_base = rdistif_base_addrs[proc_num];

	/* set all SGI/PPI as non-secure GROUP1 by default */
	mmio_write_32(gicr_base + GICR_IGROUPR0, ~0U);
	mmio_write_32(gicr_base + GICR_IGRPMODR0, 0x0);

	/* setup the default PPI/SGI priorities */
	for (index = 0; index < TOTAL_PCPU_INTR_NUM; index += 4U)
		gicr_write_ipriorityr(gicr_base, index,
				GICD_IPRIORITYR_DEF_VAL);
}

void plat_mt_gic_init(void)
{
	gicv3_distif_init();
	gicv3_cpuif_enable(plat_my_core_pos());
	mt_gic_rdistif_init();

	clear_sec_pol_ctl_en();
}
