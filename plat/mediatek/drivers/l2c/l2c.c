/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <arch_helpers.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <lib/utils_def.h>
#include <mcucfg.h>
#include <lib/mtk_init/mtk_init.h>

#define MP0_CLUSTER_CFG0_L3_SHARE_PRE_EN BIT(8)
#define MP0_CLUSTER_CFG0_L3_SHARE_EN BIT(9)

int mtk_soc_disable_l2c_sram(void)
{
	unsigned long v;

	mmio_clrbits_32(MP0_CLUSTER_CFG0, MP0_CLUSTER_CFG0_L3_SHARE_EN);

	dsb();
	__asm__ volatile ("mrs %0, S3_0_C15_C3_5" : "=r" (v));
	v |= (0xf << 4);
	__asm__ volatile ("msr S3_0_C15_C3_5, %0" : : "r" (v));
	dsb();
	do {
		__asm__ volatile ("mrs %0, S3_0_C15_C3_7" : "=r" (v));
	} while (((v >> 0x4) & 0xf) != 0xf);

	mmio_clrbits_32(MP0_CLUSTER_CFG0, MP0_CLUSTER_CFG0_L3_SHARE_PRE_EN);

	dsb();

	return 0;
}

MTK_ARCH_INIT(mtk_soc_disable_l2c_sram);
