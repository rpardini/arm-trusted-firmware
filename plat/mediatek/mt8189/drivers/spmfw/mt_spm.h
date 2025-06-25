/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#ifndef __SOC_MEDIATEK_MT8189_INCLUDE_SOC_SPM_H__
#define __SOC_MEDIATEK_MT8189_INCLUDE_SOC_SPM_H__

#include <stdint.h>
#include <platform_def.h>

/* SPM READ/WRITE CFG */
#define SPM_PROJECT_CODE			0xb16
#define SPM_REGWR_CFG_KEY			(SPM_PROJECT_CODE << 16)
#define SPM_SYSTEM_BASE_OFFSET			0x0

/* POWERON_CONFIG_EN (0x1C001000+0x000) */
#define BCLK_CG_EN_LSB				BIT(0)
#define PROJECT_CODE_LSB			BIT(16)

/* SPM_CLK_CON (0x1C001000+0x024) */
#define REG_SPM_LOCK_INFRA_DCM_LSB		BIT(0)
#define REG_CXO32K_REMOVE_EN_LSB		BIT(1)
#define REG_SPM_LEAVE_SUSPEND_MERGE_MASK_LSB	BIT(4)
#define REG_SRCLKENO0_SRC_MASK_B_LSB		BIT(8)
#define REG_SRCLKENO1_SRC_MASK_B_LSB		BIT(16)
#define REG_SRCLKENO2_SRC_MASK_B_LSB		BIT(24)

/* PCM_CON0 (0x1C001000+0x018) */
#define PCM_CK_EN_LSB				BIT(2)
#define PCM_SW_RESET_LSB			BIT(15)
#define PCM_CON0_PROJECT_CODE_LSB		BIT(16)

/* PCM_CON1 (0x1C001000+0x01C) */
#define REG_SPM_APB_INTERNAL_EN_LSB		BIT(3)
#define REG_PCM_TIMER_EN_LSB			BIT(5)
#define REG_PCM_WDT_EN_LSB			BIT(8)
#define REG_PCM_WDT_WAKE_LSB			BIT(9)
#define REG_SSPM_APB_P2P_EN_LSB			BIT(10)
#define REG_MCUPM_APB_P2P_EN_LSB		BIT(11)
#define REG_RSV_APB_P2P_EN_LSB			BIT(12)
#define RG_PCM_IRQ_MSK_LSB			BIT(15)
#define PCM_CON1_PROJECT_CODE_LSB		BIT(16)

/* SPM_WAKEUP_EVENT_MASK (0x1C001000+0x808) */
#define REG_WAKEUP_EVENT_MASK_LSB		BIT(0)

/* DDREN_DBC_CON (0x1C001000+0x890) */
#define REG_DDREN_DBC_LEN_LSB			BIT(0)
#define REG_DDREN_DBC_EN_LSB			BIT(16)

/* SPM_DVFS_CON (0x1C001000+0x3AC) */
#define SPM_DVFS_FORCE_ENABLE_LSB		BIT(2)
#define FORCE_DVFS_WAKE_LSB			BIT(3)
#define SPM_DVFSRC_ENABLE_LSB			BIT(4)
#define DVFSRC_WAKEUP_EVENT_MASK_LSB		BIT(6)
#define SPM2RC_EVENT_ABORT_LSB			BIT(7)
#define DVFSRC_LEVEL_ACK_LSB			BIT(8)

/* SPM_SW_FLAG_0 (0x1C001000+0x600) */
#define SPM_SW_FLAG_LSB				BIT(0)

/* SYS_TIMER_CON (0x1C001000+0x500) */
#define SYS_TIMER_START_EN_LSB			BIT(0)
#define SYS_TIMER_LATCH_EN_LSB			BIT(1)
#define SYS_TIMER_ID_LSB			BIT(8)
#define SYS_TIMER_VALID_LSB			BIT(31)

#define AP_WDT_TIMEOUT_SUSPEND	5400 /* 90min */
#define PCM_TIMER_SUSPEND	((AP_WDT_TIMEOUT_SUSPEND - 30) * 32768) /* 90min - 30sec */
#define RG_AXI_DCM_DIS_EN	BIT(21)
#define RG_PLLCK_SEL_NO_SPM	BIT(22)

#define MT_SPM_TIME_GET(tm) ({ tm = el3_uptime(); })

#define SPM_FW_NO_RESUME 1
#define MCUSYS_MTCMOS_ON 0
#define WAKEUP_LOG_ON	 0
#define PMIC_ONLV	 1

#define SPM_WAKEUP_EVENT_MASK_BIT0	BIT(0)

#define MT_BUS26M_EXT_LP_26M_ON_MODE (MT_SPM_EX_OP_SET_IS_ADSP\
					| MT_SPM_EX_OP_SET_IS_FM_AUDIO)

#define MT_VCORE_EXT_LP_VCORE_ON_MODE (MT_SPM_EX_OP_SET_IS_ADSP\
					| MT_SPM_EX_OP_SET_IS_USB_HEADSET\
					| MT_SPM_EX_OP_SET_IS_FM_AUDIO)

/* AP_MDSRC_REQ MD 26M ON settle time (3ms) */
#define AP_MDSRC_REQ_MD_26M_SETTLE 3

/* Setting the SPM settle time*/
#define SPM_SYSCLK_SETTLE	0x60FE	/* 1685us */

/* Setting the SPM req/ack time*/
#define SPM_ACK_TIMEOUT_US	1000

/**************************************
 * Config and Parameter
 **************************************/
#define POWER_ON_VAL0_DEF		0x0000F100
#define POWER_ON_VAL1_DEF		0x003FFE20
#define SPM_WAKEUP_EVENT_MASK_DEF	0xF97FFCFF
#define SPM_BUS_PROTECT_MASK_B_DEF	0xffffffff
#define SPM_BUS_PROTECT2_MASK_B_DEF	0xffffffff
#define MD32PCM_DMA0_CON_VAL		0x0003820E
#define MD32PCM_DMA0_START_VAL		0x00008000
#define SPM_DVFS_LEVEL_DEF		0x00000001
#define SPM_DVS_DFS_LEVEL_DEF		0x00010001
#define SPM_RESOURCE_ACK_CON0_DEF	0xCC4E4ECC
#define SPM_RESOURCE_ACK_CON1_DEF	0x00CCCCCC
#define SPM_SYSCLK_SETTLE		0x60FE /* 1685us */
#define SPM_INIT_DONE_US		20
#define PCM_WDT_TIMEOUT			(30 * 32768)
#define PCM_TIMER_MAX			((5400 - 30) * 32768) /* 90min - 30sec */

/**************************************
 * Definition and Declaration
 **************************************/
/* SPM_IRQ_MASK */
#define ISRM_TWAM		BIT(2)
#define ISRM_RET_IRQ1		BIT(9)
#define ISRM_RET_IRQ2		BIT(10)
#define ISRM_RET_IRQ3		BIT(11)
#define ISRM_RET_IRQ4		BIT(12)
#define ISRM_RET_IRQ5		BIT(13)
#define ISRM_RET_IRQ6		BIT(14)
#define ISRM_RET_IRQ7		BIT(15)
#define ISRM_RET_IRQ8		BIT(16)
#define ISRM_RET_IRQ9		BIT(17)
#define ISRM_RET_IRQ_AUX     ((ISRM_RET_IRQ9) | (ISRM_RET_IRQ8) | \
(ISRM_RET_IRQ7) | (ISRM_RET_IRQ6) | (ISRM_RET_IRQ5) | (ISRM_RET_IRQ4) | \
(ISRM_RET_IRQ3) | (ISRM_RET_IRQ2) | (ISRM_RET_IRQ1))

#define ISRM_ALL_EXC_TWAM	(ISRM_RET_IRQ_AUX)
#define ISRM_ALL		(ISRM_ALL_EXC_TWAM | ISRM_TWAM)

/* SPM_IRQ_STA */
#define ISRS_TWAM		BIT(2)
#define ISRS_PCM_RETURN		BIT(3)
#define ISRC_TWAM		ISRS_TWAM
#define ISRC_ALL_EXC_TWAM	ISRS_PCM_RETURN
#define ISRC_ALL		(ISRC_ALL_EXC_TWAM | ISRC_TWAM)

/* PCM_PWR_IO_EN */
#define PCM_PWRIO_EN_R0		BIT(0)
#define PCM_PWRIO_EN_R7		BIT(7)
#define PCM_RF_SYNC_R0		BIT(16)
#define PCM_RF_SYNC_R6		BIT(22)
#define PCM_RF_SYNC_R7		BIT(23)

