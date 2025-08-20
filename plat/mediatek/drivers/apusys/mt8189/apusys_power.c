/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* TF-A system header */
#include <common/debug.h>
#include <lib/mmio.h>
#include <lib/utils_def.h>

/* Vendor header */
#include <lpm/mt_lp_api.h>
#include <mtk_mmap_pool.h>
#include "apusys_power.h"

int apusys_kernel_apusys_pwr_rcx(uint32_t op)
{
	INFO("%s executing op:%d\n", __func__, op);

	switch (op) {
	case SMC_RCX_PWR_AFC_EN:
		mmio_write_32(APU_RPC_BASE + APU_RPC_TOP_SEL_1,
				(mmio_read_32(APU_RPC_BASE + APU_RPC_TOP_SEL_1)
								| (0x1 << 16)));
		break;
	case SMC_RCX_PWR_WAKEUP_RPC:
		mmio_write_32(APU_RPC_BASE + APU_RPC_TOP_CON, 0x00000100);
		break;
	case SMC_RCX_PWR_CG_EN:
		mmio_write_32(APU_VCORE_BASE + APUSYS_VCORE_CG_CLR, 0xFFFFFFFF);
		mmio_write_32(APU_RCX_BASE + APU_RCX_CG_CLR, 0xFFFFFFFF);
		break;
	case SMC_HW_SEMA_PWR_CTL_LOCK:
		mmio_write_32(APU_PCU_BASE + APU_HW_SEMA_PWR_CTL,
				(0x1 << SYS_APMCU));
		break;
	case SMC_HW_SEMA_PWR_CTL_UNLOCK:
		mmio_write_32(APU_PCU_BASE + APU_HW_SEMA_PWR_CTL,
				(0x1 << (SYS_APMCU + 16)));
		break;
	default:
		INFO("%s invalid op:%d\n", __func__, op);
	}

	return 0;
}

#ifndef CONFIG_MTK_APUSYS_KERNEL_LOAD_IMAGE
static void apupw_dump_reg_range(uint32_t begin_addr, uint32_t end_offset)
{
	uint32_t addr = 0x0;

	for (addr = begin_addr ; addr <= (begin_addr + end_offset)
			; addr += sizeof(uint32_t)) {
		INFO("%s 0x%08x = 0x%08x\n",
				__func__, addr, mmio_read_32(addr));
	}
}

int apusys_kernel_apusys_pwr_dump(uint32_t op)
{
	switch (op) {
	case SMC_PWR_DUMP_RPC:
		apupw_dump_reg_range(APU_RPC_BASE, 0x50);
		break;
	case SMC_PWR_DUMP_PCU:
		apupw_dump_reg_range(APU_PCU_BASE, 0x30);
		apupw_dump_reg_range(APU_PCU_BASE + 0x80, 0x4);
		apupw_dump_reg_range(APU_PCU_BASE + 0xA0, 0x4);
		apupw_dump_reg_range(APU_PCU_BASE + 0xC0, 0x0);
		apupw_dump_reg_range(APU_PCU_BASE + 0xD0, 0x0);
		break;
	case SMC_PWR_DUMP_ARE:
		apupw_dump_reg_range(APU_ARE_BASE, 0x40);
		break;
	case SMC_PWR_DUMP_ALL:
		INFO("%s SPM2APU_CON = 0x%08x %u\n", __func__,
				mmio_read_32(SPM_BASE + SPM2APU_CON), op);

		INFO("%s SOC_BUCK_ISO_CON = 0x%08x %u\n", __func__,
				mmio_read_32(SPM_BASE + SOC_BUCK_ISO_CON), op);

		INFO("%s APUSYS_AO_CTRL = 0x%08x %u\n", __func__,
				mmio_read_32(VLP_BASE + APUSYS_AO_CTRL), op);

		apupw_dump_reg_range(APU_RPC_BASE, 0x50);
		apupw_dump_reg_range(APU_PCU_BASE, 0x30);
		apupw_dump_reg_range(APU_ARE_BASE, 0x40);
		break;
	default:
		ERROR("%s invalid op code %u\n", __func__, op);
		break;
	}

	return 0;
}
#endif

static uint32_t reg_backup[2];
static void *apu_pm_suspend_event_handler(const void *arg)
{
	struct mt_lp_publish_event const *event =
		(struct mt_lp_publish_event const *) arg;

	if (!event)
		return NULL;

	if (event->id == MT_LPM_PUBEVENTS_SYS_POWER_OFF) {
		reg_backup[0] = mmio_read_32(APU_MBOX0 + D_ACX_LIMIT_OPP_REG);
		reg_backup[1] = mmio_read_32(APU_MBOX0 + ACX0_LIMIT_OPP_REG);
	} else if (event->id == MT_LPM_PUBEVENTS_SYS_POWER_ON) {
		mmio_write_32(APU_MBOX0 + D_ACX_LIMIT_OPP_REG, reg_backup[0]);
		mmio_write_32(APU_MBOX0 + ACX0_LIMIT_OPP_REG, reg_backup[1]);
	}
	return NULL;
}
MT_LP_SUBSCRIBE_SUSPEND(apu_pm_suspend_event_handler);
