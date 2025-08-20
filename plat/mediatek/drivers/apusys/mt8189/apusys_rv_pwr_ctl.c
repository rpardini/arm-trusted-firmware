/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* TF-A system header */
#include <common/debug.h>
#include <lib/mmio.h>
#include <lib/utils_def.h>
#if defined(CONFIG_MTK_DEVINFO)
#include <drivers/chip_id.h>
#endif

/* Vendor header */
#include <mtk_mmap_pool.h>
#include "apusys_rv_pwr_ctl.h"

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <common/debug.h>
#include <drivers/delay_timer.h>
#include <lib/mtk_init/mtk_init.h>
#include <lib/spinlock.h>
#include <lib/xlat_tables/xlat_tables_v2.h>

#include <drivers/apusys_rv_public.h>
#include <drivers/iommu/mtk_iommu_public.h>
#include <mtk_sip_svc.h>
#include <mtk_mmap_pool.h>


int apu_infra_lock(uint32_t op, enum apu_infra_bit_id id)
{
	uint32_t timeout_cnt = 0;
	uint32_t timeout = 1000000;

	if (op == 1) {
		//hold hw sem for infra
		mmio_write_32(APU_INFRA_HW_SEM, BIT(id));
	} else if (op == 0) {
		//release hw sem for infra
		mmio_write_32(APU_INFRA_HW_SEM, BIT(id + 16));
	}

	if (op == 0)
		goto end;

	while ((mmio_read_32(APU_INFRA_HW_SEM) & BIT(id)) != BIT(id)) {

		if (timeout_cnt++ >= timeout) {
			INFO("%s apu_infra_hwsem :0x%08x\n", __func__,
			mmio_read_32(APU_INFRA_HW_SEM));
			return -EBUSY;
		}

		mmio_write_32(APU_INFRA_HW_SEM, BIT(id));
		udelay(1);
	}
end:
	INFO("%s: op = %d, apu_infra_hwsem :0x%08x\n",
			__func__, op, mmio_read_32(APU_INFRA_HW_SEM));
	return 0;
}

int apu_polling_rpc_pwr_off(void)
{
	int timeout_cnt = 0;
	int timeout = 100 * 1000; // 100ms

	INFO("%s\n", __func__);

	//need to polling RPC power off ready.
	while (mmio_read_32(APU_RPC_STATUS) & 0x1) {
		if (timeout >= 0 && timeout_cnt++ >= timeout) {
			INFO("%s timeout rnd:%d (APU RPC 0x44 = 0x%08x)\n", __func__,
				timeout_cnt, mmio_read_32(APU_RPC_STATUS));
			return -1;
		}
		udelay(1);
	}
	return 0;
}

int apu_infra_check_last_pwr_vote(enum apu_infra_bit_id id)
{
	int pwr_total_cnt = mmio_read_32(APU_INFRA_STATUS);//bit 1~16 for APU use

	pwr_total_cnt = (~pwr_total_cnt) & 0x1fffe;//and bit 1~16
	INFO("%s: id = %d, infra status = 0x%08x\n", __func__, id, pwr_total_cnt);

	if (pwr_total_cnt == BIT(id))
		return 1;
	else
		return 0;
}


/*
 * op:
 *      0 - power off use
 *      1 - power on use
 */
void apu_infra_ctrl(uint32_t op, enum apu_infra_bit_id id)
{
	if (op == 1)
		mmio_write_32(APU_INFRA_DISABLE, BIT(id));//apu power on, bit :1 ~ 16
	else if (op == 0)
		mmio_write_32(APU_INFRA_ENABLE, BIT(id));//apu power off, bit :1 ~ 16

	INFO("%s: apu infra status bit =  0x%08x\n", __func__, mmio_read_32(APU_INFRA_STATUS));
}


/*
 * APU_SEMA_CTRL0
 *
 * usr_bit: subsys_id
 * ctl:
 *      0x1 - acquire hw semaphore
 *      0x0 - release hw semaphore
 */
