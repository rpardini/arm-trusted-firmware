// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#include <common/debug.h>
#include <platform_def.h>
#include <drivers/delay_timer.h>
#include <pll.h>
#include "include/spm_mtcmos.h"
#include "include/spm_mtcmos_internal.h"

#define POWERON_CONFIG_EN	(SPM_BASE + 0x0000)

/* Define MTCMOS power control */
#define PWR_RST_B                        (0x1 << 0)
#define PWR_ISO                          (0x1 << 1)
#define PWR_ON                           (0x1 << 2)
#define PWR_ON_2ND                       (0x1 << 3)
#define PWR_CLK_DIS                      (0x1 << 4)
#define SRAM_CKISO                       (0x1 << 5)
#define SRAM_ISOINT_B                    (0x1 << 6)
#define PWR_RTFF_SAVE_FLAG               (0x1 << 27)
#define PWR_ACK                          (0x1 << 30)
#define PWR_ACK_2ND                      (0x1U << 31)

/* Define MTCMOS Bus Protect Mask */

#define AUDIO_PROT_STEP1_0_MASK	((0x1 << 6))
#define AUDIO_PROT_STEP1_0_ACK_MASK	((0x1 << 6))
#define CAM_MAIN_PROT_STEP1_0_MASK	((0x1u << 30) \
			| (0x1u << 31))
#define CAM_MAIN_PROT_STEP1_0_ACK_MASK	((0x1u << 30) \
			| (0x1u << 31))
#define CAM_MAIN_PROT_STEP2_0_MASK	((0x1 << 9) \
			| (0x1 << 10))
#define CAM_MAIN_PROT_STEP2_0_ACK_MASK	((0x1 << 9) \
			| (0x1 << 10))
#define CONN_PROT_STEP1_0_MASK	((0x1 << 1))
#define CONN_PROT_STEP1_0_ACK_MASK	((0x1 << 1))
#define CONN_PROT_STEP1_1_MASK	((0x1 << 12))
#define CONN_PROT_STEP1_1_ACK_MASK	((0x1 << 12))
#define CONN_PROT_STEP2_0_MASK	((0x1 << 0))
#define CONN_PROT_STEP2_0_ACK_MASK	((0x1 << 0))
#define CONN_PROT_STEP2_1_MASK	((0x1 << 8))
#define CONN_PROT_STEP2_1_ACK_MASK	((0x1 << 8))
#define DIS0_PROT_STEP1_0_MASK	((0x1 << 0) \
			| (0x1 << 1))
#define DIS0_PROT_STEP1_0_ACK_MASK	((0x1 << 0) \
			| (0x1 << 1))
#define ISP_IMG1_PROT_STEP1_0_MASK	((0x1 << 3))
#define ISP_IMG1_PROT_STEP1_0_ACK_MASK	((0x1 << 3))
#define ISP_IMG1_PROT_STEP2_0_MASK	((0x1 << 7))
#define ISP_IMG1_PROT_STEP2_0_ACK_MASK	((0x1 << 7))
#define ISP_IPE_PROT_STEP1_0_MASK	((0x1 << 2))
#define ISP_IPE_PROT_STEP1_0_ACK_MASK	((0x1 << 2))
#define ISP_IPE_PROT_STEP2_0_MASK	((0x1 << 8))
#define ISP_IPE_PROT_STEP2_0_ACK_MASK	((0x1 << 8))
#define MDP0_PROT_STEP1_0_MASK	((0x1 << 18))
#define MDP0_PROT_STEP1_0_ACK_MASK	((0x1 << 18))
#define MFG1_PROT_STEP1_0_MASK	((0x1 << 20))
#define MFG1_PROT_STEP1_0_ACK_MASK	((0x1 << 20))
#define MFG1_PROT_STEP2_0_MASK	((0x1 << 0) \
			| (0x1 << 2))
#define MFG1_PROT_STEP2_0_ACK_MASK	((0x1 << 0) \
			| (0x1 << 2))
#define MFG1_PROT_STEP3_0_MASK	((0x1 << 4))
#define MFG1_PROT_STEP3_0_ACK_MASK	((0x1 << 4))
#define MFG1_PROT_STEP4_0_MASK	((0x1 << 4) \
			| (0x1 << 5))
#define MFG1_PROT_STEP4_0_ACK_MASK	((0x1 << 4) \
			| (0x1 << 5))
#define MM_INFRA_PROT_STEP1_0_MASK	((0x1 << 1) \
			| (0x1 << 2) \
			| (0x1 << 3))
#define MM_INFRA_PROT_STEP1_0_ACK_MASK	((0x1 << 1) \
			| (0x1 << 2) \
			| (0x1 << 3))
#define MM_INFRA_PROT_STEP1_1_MASK	((0x1 << 11))
#define MM_INFRA_PROT_STEP1_1_ACK_MASK	((0x1 << 11))
#define MM_INFRA_PROT_STEP2_0_MASK	((0x1 << 0) \
			| (0x1 << 7) \
			| (0x1 << 8) \
			| (0x1 << 9) \
			| (0x1 << 10) \
			| (0x1 << 11) \
			| (0x1 << 12) \
			| (0x1 << 13) \
			| (0x1 << 14) \
			| (0x1 << 15))
#define MM_INFRA_PROT_STEP2_0_ACK_MASK	((0x1 << 0) \
			| (0x1 << 7) \
			| (0x1 << 8) \
			| (0x1 << 9) \
			| (0x1 << 10) \
			| (0x1 << 11) \
			| (0x1 << 12) \
			| (0x1 << 13) \
			| (0x1 << 14) \
			| (0x1 << 15))
#define MM_INFRA_PROT_STEP2_1_MASK	((0x1 << 16))
#define MM_INFRA_PROT_STEP2_1_ACK_MASK	((0x1 << 16))
#define MM_INFRA_PROT_STEP2_2_MASK	((0x1 << 20) \
			| (0x1 << 21))
#define MM_INFRA_PROT_STEP2_2_ACK_MASK	((0x1 << 20) \
			| (0x1 << 21))
#define SSUSB_PROT_STEP1_0_MASK	((0x1 << 7))
#define SSUSB_PROT_STEP1_0_ACK_MASK	((0x1 << 7))
#define UFS0_PROT_STEP1_0_MASK	((0x1 << 5))
#define UFS0_PROT_STEP1_0_ACK_MASK	((0x1 << 5))
#define UFS0_PROT_STEP2_0_MASK	((0x1 << 4))
#define UFS0_PROT_STEP2_0_ACK_MASK	((0x1 << 4))
#define UFS0_PROT_STEP3_0_MASK	((0x1 << 6))
#define UFS0_PROT_STEP3_0_ACK_MASK	((0x1 << 6))
#define VDE0_PROT_STEP1_0_MASK	((0x1 << 20))
#define VDE0_PROT_STEP1_0_ACK_MASK	((0x1 << 20))
#define VDE0_PROT_STEP2_0_MASK	((0x1 << 13))
#define VDE0_PROT_STEP2_0_ACK_MASK	((0x1 << 13))
#define VEN0_PROT_STEP1_0_MASK	((0x1 << 12))
#define VEN0_PROT_STEP1_0_ACK_MASK	((0x1 << 12))
#define VEN0_PROT_STEP2_0_MASK	((0x1 << 12))
#define VEN0_PROT_STEP2_0_ACK_MASK	((0x1 << 12))

