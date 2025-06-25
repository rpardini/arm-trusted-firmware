/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#ifndef __MT_DPM_H__
#define __MT_DPM_H__

#include <stddef.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <mcu.h>

#define DPM_RST_OFFSET		0x7074
#define DPM_SW_RSTN		BIT(0)

#define	DPM_PM_SRAM_BASE	(IO_PHYS + 0x00900000)
#define	DPM_DM_SRAM_BASE	(IO_PHYS + 0x00920000)
#define	DPM_CFG_BASE		(IO_PHYS + 0x00940000)
#define	DPM_PM_SRAM_BASE2	(IO_PHYS + 0x00A00000)
#define	DPM_DM_SRAM_BASE2	(IO_PHYS + 0x00A20000)
#define	DPM_CFG_BASE2		(IO_PHYS + 0x00A40000)

#define DPM_CFG_CH0		DPM_CFG_BASE
#define DPM_CFG_CH1		DPM_CFG_BASE2

#define DPM_BARGS_CH0_REG0	(DPM_CFG_BASE + 0x6004)
#define DPM_BARGS_CH0_REG1	(DPM_CFG_BASE + 0x6008)
#define DRAMC_WBR		(INFRACFG_AO_BASE + 0x0b4)

#define ENABLE_DRAMC_WBR_MASK	0x2ffff

_Alignas(8) extern const unsigned char dpm_dm_bin[];
extern const unsigned char dpm_dm_bin_end[];
#define dpm_dm_bin_len (dpm_dm_bin_end - dpm_dm_bin)

_Alignas(8) extern const unsigned char dpm_pm_bin[];
extern const unsigned char dpm_pm_bin_end[];
#define dpm_pm_bin_len (dpm_pm_bin_end - dpm_pm_bin)

void dpm_reset(struct mtk_mcu *mcu);
void dpm_reset_ch1(struct mtk_mcu *mcu);

int dpm_init(void);

#endif
