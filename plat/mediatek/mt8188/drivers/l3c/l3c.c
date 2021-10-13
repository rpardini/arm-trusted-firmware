/*
 * Copyright (c) 2021, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <arch_helpers.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <lib/utils_def.h>
#include <mcucfg.h>

#define L3_SHARE_EN		9
#define L3_SHARE_PRE_EN		8

static int config_l3_size(void)
{
	/*
	 * L3 Cache Share Disable Flow:
	 * 1) Configured the “L3_SHARE_EN” to 0 for disabling the L3 Share function.
	 * 2) Executed DSB instruction.
	 * 3) Configured the “CLUSTERPWRCTLR[7:4]” register to “1111”.
	 * 4) Polling the “CLUSTERPWRSTAT[7:4]” register is equal to “1111”.
	 * 5) Configured the “L3_SHARE_PRE_EN” to 0 for releasing the SRAM power fixed.
	 * 5) Executed DSB instruction.
	 *
	 */

	unsigned long v = 0;
	/* clean l3_share_en */
	mmio_write_64(MP0_CLUSTER_CFG0, mmio_read_64(MP0_CLUSTER_CFG0) & ~(1 << L3_SHARE_EN));
	dsb();

	__asm__ volatile ("mrs %0, S3_0_C15_C3_5" : "=r" (v));
	v |= (0xf << 4);
	__asm__ volatile ("msr S3_0_C15_C3_5, %0" : : "r" (v));
	dsb();

PWR_LOOP:
	__asm__ volatile ("mrs %0, S3_0_C15_C3_7" : "=r" (v));
	if (((v >> 0x4) & 0xf) != 0xf)
		goto PWR_LOOP;

	mmio_write_64(MP0_CLUSTER_CFG0, mmio_read_64(MP0_CLUSTER_CFG0) & ~(1 << L3_SHARE_PRE_EN));
	dsb();

	return 0;
}

