/*
 * Copyright (c) 2021, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APUSYS_RV_H
#define APUSYS_RV_H

#include <lib/mmio.h>

#define ENABLE_APUSYS_EMI_PROTECTION 	(1)
#define APUSYS_NS_FW_EMI_REGION		(21)
#define APUSYS_SEC_FW_EMI_REGION	(23)

/******************************************************************************
 * REGISTER ADDRESS DEFINITION
 ******************************************************************************/
/* APU_MD32_SYSCTRL */
#define APU_MD32_SYSCTRL_SZ 0x1000
#define MD32_SYS_CTRL (APU_MD32_SYSCTRL + 0x0)
#define UP_INT_EN2 (APU_MD32_SYSCTRL + 0xc)
#define MD32_TBUF_DBG_SEL (APU_MD32_SYSCTRL + 0x84)
#define MD32_TBUF_DBG_DAT3 (APU_MD32_SYSCTRL + 0x94)
#define MD32_DBG_BUS_SEL (APU_MD32_SYSCTRL + 0x98)
#define APU_UP_SYS_DBG_EN (1UL << 16)
#define MD32_CLK_EN (APU_MD32_SYSCTRL + 0xb8)
#define UP_WAKE_HOST_MASK0 (APU_MD32_SYSCTRL + 0xbc)
#define UP_WAKE_HOST_MASK1 (APU_MD32_SYSCTRL + 0xc0)
#define MD32_MON_PC (APU_MD32_SYSCTRL + 0x838)
#define MD32_MON_LR (APU_MD32_SYSCTRL + 0x83c)
#define MD32_MON_SP (APU_MD32_SYSCTRL + 0x840)
#define MD32_STATUS (APU_MD32_SYSCTRL + 0x844)
#define MD32_TBUF_ADDR_SHIFT (16)
#define MD32_TBUF_ADDR_MASK (0x7)

#define WDT_IRQ_EN (1UL << 0)
#define MBOX0_IRQ_EN (1UL << 21)
#define MBOX1_IRQ_EN (1UL << 22)
#define MBOX2_IRQ_EN (1UL << 23)
#define DBG_APB_EN (1UL << 31)

#define MD32_G2B_CG_EN (1UL << 11)
#define MD32_DBG_EN (1UL << 10)
#define MD32_DM_AWUSER_IOMMU_EN (2U << 8)
#define MD32_DM_ARUSER_IOMMU_EN (2U << 6)
#define MD32_PM_AWUSER_IOMMU_EN (2U << 4)
#define MD32_PM_ARUSER_IOMMU_EN (2U << 2)
#define WAKE_HOST (1UL << 1)
#define MD32_SOFT_RSTN (1UL << 0)

/* APU_MD32_WDT */
#define APU_MD32_WDT_SZ 0x1000
#define WDT_INT (APU_MD32_WDT + 0x0)
#define WDT_CTRL0 (APU_MD32_WDT + 0x4)

#define WDT_EN (1UL << 31)

/* APU_SCTRL_REVISER */
#define APU_SCTRL_REVISER_SZ 0x2000
#define UP_NORMAL_DOMAIN_NS (APU_SCTRL_REVISER + 0x0)
#define UP_PRI_DOMAIN_NS (APU_SCTRL_REVISER + 0x4)
#define UP_NORMAL_DOMAIN (7)
#define UP_PRI_DOMAIN (5)
#define UP_DOMAIN_SHIFT (0)
#define UP_NS_SHIFT (4)
#define UP_IOMMU_CTRL (APU_SCTRL_REVISER + 0x8)
#define MMU_EN_SHIFT (0)
#define MMU_CTRL (1UL << 1)
#define MMU_CTRL_LOCK (1UL << 2)
#define UP_CORE0_VABASE0 (APU_SCTRL_REVISER + 0xc)
#define UP_CORE0_MVABASE0 (APU_SCTRL_REVISER + 0x10)
#define UP_CORE0_VABASE1 (APU_SCTRL_REVISER + 0x14)
#define UP_CORE0_MVABASE1 (APU_SCTRL_REVISER + 0x18)
#define VLD (1UL << 0)
#define PARTIAL_ENABLE (1UL << 1)
#define THREAD_NUM_SHIFT (2)
#define VABASE1_ADR_SHIFT (10)
#define VASIZE_1MB (1UL)
#define MVABASE_SHIFT (10)
#define USERFW_CTXT (APU_SCTRL_REVISER + 0x1000)
#define SECUREFW_CTXT (APU_SCTRL_REVISER + 0x1004)
#define CFG_4GB_SEL_EN (1UL << 2)