int apu_hw_sema_ctl_per_mbox(uint32_t sem_ctrl_addr, uint32_t sem_sta_addr
, uint8_t usr_bit, uint8_t ctl, uint32_t timeout, uint8_t bypass)
{
	uint32_t timeout_cnt = 0;
	uint8_t ctl_bit = 0;

	if (ctl == 0x1) {
		// acquire is set
		ctl_bit = 0x1;
	} else if (ctl == 0x0) {
		// release is clear
		ctl_bit = 0x2;
	} else {
		return -EINVAL;
	}


	/* return fail if semaphore currently not held by this user */
	if (ctl == 0 && ((mmio_read_32(sem_sta_addr) & BIT(usr_bit)) == 0) && !bypass) {
		ERROR("%s release error: usr_bit:%d ctl:%d (sem_addr(0x%08x) = 0x%08x)\n",
				__func__, usr_bit, ctl,	sem_sta_addr, mmio_read_32(sem_sta_addr));
		return -EINVAL;
	}

	/* INFO("%s ++ offset = 0x%x usr_bit:%d ctl:%d (sem_addr(0x%08x) = 0x%08x)\n",
	 *		__func__, offset, usr_bit, ctl, sem_addr, mmio_read_32(sem_addr));
	 */

	mmio_write_32(sem_ctrl_addr, ctl_bit);

	/* no need to check register value for semaphore release
	 * need to consider other host may acquire hw sem right after release
	 * -> register check may be fail but actually no error occurred
	 */
	if (ctl == 0)
		goto end;

	while ((mmio_read_32(sem_sta_addr) & BIT(usr_bit))
			!= BIT(usr_bit)) {

		if (timeout_cnt++ >= timeout) {
			/* timeout == 0 denotes trylock, no need to print error log */
			if (timeout > 0)
				ERROR("%s timeout usr_bit:%d ctl:%d rnd:%d(addr(0x%08x)=0x%08x)\n",
					__func__, usr_bit, ctl, timeout_cnt,
					sem_sta_addr, mmio_read_32(sem_sta_addr));
			return -EBUSY;
		}

		mmio_write_32(sem_ctrl_addr, ctl_bit);
		udelay(1);
	}

end:

	return 0;
}

int apusys_rv_pwr_ctrl(uint32_t op)
{
	int ret = 0;
	uint32_t global_ref_cnt = 0;

#if defined(CONFIG_MTK_DEVINFO)
	if (mt_get_chip_sw_ver() == CHIP_VER_E1) {
		ret = apu_infra_lock(1, APU_INFRA_SYS_APMCU);

		if (ret) {
			ERROR("%s(%d): infra sem acquire timeout\n", __func__, op);
			return ret;
		}
	}
#endif

	ret = apu_hw_sema_ctl_per_mbox(APU_MBOX(APU_HW_SEM_SYS_APMCU) + APU_MBOX_SEMA0_CTRL,
		APU_MBOX(APU_HW_SEM_SYS_APMCU) + APU_MBOX_SEMA0_STA, APU_HW_SEM_SYS_APMCU, 1,
		HW_SEM_TIMEOUT, 0);

	if (ret) {
		ERROR("%s(%d): sem acquire timeout\n", __func__, op);
#if defined(CONFIG_MTK_DEVINFO)
		if (mt_get_chip_sw_ver() == CHIP_VER_E1)
			apu_infra_lock(0, APU_INFRA_SYS_APMCU);
#endif
		return ret;
	}

	global_ref_cnt = mmio_read_32(APU_MBOX(APU_HW_SEM_SYS_APMCU) + APU_MBOX_DUMMY);

	if (global_ref_cnt > 2) {
		/* only possible to be 0/1/2 */
		ERROR("%s: global_ref_cnt(%d) > 2\n", __func__, global_ref_cnt);
	} else if (op == 0) {
		global_ref_cnt--;
		mmio_write_32(APU_MBOX(APU_HW_SEM_SYS_APMCU) + APU_MBOX_DUMMY
			, global_ref_cnt);
		/* global_ref_cnt is from 1 to 0, need to power off */
		if (global_ref_cnt == 0) {
			/* set wkup bit to 0(use mbox11 for linux power ctrl) */
			mmio_write_32(APU_MBOX_WKUP_CFG(11), 0);
		}
	} else if (op == 1) {
		global_ref_cnt++;
		mmio_write_32(APU_MBOX(APU_HW_SEM_SYS_APMCU) + APU_MBOX_DUMMY
			, global_ref_cnt);
		/* global_ref_cnt is from 0 to 1, need to power on */
		if (global_ref_cnt == 1) {
#if defined(CONFIG_MTK_DEVINFO)
			if (mt_get_chip_sw_ver() == CHIP_VER_E1)
				apu_infra_ctrl(1, APU_INFRA_SYS_APMCU);
#endif
			/* set wkup bit to 1(use mbox11 for linux power ctrl) */
			mmio_write_32(APU_MBOX_WKUP_CFG(11), 1);
		}
	}

	ret = apu_hw_sema_ctl_per_mbox(APU_MBOX(APU_HW_SEM_SYS_APMCU) + APU_MBOX_SEMA0_CTRL,
		APU_MBOX(APU_HW_SEM_SYS_APMCU) + APU_MBOX_SEMA0_STA,
		APU_HW_SEM_SYS_APMCU, 0, HW_SEM_TIMEOUT, 0);

	if (ret)
		ERROR("%s(%d): sem release timeout\n", __func__, op);

#if defined(CONFIG_MTK_DEVINFO)
	if (mt_get_chip_sw_ver() == CHIP_VER_E1) {
		if (global_ref_cnt == 0 && op == 0) {
			if (apu_infra_check_last_pwr_vote(APU_INFRA_SYS_APMCU) == 1 &&
				apu_polling_rpc_pwr_off() != 0) {
				ERROR("%s: apu polling rpc pwr off timeout\n", __func__);
			} else {
				apu_infra_ctrl(0, APU_INFRA_SYS_APMCU);
			}
		}
		apu_infra_lock(0, APU_INFRA_SYS_APMCU);
	}
#endif

	return ret;
}