/* SPM_SWINT */
#define PCM_SW_INT0		BIT(0)
#define PCM_SW_INT1		BIT(1)
#define PCM_SW_INT2		BIT(2)
#define PCM_SW_INT3		BIT(3)
#define PCM_SW_INT4		BIT(4)
#define PCM_SW_INT5		BIT(5)
#define PCM_SW_INT6		BIT(6)
#define PCM_SW_INT7		BIT(7)
#define PCM_SW_INT8		BIT(8)
#define PCM_SW_INT9		BIT(9)
#define PCM_SW_INT_ALL		(PCM_SW_INT9 | PCM_SW_INT8 | PCM_SW_INT7 | \
				PCM_SW_INT6 | PCM_SW_INT5 | PCM_SW_INT4 | PCM_SW_INT3 | \
				PCM_SW_INT2 | PCM_SW_INT1 | PCM_SW_INT0)

#define SPM_ACK_CHK_3_SEL_HW_S1 0x00350098
#define SPM_ACK_CHK_3_HW_S1_CNT 1

// DEFINE_BIT(SPM_ACK_CHK_3_CON_CLR_ALL, 1)
// DEFINE_BIT(SPM_ACK_CHK_3_CON_EN_0, 4)
// DEFINE_BIT(SPM_ACK_CHK_3_CON_EN_1, 8)
// DEFINE_BIT(SPM_ACK_CHK_3_CON_HW_MODE_TRIG, 11)

/* --- SPM Flag Define --- */
#define SPM_FLAG_DISABLE_INFRA_PDN		BIT(0)
#define SPM_FLAG_DISABLE_DPM_PDN		BIT(1)
#define SPM_FLAG_DISABLE_MCUPM_PDN		BIT(2)
#define SPM_FLAG_DISABLE_DPY_PDN		BIT(3)
#define SPM_FLAG_ENABLE_LVTS_WORKAROUND		BIT(4)
#define SPM_FLAG_DISABLE_SYSRAM_SLEEP		BIT(5)
#define SPM_FLAG_DISABLE_SSPM_SRAM_SLEEP	BIT(6)
#define SPM_FLAG_DISABLE_BUS_CLK_OFF		BIT(7)
#define SPM_FLAG_DISABLE_VCORE_DVS		BIT(8)
#define SPM_FLAG_DISABLE_DDR_DFS		BIT(9)
#define SPM_FLAG_DISABLE_EMI_DFS		BIT(10)
#define SPM_FLAG_DISABLE_BUS_DFS		BIT(11)
#define SPM_FLAG_DISABLE_COMMON_SCENARIO	BIT(12)
#define SPM_FLAG_DISABLE_CPU_PDN		BIT(13)
#define SPM_FLAG_DISABLE_ARMPLL_OFF		BIT(14)
#define SPM_FLAG_DISABLE_DDRPHY_PDN		BIT(15)
#define SPM_FLAG_KEEP_CSYSPWRACK_HIGH		BIT(16)
#define SPM_FLAG_ENABLE_VDD2_DVS		BIT(17)
#define SPM_FLAG_ENABLE_SPM_DBG_WDT_DUMP	BIT(18)
#define SPM_FLAG_RUN_COMMON_SCENARIO		BIT(19)
#define SPM_FLAG_ENABLE_AOV			BIT(21)
#define SPM_FLAG_ENABLE_MD_MUMTAS		BIT(22)
#define SPM_FLAG_DISABLE_DVFSQ			BIT(23)
#define SPM_FLAG_ENABLE_EMI_SSC			BIT(24)
#define SPM_FLAG_ENABLE_VMDDR_DVS		BIT(25)
#define SPM_FLAG_VCORE_STATE			BIT(26)
#define SPM_FLAG_VTCXO_STATE			BIT(27)
#define SPM_FLAG_INFRA_STATE			BIT(28)
#define SPM_FLAG_APSRC_STATE			BIT(29)
#define SPM_FLAG_VRF18_STATE			BIT(30)
#define SPM_FLAG_DDREN_STATE			BIT(31)

/* --- SPM Flag1 Define --- */
#define SPM_FLAG1_DISABLE_AXI_BUS_TO_26M		BIT(0)
#define SPM_FLAG1_DISABLE_SYSPLL_OFF			BIT(1)
#define SPM_FLAG1_DISABLE_PWRAP_CLK_SWITCH		BIT(2)
#define SPM_FLAG1_DISABLE_ULPOSC_OFF			BIT(3)
#define SPM_FLAG1_FW_SET_ULPOSC_ON			BIT(4)
#define SPM_FLAG1_DISABLE_EMI_CLK_TO_ULPOSC		BIT(5)
#define SPM_FLAG1_DISABLE_NO_RESUME			BIT(6)
#define SPM_FLAG1_ENABLE_VS3_VOSEL_CTRL			BIT(7)
#define SPM_FLAG1_ENABLE_VS2_VS3_VOTER			BIT(8)
#define SPM_FLAG1_POLLING_BUS_PROTECT			BIT(9)
#define SPM_FLAG1_DISABLE_SRCLKEN_LOW			BIT(10)
#define SPM_FLAG1_DISABLE_SCP_CLK_SWITCH		BIT(11)
#define SPM_FLAG1_DISABLE_TOP_26M_CK_OFF		BIT(12)
#define SPM_FLAG1_DISABLE_PCM_26M_SWITCH		BIT(13)
#define SPM_FLAG1_DISABLE_CKSQ_OFF			BIT(14)
#define SPM_FLAG1_DO_DPSW_0P725V			BIT(15)
#define SPM_FLAG1_ENABLE_COMMON_APSRC			BIT(16)
#define SPM_FLAG1_ENABLE_DFD_SOC_MTCMOS_EN		BIT(17)
#define SPM_FLAG1_ENABLE_LP5_DVFS			BIT(18)
#define SPM_FLAG1_ENABLE_EMI_DFS_BACKUP_SOLUTION	BIT(19)
#define SPM_FLAG1_DISABLE_INFRA_SRAM_SLEEP		BIT(20)
#define SPM_FLAG1_DISABLE_AXI_MEM_CLK_OFF		BIT(21)
#define SPM_FLAG1_DISABLE_VCORE_LP			BIT(22)
#define SPM_FLAG1_RESERVED_BIT23			BIT(23)
#define SPM_FLAG1_DISABLE_SCP_VREQ_MASK_CONTROL		BIT(24)
#define SPM_FLAG1_RESERVED_BIT25			BIT(25)
#define SPM_FLAG1_RESERVED_BIT26			BIT(26)
#define SPM_FLAG1_RESERVED_BIT27			BIT(27)
#define SPM_FLAG1_RESERVED_BIT28			BIT(28)
#define SPM_FLAG1_RESERVED_BIT29			BIT(29)
#define SPM_FLAG1_ENABLE_WAKE_PROF			BIT(30)
#define SPM_FLAG1_ENABLE_SLEEP_PROF			BIT(31)

/* --- SPM DEBUG Define --- */
#define SPM_DBG_DEBUG_IDX_26M_WAKE		BIT(0)
#define SPM_DBG_DEBUG_IDX_26M_SLEEP		BIT(1)
#define SPM_DBG_DEBUG_IDX_INFRA_WAKE		BIT(2)
#define SPM_DBG_DEBUG_IDX_INFRA_SLEEP		BIT(3)
#define SPM_DBG_DEBUG_IDX_APSRC_WAKE		BIT(4)
#define SPM_DBG_DEBUG_IDX_APSRC_SLEEP		BIT(5)
#define SPM_DBG_DEBUG_IDX_VRF18_WAKE		BIT(6)
#define SPM_DBG_DEBUG_IDX_VRF18_SLEEP		BIT(7)
#define SPM_DBG_DEBUG_IDX_VCORE_WAKE		BIT(8)
#define SPM_DBG_DEBUG_IDX_VCORE_OFF		BIT(9)
#define SPM_DBG_DEBUG_IDX_DDREN_WAKE		BIT(10)
#define SPM_DBG_DEBUG_IDX_DDREN_SLEEP		BIT(11)
#define SPM_DBG_DEBUG_IDX_PMIC_SLEEP		BIT(12)
#define SPM_DBG_DEBUG_IDX_PMIC_WAKE		BIT(13)
#define SPM_DBG_DEBUG_IDX_EMI_WAKE		BIT(14)
#define SPM_DBG_DEBUG_IDX_EMI_SLEEP		BIT(15)
#define SPM_DBG_DEBUG_IDX_SYSRAM_SLP		BIT(16)
#define SPM_DBG_DEBUG_IDX_SSPM_WFI		BIT(17)
#define SPM_DBG_DEBUG_IDX_SSPM_SRAM_SLP		BIT(18)
#define SPM_DBG_DEBUG_IDX_SSPM_ON		BIT(19)
#define SPM_DBG_DEBUG_IDX_SYSRAM_ON		BIT(20)
#define SPM_DBG_DEBUG_IDX_AOVBUS_130M		BIT(21)
#define SPM_DBG_DEBUG_IDX_AOV_MODE		BIT(22)
#define SPM_DBG_DEBUG_IDX_SPM_DVFS_NO_REQ	BIT(23)
#define SPM_DBG_DEBUG_IDX_SPM_NORMAL_WAKEUP	BIT(28)
#define SPM_DBG_DEBUG_IDX_SPM_WAKEUP_BY_NONE	BIT(29)