/* Define MTCMOS Power Status Mask */

#define ADSP_AO_PWR_STA_MASK			(0x1 << 9)
#define ADSP_INFRA_PWR_STA_MASK			(0x1 << 8)
#define ADSP_TOP_PWR_STA_MASK			(0x1 << 7)
#define AUDIO_PWR_STA_MASK			(0x1 << 6)
#define CAM_MAIN_PWR_STA_MASK			(0x1 << 18)
#define CAM_SUBA_PWR_STA_MASK			(0x1 << 20)
#define CAM_SUBB_PWR_STA_MASK			(0x1 << 21)
#define CONN_PWR_STA_MASK			(0x1 << 1)
#define CSI_RX_PWR_STA_MASK			(0x1 << 39)
#define DIS0_PWR_STA_MASK			(0x1 << 28)
#define DP_TX_PWR_STA_MASK			(0x1 << 32)
#define EDP_TX_PWR_STA_MASK			(0x1 << 44)
#define ISP_IMG1_PWR_STA_MASK			(0x1 << 10)
#define ISP_IMG2_PWR_STA_MASK			(0x1 << 11)
#define ISP_IPE_PWR_STA_MASK			(0x1 << 12)
#define MDP0_PWR_STA_MASK			(0x1 << 26)
#define MFG0_PWR_STA_MASK			(0x1 << 1)
#define MFG1_PWR_STA_MASK			(0x1 << 2)
#define MFG2_PWR_STA_MASK			(0x1 << 3)
#define MFG3_PWR_STA_MASK			(0x1 << 4)
#define MM_INFRA_PWR_STA_MASK			(0x1 << 30)
#define PCIE_PHY_PWR_STA_MASK			(0x1 << 46)
#define PCIE_PWR_STA_MASK			(0x1 << 45)
#define SSUSB_PWR_STA_MASK			(0x1 << 42)
#define UFS0_PHY_PWR_STA_MASK			(0x1 << 5)
#define UFS0_PWR_STA_MASK			(0x1 << 4)
#define VDE0_PWR_STA_MASK			(0x1 << 14)
#define VEN0_PWR_STA_MASK			(0x1 << 16)

/* Define Non-CPU SRAM Mask */

#define ADSP_TOP_SRAM_PDN		(0x1 << 8)
#define ADSP_TOP_SRAM_PDN_ACK		(0x1 << 12)
#define ADSP_TOP_SRAM_SLP_B		(0x1 << 9)
#define ADSP_TOP_SRAM_SLP_B_ACK		(0x1 << 13)
#define AUDIO_SRAM_PDN		(0x1 << 8)
#define AUDIO_SRAM_PDN_ACK		(0x1 << 12)
#define CAM_MAIN_SRAM_PDN		(0x1 << 8)
#define CAM_MAIN_SRAM_PDN_ACK		(0x1 << 12)
#define CAM_SUBA_SRAM_PDN		(0x1 << 8)
#define CAM_SUBA_SRAM_PDN_ACK		(0x1 << 12)
#define CAM_SUBB_SRAM_PDN		(0x1 << 8)
#define CAM_SUBB_SRAM_PDN_ACK		(0x1 << 12)
#define CSI_RX_SRAM_PDN		(0x1 << 8)
#define CSI_RX_SRAM_PDN_ACK		(0x1 << 12)
#define DIS0_SRAM_PDN		(0x1 << 8)
#define DIS0_SRAM_PDN_ACK		(0x1 << 12)
#define DP_TX_SRAM_PDN		(0x1 << 8)
#define DP_TX_SRAM_PDN_ACK		(0x1 << 12)
#define EDP_TX_SRAM_PDN		(0x1 << 8)
#define EDP_TX_SRAM_PDN_ACK		(0x1 << 12)
#define EDP_TX_SRAM_SLP_B		(0x1 << 9)
#define EDP_TX_SRAM_SLP_B_ACK		(0x1 << 13)
#define ISP_IMG1_SRAM_PDN		(0x1 << 8)
#define ISP_IMG1_SRAM_PDN_ACK		(0x1 << 12)
#define ISP_IMG2_SRAM_PDN		(0x1 << 8)
#define ISP_IMG2_SRAM_PDN_ACK		(0x1 << 12)
#define ISP_IPE_SRAM_PDN		(0x1 << 8)
#define ISP_IPE_SRAM_PDN_ACK		(0x1 << 12)
#define MDP0_SRAM_PDN		(0x1 << 8)
#define MDP0_SRAM_PDN_ACK		(0x1 << 12)
#define MFG1_SRAM_PDN		(0x1 << 8)
#define MFG1_SRAM_PDN_ACK		(0x1 << 12)
#define MFG2_SRAM_PDN		(0x1 << 8)
#define MFG2_SRAM_PDN_ACK		(0x1 << 12)
#define MFG3_SRAM_PDN		(0x1 << 8)
#define MFG3_SRAM_PDN_ACK		(0x1 << 12)
#define MM_INFRA_SRAM_PDN		(0x1 << 8)
#define MM_INFRA_SRAM_PDN_ACK		(0x1 << 12)
#define PCIE_SRAM_PDN		(0x1 << 8)
#define PCIE_SRAM_PDN_ACK		(0x1 << 12)
#define SSUSB_SRAM_PDN		(0x1 << 8)
#define SSUSB_SRAM_PDN_ACK		(0x1 << 12)
#define UFS0_SRAM_PDN		(0x1 << 8)
#define UFS0_SRAM_PDN_ACK		(0x1 << 12)
#define VDE0_SRAM_PDN		(0x1 << 8)
#define VDE0_SRAM_PDN_ACK		(0x1 << 12)
#define VEN0_SRAM_PDN		(0x1 << 8)
#define VEN0_SRAM_PDN_ACK		(0x1 << 12)

