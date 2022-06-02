/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SOC_MEDIATEK_MT8195_WDT_H
#define SOC_MEDIATEK_MT8195_WDT_H

#include <stdint.h>
#include <mt8195.h>

#define MTK_WDT_CLR_STATUS 0x230001FF

#define MTK_WDT_REQ_MOD_KEY_VAL 0x33
#define MTK_WDT_REQ_IRQ_KEY_VAL 0x44

#define MTK_WDT_REQ_MOD_KEY (BIT(31)|BIT(30)|BIT(29)|BIT(28)|BIT(27)|BIT(26)|BIT(25)|BIT(24))
#define MTK_WDT_REQ_IRQ_KEY (BIT(31)|BIT(30)|BIT(29)|BIT(28)|BIT(27)|BIT(26)|BIT(25)|BIT(24))
#define MTK_WDT_THERMAL_EN BIT(18)
#define MTK_WDT_THERMAL_IRQ BIT(18)

struct mtk_wdt_regs {
	uint32_t wdt_mode;
	uint32_t wdt_length;
	uint32_t wdt_restart;
	uint32_t wdt_status;
	uint32_t wdt_interval;
	uint32_t wdt_swrst;
	uint32_t wdt_swsysrst;
	uint32_t reserved0[5];
	uint32_t wdt_req_mode;
	uint32_t wdt_req_irq_en;
	uint32_t reserved1[2];
	uint32_t wdt_debug_ctrl;
};

/* WDT_MODE */
enum {
	MTK_WDT_MODE_KEY        = 0x22000000,
	MTK_WDT_MODE_DUAL_MODE  = 1 << 6,
	MTK_WDT_MODE_IRQ        = 1 << 3,
	MTK_WDT_MODE_EXTEN      = 1 << 2,
	MTK_WDT_MODE_EXT_POL    = 1 << 1,
	MTK_WDT_MODE_ENABLE     = 1 << 0
};

/* WDT_RESET */
enum {
	MTK_WDT_SWRST_KEY       = 0x1209,
	MTK_WDT_STA_SPM_RST     = 1 << 1,
	MTK_WDT_STA_SW_RST      = 1 << 30,
	MTK_WDT_STA_HW_RST      = 1 << 31
};

static struct mtk_wdt_regs *const mtk_wdt = (void *)RGU_BASE;
int mtk_wdt_init(void);
void mtk_wdt_clr_status(void);

#endif /* SOC_MEDIATEK_MT8195_WDT_H */