/* --- SPM DEBUG1 Define --- */
#define SPM_DBG1_DEBUG_IDX_CURRENT_IS_LP		BIT(0)
#define SPM_DBG1_DEBUG_IDX_VCORE_DVFS_START		BIT(1)
#define SPM_DBG1_DEBUG_IDX_SYSPLL_OFF			BIT(2)
#define SPM_DBG1_DEBUG_IDX_SYSPLL_ON			BIT(3)
#define SPM_DBG1_DEBUG_IDX_INFRA_MTCMOS_OFF		BIT(5)
#define SPM_DBG1_DEBUG_IDX_INFRA_MTCMOS_ON		BIT(6)
#define SPM_DBG1_DEBUG_IDX_VTCXO_SLEEP_ABORT_0		BIT(7)
#define SPM_DBG1_DEBUG_IDX_VTCXO_SLEEP_ABORT_1		BIT(8)
#define SPM_DBG1_DEBUG_IDX_SPM_DFD_SOC_MTCMOS_EN	BIT(9)
#define SPM_DBG1_DEBUG_IDX_PWRAP_CLK_TO_ULPOSC		BIT(11)
#define SPM_DBG1_DEBUG_IDX_PWRAP_CLK_TO_26M		BIT(12)
#define SPM_DBG1_DEBUG_IDX_SCP_CLK_TO_32K		BIT(13)
#define SPM_DBG1_DEBUG_IDX_SCP_CLK_TO_26M		BIT(14)
#define SPM_DBG1_DEBUG_IDX_BUS_CLK_OFF			BIT(15)
#define SPM_DBG1_DEBUG_IDX_BUS_CLK_ON			BIT(16)
#define SPM_DBG1_DEBUG_IDX_SRCLKEN2_LOW			BIT(17)
#define SPM_DBG1_DEBUG_IDX_SRCLKEN2_HIGH		BIT(18)
#define SPM_DBG1_DEBUG_IDX_VMDDRVDDQ_OFF		BIT(19)
#define SPM_DBG1_DEBUG_IDX_ULPOSC_IS_OFF_BUT_SHOULD_ON	BIT(20)
#define SPM_DBG1_DEBUG_IDX_PMIC_IRQ_ACK_LOW_ABORT	BIT(21)
#define SPM_DBG1_DEBUG_IDX_PMIC_IRQ_ACK_HIGH_ABORT	BIT(22)
#define SPM_DBG1_DEBUG_IDX_PWRAP_SLEEP_ACK_LOW_ABORT	BIT(23)
#define SPM_DBG1_DEBUG_IDX_PWRAP_SLEEP_ACK_HIGH_ABORT	BIT(24)
#define SPM_DBG1_DEBUG_IDX_VMDDRVDDQ_ON			BIT(25)
#define SPM_DBG1_DEBUG_IDX_SCP_SLP_ACK_LOW_ABORT	BIT(26)
#define SPM_DBG1_DEBUG_IDX_SCP_SLP_ACK_HIGH_ABORT	BIT(27)
#define SPM_DBG1_DEBUG_IDX_SPM_PMIF_CMD_RDY_ABORT	BIT(28)
#define SPM_DBG1_DEBUG_IDX_MCUPM_RESORE			BIT(29)
#define SPM_DBG1_DEBUG_IDX_DISABLE_DVFSRC		BIT(31)

/* SPM_EVENT_COUNTER_CLEAR (0x1C001000+0x8B8) */
#define REG_SPM_EVENT_COUNTER_CLR_LSB		BIT(0)

/* SYS_TIMER_CON (0x10006000+0x98C) */
#define SYS_TIMER_START_EN_LSB			BIT(0)

//-- MD32PCM_STA1 define
#define R12_PCM_TIMER_B			BIT(0)
#define R12_TWAM_PMSR_DVFSRC		BIT(1)
#define R12_KP_IRQ_B			BIT(2)
#define R12_APWDT_EVENT_B		BIT(3)
#define R12_APXGPT_EVENT_B		BIT(4)
#define R12_CONN2AP_WAKEUP_B		BIT(5)
#define R12_EINT_EVENT_B		BIT(6)
#define R12_CONN_WDT_IRQ_B		BIT(7)
#define R12_CCIF0_EVENT_B		BIT(8)
#define R12_CCIF1_EVENT_B		BIT(9)
#define R12_SSPM2SPM_WAKEUP_B		BIT(10)
#define R12_SCP2SPM_WAKEUP_B		BIT(11)
#define R12_VADSP2SPM_WAKEUP_B		BIT(12)
#define R12_PCM_WDT_WAKEUP_B		BIT(13)
#define R12_USB0_CDSC_B			BIT(14)
#define R12_USB0_POWERDWN_B		BIT(15)
#define R12_SBD_INTR_B			BIT(16)
#define R12_UART2SPM_IRQ_B		BIT(17)
#define R12_SYS_TIMER_EVENT_B		BIT(18)
#define R12_EINT_EVENT_SECURE_B		BIT(19)
#define R12_AFE_IRQ_MCU_B		BIT(20)
#define R12_THERM_CTRL_EVENT_B		BIT(21)
#define R12_SYS_CIRQ_IRQ_B		BIT(22)
#define R12_MD2AP_PEER_EVENT_B		BIT(23)
#define R12_CSYSPWREQ_B			BIT(24)
#define R12_MD_WDT_B			BIT(25)
#define R12_AP2AP_PEER_WAKEUP_B		BIT(26)
#define R12_SEJ_B			BIT(27)
#define R12_CPU_WAKEUP			BIT(28)
#define R12_APUSYS_WAKE_HOST_B		BIT(29)
#define R12_PCIE_MAC_IRQ_WAKE_B		BIT(30)
#define R12_MSDC_WAKEUP_EVENT_B		BIT(31)
/* SPM_EVENT_COUNTER_CLEAR (0x1C001000+0x8B8) */
#define REG_SPM_EVENT_COUNTER_CLR_LSBBIT 0
struct pwr_ctrl {
	/* for SPM */
	uint32_t pcm_flags;
	/* can override pcm_flags */
	uint32_t pcm_flags_cust;
	/* set bit of pcm_flags, after pcm_flags_cust */
	uint32_t pcm_flags_cust_set;
	/* clr bit of pcm_flags, after pcm_flags_cust */
	uint32_t pcm_flags_cust_clr;
	uint32_t pcm_flags1;
	/* can override pcm_flags1 */
	uint32_t pcm_flags1_cust;
	/* set bit of pcm_flags1, after pcm_flags1_cust */
	uint32_t pcm_flags1_cust_set;
	/* clr bit of pcm_flags1, after pcm_flags1_cust */
	uint32_t pcm_flags1_cust_clr;
	/* @ 1T 32K */
	uint32_t timer_val;
	/* @ 1T 32K, can override timer_val */
	uint32_t timer_val_cust;
	/* stress for dpidle */
	uint32_t timer_val_ramp_en;
	/* stress for suspend */
	uint32_t timer_val_ramp_en_sec;
	uint32_t wake_src;
	/* can override wake_src */
	uint32_t wake_src_cust;
	uint32_t wakelock_timer_val;
	/* disable wdt in suspend */
	uint8_t wdt_disable;

	/* SPM_CLK_CON */
	uint8_t reg_spm_lock_infra_dcm_lsb;
	uint8_t reg_cxo32k_remove_en_lsb;
	uint8_t reg_spm_leave_suspend_merge_mask_lsb;
	uint8_t reg_sysclk0_src_mask_b_lsb;
	uint8_t reg_sysclk1_src_mask_b_lsb;
	uint8_t reg_sysclk2_src_mask_b_lsb;

	/* SPM_AP_STANDBY_CON */
	uint8_t reg_wfi_op;
	uint8_t reg_wfi_type;
	uint8_t reg_mp0_cputop_idle_mask;
	uint8_t reg_mp1_cputop_idle_mask;
	uint8_t reg_mcusys_idle_mask;
	uint8_t reg_csyspwrup_req_mask_lsb;
	uint8_t reg_wfi_af_sel;
	uint8_t reg_cpu_sleep_wfi;