enum {
	DBG_ID_CONN =		0,
	DBG_ID_UFS0 =		1,
	DBG_ID_UFS0_PHY =		2,
	DBG_ID_AUDIO =		3,
	DBG_ID_ADSP_TOP_SHUTDOWN =		4,
	DBG_ID_ADSP_TOP_DORMANT =		5,
	DBG_ID_ADSP_INFRA =		6,
	DBG_ID_ADSP_AO =		7,
	DBG_ID_ISP_IMG1 =		8,
	DBG_ID_ISP_IMG2 =		9,
	DBG_ID_ISP_IPE =		10,
	DBG_ID_VDE0 =		11,
	DBG_ID_VEN0 =		12,
	DBG_ID_CAM_MAIN =		13,
	DBG_ID_CAM_SUBA =		14,
	DBG_ID_CAM_SUBB =		15,
	DBG_ID_MDP0 =		16,
	DBG_ID_DISP =		17,
	DBG_ID_MM_INFRA =		18,
	DBG_ID_DP_TX =		19,
	DBG_ID_CSI_RX =		20,
	DBG_ID_SSUSB =		21,
	DBG_ID_MFG0 =		22,
	DBG_ID_MFG1 =		23,
	DBG_ID_MFG2 =		24,
	DBG_ID_MFG3 =		25,
	DBG_ID_EDP_TX_SHUTDOWN =		26,
	DBG_ID_EDP_TX_DORMANT =		27,
	DBG_ID_PCIE =		28,
	DBG_ID_PCIE_PHY =		29,
	DBG_ID_NUM =		30,
};

#define INCREASE_STEPS \
	do { \
		DBG_STEP++; \
		loop_cnt = 0; \
	} while (0)

static int DBG_ID;
static int DBG_STA;
static int DBG_STEP;
static unsigned int loop_cnt = 0;

#ifndef IGNORE_MTCMOS_CHECK

static void ram_console_update(void)
{
	if (loop_cnt > 10000) {
		WARN( "%s %d MTCMOS hang at %s flow step %d\n",
				"[clkmgr]",
				DBG_ID,
				DBG_STA ? "pwron":"pdn",
				DBG_STEP);
	}

	loop_cnt++;
}

#endif

/**************************************
 * for non-CPU MTCMOS
 **************************************/