/* return value:
 *     0: semaphore acquired successfully
 *     non 0: semaphore acquired fail
 */
int rv_iommu_hw_sem_trylock(void)
{
	int ret = 0;

	ret = apu_hw_sema_ctl_per_mbox(APU_MBOX(APU_HW_SEM_SYS_APMCU)
	+ APU_MBOX_SEMA1_CTRL, APU_MBOX(APU_HW_SEM_SYS_APMCU)
	+ APU_MBOX_SEMA1_STA, APU_HW_SEM_SYS_APMCU, 1, 0, 0);
	INFO("%s: apu_hw_sema_ctl return %d\n", __func__, ret);

	return ret;
}

/* return value:
 *     0: semaphore release successfully
 *     non 0: semaphore release fail
 */
int rv_iommu_hw_sem_unlock(void)
{
	int ret = 0;

	ret = apu_hw_sema_ctl_per_mbox(APU_MBOX(APU_HW_SEM_SYS_APMCU)
		+ APU_MBOX_SEMA1_CTRL, APU_MBOX(APU_HW_SEM_SYS_APMCU)
		+ APU_MBOX_SEMA1_STA, APU_HW_SEM_SYS_APMCU, 0, 0, 0);
	INFO("%s: apu_hw_sema_ctl return %d\n", __func__, ret);

	return ret;
}

int release_scp_hw_sem(void)
{
	int ret = 0;

	/* force release hw sem SCP bit in recovery flow */
	ret = apu_hw_sema_ctl_per_mbox(APU_MBOX(APU_HW_SEM_SYS_SCP)
	+ APU_MBOX_SEMA0_CTRL, APU_MBOX(APU_HW_SEM_SYS_SCP)
	+ APU_MBOX_SEMA0_STA, APU_HW_SEM_SYS_SCP, 0, HW_SEM_TIMEOUT, 1);

	return ret;
}


/*
 * APU_SEMA_CTRL0
 * [15:00]      SEMA_KEY_SET    Each bit corresponds to different user.
 * [31:16]      SEMA_KEY_CLR    Each bit corresponds to different user.
 *
 * usr_bit: subsys_id
 * ctl:
 *      0x1 - acquire hw semaphore
 *      0x0 - release hw semaphore
 */
int apu_hw_sema_ctl(uint32_t sem_addr, uint8_t usr_bit, uint8_t ctl, uint32_t timeout,
uint8_t bypass)
{
	uint32_t timeout_cnt = 0;
	uint8_t ctl_bit = 0;

	if (ctl == 0x1) {
		// acquire is set
		ctl_bit = usr_bit;
	} else if (ctl == 0x0) {
		// release is clear
		ctl_bit = usr_bit + 16;
	} else {
		return -EINVAL;
	}

	/* return fail if semaphore currently not held by this user */
	if (ctl == 0 && ((mmio_read_32(sem_addr) & BIT(ctl_bit)) == 0) && !bypass) {
		ERROR("%s release error: usr_bit:%d ctl:%d (sem_addr(0x%08x) = 0x%08x)\n",
				__func__, usr_bit, ctl,	sem_addr, mmio_read_32(sem_addr));
		return -EINVAL;
	}

	/* INFO("%s ++ offset = 0x%x usr_bit:%d ctl:%d (sem_addr(0x%08x) = 0x%08x)\n",
	 *		__func__, offset, usr_bit, ctl, sem_addr, mmio_read_32(sem_addr));
	 */

	mmio_write_32(sem_addr, BIT(ctl_bit));

	/* no need to check register value for semaphore release
	 * need to consider other host may acquire hw sem right after release
	 * -> register check may be fail but actually no error occurred
	 */
	if (ctl == 0)
		goto end;

	while ((mmio_read_32(sem_addr) & BIT(ctl_bit))
			>> ctl_bit != ctl) {

		if (timeout_cnt++ >= timeout) {
			/* timeout == 0 denotes trylock, no need to print error log */
			if (timeout > 0)
				ERROR("%s timeout usr_bit:%d ctl:%d rnd:%d(addr(0x%08x)=0x%08x)\n",
					__func__, usr_bit, ctl, timeout_cnt,
					sem_addr, mmio_read_32(sem_addr));
			return -EBUSY;
		}

		mmio_write_32(sem_addr, BIT(ctl_bit));
		udelay(1);
	}

end:
	/* INFO("%s sem_addr = 0x%x usr_bit:%d ctl:%d (sem_addr(0x%08x) = 0x%08x)(%u)\n",
	 *		__func__, sem_addr, usr_bit, ctl, sem_addr,
	 *		mmio_read_32(sem_addr), timeout_cnt);
	 */

	return 0;
}


int apu_infra_dcm_setup(void)
{
	INFO("%s: enter\n", __func__);
	mmio_write_32(APU_REG_AO_GLUE_CONFG,
		mmio_read_32(APU_REG_AO_GLUE_CONFG) | BIT(24) | BIT(26));

	return 0;
}
