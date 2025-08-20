/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APUSYS_POWER_H
#define APUSYS_POWER_H

#include <platform_def.h>

enum subsys_id {
	SYS_APMCU = 0,
	SYS_APU,
	SYS_SCP_LP,
	SYS_SCP_NP,
	SYS_MAX,
};

enum smc_rcx_pwr_op {
	SMC_RCX_PWR_AFC_EN = 0,
	SMC_RCX_PWR_WAKEUP_RPC,
	SMC_RCX_PWR_CG_EN,
	SMC_RCX_PWR_HW_SEMA,
	SMC_HW_SEMA_PWR_CTL_LOCK,
	SMC_HW_SEMA_PWR_CTL_UNLOCK,
};

enum smc_pwr_dump {
	SMC_PWR_DUMP_RPC = 0,
	SMC_PWR_DUMP_PCU,
	SMC_PWR_DUMP_ARE,
	SMC_PWR_DUMP_ALL,
};

/* VLP AO related reg */
#define VLP_BASE		(VLP_AO_REG_BASE)
#define APUSYS_AO_CTRL		(0x200)

/* SPM related reg */
#define SPM2APU_CON		(0x414)
#define SOC_BUCK_ISO_CON	(0xFAC)
#define SPM_SEMA_M0		(0x69C)
#define SPM_HW_SEMA_MASTER	SPM_SEMA_M0

/* APU related reg */
#define APU_VCORE_BASE		(APU_RCX_VCORE_CONFIG)
#define APU_RCX_BASE		(APU_RCX_CONFIG)
#define APU_RPC_BASE		(APU_RPCTOP)
#define APU_PCU_BASE		(APU_PCUTOP)
#define APU_ARE_BASE		(APU_ARETOP_ARE)

#define APUSYS_VCORE_CG_CLR	(0x0008)
#define APU_RCX_CG_CLR		(0x0008)
#define APU_RPC_TOP_CON		(0x0000)
#define APU_RPC_TOP_SEL_1	(0x0018)

/* APU PCU HW semaphore */
#define APU_PCU_SEMA_CTRL0      (0x0200)
#define APU_HW_SEMA_PWR_CTL     APU_PCU_SEMA_CTRL0

#define D_ACX_LIMIT_OPP_REG     (0x428)
#define ACX0_LIMIT_OPP_REG      (0x42C)

#define APU_ARETOP_VALID_START	(APU_ARETOP_ARE)
#define APU_ARETOP_VALID_END	(APU_ARETOP_ARE + APU_ARETOP_ARE_SZ)
#endif