void spm_mtcmos_ctrl_conn(int state)
{
	DBG_ID = DBG_ID_CONN;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MCU_CONNSYS_PROTECT_EN_STA_0_SET, CONN_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MCU_CONNSYS_PROTECT_RDY_STA_0) & CONN_PROT_STEP1_0_ACK_MASK) != CONN_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_INFRASYS_PROTECT_EN_STA_1_SET, CONN_PROT_STEP1_1_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_INFRASYS_PROTECT_RDY_STA_1) & CONN_PROT_STEP1_1_ACK_MASK) != CONN_PROT_STEP1_1_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MCU_CONNSYS_PROTECT_EN_STA_0_SET, CONN_PROT_STEP2_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MCU_CONNSYS_PROTECT_RDY_STA_0) & CONN_PROT_STEP2_0_ACK_MASK) != CONN_PROT_STEP2_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_INFRASYS_PROTECT_EN_STA_0_SET, CONN_PROT_STEP2_1_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_INFRASYS_PROTECT_RDY_STA_0) & CONN_PROT_STEP2_1_ACK_MASK) != CONN_PROT_STEP2_1_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(CONN_PWR_CON, spm_read(CONN_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(CONN_PWR_CON, spm_read(CONN_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(CONN_PWR_CON, spm_read(CONN_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(CONN_PWR_CON, spm_read(CONN_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(CONN_PWR_CON, spm_read(CONN_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(CONN_PWR_CON) & PWR_ACK)
				|| (spm_read(CONN_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(CONN_PWR_CON, spm_read(CONN_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(CONN_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(CONN_PWR_CON, spm_read(CONN_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(CONN_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(CONN_PWR_CON, spm_read(CONN_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(CONN_PWR_CON, spm_read(CONN_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(CONN_PWR_CON, spm_read(CONN_PWR_CON) | PWR_RST_B);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_INFRASYS_PROTECT_EN_STA_0_CLR, CONN_PROT_STEP2_1_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MCU_CONNSYS_PROTECT_EN_STA_0_CLR, CONN_PROT_STEP2_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_INFRASYS_PROTECT_EN_STA_1_CLR, CONN_PROT_STEP1_1_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MCU_CONNSYS_PROTECT_EN_STA_0_CLR, CONN_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_ufs0(int state)
{
	DBG_ID = DBG_ID_UFS0;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(VLPCFG_REG_BUS_VLP_TOPAXI_PROTECTEN_SET, UFS0_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(VLPCFG_REG_BUS_VLP_TOPAXI_PROTECTEN_STA1) & UFS0_PROT_STEP1_0_ACK_MASK) != UFS0_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_PERISYS_PROTECT_EN_STA_0_SET, UFS0_PROT_STEP2_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_PERISYS_PROTECT_RDY_STA_0) & UFS0_PROT_STEP2_0_ACK_MASK) != UFS0_PROT_STEP2_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(VLPCFG_REG_BUS_VLP_TOPAXI_PROTECTEN_SET, UFS0_PROT_STEP3_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(VLPCFG_REG_BUS_VLP_TOPAXI_PROTECTEN_STA1) & UFS0_PROT_STEP3_0_ACK_MASK) != UFS0_PROT_STEP3_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set UFS0_SRAM_PDN = 1" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) | UFS0_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until UFS0_SRAM_PDN_ACK = 1" */
		while ((spm_read(UFS0_PWR_CON) & UFS0_SRAM_PDN_ACK) != UFS0_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(UFS0_PWR_CON) & PWR_ACK)
				|| (spm_read(UFS0_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(UFS0_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(UFS0_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) | PWR_RST_B);
		/* TINFO="Set UFS0_SRAM_PDN = 0" */
		spm_write(UFS0_PWR_CON, spm_read(UFS0_PWR_CON) & ~UFS0_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until UFS0_SRAM_PDN_ACK = 0" */
		while (spm_read(UFS0_PWR_CON) & UFS0_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(VLPCFG_REG_BUS_VLP_TOPAXI_PROTECTEN_CLR, UFS0_PROT_STEP3_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_PERISYS_PROTECT_EN_STA_0_CLR, UFS0_PROT_STEP2_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(VLPCFG_REG_BUS_VLP_TOPAXI_PROTECTEN_CLR, UFS0_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_ufs0_phy(int state)
{
	DBG_ID = DBG_ID_UFS0_PHY;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(UFS0_PHY_PWR_CON, spm_read(UFS0_PHY_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(UFS0_PHY_PWR_CON, spm_read(UFS0_PHY_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(UFS0_PHY_PWR_CON, spm_read(UFS0_PHY_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(UFS0_PHY_PWR_CON, spm_read(UFS0_PHY_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(UFS0_PHY_PWR_CON, spm_read(UFS0_PHY_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(UFS0_PHY_PWR_CON) & PWR_ACK)
				|| (spm_read(UFS0_PHY_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(UFS0_PHY_PWR_CON, spm_read(UFS0_PHY_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(UFS0_PHY_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(UFS0_PHY_PWR_CON, spm_read(UFS0_PHY_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(UFS0_PHY_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(UFS0_PHY_PWR_CON, spm_read(UFS0_PHY_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(UFS0_PHY_PWR_CON, spm_read(UFS0_PHY_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(UFS0_PHY_PWR_CON, spm_read(UFS0_PHY_PWR_CON) | PWR_RST_B);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_audio(int state)
{
	DBG_ID = DBG_ID_AUDIO;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_PERISYS_PROTECT_EN_STA_0_SET, AUDIO_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_PERISYS_PROTECT_RDY_STA_0) & AUDIO_PROT_STEP1_0_ACK_MASK) != AUDIO_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set AUDIO_SRAM_PDN = 1" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) | AUDIO_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until AUDIO_SRAM_PDN_ACK = 1" */
		while ((spm_read(AUDIO_PWR_CON) & AUDIO_SRAM_PDN_ACK) != AUDIO_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(AUDIO_PWR_CON) & PWR_ACK)
				|| (spm_read(AUDIO_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(AUDIO_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(AUDIO_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) | PWR_RST_B);
		/* TINFO="Set AUDIO_SRAM_PDN = 0" */
		spm_write(AUDIO_PWR_CON, spm_read(AUDIO_PWR_CON) & ~AUDIO_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until AUDIO_SRAM_PDN_ACK = 0" */
		while (spm_read(AUDIO_PWR_CON) & AUDIO_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_PERISYS_PROTECT_EN_STA_0_CLR, AUDIO_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_adsp_top_shutdown(int state)
{
	DBG_ID = DBG_ID_ADSP_TOP_SHUTDOWN;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set SRAM_CKISO = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | SRAM_CKISO);
		/* TINFO="Set SRAM_ISOINT_B = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~SRAM_ISOINT_B);
		/* TINFO="Set ADSP_TOP_SRAM_PDN = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | ADSP_TOP_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ADSP_TOP_SRAM_PDN_ACK = 1" */
		while ((spm_read(ADSP_TOP_PWR_CON) & ADSP_TOP_SRAM_PDN_ACK) != ADSP_TOP_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(ADSP_TOP_PWR_CON) & PWR_ACK)
				|| (spm_read(ADSP_TOP_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(ADSP_TOP_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(ADSP_TOP_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set ~PWR_CLK_DIS = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | PWR_RST_B);
		/* TINFO="Set ADSP_TOP_SRAM_PDN = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~ADSP_TOP_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ADSP_TOP_SRAM_PDN_ACK = 0" */
		while (spm_read(ADSP_TOP_PWR_CON) & ADSP_TOP_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set SRAM_ISOINT_B = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | SRAM_ISOINT_B);
		/* TINFO="Set SRAM_CKISO = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~SRAM_CKISO);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_adsp_top_dormant(int state)
{
	DBG_ID = DBG_ID_ADSP_TOP_DORMANT;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set SRAM_CKISO = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | SRAM_CKISO);
		/* TINFO="Set SRAM_ISOINT_B = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~SRAM_ISOINT_B);
		/* TINFO="Set ADSP_TOP_SRAM_SLP_B = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~ADSP_TOP_SRAM_SLP_B);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ADSP_TOP_SRAM_SLP_B_ACK = 0" */
		while (spm_read(ADSP_TOP_PWR_CON) & ADSP_TOP_SRAM_SLP_B_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(ADSP_TOP_PWR_CON) & PWR_ACK)
				|| (spm_read(ADSP_TOP_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(ADSP_TOP_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(ADSP_TOP_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | PWR_RST_B);
		/* TINFO="Set ADSP_TOP_SRAM_SLP_B = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | ADSP_TOP_SRAM_SLP_B);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ADSP_TOP_SRAM_SLP_B_ACK = 1" */
		while ((spm_read(ADSP_TOP_PWR_CON) & ADSP_TOP_SRAM_SLP_B_ACK) != ADSP_TOP_SRAM_SLP_B_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set SRAM_ISOINT_B = 1" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) | SRAM_ISOINT_B);
		/* TINFO="Set SRAM_CKISO = 0" */
		spm_write(ADSP_TOP_PWR_CON, spm_read(ADSP_TOP_PWR_CON) & ~SRAM_CKISO);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_adsp_infra(int state)
{
	DBG_ID = DBG_ID_ADSP_INFRA;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(ADSP_INFRA_PWR_CON, spm_read(ADSP_INFRA_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(ADSP_INFRA_PWR_CON, spm_read(ADSP_INFRA_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(ADSP_INFRA_PWR_CON, spm_read(ADSP_INFRA_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(ADSP_INFRA_PWR_CON, spm_read(ADSP_INFRA_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(ADSP_INFRA_PWR_CON, spm_read(ADSP_INFRA_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(ADSP_INFRA_PWR_CON) & PWR_ACK)
				|| (spm_read(ADSP_INFRA_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(ADSP_INFRA_PWR_CON, spm_read(ADSP_INFRA_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(ADSP_INFRA_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(ADSP_INFRA_PWR_CON, spm_read(ADSP_INFRA_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(ADSP_INFRA_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(ADSP_INFRA_PWR_CON, spm_read(ADSP_INFRA_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(ADSP_INFRA_PWR_CON, spm_read(ADSP_INFRA_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(ADSP_INFRA_PWR_CON, spm_read(ADSP_INFRA_PWR_CON) | PWR_RST_B);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_adsp_ao(int state)
{
	DBG_ID = DBG_ID_ADSP_AO;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(ADSP_AO_PWR_CON, spm_read(ADSP_AO_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(ADSP_AO_PWR_CON, spm_read(ADSP_AO_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(ADSP_AO_PWR_CON, spm_read(ADSP_AO_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(ADSP_AO_PWR_CON, spm_read(ADSP_AO_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(ADSP_AO_PWR_CON, spm_read(ADSP_AO_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(ADSP_AO_PWR_CON) & PWR_ACK)
				|| (spm_read(ADSP_AO_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(ADSP_AO_PWR_CON, spm_read(ADSP_AO_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(ADSP_AO_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(ADSP_AO_PWR_CON, spm_read(ADSP_AO_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(ADSP_AO_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(ADSP_AO_PWR_CON, spm_read(ADSP_AO_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(ADSP_AO_PWR_CON, spm_read(ADSP_AO_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(ADSP_AO_PWR_CON, spm_read(ADSP_AO_PWR_CON) | PWR_RST_B);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_isp_img1(int state)
{
	DBG_ID = DBG_ID_ISP_IMG1;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_SET, ISP_IMG1_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_0) & ISP_IMG1_PROT_STEP1_0_ACK_MASK) != ISP_IMG1_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_SET, ISP_IMG1_PROT_STEP2_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_1) & ISP_IMG1_PROT_STEP2_0_ACK_MASK) != ISP_IMG1_PROT_STEP2_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set ISP_IMG1_SRAM_PDN = 1" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) | ISP_IMG1_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ISP_IMG1_SRAM_PDN_ACK = 1" */
		while ((spm_read(ISP_IMG1_PWR_CON) & ISP_IMG1_SRAM_PDN_ACK) != ISP_IMG1_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(ISP_IMG1_PWR_CON) & PWR_ACK)
				|| (spm_read(ISP_IMG1_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(ISP_IMG1_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(ISP_IMG1_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) | PWR_RST_B);
		/* TINFO="Set ISP_IMG1_SRAM_PDN = 0" */
		spm_write(ISP_IMG1_PWR_CON, spm_read(ISP_IMG1_PWR_CON) & ~ISP_IMG1_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ISP_IMG1_SRAM_PDN_ACK = 0" */
		while (spm_read(ISP_IMG1_PWR_CON) & ISP_IMG1_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_CLR, ISP_IMG1_PROT_STEP2_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_CLR, ISP_IMG1_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_isp_img2(int state)
{
	DBG_ID = DBG_ID_ISP_IMG2;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set ISP_IMG2_SRAM_PDN = 1" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) | ISP_IMG2_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ISP_IMG2_SRAM_PDN_ACK = 1" */
		while ((spm_read(ISP_IMG2_PWR_CON) & ISP_IMG2_SRAM_PDN_ACK) != ISP_IMG2_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(ISP_IMG2_PWR_CON) & PWR_ACK)
				|| (spm_read(ISP_IMG2_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(ISP_IMG2_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(ISP_IMG2_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) | PWR_RST_B);
		/* TINFO="Set ISP_IMG2_SRAM_PDN = 0" */
		spm_write(ISP_IMG2_PWR_CON, spm_read(ISP_IMG2_PWR_CON) & ~ISP_IMG2_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ISP_IMG2_SRAM_PDN_ACK = 0" */
		while (spm_read(ISP_IMG2_PWR_CON) & ISP_IMG2_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_isp_ipe(int state)
{
	DBG_ID = DBG_ID_ISP_IPE;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_SET, ISP_IPE_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_0) & ISP_IPE_PROT_STEP1_0_ACK_MASK) != ISP_IPE_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_SET, ISP_IPE_PROT_STEP2_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_1) & ISP_IPE_PROT_STEP2_0_ACK_MASK) != ISP_IPE_PROT_STEP2_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set ISP_IPE_SRAM_PDN = 1" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) | ISP_IPE_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ISP_IPE_SRAM_PDN_ACK = 1" */
		while ((spm_read(ISP_IPE_PWR_CON) & ISP_IPE_SRAM_PDN_ACK) != ISP_IPE_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(ISP_IPE_PWR_CON) & PWR_ACK)
				|| (spm_read(ISP_IPE_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(ISP_IPE_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(ISP_IPE_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) | PWR_RST_B);
		/* TINFO="Set ISP_IPE_SRAM_PDN = 0" */
		spm_write(ISP_IPE_PWR_CON, spm_read(ISP_IPE_PWR_CON) & ~ISP_IPE_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until ISP_IPE_SRAM_PDN_ACK = 0" */
		while (spm_read(ISP_IPE_PWR_CON) & ISP_IPE_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_CLR, ISP_IPE_PROT_STEP2_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_CLR, ISP_IPE_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_vde0(int state)
{
	DBG_ID = DBG_ID_VDE0;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_SET, VDE0_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_0) & VDE0_PROT_STEP1_0_ACK_MASK) != VDE0_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_SET, VDE0_PROT_STEP2_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_1) & VDE0_PROT_STEP2_0_ACK_MASK) != VDE0_PROT_STEP2_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set VDE0_SRAM_PDN = 1" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) | VDE0_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until VDE0_SRAM_PDN_ACK = 1" */
		while ((spm_read(VDE0_PWR_CON) & VDE0_SRAM_PDN_ACK) != VDE0_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(VDE0_PWR_CON) & PWR_ACK)
				|| (spm_read(VDE0_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(VDE0_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(VDE0_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) | PWR_RST_B);
		/* TINFO="Set VDE0_SRAM_PDN = 0" */
		spm_write(VDE0_PWR_CON, spm_read(VDE0_PWR_CON) & ~VDE0_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until VDE0_SRAM_PDN_ACK = 0" */
		while (spm_read(VDE0_PWR_CON) & VDE0_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_CLR, VDE0_PROT_STEP2_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_CLR, VDE0_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_ven0(int state)
{
	DBG_ID = DBG_ID_VEN0;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_SET, VEN0_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_0) & VEN0_PROT_STEP1_0_ACK_MASK) != VEN0_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_SET, VEN0_PROT_STEP2_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_1) & VEN0_PROT_STEP2_0_ACK_MASK) != VEN0_PROT_STEP2_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set VEN0_SRAM_PDN = 1" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) | VEN0_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until VEN0_SRAM_PDN_ACK = 1" */
		while ((spm_read(VEN0_PWR_CON) & VEN0_SRAM_PDN_ACK) != VEN0_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(VEN0_PWR_CON) & PWR_ACK)
				|| (spm_read(VEN0_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(VEN0_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(VEN0_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) | PWR_RST_B);
		/* TINFO="Set VEN0_SRAM_PDN = 0" */
		spm_write(VEN0_PWR_CON, spm_read(VEN0_PWR_CON) & ~VEN0_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until VEN0_SRAM_PDN_ACK = 0" */
		while (spm_read(VEN0_PWR_CON) & VEN0_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_CLR, VEN0_PROT_STEP2_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_CLR, VEN0_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_cam_main(int state)
{
	DBG_ID = DBG_ID_CAM_MAIN;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_SET, CAM_MAIN_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_0) & CAM_MAIN_PROT_STEP1_0_ACK_MASK) != CAM_MAIN_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_SET, CAM_MAIN_PROT_STEP2_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_1) & CAM_MAIN_PROT_STEP2_0_ACK_MASK) != CAM_MAIN_PROT_STEP2_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set CAM_MAIN_SRAM_PDN = 1" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) | CAM_MAIN_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until CAM_MAIN_SRAM_PDN_ACK = 1" */
		while ((spm_read(CAM_MAIN_PWR_CON) & CAM_MAIN_SRAM_PDN_ACK) != CAM_MAIN_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(CAM_MAIN_PWR_CON) & PWR_ACK)
				|| (spm_read(CAM_MAIN_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(CAM_MAIN_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(CAM_MAIN_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) | PWR_RST_B);
		/* TINFO="Set CAM_MAIN_SRAM_PDN = 0" */
		spm_write(CAM_MAIN_PWR_CON, spm_read(CAM_MAIN_PWR_CON) & ~CAM_MAIN_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until CAM_MAIN_SRAM_PDN_ACK = 0" */
		while (spm_read(CAM_MAIN_PWR_CON) & CAM_MAIN_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_CLR, CAM_MAIN_PROT_STEP2_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_CLR, CAM_MAIN_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_cam_suba(int state)
{
	DBG_ID = DBG_ID_CAM_SUBA;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set CAM_SUBA_SRAM_PDN = 1" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) | CAM_SUBA_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until CAM_SUBA_SRAM_PDN_ACK = 1" */
		while ((spm_read(CAM_SUBA_PWR_CON) & CAM_SUBA_SRAM_PDN_ACK) != CAM_SUBA_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(CAM_SUBA_PWR_CON) & PWR_ACK)
				|| (spm_read(CAM_SUBA_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(CAM_SUBA_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(CAM_SUBA_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) | PWR_RST_B);
		/* TINFO="Set CAM_SUBA_SRAM_PDN = 0" */
		spm_write(CAM_SUBA_PWR_CON, spm_read(CAM_SUBA_PWR_CON) & ~CAM_SUBA_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until CAM_SUBA_SRAM_PDN_ACK = 0" */
		while (spm_read(CAM_SUBA_PWR_CON) & CAM_SUBA_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_cam_subb(int state)
{
	DBG_ID = DBG_ID_CAM_SUBB;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set CAM_SUBB_SRAM_PDN = 1" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) | CAM_SUBB_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until CAM_SUBB_SRAM_PDN_ACK = 1" */
		while ((spm_read(CAM_SUBB_PWR_CON) & CAM_SUBB_SRAM_PDN_ACK) != CAM_SUBB_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(CAM_SUBB_PWR_CON) & PWR_ACK)
				|| (spm_read(CAM_SUBB_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(CAM_SUBB_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(CAM_SUBB_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) | PWR_RST_B);
		/* TINFO="Set CAM_SUBB_SRAM_PDN = 0" */
		spm_write(CAM_SUBB_PWR_CON, spm_read(CAM_SUBB_PWR_CON) & ~CAM_SUBB_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until CAM_SUBB_SRAM_PDN_ACK = 0" */
		while (spm_read(CAM_SUBB_PWR_CON) & CAM_SUBB_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_mdp0(int state)
{
	DBG_ID = DBG_ID_MDP0;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_SET, MDP0_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_0) & MDP0_PROT_STEP1_0_ACK_MASK) != MDP0_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set MDP0_SRAM_PDN = 1" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) | MDP0_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MDP0_SRAM_PDN_ACK = 1" */
		while ((spm_read(MDP0_PWR_CON) & MDP0_SRAM_PDN_ACK) != MDP0_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(MDP0_PWR_CON) & PWR_ACK)
				|| (spm_read(MDP0_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(MDP0_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(MDP0_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) | PWR_RST_B);
		/* TINFO="Set MDP0_SRAM_PDN = 0" */
		spm_write(MDP0_PWR_CON, spm_read(MDP0_PWR_CON) & ~MDP0_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MDP0_SRAM_PDN_ACK = 0" */
		while (spm_read(MDP0_PWR_CON) & MDP0_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_CLR, MDP0_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_disp(int state)
{
	DBG_ID = DBG_ID_DISP;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_SET, DIS0_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_0) & DIS0_PROT_STEP1_0_ACK_MASK) != DIS0_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set DIS0_SRAM_PDN = 1" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) | DIS0_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until DIS0_SRAM_PDN_ACK = 1" */
		while ((spm_read(DIS0_PWR_CON) & DIS0_SRAM_PDN_ACK) != DIS0_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(DIS0_PWR_CON) & PWR_ACK)
				|| (spm_read(DIS0_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(DIS0_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(DIS0_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) | PWR_RST_B);
		/* TINFO="Set DIS0_SRAM_PDN = 0" */
		spm_write(DIS0_PWR_CON, spm_read(DIS0_PWR_CON) & ~DIS0_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until DIS0_SRAM_PDN_ACK = 0" */
		while (spm_read(DIS0_PWR_CON) & DIS0_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_0_CLR, DIS0_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_mm_infra(int state)
{
	DBG_ID = DBG_ID_MM_INFRA;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_SET, MM_INFRA_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_1) & MM_INFRA_PROT_STEP1_0_ACK_MASK) != MM_INFRA_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_INFRASYS_PROTECT_EN_STA_1_SET, MM_INFRA_PROT_STEP1_1_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_INFRASYS_PROTECT_RDY_STA_1) & MM_INFRA_PROT_STEP1_1_ACK_MASK) != MM_INFRA_PROT_STEP1_1_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_SET, MM_INFRA_PROT_STEP2_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MMSYS_PROTECT_RDY_STA_1) & MM_INFRA_PROT_STEP2_0_ACK_MASK) != MM_INFRA_PROT_STEP2_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_INFRASYS_PROTECT_EN_STA_0_SET, MM_INFRA_PROT_STEP2_1_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_INFRASYS_PROTECT_RDY_STA_0) & MM_INFRA_PROT_STEP2_1_ACK_MASK) != MM_INFRA_PROT_STEP2_1_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_EMISYS_PROTECT_EN_STA_0_SET, MM_INFRA_PROT_STEP2_2_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_EMISYS_PROTECT_RDY_STA_0) & MM_INFRA_PROT_STEP2_2_ACK_MASK) != MM_INFRA_PROT_STEP2_2_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set MM_INFRA_SRAM_PDN = 1" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) | MM_INFRA_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MM_INFRA_SRAM_PDN_ACK = 1" */
		while ((spm_read(MM_INFRA_PWR_CON) & MM_INFRA_SRAM_PDN_ACK) != MM_INFRA_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(MM_INFRA_PWR_CON) & PWR_ACK)
				|| (spm_read(MM_INFRA_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(MM_INFRA_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(MM_INFRA_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) | PWR_RST_B);
		/* TINFO="Set MM_INFRA_SRAM_PDN = 0" */
		spm_write(MM_INFRA_PWR_CON, spm_read(MM_INFRA_PWR_CON) & ~MM_INFRA_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MM_INFRA_SRAM_PDN_ACK = 0" */
		while (spm_read(MM_INFRA_PWR_CON) & MM_INFRA_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_EMISYS_PROTECT_EN_STA_0_CLR, MM_INFRA_PROT_STEP2_2_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_INFRASYS_PROTECT_EN_STA_0_CLR, MM_INFRA_PROT_STEP2_1_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_CLR, MM_INFRA_PROT_STEP2_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_INFRASYS_PROTECT_EN_STA_1_CLR, MM_INFRA_PROT_STEP1_1_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MMSYS_PROTECT_EN_STA_1_CLR, MM_INFRA_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_dp_tx(int state)
{
	DBG_ID = DBG_ID_DP_TX;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set DP_TX_SRAM_PDN = 1" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) | DP_TX_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until DP_TX_SRAM_PDN_ACK = 1" */
		while ((spm_read(DP_TX_PWR_CON) & DP_TX_SRAM_PDN_ACK) != DP_TX_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(DP_TX_PWR_CON) & PWR_ACK)
				|| (spm_read(DP_TX_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(DP_TX_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(DP_TX_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) | PWR_RST_B);
		/* TINFO="Set DP_TX_SRAM_PDN = 0" */
		spm_write(DP_TX_PWR_CON, spm_read(DP_TX_PWR_CON) & ~DP_TX_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until DP_TX_SRAM_PDN_ACK = 0" */
		while (spm_read(DP_TX_PWR_CON) & DP_TX_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_csi_rx(int state)
{
	DBG_ID = DBG_ID_CSI_RX;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set CSI_RX_SRAM_PDN = 1" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) | CSI_RX_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until CSI_RX_SRAM_PDN_ACK = 1" */
		while ((spm_read(CSI_RX_PWR_CON) & CSI_RX_SRAM_PDN_ACK) != CSI_RX_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(CSI_RX_PWR_CON) & PWR_ACK)
				|| (spm_read(CSI_RX_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(CSI_RX_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(CSI_RX_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) | PWR_RST_B);
		/* TINFO="Set CSI_RX_SRAM_PDN = 0" */
		spm_write(CSI_RX_PWR_CON, spm_read(CSI_RX_PWR_CON) & ~CSI_RX_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until CSI_RX_SRAM_PDN_ACK = 0" */
		while (spm_read(CSI_RX_PWR_CON) & CSI_RX_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_ssusb(int state)
{
	DBG_ID = DBG_ID_SSUSB;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_PERISYS_PROTECT_EN_STA_0_SET, SSUSB_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_PERISYS_PROTECT_RDY_STA_0) & SSUSB_PROT_STEP1_0_ACK_MASK) != SSUSB_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set SSUSB_SRAM_PDN = 1" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) | SSUSB_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until SSUSB_SRAM_PDN_ACK = 1" */
		while ((spm_read(SSUSB_PWR_CON) & SSUSB_SRAM_PDN_ACK) != SSUSB_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(SSUSB_PWR_CON) & PWR_ACK)
				|| (spm_read(SSUSB_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(SSUSB_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(SSUSB_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) | PWR_RST_B);
		/* TINFO="Set SSUSB_SRAM_PDN = 0" */
		spm_write(SSUSB_PWR_CON, spm_read(SSUSB_PWR_CON) & ~SSUSB_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until SSUSB_SRAM_PDN_ACK = 0" */
		while (spm_read(SSUSB_PWR_CON) & SSUSB_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_PERISYS_PROTECT_EN_STA_0_CLR, SSUSB_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_mfg0(int state)
{
	DBG_ID = DBG_ID_MFG0;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(MFG0_PWR_CON, spm_read(MFG0_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(MFG0_PWR_CON, spm_read(MFG0_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(MFG0_PWR_CON, spm_read(MFG0_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(MFG0_PWR_CON, spm_read(MFG0_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(MFG0_PWR_CON, spm_read(MFG0_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(MFG0_PWR_CON) & PWR_ACK)
				|| (spm_read(MFG0_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(MFG0_PWR_CON, spm_read(MFG0_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(MFG0_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(MFG0_PWR_CON, spm_read(MFG0_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(MFG0_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(MFG0_PWR_CON, spm_read(MFG0_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(MFG0_PWR_CON, spm_read(MFG0_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(MFG0_PWR_CON, spm_read(MFG0_PWR_CON) | PWR_RST_B);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_mfg1(int state)
{
	DBG_ID = DBG_ID_MFG1;
	DBG_STA = state;
	DBG_STEP = 0;

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_INFRASYS_PROTECT_EN_STA_1_SET, MFG1_PROT_STEP1_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_INFRASYS_PROTECT_RDY_STA_1) & MFG1_PROT_STEP1_0_ACK_MASK) != MFG1_PROT_STEP1_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MD_MFGSYS_PROTECT_EN_STA_0_SET, MFG1_PROT_STEP2_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MD_MFGSYS_PROTECT_RDY_STA_0) & MFG1_PROT_STEP2_0_ACK_MASK) != MFG1_PROT_STEP2_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(INFRACFG_AO_MD_MFGSYS_PROTECT_EN_STA_0_SET, MFG1_PROT_STEP3_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(INFRACFG_AO_MD_MFGSYS_PROTECT_RDY_STA_0) & MFG1_PROT_STEP3_0_ACK_MASK) != MFG1_PROT_STEP3_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set bus protect" */
		spm_write(EMICFG_AO_MEM_GALS_SLP_PROT_EN_SET, MFG1_PROT_STEP4_0_MASK);
		#ifndef IGNORE_MTCMOS_CHECK
		while ((spm_read(EMICFG_AO_MEM_GALS_SLP_PROT_RDY) & MFG1_PROT_STEP4_0_ACK_MASK) != MFG1_PROT_STEP4_0_ACK_MASK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set MFG1_SRAM_PDN = 1" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) | MFG1_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MFG1_SRAM_PDN_ACK = 1" */
		while ((spm_read(MFG1_PWR_CON) & MFG1_SRAM_PDN_ACK) != MFG1_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(MFG1_PWR_CON) & PWR_ACK)
				|| (spm_read(MFG1_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(MFG1_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(MFG1_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) | PWR_RST_B);
		/* TINFO="Set MFG1_SRAM_PDN = 0" */
		spm_write(MFG1_PWR_CON, spm_read(MFG1_PWR_CON) & ~MFG1_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MFG1_SRAM_PDN_ACK = 0" */
		while (spm_read(MFG1_PWR_CON) & MFG1_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Release bus protect" */
		spm_write(EMICFG_AO_MEM_GALS_SLP_PROT_EN_CLR, MFG1_PROT_STEP4_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MD_MFGSYS_PROTECT_EN_STA_0_CLR, MFG1_PROT_STEP3_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_MD_MFGSYS_PROTECT_EN_STA_0_CLR, MFG1_PROT_STEP2_0_MASK);
		/* TINFO="Release bus protect" */
		spm_write(INFRACFG_AO_INFRASYS_PROTECT_EN_STA_1_CLR, MFG1_PROT_STEP1_0_MASK);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_mfg2(int state)
{
	DBG_ID = DBG_ID_MFG2;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set MFG2_SRAM_PDN = 1" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) | MFG2_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MFG2_SRAM_PDN_ACK = 1" */
		while ((spm_read(MFG2_PWR_CON) & MFG2_SRAM_PDN_ACK) != MFG2_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(MFG2_PWR_CON) & PWR_ACK)
				|| (spm_read(MFG2_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(MFG2_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(MFG2_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) | PWR_RST_B);
		/* TINFO="Set MFG2_SRAM_PDN = 0" */
		spm_write(MFG2_PWR_CON, spm_read(MFG2_PWR_CON) & ~MFG2_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MFG2_SRAM_PDN_ACK = 0" */
		while (spm_read(MFG2_PWR_CON) & MFG2_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_mfg3(int state)
{
	DBG_ID = DBG_ID_MFG3;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set MFG3_SRAM_PDN = 1" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) | MFG3_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MFG3_SRAM_PDN_ACK = 1" */
		while ((spm_read(MFG3_PWR_CON) & MFG3_SRAM_PDN_ACK) != MFG3_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(MFG3_PWR_CON) & PWR_ACK)
				|| (spm_read(MFG3_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(MFG3_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(MFG3_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) | PWR_RST_B);
		/* TINFO="Set MFG3_SRAM_PDN = 0" */
		spm_write(MFG3_PWR_CON, spm_read(MFG3_PWR_CON) & ~MFG3_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until MFG3_SRAM_PDN_ACK = 0" */
		while (spm_read(MFG3_PWR_CON) & MFG3_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_edp_tx_shutdown(int state)
{
	DBG_ID = DBG_ID_EDP_TX_SHUTDOWN;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set SRAM_CKISO = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | SRAM_CKISO);
		/* TINFO="Set SRAM_ISOINT_B = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~SRAM_ISOINT_B);
		/* TINFO="Set EDP_TX_SRAM_PDN = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | EDP_TX_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until EDP_TX_SRAM_PDN_ACK = 1" */
		while ((spm_read(EDP_TX_PWR_CON) & EDP_TX_SRAM_PDN_ACK) != EDP_TX_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(EDP_TX_PWR_CON) & PWR_ACK)
				|| (spm_read(EDP_TX_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(EDP_TX_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(EDP_TX_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set ~PWR_CLK_DIS = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | PWR_RST_B);
		/* TINFO="Set EDP_TX_SRAM_PDN = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~EDP_TX_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until EDP_TX_SRAM_PDN_ACK = 0" */
		while (spm_read(EDP_TX_PWR_CON) & EDP_TX_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set SRAM_ISOINT_B = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | SRAM_ISOINT_B);
		/* TINFO="Set SRAM_CKISO = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~SRAM_CKISO);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_edp_tx_dormant(int state)
{
	DBG_ID = DBG_ID_EDP_TX_DORMANT;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set SRAM_CKISO = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | SRAM_CKISO);
		/* TINFO="Set SRAM_ISOINT_B = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~SRAM_ISOINT_B);
		/* TINFO="Set EDP_TX_SRAM_SLP_B = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~EDP_TX_SRAM_SLP_B);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until EDP_TX_SRAM_SLP_B_ACK = 0" */
		while (spm_read(EDP_TX_PWR_CON) & EDP_TX_SRAM_SLP_B_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(EDP_TX_PWR_CON) & PWR_ACK)
				|| (spm_read(EDP_TX_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(EDP_TX_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(EDP_TX_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | PWR_RST_B);
		/* TINFO="Set EDP_TX_SRAM_SLP_B = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | EDP_TX_SRAM_SLP_B);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until EDP_TX_SRAM_SLP_B_ACK = 1" */
		while ((spm_read(EDP_TX_PWR_CON) & EDP_TX_SRAM_SLP_B_ACK) != EDP_TX_SRAM_SLP_B_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set SRAM_ISOINT_B = 1" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) | SRAM_ISOINT_B);
		/* TINFO="Set SRAM_CKISO = 0" */
		spm_write(EDP_TX_PWR_CON, spm_read(EDP_TX_PWR_CON) & ~SRAM_CKISO);
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_pcie(int state)
{
	DBG_ID = DBG_ID_PCIE;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set PCIE_SRAM_PDN = 1" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) | PCIE_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PCIE_SRAM_PDN_ACK = 1" */
		while ((spm_read(PCIE_PWR_CON) & PCIE_SRAM_PDN_ACK) != PCIE_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(PCIE_PWR_CON) & PWR_ACK)
				|| (spm_read(PCIE_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(PCIE_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(PCIE_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) | PWR_RST_B);
		/* TINFO="Set PCIE_SRAM_PDN = 0" */
		spm_write(PCIE_PWR_CON, spm_read(PCIE_PWR_CON) & ~PCIE_SRAM_PDN);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PCIE_SRAM_PDN_ACK = 0" */
		while (spm_read(PCIE_PWR_CON) & PCIE_SRAM_PDN_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif
	}

	INCREASE_STEPS;
}

void spm_mtcmos_ctrl_pcie_phy(int state)
{
	DBG_ID = DBG_ID_PCIE_PHY;
	DBG_STA = state;
	DBG_STEP = 0;

	/* TINFO="enable SPM register control" */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (state == STA_POWER_DOWN) {
		/* STA_POWER_DOWN */
		/* TINFO="Set PWR_ISO = 1" */
		spm_write(PCIE_PHY_PWR_CON, spm_read(PCIE_PHY_PWR_CON) | PWR_ISO);
		/* TINFO="Set PWR_CLK_DIS = 1" */
		spm_write(PCIE_PHY_PWR_CON, spm_read(PCIE_PHY_PWR_CON) | PWR_CLK_DIS);
		/* TINFO="Set PWR_RST_B = 0" */
		spm_write(PCIE_PHY_PWR_CON, spm_read(PCIE_PHY_PWR_CON) & ~PWR_RST_B);
		/* TINFO="Set PWR_ON = 0" */
		spm_write(PCIE_PHY_PWR_CON, spm_read(PCIE_PHY_PWR_CON) & ~PWR_ON);
		/* TINFO="Set PWR_ON_2ND = 0" */
		spm_write(PCIE_PHY_PWR_CON, spm_read(PCIE_PHY_PWR_CON) & ~PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0" */
		while ((spm_read(PCIE_PHY_PWR_CON) & PWR_ACK)
				|| (spm_read(PCIE_PHY_PWR_CON) & PWR_ACK_2ND))
			ram_console_update();

		INCREASE_STEPS;
		#endif
	} else {
		/* STA_POWER_ON */
		/* TINFO="Set PWR_ON = 1" */
		spm_write(PCIE_PHY_PWR_CON, spm_read(PCIE_PHY_PWR_CON) | PWR_ON);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS = 1" */
		while ((spm_read(PCIE_PHY_PWR_CON) & PWR_ACK) != PWR_ACK)
			ram_console_update();

		INCREASE_STEPS;
		#endif

		udelay(50);

		/* TINFO="Set PWR_ON_2ND = 1" */
		spm_write(PCIE_PHY_PWR_CON, spm_read(PCIE_PHY_PWR_CON) | PWR_ON_2ND);
		#ifndef IGNORE_MTCMOS_CHECK
		/* TINFO="Wait until PWR_STATUS_2ND = 1" */
		while ((spm_read(PCIE_PHY_PWR_CON) & PWR_ACK_2ND) != PWR_ACK_2ND)
			ram_console_update();

		INCREASE_STEPS;
		#endif
		/* TINFO="Set PWR_CLK_DIS = 0" */
		spm_write(PCIE_PHY_PWR_CON, spm_read(PCIE_PHY_PWR_CON) & ~PWR_CLK_DIS);
		/* TINFO="Set PWR_ISO = 0" */
		spm_write(PCIE_PHY_PWR_CON, spm_read(PCIE_PHY_PWR_CON) & ~PWR_ISO);
		/* TINFO="Set PWR_RST_B = 1" */
		spm_write(PCIE_PHY_PWR_CON, spm_read(PCIE_PHY_PWR_CON) | PWR_RST_B);
	}

	INCREASE_STEPS;
}

void spm_power_on(void)
{
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));
}
