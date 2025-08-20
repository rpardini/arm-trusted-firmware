/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APUSYS_RV_PWR_CTRL_H
#define APUSYS_RV_PWR_CTRL_H

#include <platform_def.h>

enum apu_hw_sem_sys_id {
	APU_HW_SEM_SYS_APU = 0UL, /* mbox0 */
	APU_HW_SEM_SYS_GZ = 1UL, /* mbox1 */
	APU_HW_SEM_SYS_SCP = 3UL, /* mbox3 */
	APU_HW_SEM_SYS_APMCU = 11UL, /* mbox11 */
};


int apusys_rv_pwr_ctrl(uint32_t op);
int rv_iommu_hw_sem_unlock(void);
int rv_iommu_hw_sem_trylock(void);
int release_scp_hw_sem(void);
int apu_hw_sema_ctl(uint32_t sem_addr, uint8_t usr_bit, uint8_t ctl,
	uint32_t timeout,	uint8_t bypass);

#define HW_SEM_TIMEOUT (300) /* 300 us */

/* APU MBOX */
#define APU_MBOX0_SZ 0x100000
#define MBOX_WKUP_CFG (0x80)
#define MBOX_WKUP_MASK (0x84)
#define MBOX_FUNC_CFG (0xb0)
#define MBOX_DOMAIN_CFG (0xe0)

#define MBOX_CTRL_LOCK (1UL << 0)
#define MBOX_NO_MPU_SHIFT (16)
#define MBOX_RC_SHIFT (24)

#define MBOX_RX_NS_SHIFT (16)
#define MBOX_RX_DOMAIN_SHIFT (17)
#define MBOX_TX_NS_SHIFT (24)
#define MBOX_TX_DOMAIN_SHIFT (25)

#define APU_REG_AO_GLUE_CONFG (APU_AO_CTRL + 0x20)

#define ENABLE_INFRA_WA

enum apu_infra_bit_id {
	APU_INFRA_SYS_APMCU = 1UL,
	APU_INFRA_SYS_GZ = 2UL,
	APU_INFRA_SYS_SCP = 3UL,
};


#define APU_MBOX(i)		\
		(APU_MBOX0 + 0x10000 * i)

#define APU_MBOX_FUNC_CFG(i) \
		(APU_MBOX(i) + MBOX_FUNC_CFG)
#define APU_MBOX_DOMAIN_CFG(i) \
		(APU_MBOX(i) + MBOX_DOMAIN_CFG)
#define APU_MBOX_WKUP_CFG(i) \
		(APU_MBOX(i) + MBOX_WKUP_CFG)


/* bypass mbox register Dump for secure master */
#define APU_MBOX_DBG_EN (0x190f2380)

/* apu_mbox register definition for mbox addr change*/
#define APU_MBOX_SEMA0_CTRL (0x090)
#define APU_MBOX_SEMA0_RST  (0x094)
#define APU_MBOX_SEMA0_STA  (0x098)
#define APU_MBOX_SEMA1_CTRL (0x0A0)
#define APU_MBOX_SEMA1_RST  (0x0A4)
#define APU_MBOX_SEMA1_STA  (0x0A8)
#define APU_MBOX_DUMMY      (0x040)
#define APU_MBOX_OFFSET(i)	(0x10000 * i)

/* apu infra workaround */
#define APU_INFRA_BASE    (0x1002c000)
#define APU_INFRA_SZ          (0x1000)
#define APU_INFRA_DISABLE (0x1002cc18)
#define APU_INFRA_ENABLE  (0x1002cc14)
#define APU_INFRA_STATUS  (0x1002cc10)
#define APU_INFRA_HW_SEM  (0x190b0e00)
#define APU_RPC_STATUS    (0x190f0044)

#endif