	/* SPM_SRC_REQ */
	uint8_t reg_spm_adsp_mailbox_req;
	uint8_t reg_spm_apsrc_req;
	uint8_t reg_spm_ddren_req;
	uint8_t reg_spm_dvfs_req;
	uint8_t reg_spm_emi_req;
	uint8_t reg_spm_f26m_req;
	uint8_t reg_spm_infra_req;
	uint8_t reg_spm_pmic_req;
	uint8_t reg_spm_scp_mailbox_req;
	uint8_t reg_spm_sspm_mailbox_req;
	uint8_t reg_spm_sw_mailbox_req;
	uint8_t reg_spm_vcore_req;
	uint8_t reg_spm_vrf18_req;
	uint8_t adsp_mailbox_state;
	uint8_t apsrc_state;
	uint8_t ddren_state;
	uint8_t dvfs_state;
	uint8_t emi_state;
	uint8_t f26m_state;
	uint8_t infra_state;
	uint8_t pmic_state;
	uint8_t scp_mailbox_state;
	uint8_t sspm_mailbox_state;
	uint8_t sw_mailbox_state;
	uint8_t vcore_state;
	uint8_t vrf18_state;

	/* SPM_SRC_MASK_0 */
	uint8_t reg_apu_apsrc_req_mask_b;
	uint8_t reg_apu_ddren_req_mask_b;
	uint8_t reg_apu_emi_req_mask_b;
	uint8_t reg_apu_infra_req_mask_b;
	uint8_t reg_apu_pmic_req_mask_b;
	uint8_t reg_apu_srcclkena_mask_b;
	uint8_t reg_apu_vrf18_req_mask_b;
	uint8_t reg_audio_dsp_apsrc_req_mask_b;
	uint8_t reg_audio_dsp_ddren_req_mask_b;
	uint8_t reg_audio_dsp_emi_req_mask_b;
	uint8_t reg_audio_dsp_infra_req_mask_b;
	uint8_t reg_audio_dsp_pmic_req_mask_b;
	uint8_t reg_audio_dsp_srcclkena_mask_b;
	uint8_t reg_audio_dsp_vcore_req_mask_b;
	uint8_t reg_audio_dsp_vrf18_req_mask_b;
	uint8_t reg_cam_apsrc_req_mask_b;
	uint8_t reg_cam_ddren_req_mask_b;
	uint8_t reg_cam_emi_req_mask_b;
	uint8_t reg_cam_infra_req_mask_b;
	uint8_t reg_cam_pmic_req_mask_b;
	uint8_t reg_cam_srcclkena_mask_b;
	uint8_t reg_cam_vrf18_req_mask_b;
	uint8_t reg_mdp_emi_req_mask_b;

	/* SPM_SRC_MASK_1 */
	uint32_t reg_ccif_apsrc_req_mask_b;
	uint32_t reg_ccif_emi_req_mask_b;

	/* SPM_SRC_MASK_2 */
	uint32_t reg_ccif_infra_req_mask_b;
	uint32_t reg_ccif_pmic_req_mask_b;

	/* SPM_SRC_MASK_3 */
	uint32_t reg_ccif_srcclkena_mask_b;
	uint32_t reg_ccif_vrf18_req_mask_b;
	uint8_t reg_ccu_apsrc_req_mask_b;
	uint8_t reg_ccu_ddren_req_mask_b;
	uint8_t reg_ccu_emi_req_mask_b;
	uint8_t reg_ccu_infra_req_mask_b;
	uint8_t reg_ccu_pmic_req_mask_b;
	uint8_t reg_ccu_srcclkena_mask_b;
	uint8_t reg_ccu_vrf18_req_mask_b;
	uint8_t reg_cg_check_apsrc_req_mask_b;

	/* SPM_SRC_MASK_4 */
	uint8_t reg_cg_check_ddren_req_mask_b;
	uint8_t reg_cg_check_emi_req_mask_b;
	uint8_t reg_cg_check_infra_req_mask_b;
	uint8_t reg_cg_check_pmic_req_mask_b;
	uint8_t reg_cg_check_srcclkena_mask_b;
	uint8_t reg_cg_check_vcore_req_mask_b;
	uint8_t reg_cg_check_vrf18_req_mask_b;
	uint8_t reg_conn_apsrc_req_mask_b;
	uint8_t reg_conn_ddren_req_mask_b;
	uint8_t reg_conn_emi_req_mask_b;
	uint8_t reg_conn_infra_req_mask_b;
	uint8_t reg_conn_pmic_req_mask_b;
	uint8_t reg_conn_srcclkena_mask_b;
	uint8_t reg_conn_srcclkenb_mask_b;
	uint8_t reg_conn_vcore_req_mask_b;
	uint8_t reg_conn_vrf18_req_mask_b;
	uint8_t reg_cpueb_apsrc_req_mask_b;
	uint8_t reg_cpueb_ddren_req_mask_b;
	uint8_t reg_cpueb_emi_req_mask_b;
	uint8_t reg_cpueb_infra_req_mask_b;
	uint8_t reg_cpueb_pmic_req_mask_b;
	uint8_t reg_cpueb_srcclkena_mask_b;
	uint8_t reg_cpueb_vrf18_req_mask_b;
	uint8_t reg_disp0_apsrc_req_mask_b;
	uint8_t reg_disp0_ddren_req_mask_b;
	uint8_t reg_disp0_emi_req_mask_b;
	uint8_t reg_disp0_infra_req_mask_b;
	uint8_t reg_disp0_pmic_req_mask_b;
	uint8_t reg_disp0_srcclkena_mask_b;
	uint8_t reg_disp0_vrf18_req_mask_b;
	uint8_t reg_disp1_apsrc_req_mask_b;
	uint8_t reg_disp1_ddren_req_mask_b;

	/* SPM_SRC_MASK_5 */
	uint8_t reg_disp1_emi_req_mask_b;
	uint8_t reg_disp1_infra_req_mask_b;
	uint8_t reg_disp1_pmic_req_mask_b;
	uint8_t reg_disp1_srcclkena_mask_b;
	uint8_t reg_disp1_vrf18_req_mask_b;
	uint8_t reg_dpm_apsrc_req_mask_b;
	uint8_t reg_dpm_ddren_req_mask_b;
	uint8_t reg_dpm_emi_req_mask_b;
	uint8_t reg_dpm_infra_req_mask_b;
	uint8_t reg_dpm_pmic_req_mask_b;
	uint8_t reg_dpm_srcclkena_mask_b;

	/* SPM_SRC_MASK_6 */
	uint8_t reg_dpm_vcore_req_mask_b;
	uint8_t reg_dpm_vrf18_req_mask_b;
	uint8_t reg_dpmaif_apsrc_req_mask_b;
	uint8_t reg_dpmaif_ddren_req_mask_b;
	uint8_t reg_dpmaif_emi_req_mask_b;
	uint8_t reg_dpmaif_infra_req_mask_b;
	uint8_t reg_dpmaif_pmic_req_mask_b;
	uint8_t reg_dpmaif_srcclkena_mask_b;
	uint8_t reg_dpmaif_vrf18_req_mask_b;
	uint8_t reg_dvfsrc_level_req_mask_b;
	uint8_t reg_emisys_apsrc_req_mask_b;
	uint8_t reg_emisys_ddren_req_mask_b;
	uint8_t reg_emisys_emi_req_mask_b;
	uint8_t reg_gce_d_apsrc_req_mask_b;
	uint8_t reg_gce_d_ddren_req_mask_b;
	uint8_t reg_gce_d_emi_req_mask_b;
	uint8_t reg_gce_d_infra_req_mask_b;
	uint8_t reg_gce_d_pmic_req_mask_b;
	uint8_t reg_gce_d_srcclkena_mask_b;
	uint8_t reg_gce_d_vrf18_req_mask_b;
	uint8_t reg_gce_m_apsrc_req_mask_b;
	uint8_t reg_gce_m_ddren_req_mask_b;
	uint8_t reg_gce_m_emi_req_mask_b;
	uint8_t reg_gce_m_infra_req_mask_b;
	uint8_t reg_gce_m_pmic_req_mask_b;
	uint8_t reg_gce_m_srcclkena_mask_b;

