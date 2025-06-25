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

#define SSPM_SW_RSTN		(SSPM_CFG_BASE + 0x000)

static void reset_sspm(struct mtk_mcu *mcu)
{
	mmio_write_32(SSPM_SW_RSTN, 0x1);
}

static struct mtk_mcu sspm = {
	.firmware_name = "sspm.bin",
	.load_buffer = sspm_bin,
	.run_address = (void *) SSPM_SRAM_BASE,
	.reset = reset_sspm,
};

int sspm_init(void)
{
	NOTICE("%s\n", __func__);

	sspm.run_size = sspm_bin_len;
	if (mtk_init_mcu(&sspm))
		ERROR("%s() failed\n", __func__);
	return 0;
}

MTK_ARCH_INIT(sspm_init);
