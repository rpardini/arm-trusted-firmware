/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ___DPM_H__
#define ___DPM_H__

#include <stddef.h>
#include <string.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <mcu.h>

#define	DPM_PM_SRAM_BASE	(IO_PHYS + 0x00900000)
#define	DPM_DM_SRAM_BASE	(IO_PHYS + 0x00920000)
#define	DPM_CFG_BASE		(IO_PHYS + 0x00940000)
#define	DPM_PM_SRAM_BASE2	(IO_PHYS + 0x00A00000)
#define	DPM_DM_SRAM_BASE2	(IO_PHYS + 0x00A20000)
#define	DPM_CFG_BASE2		(IO_PHYS + 0x00A40000)

#define CONFIG_DPM_DM_FIRMWARE "dpm.dm"
#define CONFIG_DPM_PM_FIRMWARE "dpm.pm"

struct dpm_regs {
	uint32_t sw_rstn;
	uint32_t rsvd_0[3072];
	uint32_t mclk_div;
	uint32_t rsvd_1[3071];
	uint32_t twam_window_len;
	uint32_t twam_mon_type;
	uint32_t rsvd_2[1022];
	uint32_t low_power_cfg_0;
	uint32_t low_power_cfg_1;
	uint32_t rsvd_3[1];
	uint32_t fsm_out_ctrl_0;
	uint32_t rsvd_4[8];
	uint32_t fsm_cfg_1;
	uint32_t low_power_cfg_3;
	uint32_t dfd_dbug_0;
	uint32_t rsvd_5[28];
	uint32_t status_4;
};

#define DPM_SW_RSTN_RESET	BIT(0)
#define DPM_MEM_RATIO_OFFSET	28
#define DPM_MEM_RATIO_MASK	(0x3 << DPM_MEM_RATIO_OFFSET)
#define DPM_MEM_RATIO_CFG1	(1 << DPM_MEM_RATIO_OFFSET)
#define DRAMC_MCU_SRAM_ISOINT_B_LSB		BIT(1)
#define DRAMC_MCU2_SRAM_ISOINT_B_LSB		BIT(1)
#define DRAMC_MCU_SRAM_SLEEP_B_LSB		BIT(4)
#define DRAMC_MCU2_SRAM_SLEEP_B_LSB		BIT(4)

static struct dpm_regs *const mtk_dpm = (void *)DPM_CFG_BASE;

void dpm_reset(struct mtk_mcu *mcu);
int dpm_init(void);
int dpm_4ch_para_setting(void);
int dpm_4ch_init(void);

#endif