	/* SPM_SRC_MASK_7 */
	uint8_t reg_gce_m_vrf18_req_mask_b;
	uint8_t reg_gpueb_apsrc_req_mask_b;
	uint8_t reg_gpueb_ddren_req_mask_b;
	uint8_t reg_gpueb_emi_req_mask_b;
	uint8_t reg_gpueb_infra_req_mask_b;
	uint8_t reg_gpueb_pmic_req_mask_b;
	uint8_t reg_gpueb_srcclkena_mask_b;
	uint8_t reg_gpueb_vrf18_req_mask_b;
	uint8_t reg_hwccf_apsrc_req_mask_b;
	uint8_t reg_hwccf_ddren_req_mask_b;
	uint8_t reg_hwccf_emi_req_mask_b;
	uint8_t reg_hwccf_infra_req_mask_b;
	uint8_t reg_hwccf_pmic_req_mask_b;
	uint8_t reg_hwccf_srcclkena_mask_b;
	uint8_t reg_hwccf_vcore_req_mask_b;
	uint8_t reg_hwccf_vrf18_req_mask_b;
	uint8_t reg_img_apsrc_req_mask_b;
	uint8_t reg_img_ddren_req_mask_b;
	uint8_t reg_img_emi_req_mask_b;
	uint8_t reg_img_infra_req_mask_b;
	uint8_t reg_img_pmic_req_mask_b;
	uint8_t reg_img_srcclkena_mask_b;
	uint8_t reg_img_vrf18_req_mask_b;
	uint8_t reg_infrasys_apsrc_req_mask_b;
	uint8_t reg_infrasys_ddren_req_mask_b;
	uint8_t reg_infrasys_emi_req_mask_b;
	uint8_t reg_ipic_infra_req_mask_b;
	uint8_t reg_ipic_vrf18_req_mask_b;
	uint8_t reg_mcu_apsrc_req_mask_b;
	uint8_t reg_mcu_ddren_req_mask_b;
	uint8_t reg_mcu_emi_req_mask_b;

	/* SPM_SRC_MASK_8 */
	uint8_t reg_mcusys_apsrc_req_mask_b;
	uint8_t reg_mcusys_ddren_req_mask_b;
	uint8_t reg_mcusys_emi_req_mask_b;
	uint8_t reg_mcusys_infra_req_mask_b;

	/* SPM_SRC_MASK_9 */
	uint8_t reg_mcusys_pmic_req_mask_b;
	uint8_t reg_mcusys_srcclkena_mask_b;
	uint8_t reg_mcusys_vrf18_req_mask_b;
	uint8_t reg_md_apsrc_req_mask_b;
	uint8_t reg_md_ddren_req_mask_b;
	uint8_t reg_md_emi_req_mask_b;
	uint8_t reg_md_infra_req_mask_b;
	uint8_t reg_md_pmic_req_mask_b;
	uint8_t reg_md_srcclkena_mask_b;
	uint8_t reg_md_srcclkena1_mask_b;
	uint8_t reg_md_vcore_req_mask_b;

	/* SPM_SRC_MASK_10 */
	uint8_t reg_md_vrf18_req_mask_b;
	uint8_t reg_mdp_apsrc_req_mask_b;
	uint8_t reg_mdp_ddren_req_mask_b;
	uint8_t reg_mm_proc_apsrc_req_mask_b;
	uint8_t reg_mm_proc_ddren_req_mask_b;
	uint8_t reg_mm_proc_emi_req_mask_b;
	uint8_t reg_mm_proc_infra_req_mask_b;
	uint8_t reg_mm_proc_pmic_req_mask_b;
	uint8_t reg_mm_proc_srcclkena_mask_b;
	uint8_t reg_mm_proc_vrf18_req_mask_b;
	uint8_t reg_mmsys_apsrc_req_mask_b;
	uint8_t reg_mmsys_ddren_req_mask_b;
	uint8_t reg_mmsys_vrf18_req_mask_b;
	uint8_t reg_pcie0_apsrc_req_mask_b;
	uint8_t reg_pcie0_ddren_req_mask_b;
	uint8_t reg_pcie0_infra_req_mask_b;
	uint8_t reg_pcie0_srcclkena_mask_b;
	uint8_t reg_pcie0_vrf18_req_mask_b;
	uint8_t reg_pcie1_apsrc_req_mask_b;
	uint8_t reg_pcie1_ddren_req_mask_b;
	uint8_t reg_pcie1_infra_req_mask_b;
	uint8_t reg_pcie1_srcclkena_mask_b;
	uint8_t reg_pcie1_vrf18_req_mask_b;
	uint8_t reg_perisys_apsrc_req_mask_b;
	uint8_t reg_perisys_ddren_req_mask_b;
	uint8_t reg_perisys_emi_req_mask_b;
	uint8_t reg_perisys_infra_req_mask_b;
	uint8_t reg_perisys_pmic_req_mask_b;
	uint8_t reg_perisys_srcclkena_mask_b;
	uint8_t reg_perisys_vcore_req_mask_b;
	uint8_t reg_perisys_vrf18_req_mask_b;
	uint8_t reg_scp_apsrc_req_mask_b;

	/* SPM_SRC_MASK_11 */
	uint8_t reg_scp_ddren_req_mask_b;
	uint8_t reg_scp_emi_req_mask_b;
	uint8_t reg_scp_infra_req_mask_b;
	uint8_t reg_scp_pmic_req_mask_b;
	uint8_t reg_scp_srcclkena_mask_b;
	uint8_t reg_scp_vcore_req_mask_b;
	uint8_t reg_scp_vrf18_req_mask_b;
	uint8_t reg_srcclkeni_infra_req_mask_b;
	uint8_t reg_srcclkeni_pmic_req_mask_b;
	uint8_t reg_srcclkeni_srcclkena_mask_b;
	uint8_t reg_sspm_apsrc_req_mask_b;
	uint8_t reg_sspm_ddren_req_mask_b;
	uint8_t reg_sspm_emi_req_mask_b;
	uint8_t reg_sspm_infra_req_mask_b;
	uint8_t reg_sspm_pmic_req_mask_b;
	uint8_t reg_sspm_srcclkena_mask_b;
	uint8_t reg_sspm_vrf18_req_mask_b;
	uint8_t reg_ssr_apsrc_req_mask_b;
	uint8_t reg_ssr_ddren_req_mask_b;
	uint8_t reg_ssr_emi_req_mask_b;
	uint8_t reg_ssr_infra_req_mask_b;
	uint8_t reg_ssr_pmic_req_mask_b;
	uint8_t reg_ssr_srcclkena_mask_b;
	uint8_t reg_ssr_vrf18_req_mask_b;
	uint8_t reg_ufs_apsrc_req_mask_b;
	uint8_t reg_ufs_ddren_req_mask_b;
	uint8_t reg_ufs_emi_req_mask_b;
	uint8_t reg_ufs_infra_req_mask_b;
	uint8_t reg_ufs_pmic_req_mask_b;

	/* SPM_SRC_MASK_12 */
	uint8_t reg_ufs_srcclkena_mask_b;
	uint8_t reg_ufs_vrf18_req_mask_b;
	uint8_t reg_vdec_apsrc_req_mask_b;
	uint8_t reg_vdec_ddren_req_mask_b;
	uint8_t reg_vdec_emi_req_mask_b;
	uint8_t reg_vdec_infra_req_mask_b;
	uint8_t reg_vdec_pmic_req_mask_b;
	uint8_t reg_vdec_srcclkena_mask_b;
	uint8_t reg_vdec_vrf18_req_mask_b;
	uint8_t reg_venc_apsrc_req_mask_b;
	uint8_t reg_venc_ddren_req_mask_b;
	uint8_t reg_venc_emi_req_mask_b;
	uint8_t reg_venc_infra_req_mask_b;
	uint8_t reg_venc_pmic_req_mask_b;
	uint8_t reg_venc_srcclkena_mask_b;
	uint8_t reg_venc_vrf18_req_mask_b;
	uint8_t reg_ipe_apsrc_req_mask_b;
	uint8_t reg_ipe_ddren_req_mask_b;
	uint8_t reg_ipe_emi_req_mask_b;
	uint8_t reg_ipe_infra_req_mask_b;
	uint8_t reg_ipe_pmic_req_mask_b;
	uint8_t reg_ipe_srcclkena_mask_b;
	uint8_t reg_ipe_vrf18_req_mask_b;
	uint8_t reg_ufs_vcore_req_mask_b;

	/* SPM_EVENT_CON_MISC */
	uint8_t reg_srcclken_fast_resp;
	uint8_t reg_csyspwrup_ack_mask;

	/* SPM_WAKEUP_EVENT_MASK */
	uint32_t reg_wakeup_event_mask;

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	uint32_t reg_ext_wakeup_event_mask;
};

