/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#ifndef __MMINFRA__PRIV_H__
#define __MMINFRA__PRIV_H__

#include <platform_def.h>

#define MMINFRA_BASE			MMINFRA_CONFIG_BASE
#define MMINFRA_DISP_AID_REMAP		(MMINFRA_BASE + 0x860)
#define MMINFRA_GCE_AID_REMAP		(MMINFRA_BASE + 0x8a0)
#define MMINFRA_MDP_AID_REMAP		(MMINFRA_BASE + 0x8e0)
#define MMINFRA_VENC_AID_REMAP		(MMINFRA_BASE + 0x8f0)
#define MMINFRA_VDEC_AID_REMAP		(MMINFRA_BASE + 0x8f8)
#define MMINFRA_AID_REMAP_DOM2AID_EN	(MMINFRA_BASE + 0x900)

#define GCED_BASE			(IO_PHYS + 0x0E980000)
#define GCED_MMU_EN			(GCED_BASE + 0x54)
#define GCEM_BASE			(IO_PHYS + 0x0E990000)
#define GCEM_MMU_EN			(GCEM_BASE + 0x54)

#define GCE_MMU_EN_VAL			BIT(17)

#define GCED_GCTL_VALUE			(GCED_BASE + 0x48)

#define MM_HRE_SRAM_CON			(SPM_BASE + 0xEF0)
#define MM_HRE_SRAM_CON_DEF_VAL		0x7007E
#define MM_HRE_SRAM_CON_PWR_ON_MASK	(0x7 << 16)

#define GCE_D_CTRL_ARMMU_EN		BIT(17)
#define GCE_D_CTRL_AWMMU_EN		BIT(19)
#define GCE_M_CTRL_ARMMU_EN		BIT(21)
#define GCE_M_CTRL_AWMMU_EN		BIT(23)
#define DOM2AID_INFRA_EN		BIT(27)

#define DISP_AID_MASK			0xf
#define GCE_AID_MASK			0x7
#define MDP_AID_MASK			0x7
#define VENC_AID_MASK			0xf
#define VDEC_AID_MASK			0xf

#define DISP_AID_WIDTH			4
#define GCE_AID_WIDTH			3
#define MDP_AID_WIDTH			3
#define VENC_AID_WIDTH			4
#define VDEC_AID_WIDTH			4

#define SMI_L1LEN		        0x100
#define SMI_BUS_SEL		        0x220
#define SMI_M4U_TH		        0x234
#define SMI_FIFO_TH1		        0x238
#define SMI_FIFO_TH2		        0x23c
#define SMI_PREULTRA_MASK1	        0x244
#define SMI_DCM			        0x300
#define SMI_DUMMY		        0x444

#define CLK_MMINFRA_SMI_MASK            BIT(2)

#endif /* __MMINFRA__PRIV_H__ */
