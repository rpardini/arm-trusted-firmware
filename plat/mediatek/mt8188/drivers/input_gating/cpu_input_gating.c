/*
 * Copyright (c) 2021, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <lib/mtk_init/mtk_init.h>
#include <platform_def.h>
#include <lib/mmio.h>


#define CPU6_IG_EN_REG	(MCUCFG_BASE + 0x3308)
#define CPU7_IG_EN_REG	(MCUCFG_BASE + 0x3b08)
#define DISABLE_CPU_IG	0xfc000001

int disable_cpu_input_gating(void)
{
	mmio_write_32(CPU6_IG_EN_REG, DISABLE_CPU_IG);
	mmio_write_32(CPU7_IG_EN_REG, DISABLE_CPU_IG);
	return 0;
}

MTK_ARCH_INIT(disable_cpu_input_gating);
