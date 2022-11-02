/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <lib/mmio.h>
#include <common/debug.h>
#include <wdt.h>

#define MTK_WDT_CLR_STATUS_VAL 0x22
#define MTK_WDT_CLR_STATUS_BITFIELD_SHIFT 24
#define MTK_WDT_CLR_STATUS_MASK (BIT(31)|BIT(30)|BIT(29)|BIT(28)|BIT(27)|BIT(26)|BIT(25)|BIT(24))

void mtk_wdt_clr_status(void)
{
	mmio_clrsetbits_32(&mtk_wdt->wdt_mode, MTK_WDT_CLR_STATUS_MASK,
		MTK_WDT_CLR_STATUS_VAL << MTK_WDT_CLR_STATUS_BITFIELD_SHIFT);
}
