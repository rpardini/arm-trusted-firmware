/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APUSYS_RV_H
#define APUSYS_RV_H

#include <lib/mmio.h>


struct apusys_rv_plat_ops {
	int (*apusys_rv_ops_setup_reviser)(void);
	int (*apusys_rv_ops_setup_apummu)(void);
	int (*apusys_rv_ops_pwr_ctrl)(uint32_t op);
	int (*apusys_rv_ops_reset_mp)(void);
	int (*apusys_rv_ops_setup_boot)(void);
	int (*apusys_rv_ops_start_mp)(void);
	int (*apusys_rv_ops_stop_mp)(void);
	int (*apusys_rv_ops_drv_init)(void);
	int (*apusys_rv_ops_mbox_mpu_init)(void);
	int (*apusys_rv_ops_setup_ce_bin)(void);
	int (*apusys_rv_ops_setup_normal_mem)(uint64_t addr, uint64_t size);
	int (*apusys_rv_ops_setup_secure_mem)(uint64_t addr, uint64_t size);
	int (*apusys_rv_ops_kernel_map_secure_iova)();
	int (*apusys_rv_ops_setup_aee_coredump_mem)(uint64_t addr, uint64_t size);
	int (*apusys_rv_ops_disable_wdt_isr)(void);
	int (*apusys_rv_ops_clear_wdt_isr)(void);
	int (*apusys_rv_ops_cg_gating)(void);
	int (*apusys_rv_ops_cg_ungating)(void);
	int (*apusys_rv_ops_coredump_shadow_copy)(void);
	int (*apusys_rv_ops_tcmdump)(void);
	int (*apusys_rv_ops_tcmdump_wa)(void);
	int (*apusys_rv_ops_ramdump)(void);
	int (*apusys_rv_ops_tbufdump)(void);
	int (*apusys_rv_ops_cachedump)(void);
	int (*apusys_rv_ops_dbg_apb_attach)(void);
	int (*apusys_rv_ops_regdump)(uint32_t dbg_apb_status);
	int (*apusys_rv_ops_ce_reset)(struct smccc_res *smccc_ret);
	int (*apusys_rv_ops_ce_regdump)(uint32_t op, struct smccc_res *smccc_ret);
	int (*apusys_rv_ops_ce_mask_init)(void);
	int (*apusys_rv_ops_ce_sram_dump)(void);
	int (*apusys_rv_ops_release_scp_hw_sem)(void);
	int (*apusys_rv_ops_logtop_reg_dump)(uint32_t op, struct smccc_res *smccc_ret);
	int (*apusys_rv_ops_logtop_reg_write)(uint32_t op, uint32_t write_val,
		struct smccc_res *smccc_ret);
	int (*apusys_rv_ops_logtop_reg_w1c)(uint32_t op, struct smccc_res *smccc_ret);
	int (*apusys_rv_ops_decode_apu_exp_irq)(void);
	int (*apusys_rv_ops_ce_debug_regdump)(void);
	int (*apusys_rv_ops_setup_tcm_log_mem)(uint64_t addr, uint64_t size);
	int (*apusys_rv_ops_dump_tcm_log)(void);
	int (*apusys_rv_ops_cold_boot_clr_mbox_dummy)(void);
	int (*apusys_rv_ops_ce_reg_write)(uint32_t op, uint32_t write_val);
};

extern struct apusys_rv_plat_ops apusys_rv_ops;

/******************************************************************************
 * REGISTER ADDRESS DEFINITION
 ******************************************************************************/
/* NOTICE!!! FOR GILTCH WA PROBLEM, PLEASE ADD HW SEM  */
/* APU_MD32_SYSCTRL */
#define APU_MD32_SYSCTRL_SZ	(0x1000)
#define MD32_SYS_CTRL		(APU_MD32_SYSCTRL + 0x0)
#define UP_INT_EN2		(APU_MD32_SYSCTRL + 0xc)
#define MD32_TBUF_DBG_SEL	(APU_MD32_SYSCTRL + 0x84)
#define MD32_TBUF_DBG_DAT3	(APU_MD32_SYSCTRL + 0x94)
#define MD32_DBG_BUS_SEL	(APU_MD32_SYSCTRL + 0x98)
#define MD32_DBG_BUS_OUT	(APU_MD32_SYSCTRL + 0x9C)
#define APU_UP_SYS_DBG_EN	(1UL << 16)
#define MD32_CLK_EN		(APU_MD32_SYSCTRL + 0xb8)
#define UP_WAKE_HOST_MASK0	(APU_MD32_SYSCTRL + 0xbc)
#define UP_WAKE_HOST_MASK1	(APU_MD32_SYSCTRL + 0xc0)
#define MD32_MON_PC		(APU_MD32_SYSCTRL + 0x838)
#define MD32_MON_LR		(APU_MD32_SYSCTRL + 0x83c)
#define MD32_MON_SP		(APU_MD32_SYSCTRL + 0x840)
#define MD32_STATUS		(APU_MD32_SYSCTRL + 0x844)
#define MD32_TBUF_ADDR_SHIFT	(16)
#define MD32_TBUF_ADDR_MASK	(0x7)

#define WDT_IRQ_EN	(1UL << 0)
#define MBOX0_IRQ_EN	(1UL << 21)
#define MBOX1_IRQ_EN	(1UL << 22)
#define MBOX2_IRQ_EN	(1UL << 23)
#define DBG_APB_EN	(1UL << 31)

