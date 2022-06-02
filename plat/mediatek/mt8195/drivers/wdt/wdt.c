/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <lib/mmio.h>
#include <common/debug.h>
#include <wdt.h>

void mtk_wdt_set_req(void)
{
	/* To enable thermal hardware reset, we need to
	 * enable thermal control request and set it to reboot mode.
	 * Note that because thermal throttle (by lowering cpu frequency)
	 * is currently enabled, the thermal hardware reset shouldn't be
	 * triggered under normal circumstances.
	 */
	mmio_setbits_32(&mtk_wdt->wdt_req_mode, MTK_WDT_THERMAL_EN);
	mmio_clrsetbits_32(&mtk_wdt->wdt_req_mode, MTK_WDT_REQ_MOD_KEY, MTK_WDT_REQ_MOD_KEY_VAL << 24);

	mmio_clrbits_32(&mtk_wdt->wdt_req_irq_en, MTK_WDT_THERMAL_IRQ);
	mmio_clrsetbits_32(&mtk_wdt->wdt_req_irq_en, MTK_WDT_REQ_IRQ_KEY, MTK_WDT_REQ_IRQ_KEY_VAL << 24);
}

void mtk_wdt_clr_status(void)
{
	mmio_write_32(&mtk_wdt->wdt_mode, MTK_WDT_CLR_STATUS);
}

static inline void mtk_wdt_swreset(void)
{
	NOTICE("%s() called!\n", __func__);

	mmio_setbits_32(&mtk_wdt->wdt_mode, MTK_WDT_MODE_EXTEN | MTK_WDT_MODE_KEY);
	mmio_write_32(&mtk_wdt->wdt_swrst, MTK_WDT_SWRST_KEY);
}

int mtk_wdt_init(void)
{
	uint32_t wdt_sta;

	mtk_wdt_set_req();

	wdt_sta = mmio_read_32(&mtk_wdt->wdt_status);
	/* Writing mode register will clear status register */
	mtk_wdt_clr_status();

	NOTICE("WDT: Status = 0x%x\n", wdt_sta);

	NOTICE("WDT: Last reset was ");
	if (!wdt_sta) {
		printf("cold boot\n");
	} else {
		if (wdt_sta & MTK_WDT_STA_HW_RST)
			printf("hardware watchdog\n");
		else if (wdt_sta & MTK_WDT_STA_SW_RST)
			printf("normal software reboot\n");
		else if (wdt_sta & MTK_WDT_STA_SPM_RST)
			printf("SPM reboot\n");
		else
			printf("other reset type: 0x%x\n", wdt_sta);
	}

	/* Config watchdog reboot mode:
	 * Clearing bits:
	 * DUAL_MODE & IRQ: trigger reset instead of irq then reset.
	 * EXT_POL: select watchdog output signal as active low.
	 * ENABLE: disable watchdog on initialization.
	 * Setting bit EXTEN to enable watchdog output.
	 */
	mmio_clrsetbits_32(&mtk_wdt->wdt_mode,
			MTK_WDT_MODE_DUAL_MODE | MTK_WDT_MODE_IRQ |
			MTK_WDT_MODE_EXT_POL | MTK_WDT_MODE_ENABLE,
			MTK_WDT_MODE_EXTEN | MTK_WDT_MODE_KEY);

	return wdt_sta;
}
