// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#include <stddef.h>
#include <string.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <mcu.h>
#include <lib/mtk_init/mtk_init.h>
#include "mt_sspm.h"
#include "mt_spm.h"
#include "mt_spm_reg.h"

static void reset_sspm(struct mtk_mcu *mcu)
{
	mmio_write_32(SSPM_CFGREG_SYS_SEL, NON_CACHE_DIS | PURE_TCM_EN);
	mmio_write_32(SSPM_SW_RSTN, SW_RSTN);
}

static struct mtk_mcu sspm = {
	.firmware_name = "sspm.bin",
	.load_buffer = sspm_bin,
	.run_address = (void *) SSPM_SRAM_BASE,
	.reset = reset_sspm,
};

void sspm_enable_sram(void)
{
	mmio_write_32(SPM_BASE, SPM_PRJ_CODE);
	mmio_write_32(SSPM_SRAM_CON, SSPM_SRAM_SLEEP_B | SSPM_SRAM_ISOINT_B);
}

int sspm_init(void)
{
	NOTICE("%s\n", __func__);

	/* Set to secure on accessing SSPM TCM */
	mmio_write_32(VLP_A0_APC_CON, 0);
	mmio_write_32(VLP_A0_MAS_SEC_0, M2_SEC);

	sspm_enable_sram();

	mmio_write_32(SSPM_CK_EN, SSPM_CLK_EN_ALL);

	mdelay(200);

	sspm.run_size = sspm_bin_len;
	if (mtk_init_mcu(&sspm))
		ERROR("%s() failed\n", __func__);

	return 0;
}

MTK_ARCH_INIT(sspm_init);
