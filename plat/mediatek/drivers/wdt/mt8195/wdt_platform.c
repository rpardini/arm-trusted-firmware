/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <lib/mmio.h>
#include <common/debug.h>
#include <wdt.h>

void mtk_wdt_clr_status(void)
{
	mmio_write_32(&mtk_wdt->wdt_mode, MTK_WDT_CLR_STATUS);
}