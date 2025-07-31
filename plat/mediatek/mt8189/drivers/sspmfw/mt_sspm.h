/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#define SPM_PRJ_CODE		0xB160001
#define SSPM_SRAM_SLEEP_B	0x10
#define SSPM_SRAM_ISOINT_B	0x2

#define SSPM_SW_RSTN		(SSPM_CFG_BASE + 0x000)
#define SW_RSTN			BIT(0)

#define SSPM_CFGREG_SYS_SEL	(SSPM_CFG_BASE + 0x160)
#define NON_CACHE_DIS		BIT(1)
#define PURE_TCM_EN		BIT(0)

#define SSPM_CK_EN		(SSPM_CFG_BASE + 0x3000)
#define SSPM_CLK_EN_ALL		0xFFFFFFFF

#define VLP_A0_MAS_SEC_0	(DEVAPC_VLP_AO_BASE + 0xA00)
#define M2_SEC			BIT(2)
#define VLP_A0_APC_CON		(DEVAPC_VLP_AO_BASE + 0xF00)

_Alignas(8) extern const unsigned char sspm_bin[];
extern const unsigned char sspm_bin_end[];
#define sspm_bin_len (sspm_bin_end - sspm_bin)
