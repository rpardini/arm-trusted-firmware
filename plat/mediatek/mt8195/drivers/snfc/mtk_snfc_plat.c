// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2022 MediaTek Inc. All rights reserved.
 */

#include <stdio.h>
#include <drivers/delay_timer.h>
#include <stdio.h>
#include <lib/mmio.h>
#include <stdbool.h>
#include <platform_def.h>

static void gpio_cfg_bit32(unsigned long addr, unsigned int field, unsigned int val)
{
	uint32_t tv = mmio_read_32(addr);

	tv &= ~(field);
	tv |= val;
	mmio_write_32(addr, tv);
}

void  mtk_snfc_plat(void)
{
	unsigned long long size;
	/* snfc gpio setting */
	gpio_cfg_bit32(SNFC_GPIO_GPOUP1, 0x7700, 0x2200);
	gpio_cfg_bit32(SNFC_GPIO_GPOUP2, 0x77770000, 0x22220000);

	gpio_cfg_bit32(SNFC_PD0_SET_GROUP, 0x1E0000, 0x1A0000);
	gpio_cfg_bit32(SNFC_PU0_SET_GROUP, 0x40000, 0x40000);
	gpio_cfg_bit32(SNFC_PD1_SET_GROUP, 0x1400, 0x0000);
	gpio_cfg_bit32(SNFC_PU1_SET_GROUP, 0x1400, 0x1400);

	gpio_cfg_bit32(SNFC_DRIVING0_SET, 0x3FFC0000, 0x36D80000);
	gpio_cfg_bit32(SNFC_DRIVING1_SET, 0x7000, 0x6000);
	gpio_cfg_bit32(SNFC_DRIVING2_SET, 0x7, 0x06);

	mtk_nor_probe();

	spi_nor_init(&size, NULL);
}