struct mtk_spm_regs {
	uint32_t poweron_config_set;
	uint32_t spm_power_on_val0;
	uint32_t spm_power_on_val1;
	uint32_t spm_power_on_val2;
	uint32_t spm_power_on_val3;
	uint32_t pcm_pwr_io_en;
	uint32_t pcm_con0;
	uint32_t pcm_con1;
	uint32_t spm_sram_sleep_ctrl;
	uint32_t spm_clk_con;
	uint32_t spm_clk_settle;
	uint32_t spm_clk_con1;
	uint32_t  reserved0[4];
	uint32_t spm_sw_rst_con;
	uint32_t spm_sw_rst_con_set;
	uint32_t spm_sw_rst_con_clr;
	uint32_t  reserved1;
	uint32_t spm_sec_read_mask;
	uint32_t spm_one_time_lock_l;
	uint32_t spm_one_time_lock_m;
	uint32_t spm_one_time_lock_h;
	uint32_t  reserved2[9];
	uint32_t sspm_clk_con;
	uint32_t scp_clk_con;
	uint32_t  reserved3;
	uint32_t spm_swint;
	uint32_t spm_swint_set;
	uint32_t spm_swint_clr;
	uint32_t  reserved4[5];
	uint32_t spm_cpu_wakeup_event;
	uint32_t spm_irq_mask;
	uint32_t  reserved5[18];
	uint32_t md32pcm_scu_ctrl0;
	uint32_t md32pcm_scu_ctrl1;
	uint32_t md32pcm_scu_ctrl2;
	uint32_t md32pcm_scu_ctrl3;
	uint32_t md32pcm_scu_sta0;
	uint32_t reserved6[5];
	uint32_t spm_irq_sta;
	uint32_t reserved7;
	uint32_t md32pcm_wakeup_sta;
	uint32_t md32pcm_event_sta;
	uint32_t reserved8[2];
	uint32_t spm_wakeup_misc;
	uint32_t reserved9[8];
	uint32_t spm_ck_sta;
	uint32_t reserved10[10];
	uint32_t md32pcm_sta;
	uint32_t md32pcm_pc;
	uint32_t reserved11[26];
	uint32_t spm_ap_standby_con;
	uint32_t cpu_wfi_en;
	uint32_t cpu_wfi_en_set;
	uint32_t cpu_wfi_en_clr;
	uint32_t ext_int_wakeup_req;
	uint32_t ext_int_wakeup_req_set;
	uint32_t ext_int_wakeup_req_clr;
	uint32_t mcusys_idle_sta;
	uint32_t cpu_pwr_status;
	uint32_t sw2spm_wakeup;
	uint32_t sw2spm_wakeup_set;
	uint32_t sw2spm_wakeup_clr;
	uint32_t sw2spm_mailbox_0;
	uint32_t sw2spm_mailbox_1;
	uint32_t sw2spm_mailbox_2;
	uint32_t sw2spm_mailbox_3;
	uint32_t spm2sw_mailbox_0;
	uint32_t spm2sw_mailbox_1;
	uint32_t spm2sw_mailbox_2;
	uint32_t spm2sw_mailbox_3;
	uint32_t spm2mcupm_con;
	uint32_t reserved12[3];
	uint32_t spm_mcusys_pwr_con;
	uint32_t spm_cputop_pwr_con;
	uint32_t spm_cpu0_pwr_con;
	uint32_t spm_cpu1_pwr_con;
	uint32_t spm_cpu2_pwr_con;
	uint32_t spm_cpu3_pwr_con;
	uint32_t spm_cpu4_pwr_con;
	uint32_t spm_cpu5_pwr_con;
	uint32_t spm_cpu6_pwr_con;
	uint32_t spm_cpu7_pwr_con;
	uint32_t spm_mcupm_spmc_con;
	uint32_t reserved13[5];
	uint32_t spm_dpm_p2p_sta;
	uint32_t spm_dpm_p2p_con;
	uint32_t spm_dpm_intf_sta;
	uint32_t spm_dpm_wb_con;
	uint32_t spm_ack_chk_timer_3;
	uint32_t spm_ack_chk_sta_3;
	uint32_t reserved14[18];
	uint32_t spm_pwrap_con;
	uint32_t spm_pwrap_con_sta;
	uint32_t spm_pmic_spmi_con;
	uint32_t reserved15;
	uint32_t spm_pwrap_cmd0;
	uint32_t spm_pwrap_cmd1;
	uint32_t spm_pwrap_cmd2;
	uint32_t spm_pwrap_cmd3;
	uint32_t spm_pwrap_cmd4;
	uint32_t spm_pwrap_cmd5;
	uint32_t spm_pwrap_cmd6;
	uint32_t spm_pwrap_cmd7;
	uint32_t spm_pwrap_cmd8;
	uint32_t spm_pwrap_cmd9;
	uint32_t spm_pwrap_cmd10;
	uint32_t spm_pwrap_cmd11;
	uint32_t spm_pwrap_cmd12;
	uint32_t spm_pwrap_cmd13;
	uint32_t spm_pwrap_cmd14;
	uint32_t spm_pwrap_cmd15;
	uint32_t spm_pwrap_cmd16;
	uint32_t spm_pwrap_cmd17;
	uint32_t spm_pwrap_cmd18;
	uint32_t spm_pwrap_cmd19;
	uint32_t spm_pwrap_cmd20;
	uint32_t spm_pwrap_cmd21;
	uint32_t spm_pwrap_cmd22;
	uint32_t spm_pwrap_cmd23;
	uint32_t spm_pwrap_cmd24;
	uint32_t spm_pwrap_cmd25;
	uint32_t spm_pwrap_cmd26;
	uint32_t spm_pwrap_cmd27;
	uint32_t spm_pwrap_cmd28;
	uint32_t spm_pwrap_cmd29;
	uint32_t spm_pwrap_cmd30;
	uint32_t spm_pwrap_cmd31;
	uint32_t dvfsrc_event_sta;
	uint32_t spm_force_dvfs;
	uint32_t spm_dvfs_sta;
	uint32_t spm_dvs_dfs_level;
	uint32_t spm_dvfs_level;
	uint32_t spm_dvfs_opp;
	uint32_t spm_ultra_req;
	uint32_t spm_dvfs_con;
	uint32_t spm_sramrc_con;
	uint32_t spm_srclkenrc_con;
	uint32_t spm_dpsw_con;
	uint32_t reserved16[17];
	uint32_t ulposc_con;
	uint32_t ap_mdsrc_req;
	uint32_t spm2md_switch_ctrl;
	uint32_t rc_spm_ctrl;
	uint32_t spm2gpupm_con;
	uint32_t spm2apu_con;
	uint32_t spm2efuse_con;
	uint32_t spm2dfd_con;
	uint32_t rsv_pll_con;
	uint32_t emi_slb_con;
	uint32_t spm_suspend_flag_con;
	uint32_t spm2pmsr_con;
	uint32_t spm_topck_rtff_con;
	uint32_t emi_shf_con;
	uint32_t cirq_byoass_con;
	uint32_t aoc_vcore_sram_con;
	uint32_t reserved17[8];
	uint32_t reg_module_sw_cg_ddren_req_mask_0;
	uint32_t reg_module_sw_cg_ddren_req_mask_1;
	uint32_t reg_module_sw_cg_ddren_req_mask_2;
	uint32_t reg_module_sw_cg_ddren_req_mask_3;
	uint32_t reg_module_sw_cg_vrf18_req_mask_0;
	uint32_t reg_module_sw_cg_vrf18_req_mask_1;
	uint32_t reg_module_sw_cg_vrf18_req_mask_2;
	uint32_t reg_module_sw_cg_vrf18_req_mask_3;
	uint32_t reg_module_sw_cg_infra_req_mask_0;
	uint32_t reg_module_sw_cg_infra_req_mask_1;
	uint32_t reg_module_sw_cg_infra_req_mask_2;
	uint32_t reg_module_sw_cg_infra_req_mask_3;
	uint32_t reg_module_sw_cg_f26m_req_mask_0;
	uint32_t reg_module_sw_cg_f26m_req_mask_1;
	uint32_t reg_module_sw_cg_f26m_req_mask_2;
	uint32_t reg_module_sw_cg_f26m_req_mask_3;
	uint32_t reg_module_sw_cg_vcore_req_mask_0;
	uint32_t reg_module_sw_cg_vcore_req_mask_1;
	uint32_t reg_module_sw_cg_vcore_req_mask_2;
	uint32_t reg_module_sw_cg_vcore_req_mask_3;
	uint32_t reg_pwr_status_ddren_req_mask;
	uint32_t reg_pwr_status_vrf18_req_mask;
	uint32_t reg_pwr_status_infra_req_mask;
	uint32_t reg_pwr_status_f26m_req_mask;
	uint32_t reg_pwr_status_pmic_req_mask;
	uint32_t reg_pwr_status_vcore_req_mask;
	uint32_t reg_pwr_status_msb_ddren_req_mask;
	uint32_t reg_pwr_status_msb_vrf18_req_mask;
	uint32_t reg_pwr_status_msb_infra_req_mask;
	uint32_t reg_pwr_status_msb_f26m_req_mask;
	uint32_t reg_pwr_status_msb_pmic_req_mask;
	uint32_t reg_pwr_status_msb_vcore_req_mask;
	uint32_t reg_module_busy_msb_ddren_req_mask;
	uint32_t reg_module_busy_msb_vrf18_req_mask;
	uint32_t reg_module_busy_msb_infra_req_mask;
	uint32_t reg_module_busy_msb_f26m_req_mask;
	uint32_t reg_module_busy_msb_pmic_req_mask;
	uint32_t reg_module_busy_msb_vcore_req_mask;
	uint32_t reserved18[2];
	uint32_t sys_timer_con;
	uint32_t sys_timer_value_l;
	uint32_t sys_timer_value_h;
	uint32_t sys_timer_start_l;
	uint32_t sys_timer_start_h;
	uint32_t sys_timer_latch_l_00;
	uint32_t sys_timer_latch_h_00;
	uint32_t sys_timer_latch_l_01;
	uint32_t sys_timer_latch_h_01;
	uint32_t sys_timer_latch_l_02;
	uint32_t sys_timer_latch_h_02;
	uint32_t sys_timer_latch_l_03;
	uint32_t sys_timer_latch_h_03;
	uint32_t sys_timer_latch_l_04;
	uint32_t sys_timer_latch_h_04;
	uint32_t sys_timer_latch_l_05;
	uint32_t sys_timer_latch_h_05;
	uint32_t sys_timer_latch_l_06;
	uint32_t sys_timer_latch_h_06;
	uint32_t sys_timer_latch_l_07;
	uint32_t sys_timer_latch_h_07;
	uint32_t sys_timer_latch_l_08;
	uint32_t sys_timer_latch_h_08;
	uint32_t sys_timer_latch_l_09;
	uint32_t sys_timer_latch_h_09;
	uint32_t sys_timer_latch_l_10;
	uint32_t sys_timer_latch_h_10;
	uint32_t sys_timer_latch_l_11;
	uint32_t sys_timer_latch_h_11;
	uint32_t sys_timer_latch_l_12;
	uint32_t sys_timer_latch_h_12;
	uint32_t sys_timer_latch_l_13;
	uint32_t sys_timer_latch_h_13;
	uint32_t sys_timer_latch_l_14;
	uint32_t sys_timer_latch_h_14;
	uint32_t sys_timer_latch_l_15;
	uint32_t sys_timer_latch_h_15;
	uint32_t pcm_timer_val;
	uint32_t pcm_timer_out;
	uint32_t spm_counter_0;
	uint32_t spm_counter_1;
	uint32_t spm_counter_2;
	uint32_t pcm_wdt_val;
	uint32_t pcm_wdt_out;
	uint32_t reserved19[20];
	uint32_t spm_sw_flag_0;
	uint32_t spm_sw_debug_0;
	uint32_t spm_sw_flag_1;
	uint32_t spm_sw_debug_1;
	uint32_t spm_sw_rsv[9];
	uint32_t spm_bk_wake_event;
	uint32_t spm_bk_vtcxo_dur;
	uint32_t spm_bk_wake_misc;
	uint32_t spm_bk_pcm_timer;
	uint32_t reserved20[3];
	uint32_t spm_rsv_con_0;
	uint32_t spm_rsv_con_1;
	uint32_t spm_rsv_sta_0;
	uint32_t spm_rsv_sta_1;
	uint32_t spm_spare_con;
	uint32_t spm_spare_con_set;
	uint32_t spm_spare_con_clr;
	uint32_t spm_cross_wake_m00_req;
	uint32_t spm_cross_wake_m01_req;
	uint32_t spm_cross_wake_m02_req;
	uint32_t spm_cross_wake_m03_req;
	uint32_t scp_vcore_level;
	uint32_t spm_ddren_ack_sel_con;
	uint32_t spm_sw_flag_2;
	uint32_t spm_sw_debug_2;
	uint32_t spm_dv_con_0;
	uint32_t spm_dv_con_1;
	uint32_t reserved21[2];
	uint32_t spm_sema_m0;
	uint32_t spm_sema_m1;
	uint32_t spm_sema_m2;
	uint32_t spm_sema_m3;
	uint32_t spm_sema_m4;
	uint32_t spm_sema_m5;
	uint32_t spm_sema_m6;
	uint32_t spm_sema_m7;
	uint32_t spm2adsp_mailbox;
	uint32_t adsp2spm_mailbox;
	uint32_t vcore_rtff_ctrl_mask_set;
	uint32_t vcore_rtff_ctrl_mask_clr;
	uint32_t spm2pmcu_mailbox_0;
	uint32_t spm2pmcu_mailbox_1;
	uint32_t spm2pmcu_mailbox_2;
	uint32_t spm2pmcu_mailbox_3;
	uint32_t pmcu2spm_mailbox_0;
	uint32_t pmcu2spm_mailbox_1;
	uint32_t pmcu2spm_mailbox_2;
	uint32_t pmcu2spm_mailbox_3;
	uint32_t spm2scp_mailbox;
	uint32_t scp2spm_mailbox;
	uint32_t scp_aov_bus_con;
	uint32_t vcore_rtff_ctrl_mask;
	uint32_t spm_sram_srclkeno_mask;
	uint32_t emi_pdn_req;
	uint32_t emi_busy_req;
	uint32_t emi_reserved_sta;
	uint32_t sc_univpll_div_rst_b;
	uint32_t eco_armpll_div_clock_off;
	uint32_t spm_mcdsr_cg_check_x1;
	uint32_t spm_sodi2_cg_check_x1;
	uint32_t reserved22[57];
	uint32_t spm_wakeup_sta;
	uint32_t spm_wakeup_ext_sta;
	uint32_t spm_wakeup_event_mask;
	uint32_t spm_wakeup_event_ext_mask;
	uint32_t spm_wakeup_event_sens;
	uint32_t spm_wakeup_event_clear;
	uint32_t spm_src_req;
	uint32_t spm_src_mask_0;
	uint32_t spm_src_mask_1;
	uint32_t spm_src_mask_2;
	uint32_t spm_src_mask_3;
	uint32_t spm_src_mask_4;
	uint32_t spm_src_mask_5;
	uint32_t spm_src_mask_6;
	uint32_t spm_src_mask_7;
	uint32_t spm_src_mask_8;
	uint32_t spm_src_mask_9;
	uint32_t spm_src_mask_10;
	uint32_t spm_src_mask_11;
	uint32_t spm_src_mask_12;
	uint32_t src_req_sta_0;
	uint32_t src_req_sta_1;
	uint32_t src_req_sta_2;
	uint32_t src_req_sta_3;
	uint32_t src_req_sta_4;
	uint32_t src_req_sta_5;
	uint32_t src_req_sta_6;
	uint32_t src_req_sta_7;
	uint32_t src_req_sta_8;
	uint32_t src_req_sta_9;
	uint32_t src_req_sta_10;
	uint32_t src_req_sta_11;
	uint32_t src_req_sta_12;
	uint32_t spm_ipc_wakeup_req;
	uint32_t ipc_wakeup_req_mask_sta;
	uint32_t spm_event_con_misc;
	uint32_t ddren_dbc_con;
	uint32_t spm_resource_ack_con0;
	uint32_t spm_resource_ack_con1;
	uint32_t spm_resource_ack_mask0;
	uint32_t spm_resource_ack_mask1;
	uint32_t spm_resource_ack_mask2;
	uint32_t spm_resource_ack_mask3;
	uint32_t spm_resource_ack_mask4;
	uint32_t spm_resource_ack_mask5;
	uint32_t spm_resource_ack_mask6;
	uint32_t spm_event_counter_clear;
	uint32_t spm_vcore_event_count_sta;
	uint32_t spm_pmic_event_count_sta;
	uint32_t spm_srcclkena_event_count_sta;
	uint32_t spm_infra_event_count_sta;
	uint32_t spm_vrf18_event_count_sta;
	uint32_t spm_emi_event_count_sta;
	uint32_t spm_apsrc_event_count_sta;
	uint32_t spm_ddren_event_count_sta;
	uint32_t pcm_wdt_latch_0;
	uint32_t pcm_wdt_latch_1;
	uint32_t pcm_wdt_latch_2;
	uint32_t pcm_wdt_latch_3;
	uint32_t pcm_wdt_latch_4;
	uint32_t pcm_wdt_latch_5;
	uint32_t pcm_wdt_latch_6;
	uint32_t pcm_wdt_latch_7;
	uint32_t pcm_wdt_latch_8;
	uint32_t pcm_wdt_latch_9;
	uint32_t pcm_wdt_latch_10;
	uint32_t pcm_wdt_latch_11;
	uint32_t pcm_wdt_latch_12;
	uint32_t pcm_wdt_latch_13;
	uint32_t pcm_wdt_latch_14;
	uint32_t pcm_wdt_latch_15;
	uint32_t pcm_wdt_latch_16;
	uint32_t pcm_wdt_latch_17;
	uint32_t pcm_wdt_latch_18;
	uint32_t pcm_wdt_latch_19;
	uint32_t pcm_wdt_latch_20;
	uint32_t pcm_wdt_latch_21;
	uint32_t pcm_wdt_latch_22;
	uint32_t pcm_wdt_latch_23;
	uint32_t pcm_wdt_latch_24;
	uint32_t pcm_wdt_latch_25;
	uint32_t pcm_wdt_latch_26;
	uint32_t pcm_wdt_latch_27;
	uint32_t pcm_wdt_latch_28;
	uint32_t pcm_wdt_latch_29;
	uint32_t pcm_wdt_latch_30;
	uint32_t pcm_wdt_latch_31;
	uint32_t pcm_wdt_latch_32;
	uint32_t pcm_wdt_latch_33;
	uint32_t pcm_wdt_latch_34;
	uint32_t pcm_wdt_latch_35;
	uint32_t pcm_wdt_latch_36;
	uint32_t pcm_wdt_latch_37;
	uint32_t pcm_wdt_latch_38;
	uint32_t pcm_wdt_latch_39;
	uint32_t pcm_wdt_latch_40;
	uint32_t pcm_wdt_latch_spare_0;
	uint32_t pcm_wdt_latch_spare_1;
	uint32_t pcm_wdt_latch_spare_2;
	uint32_t pcm_wdt_latch_spare_3;
	uint32_t pcm_wdt_latch_spare_4;
	uint32_t pcm_wdt_latch_spare_5;
	uint32_t pcm_wdt_latch_spare_6;
	uint32_t pcm_wdt_latch_spare_7;
	uint32_t pcm_wdt_latch_spare_8;
	uint32_t pcm_wdt_latch_spare_9;
	uint32_t dramc_gating_err_latch_0;
	uint32_t dramc_gating_err_latch_1;
	uint32_t dramc_gating_err_latch_2;
	uint32_t dramc_gating_err_latch_3;
	uint32_t dramc_gating_err_latch_4;
	uint32_t dramc_gating_err_latch_5;
	uint32_t dramc_gating_err_latch_spare_0;
	uint32_t spm_debug_con;
	uint32_t spm_ack_chk_con_0;
	uint32_t spm_ack_chk_sel_0;
	uint32_t spm_ack_chk_timer_0;
	uint32_t spm_ack_chk_sta_0;
	uint32_t spm_ack_chk_con_1;
	uint32_t spm_ack_chk_sel_1;
	uint32_t spm_ack_chk_timer_1;
	uint32_t spm_ack_chk_sta_1;
	uint32_t spm_ack_chk_con_2;
	uint32_t spm_ack_chk_sel_2;
	uint32_t spm_ack_chk_timer_2;
	uint32_t spm_ack_chk_sta_2;
	uint32_t spm_ack_chk_con_3;
	uint32_t spm_ack_chk_sel_3;
	uint32_t md32pcm_cfgreg_sw_rstn; /* 0xA00 */
	uint32_t reserved24[127];
	uint32_t md32pcm_dma0_src; /* 0xC00 */
	uint32_t md32pcm_dma0_dst;
	uint32_t md32pcm_dma0_wppt;
	uint32_t md32pcm_dma0_wpto;
	uint32_t md32pcm_dma0_count;
	uint32_t md32pcm_dma0_con;
	uint32_t md32pcm_dma0_start; /* 0xC18 */
	uint32_t reserved25[2];
	uint32_t md32pcm_dma0_rlct; /* 0x0C24 */
	uint32_t reserved23[118];
	uint32_t md1_pwr_con; /* 0x0E00 */
	uint32_t conn_pwr_con;
	uint32_t ifr_pwr_con;
	uint32_t peri_pwr_con;
	uint32_t ufs0_pwr_con;
	uint32_t ufs0_phy_pwr_con;
	uint32_t audio_pwr_con;
	uint32_t adsp_top_pwr_con;
	uint32_t adsp_infra_pwr_con;
	uint32_t adsp_ao_pwr_con;
	uint32_t isp_img1_pwr_con;
	uint32_t isp_img2_pwr_con;
	uint32_t isp_ipe_pwr_con;
	uint32_t isp_vcore_pwr_con;
	uint32_t vde0_pwr_con;
	uint32_t vde1_pwr_con;
	uint32_t ven0_pwr_con;
	uint32_t ven1_pwr_con;
	uint32_t cam_main_pwr_con;
	uint32_t cam_mraw_pwr_con;
	uint32_t cam_suba_pwr_con;
	uint32_t cam_subb_pwr_con;
	uint32_t cam_subc_pwr_con;
	uint32_t cam_vcore_pwr_con;
	uint32_t cam_ccu_pwr_con;
	uint32_t cam_ccu_ao_pwr_con;
	uint32_t mdp0_pwr_con;
	uint32_t mdp1_pwr_con;
	uint32_t dis0_pwr_con;
	uint32_t dis1_pwr_con;
	uint32_t mm_infra_pwr_con;
	uint32_t mm_proc_pwr_con;
	uint32_t dp_tx_pwr_con;
	uint32_t scp_core_pwr_con;
	uint32_t scp_peri_pwr_con;
	uint32_t dpm0_pwr_con;
	uint32_t dpm1_pwr_con;
	uint32_t emi0_pwr_con;
	uint32_t emi1_pwr_con;
	uint32_t csi_rx_pwr_con;
	uint32_t ssrsys_pwr_con;
	uint32_t sspm_pwr_con;
	uint32_t ssusb_pwr_con;
	uint32_t ssusb_phy_pwr_con;
	uint32_t cpueb_pwr_con;
	uint32_t mfg0_pwr_con;
	uint32_t mfg1_pwr_con;
	uint32_t mfg2_pwr_con;
	uint32_t mfg3_pwr_con;
	uint32_t mfg4_pwr_con;
	uint32_t mfg5_pwr_con;
	uint32_t mfg6_pwr_con;
	uint32_t mfg7_pwr_con;
	uint32_t adsp_hre_sram_con;
	uint32_t ccu_sleep_sram_con;
	uint32_t efuse_sram_con;
	uint32_t emi_hre_sram_con;
	uint32_t emi_slb_sram_con;
	uint32_t infra_hre_sram_con;
	uint32_t infra_sleep_sram_con;
	uint32_t mm_hre_sram_con;
	uint32_t nth_emi_slb_sram_con;
	uint32_t nth_emi_slb_sram_ack;
	uint32_t peri_sleep_sram_con;
	uint32_t spm_sram_con;
	uint32_t sspm_sram_con;
	uint32_t ssr_sleep_sram_con;
	uint32_t sth_emi_slb_sram_con;
	uint32_t sth_emi_slb_sram_ack;
	uint32_t ufs_pdn_sram_con;
	uint32_t ufs_sleep_sram_con;
	uint32_t unipro_pdn_sram_con;
	uint32_t cpu_buck_iso_con;
	uint32_t md_buck_iso_con;
	uint32_t soc_buck_iso_con;
	uint32_t soc_buck_iso_con_set;
	uint32_t soc_buck_iso_con_clr;
	uint32_t soc_buck_iso_con_2;
	uint32_t soc_buck_iso_con_2_set;
	uint32_t soc_buck_iso_con_2_clr;
	uint32_t pwr_status;
	uint32_t pwr_status_2nd;
	uint32_t pwr_status_msb;
	uint32_t pwr_status_msb_2nd;
	uint32_t xpu_pwr_status;
	uint32_t xpu_pwr_status_2nd;
	uint32_t dfd_soc_pwr_latch;
	uint32_t subsys_pm_bypass;
	uint32_t vadsp_hre_sram_con;
	uint32_t vadsp_hre_sram_ack;
	uint32_t gcpu_sram_con;
	uint32_t gcpu_sram_ack;
	uint32_t edp_tx_pwr_con;
	uint32_t pcie_pwr_con;
	uint32_t pcie_phy_pwr_con;
	uint32_t reserved26;
	uint32_t spm_twam_con;
	uint32_t spm_twam_window_len;
	uint32_t spm_twam_idle_sel;
	uint32_t spm_twam_last_sta0;
	uint32_t spm_twam_last_sta1;
	uint32_t spm_twam_last_sta2;
	uint32_t spm_twam_last_sta3;
	uint32_t spm_twam_curr_sta0;
	uint32_t spm_twam_curr_sta1;
	uint32_t spm_twam_curr_sta2;
	uint32_t spm_twam_curr_sta3;
	uint32_t spm_twam_timer_out;
};

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;
_Alignas(8) extern const unsigned char spm_bin[];
extern const unsigned char spm_bin_end[];
#define spm_bin_len (spm_bin_end - spm_bin)

#endif  /* __SOC_MEDIATEK_MT8189_INCLUDE_SOC_SPM_H__ */
