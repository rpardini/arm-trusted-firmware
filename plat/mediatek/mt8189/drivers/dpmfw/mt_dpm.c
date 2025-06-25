// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#include <lib/mtk_init/mtk_init.h>
#include "mt_dpm.h"

#define CONFIG_DPM_DM_FIRMWARE "dpm.dm"
#define CONFIG_DPM_PM_FIRMWARE "dpm.pm"

static struct mtk_mcu dpm_mcu[] = {
	{
		.firmware_name = CONFIG_DPM_DM_FIRMWARE,
		.load_buffer = dpm_dm_bin,
		.run_address = (void *)DPM_DM_SRAM_BASE,
	},
	{
		.firmware_name = CONFIG_DPM_PM_FIRMWARE,
		.load_buffer = dpm_pm_bin,
		.run_address = (void *)DPM_PM_SRAM_BASE,
		.reset = dpm_reset,
	},
	{
		.firmware_name = CONFIG_DPM_DM_FIRMWARE,
		.load_buffer = dpm_dm_bin,
		.run_address = (void *)DPM_DM_SRAM_BASE2,
	},
	{
		.firmware_name = CONFIG_DPM_PM_FIRMWARE,
		.load_buffer = dpm_pm_bin,
		.run_address = (void *)DPM_PM_SRAM_BASE2,
		.reset = dpm_reset_ch1,
	},
};

/*ch0*/
void dpm_reset(struct mtk_mcu *mcu)
{
	/* free RST */
	mmio_setbits_32(DPM_CFG_CH0 + DPM_RST_OFFSET, DPM_SW_RSTN);
}

/*ch1*/
void dpm_reset_ch1(struct mtk_mcu *mcu)
{
	/* free RST */
	mmio_setbits_32(DPM_CFG_CH1 + DPM_RST_OFFSET, DPM_SW_RSTN);
}

int dpm_init(void)
{
	int i;
	struct mtk_mcu *dpm;

	NOTICE("%s\n", __func__);

	dpm_mcu[0].run_size = dpm_dm_bin_len;
	dpm_mcu[1].run_size = dpm_pm_bin_len;
	dpm_mcu[2].run_size = dpm_dm_bin_len;
	dpm_mcu[3].run_size = dpm_pm_bin_len;
	for (i = 0; i < ARRAY_SIZE(dpm_mcu); i++) {
		dpm = &dpm_mcu[i];
		if (mtk_init_mcu(dpm))
			return -1;
	}

	return 0;
}

MTK_ARCH_INIT(dpm_init);