/* APU_AO_CTRL */
#define APU_AO_CTRL_SZ 0x1000
#define MD32_PRE_DEFINE (APU_AO_CTRL + 0x0)
#define MD32_BOOT_CTRL (APU_AO_CTRL + 0x4)
#define MD32_RUNSTALL (APU_AO_CTRL + 0x8)
#define APU_AO_SECURE_FW (APU_AO_CTRL + 0x10)

#define PREDEFINE_NON_CACHE (0U)
#define PREDEFINE_TCM (1UL)
#define PREDEFINE_CACHE (2U)
#define PREDEFINE_CACHE_TCM (3U)

#define PREDEF_1G_OFS (0)
#define PREDEF_2G_OFS (2)
#define PREDEF_3G_OFS (4)
#define PREDEF_4G_OFS (6)

/* APU MBOX */
#define APU_MBOX0_SZ 0x1000
#define APU_MBOX1_SZ 0x1000
#define MBOX_FUNC_CFG (0xb0)
#define MBOX_DOMAIN_CFG (0xe0)

#define MBOX_CTRL_LOCK (1UL << 0)
#define MBOX_NO_MPU_SHIFT (16)
#define MBOX_RC_SHIFT (24)

#define MBOX_RX_NS_SHIFT (16)
#define MBOX_RX_DOMAIN_SHIFT (17)
#define MBOX_TX_NS_SHIFT (24)
#define MBOX_TX_DOMAIN_SHIFT (25)

#define APU_MBOX(i) \
		((i < 8) ? (APU_MBOX0 + 0x100 * i) : (APU_MBOX1 + 0x100 * (i - 8)))

#define APU_MBOX_FUNC_CFG(i) \
		(APU_MBOX(i) + MBOX_FUNC_CFG)
#define APU_MBOX_DOMAIN_CFG(i) \
		(APU_MBOX(i) + MBOX_DOMAIN_CFG)

/* APU_RPCTOP */
#define APU_RPCTOP_SZ 0x1000

/* APU_RCX_CONFIG */
#define APU_RCX_CONFIG_SZ 0x1000

/* APU_RCX_VCORE_CONFIG */
#define APU_RCX_VCORE_CONFIG_SZ 0x1000

#define PWR_RDY			(APU_RPCTOP + 0x44)
#define VCORE_CLK_CRTL		(APU_RCX_VCORE_CONFIG)
#define RCX_CLK_CRTL		(APU_RCX_CONFIG)

#define bits_get(x, m, o)	((x & (m << o)) >> o)
#define PWR_STATUS(x)		bits_get(x, 1, 0)
#define ROUNDUP(a, b)		(((a) + ((b)-1)) & ~((b)-1))

#define APUSYS_COREDUMP_SHADOW_MEM_SIZE	0x200000	/* 2M */
#define APUSYS_MEM_ALIGN		0x10000		/* 64K (minimal size for EMI MPU) */
#define APUSYS_MEM_LIMIT 		0x90000000	/* max address can APUSYS remap */
#define APUSYS_FW_ALIGN 		16 		/* for mdla dma alignment limitation */
#define APUSYS_IOVA_PAGE 		0x4000		/* secure iova page table size*/

/* image name definition */
#define IMG_NAME_APUSYS_A	"tinysys-apusys-RV33_A"

/******************************************************************************
 * 1. New entries must be appended to the end of the structure.
 * 2. Do NOT use conditional option such as #ifdef inside the structure.
 */
enum PT_ID_APUSYS {
	PT_ID_APUSYS_FW,
	PT_ID_APUSYS_XFILE,
	PT_ID_MDLA_FW_BOOT,
	PT_ID_MDLA_FW_MAIN,
	PT_ID_MDLA_XFILE,
	PT_ID_MVPU_FW,
	PT_ID_MVPU_XFILE,
	PT_ID_MVPU_SEC_FW,
	PT_ID_MVPU_SEC_XFILE
};

#define PART_HEADER_DEFAULT_ADDR	(0xFFFFFFFF)
#define LOAD_ADDR_MODE_BACKWARD		(0x00000000)
#define PART_MAGIC			0x58881688
#define EXT_MAGIC			0x58891689
#define PT_MAGIC			0x58901690

struct ptimg_hdr_t {
	uint32_t magic;     /* magic number*/
	uint32_t hdr_size;  /* header size */
	uint32_t img_size;  /* img size */
	uint32_t align;     /* alignment */
	uint32_t id;        /* image id */
	uint32_t addr;      /* memory addr */
};

#endif /* APUSYS_RV_H */