#define MD32_G2B_CG_EN		(1UL << 11)
#define MD32_DBG_EN		(1UL << 10)
#define MD32_DM_AWUSER_IOMMU_EN	(2U << 8)
#define MD32_DM_ARUSER_IOMMU_EN	(2U << 6)
#define MD32_PM_AWUSER_IOMMU_EN	(2U << 4)
#define MD32_PM_ARUSER_IOMMU_EN	(2U << 2)
#define WAKE_HOST		(1UL << 1)
#define MD32_SOFT_RSTN		(1UL << 0)

/* APU_MD32_WDT */
#define APU_MD32_WDT_SZ	(0x1000)
#define WDT_INT		(APU_MD32_WDT + 0x0)
#define WDT_CTRL0	(APU_MD32_WDT + 0x4)

#define WDT_EN 		(1UL << 31)


/* APU_SCTRL_REVISER */
#define APU_SCTRL_REVISER_SZ	(0x2000)
#define UP_NORMAL_DOMAIN_NS	(APU_SCTRL_REVISER + 0x0)
#define UP_PRI_DOMAIN_NS	(APU_SCTRL_REVISER + 0x4)
#define UP_NORMAL_DOMAIN	(7)
#define UP_PRI_DOMAIN		(5)
#define UP_DOMAIN_SHIFT		(0)
#define UP_NS_SHIFT		(4)
#define UP_IOMMU_CTRL		(APU_SCTRL_REVISER + 0x8)
#define MMU_EN_SHIFT		(0)
#define MMU_CTRL		(1UL << 1)
#define MMU_CTRL_LOCK		(1UL << 2)
#define UP_CORE0_VABASE0	(APU_SCTRL_REVISER + 0xc)
#define UP_CORE0_MVABASE0	(APU_SCTRL_REVISER + 0x10)
#define UP_CORE0_VABASE1	(APU_SCTRL_REVISER + 0x14)
#define UP_CORE0_MVABASE1	(APU_SCTRL_REVISER + 0x18)
#define VLD			(1UL << 0)
#define PARTIAL_ENABLE		(1UL << 1)
#define THREAD_NUM_SHIFT	(2)
#define VABASE1_ADR_SHIFT	(10)
#define VASIZE_1MB		(1UL)
#define MVABASE_SHIFT		(10)
#define USERFW_CTXT		(APU_SCTRL_REVISER + 0x1000)
#define SECUREFW_CTXT		(APU_SCTRL_REVISER + 0x1004)
#define CFG_4GB_SEL_EN		(1UL << 2)

/* APU_AO_CTRL */
#define APU_AO_CTRL_SZ		(0x1000)
#define MD32_PRE_DEFINE		(APU_AO_CTRL + 0x0)
#define MD32_BOOT_CTRL		(APU_AO_CTRL + 0x4)
#define MD32_RUNSTALL		(APU_AO_CTRL + 0x8)
#define APU_AO_SECURE_FW	(APU_AO_CTRL + 0x10)

#define PREDEFINE_NON_CACHE	(0U)
#define PREDEFINE_TCM		(1UL)
#define PREDEFINE_CACHE		(2U)
#define PREDEFINE_CACHE_TCM	(3U)

#define PREDEF_1G_OFS (0)
#define PREDEF_2G_OFS (2)
#define PREDEF_3G_OFS (4)
#define PREDEF_4G_OFS (6)


/* APU_RPCTOP */
#define APU_RPCTOP_SZ (0x1000)

/* APU_RCX_CONFIG */
#define APU_RCX_CONFIG_SZ (0x1000)

/* APU_RCX_VCORE_CONFIG */
#define APU_RCX_VCORE_CONFIG_SZ (0x1000)

#define PWR_RDY             (APU_RPCTOP + 0x44)
#define VCORE_CLK_CRTL      (APU_RCX_VCORE_CONFIG)
#define RCX_CLK_CRTL        (APU_RCX_CONFIG)

#define bits_get(x, m, o)   ((x & (m << o)) >> o)
#define PWR_STATUS(x)       bits_get(x, 1, 0)

#define APUSYS_IOVA_PAGE_TBL_SIZE 0x4000 /* secure iova page table size */

/* APU_LOGTOP */
#define APU_LOGTOP_CON		(APU_LOGTOP + 0x0)
#define APU_LOG_BUF_T_SIZE	(APU_LOGTOP + 0x78)
#define APU_LOG_BUF_W_PTR	(APU_LOGTOP + 0x80)
#define APU_LOG_BUF_R_PTR	(APU_LOGTOP + 0x84)
#define HW_SEMA2		(APU_ARE_REG_BASE + 0x0E08)
#define HW_SEMA_USER		(0x2)
#define HW_SEMA_LOGGER_USER	(0x3)
#define MAX_SMC_OP_NUM		(0x3)
#define APU_LOG_ROW_SIZE	(0x80)
#define APU_LOG_SYNC_CODE	(0xA5A5)
#define APU_LOG_SYNC_MASK	(0xFFFF)

#define APUSYS_TCM_LP_LOG_OFFSET         (0x01100)
#define APUSYS_TCM_LP_LOG_SIZE           (0xFEF00)
#define APUSYS_TCM_LP_LOG_HEADER_SIZE    (0x00080)

enum {
	SMC_OP_APU_LOG_BUF_NULL = 0,
	SMC_OP_APU_LOG_BUF_T_SIZE,
	SMC_OP_APU_LOG_BUF_W_PTR,
	SMC_OP_APU_LOG_BUF_R_PTR,
	SMC_OP_APU_LOG_BUF_CON,
	SMC_OP_APU_LOG_BUF_NUM
};

#endif /* APUSYS_RV_H */
