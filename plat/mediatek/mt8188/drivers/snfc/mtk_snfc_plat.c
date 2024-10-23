// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2022 MediaTek Inc. All rights reserved.
 */

#include <stdio.h>
#include <drivers/delay_timer.h>
#include <stdio.h>
#include <lib/mmio.h>
#include <stdbool.h>
#include <mtk_snfc_plat.h>

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
	gpio_cfg_bit32(SNFC_GPIO_GPOUP1, 0x77700000, 0x22200000);
	gpio_cfg_bit32(SNFC_GPIO_GPOUP2, 0x777, 0x222);

	gpio_cfg_bit32(SNFC_PU_SET_GROUP, 0xFC0, 0xC80);
	gpio_cfg_bit32(SNFC_PD_SET_GROUP, 0xFC0, 0x340);

	gpio_cfg_bit32(SNFC_DRIVING_SET, 0x3F000000, ((2 << 24) | (2 << 27)));

	mtk_nor_probe();

	spi_nor_init(&size, NULL);
}
