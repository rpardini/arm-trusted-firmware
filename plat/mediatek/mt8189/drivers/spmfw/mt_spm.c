// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#include <lib/mtk_init/mtk_init.h>
#include <string.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <mtk_plat_common.h>
#include <platform_def.h>
#include <assert.h>
#include <mcu.h>
#include <drivers/delay_timer.h>
#include <mt_spm_reg.h>

#include "mt_spm.h"

#define INFRA_BUS_DCM_CTRL	(INFRACFG_AO_BASE + 0x070)
#define CLK_SCP_CFG_0		(CKSYS_BASE + 0x200)
#define CLK_SCP_CFG_1		(CKSYS_BASE + 0x210)
#define SPMFW_HEADER_SIZE	16
#define SPM_SYSCLK_SETTLE	0x60FE	/* 1685us */

/* MD32PCM ADDR for SPM code fetch */
#define MD32PCM_BASE				(SPM_BASE + 0x0A00)
#define MD32PCM_CFGREG_SW_RSTN			(MD32PCM_BASE + 0x0000)
#define MD32PCM_DMA0_SRC			(MD32PCM_BASE + 0x0200)
#define MD32PCM_DMA0_DST			(MD32PCM_BASE + 0x0204)
#define MD32PCM_DMA0_WPPT			(MD32PCM_BASE + 0x0208)
#define MD32PCM_DMA0_WPTO			(MD32PCM_BASE + 0x020C)
#define MD32PCM_DMA0_COUNT			(MD32PCM_BASE + 0x0210)
#define MD32PCM_DMA0_CON			(MD32PCM_BASE + 0x0214)
#define MD32PCM_DMA0_START			(MD32PCM_BASE + 0x0218)
#define MD32PCM_DMA0_RLCT			(MD32PCM_BASE + 0x0224)
#define MD32PCM_INTC_IRQ_RAW_STA		(MD32PCM_BASE + 0x033C)

struct pcm_desc {
	uint32_t pmem_words;
	uint32_t total_words;
	uint32_t pmem_start;
	uint32_t dmem_start;
};

struct dyna_load_pcm {
	uint8_t *buf;		/* binary array */
	struct pcm_desc desc;
};

static const struct pwr_ctrl spm_init_ctrl = {
	/* For SPM, this flag is not auto-gen. */
	.pcm_flags = SPM_FLAG_DISABLE_VCORE_DVS
		   | SPM_FLAG_DISABLE_DDR_DFS
		   | SPM_FLAG_DISABLE_EMI_DFS
		   | SPM_FLAG_DISABLE_BUS_DFS
		   | SPM_FLAG_RUN_COMMON_SCENARIO,

	/* SPM_SRC_REQ */
	.reg_spm_adsp_mailbox_req = 0,
	.reg_spm_apsrc_req = 0,
	.reg_spm_ddren_req = 0,
	.reg_spm_dvfs_req = 0,
	.reg_spm_emi_req = 0,
	.reg_spm_f26m_req = 0,
	.reg_spm_infra_req = 0,
	.reg_spm_pmic_req = 0,
	.reg_spm_scp_mailbox_req = 0,
	.reg_spm_sspm_mailbox_req = 0,
	.reg_spm_sw_mailbox_req = 0,
	.reg_spm_vcore_req = 1,
	.reg_spm_vrf18_req = 0,
	.adsp_mailbox_state = 0,
	.apsrc_state = 0,
	.ddren_state = 0,
	.dvfs_state = 0,
	.emi_state = 0,
	.f26m_state = 0,
	.infra_state = 0,
	.pmic_state = 0,
	.scp_mailbox_state = 0,
	.sspm_mailbox_state = 0,
	.sw_mailbox_state = 0,
	.vcore_state = 0,
	.vrf18_state = 0,

	/* SPM_SRC_MASK_0 */
	.reg_apu_apsrc_req_mask_b = 0x1,
	.reg_apu_ddren_req_mask_b = 0x1,
	.reg_apu_emi_req_mask_b = 0x1,
	.reg_apu_infra_req_mask_b = 0x1,
	.reg_apu_pmic_req_mask_b = 0x1,
	.reg_apu_srcclkena_mask_b = 0x1,
	.reg_apu_vrf18_req_mask_b = 0x1,
	.reg_audio_dsp_apsrc_req_mask_b = 0x0,
	.reg_audio_dsp_ddren_req_mask_b = 0x0,
	.reg_audio_dsp_emi_req_mask_b = 0x0,
	.reg_audio_dsp_infra_req_mask_b = 0x0,
	.reg_audio_dsp_pmic_req_mask_b = 0x0,
	.reg_audio_dsp_srcclkena_mask_b = 0x0,
	.reg_audio_dsp_vcore_req_mask_b = 0x0,
	.reg_audio_dsp_vrf18_req_mask_b = 0x0,
	.reg_cam_apsrc_req_mask_b = 0x1,
	.reg_cam_ddren_req_mask_b = 0x1,
	.reg_cam_emi_req_mask_b = 0x1,
	.reg_cam_infra_req_mask_b = 0x0,
	.reg_cam_pmic_req_mask_b = 0x0,
	.reg_cam_srcclkena_mask_b = 0x0,
	.reg_cam_vrf18_req_mask_b = 0x0,
	.reg_mdp_emi_req_mask_b = 0x1,

	/* SPM_SRC_MASK_1 */
	.reg_ccif_apsrc_req_mask_b = 0x0,
	.reg_ccif_emi_req_mask_b = 0xfff,

	/* SPM_SRC_MASK_2 */
	.reg_ccif_infra_req_mask_b = 0x0,
	.reg_ccif_pmic_req_mask_b = 0xfff,

	/* SPM_SRC_MASK_3 */
	.reg_ccif_srcclkena_mask_b = 0x0,
	.reg_ccif_vrf18_req_mask_b = 0xfff,
	.reg_ccu_apsrc_req_mask_b = 0x0,
	.reg_ccu_ddren_req_mask_b = 0x0,
	.reg_ccu_emi_req_mask_b = 0x0,
	.reg_ccu_infra_req_mask_b = 0x0,
	.reg_ccu_pmic_req_mask_b = 0x0,
	.reg_ccu_srcclkena_mask_b = 0x0,
	.reg_ccu_vrf18_req_mask_b = 0x0,
	.reg_cg_check_apsrc_req_mask_b = 0x1,

	/* SPM_SRC_MASK_4 */
	.reg_cg_check_ddren_req_mask_b = 0x1,
	.reg_cg_check_emi_req_mask_b = 0x1,
	.reg_cg_check_infra_req_mask_b = 0x1,
	.reg_cg_check_pmic_req_mask_b = 0x1,
	.reg_cg_check_srcclkena_mask_b = 0x1,
	.reg_cg_check_vcore_req_mask_b = 0x1,
	.reg_cg_check_vrf18_req_mask_b = 0x1,
	.reg_conn_apsrc_req_mask_b = 0x1,
	.reg_conn_ddren_req_mask_b = 0x1,
	.reg_conn_emi_req_mask_b = 0x1,
	.reg_conn_infra_req_mask_b = 0x1,
	.reg_conn_pmic_req_mask_b = 0x1,
	.reg_conn_srcclkena_mask_b = 0x1,
	.reg_conn_srcclkenb_mask_b = 0x1,
	.reg_conn_vcore_req_mask_b = 0x1,
	.reg_conn_vrf18_req_mask_b = 0x1,
	.reg_cpueb_apsrc_req_mask_b = 0x1,
	.reg_cpueb_ddren_req_mask_b = 0x1,
	.reg_cpueb_emi_req_mask_b = 0x1,
	.reg_cpueb_infra_req_mask_b = 0x1,
	.reg_cpueb_pmic_req_mask_b = 0x1,
	.reg_cpueb_srcclkena_mask_b = 0x1,
	.reg_cpueb_vrf18_req_mask_b = 0x1,
	.reg_disp0_apsrc_req_mask_b = 0x1,
	.reg_disp0_ddren_req_mask_b = 0x1,
	.reg_disp0_emi_req_mask_b = 0x1,
	.reg_disp0_infra_req_mask_b = 0x1,
	.reg_disp0_pmic_req_mask_b = 0x0,
	.reg_disp0_srcclkena_mask_b = 0x0,
	.reg_disp0_vrf18_req_mask_b = 0x1,
	.reg_disp1_apsrc_req_mask_b = 0x0,
	.reg_disp1_ddren_req_mask_b = 0x0,

	/* SPM_SRC_MASK_5 */
	.reg_disp1_emi_req_mask_b = 0x0,
	.reg_disp1_infra_req_mask_b = 0x0,
	.reg_disp1_pmic_req_mask_b = 0x0,
	.reg_disp1_srcclkena_mask_b = 0x0,
	.reg_disp1_vrf18_req_mask_b = 0x0,
	.reg_dpm_apsrc_req_mask_b = 0xf,
	.reg_dpm_ddren_req_mask_b = 0xf,
	.reg_dpm_emi_req_mask_b = 0xf,
	.reg_dpm_infra_req_mask_b = 0xf,
	.reg_dpm_pmic_req_mask_b = 0xf,
	.reg_dpm_srcclkena_mask_b = 0xf,

	/* SPM_SRC_MASK_6 */
	.reg_dpm_vcore_req_mask_b = 0xf,
	.reg_dpm_vrf18_req_mask_b = 0xf,
	.reg_dpmaif_apsrc_req_mask_b = 0x1,
	.reg_dpmaif_ddren_req_mask_b = 0x1,
	.reg_dpmaif_emi_req_mask_b = 0x1,
	.reg_dpmaif_infra_req_mask_b = 0x1,
	.reg_dpmaif_pmic_req_mask_b = 0x1,
	.reg_dpmaif_srcclkena_mask_b = 0x1,
	.reg_dpmaif_vrf18_req_mask_b = 0x1,
	.reg_dvfsrc_level_req_mask_b = 0x1,
	.reg_emisys_apsrc_req_mask_b = 0x0,
	.reg_emisys_ddren_req_mask_b = 0x1,
	.reg_emisys_emi_req_mask_b = 0x0,
	.reg_gce_d_apsrc_req_mask_b = 0x1,
	.reg_gce_d_ddren_req_mask_b = 0x1,
	.reg_gce_d_emi_req_mask_b = 0x1,
	.reg_gce_d_infra_req_mask_b = 0x0,
	.reg_gce_d_pmic_req_mask_b = 0x0,
	.reg_gce_d_srcclkena_mask_b = 0x0,
	.reg_gce_d_vrf18_req_mask_b = 0x0,
	.reg_gce_m_apsrc_req_mask_b = 0x1,
	.reg_gce_m_ddren_req_mask_b = 0x1,
	.reg_gce_m_emi_req_mask_b = 0x1,
	.reg_gce_m_infra_req_mask_b = 0x0,
	.reg_gce_m_pmic_req_mask_b = 0x0,
	.reg_gce_m_srcclkena_mask_b = 0x0,

	/* SPM_SRC_MASK_7 */
	.reg_gce_m_vrf18_req_mask_b = 0x0,
	.reg_gpueb_apsrc_req_mask_b = 0x0,
	.reg_gpueb_ddren_req_mask_b = 0x0,
	.reg_gpueb_emi_req_mask_b = 0x0,
	.reg_gpueb_infra_req_mask_b = 0x0,
	.reg_gpueb_pmic_req_mask_b = 0x0,
	.reg_gpueb_srcclkena_mask_b = 0x0,
	.reg_gpueb_vrf18_req_mask_b = 0x0,
	.reg_hwccf_apsrc_req_mask_b = 0x1,
	.reg_hwccf_ddren_req_mask_b = 0x1,
	.reg_hwccf_emi_req_mask_b = 0x1,
	.reg_hwccf_infra_req_mask_b = 0x1,
	.reg_hwccf_pmic_req_mask_b = 0x1,
	.reg_hwccf_srcclkena_mask_b = 0x1,
	.reg_hwccf_vcore_req_mask_b = 0x1,
	.reg_hwccf_vrf18_req_mask_b = 0x1,
	.reg_img_apsrc_req_mask_b = 0x1,
	.reg_img_ddren_req_mask_b = 0x1,
	.reg_img_emi_req_mask_b = 0x1,
	.reg_img_infra_req_mask_b = 0x0,
	.reg_img_pmic_req_mask_b = 0x0,
	.reg_img_srcclkena_mask_b = 0x0,
	.reg_img_vrf18_req_mask_b = 0x0,
	.reg_infrasys_apsrc_req_mask_b = 0x1,
	.reg_infrasys_ddren_req_mask_b = 0x1,
	.reg_infrasys_emi_req_mask_b = 0x1,
	.reg_ipic_infra_req_mask_b = 0x1,
	.reg_ipic_vrf18_req_mask_b = 0x1,
	.reg_mcu_apsrc_req_mask_b = 0x0,
	.reg_mcu_ddren_req_mask_b = 0x0,
	.reg_mcu_emi_req_mask_b = 0x0,

	/* SPM_SRC_MASK_8 */
	.reg_mcusys_apsrc_req_mask_b = 0x7,
	.reg_mcusys_ddren_req_mask_b = 0x7,
	.reg_mcusys_emi_req_mask_b = 0x7,
	.reg_mcusys_infra_req_mask_b = 0x0,

	/* SPM_SRC_MASK_9 */
	.reg_mcusys_pmic_req_mask_b = 0x0,
	.reg_mcusys_srcclkena_mask_b = 0x0,
	.reg_mcusys_vrf18_req_mask_b = 0x0,
	.reg_md_apsrc_req_mask_b = 0x0,
	.reg_md_ddren_req_mask_b = 0x0,
	.reg_md_emi_req_mask_b = 0x0,
	.reg_md_infra_req_mask_b = 0x0,
	.reg_md_pmic_req_mask_b = 0x0,
	.reg_md_srcclkena_mask_b = 0x0,
	.reg_md_srcclkena1_mask_b = 0x0,
	.reg_md_vcore_req_mask_b = 0x0,

	/* SPM_SRC_MASK_10 */
	.reg_md_vrf18_req_mask_b = 0x0,
	.reg_mdp_apsrc_req_mask_b = 0x0,
	.reg_mdp_ddren_req_mask_b = 0x0,
	.reg_mm_proc_apsrc_req_mask_b = 0x0,
	.reg_mm_proc_ddren_req_mask_b = 0x0,
	.reg_mm_proc_emi_req_mask_b = 0x0,
	.reg_mm_proc_infra_req_mask_b = 0x0,
	.reg_mm_proc_pmic_req_mask_b = 0x0,
	.reg_mm_proc_srcclkena_mask_b = 0x0,
	.reg_mm_proc_vrf18_req_mask_b = 0x0,
	.reg_mmsys_apsrc_req_mask_b = 0x0,
	.reg_mmsys_ddren_req_mask_b = 0x0,
	.reg_mmsys_vrf18_req_mask_b = 0x0,
	.reg_pcie0_apsrc_req_mask_b = 0x0,
	.reg_pcie0_ddren_req_mask_b = 0x0,
	.reg_pcie0_infra_req_mask_b = 0x0,
	.reg_pcie0_srcclkena_mask_b = 0x0,
	.reg_pcie0_vrf18_req_mask_b = 0x0,
	.reg_pcie1_apsrc_req_mask_b = 0x0,
	.reg_pcie1_ddren_req_mask_b = 0x0,
	.reg_pcie1_infra_req_mask_b = 0x0,
	.reg_pcie1_srcclkena_mask_b = 0x0,
	.reg_pcie1_vrf18_req_mask_b = 0x0,
	.reg_perisys_apsrc_req_mask_b = 0x1,
	.reg_perisys_ddren_req_mask_b = 0x1,
	.reg_perisys_emi_req_mask_b = 0x1,
	.reg_perisys_infra_req_mask_b = 0x1,
	.reg_perisys_pmic_req_mask_b = 0x1,
	.reg_perisys_srcclkena_mask_b = 0x1,
	.reg_perisys_vcore_req_mask_b = 0x1,
	.reg_perisys_vrf18_req_mask_b = 0x1,
	.reg_scp_apsrc_req_mask_b = 0x1,

	/* SPM_SRC_MASK_11 */
	.reg_scp_ddren_req_mask_b = 0x1,
	.reg_scp_emi_req_mask_b = 0x1,
	.reg_scp_infra_req_mask_b = 0x1,
	.reg_scp_pmic_req_mask_b = 0x1,
	.reg_scp_srcclkena_mask_b = 0x1,
	.reg_scp_vcore_req_mask_b = 0x1,
	.reg_scp_vrf18_req_mask_b = 0x1,
	.reg_srcclkeni_infra_req_mask_b = 0x1,
	.reg_srcclkeni_pmic_req_mask_b = 0x1,
	.reg_srcclkeni_srcclkena_mask_b = 0x1,
	.reg_sspm_apsrc_req_mask_b = 0x1,
	.reg_sspm_ddren_req_mask_b = 0x1,
	.reg_sspm_emi_req_mask_b = 0x1,
	.reg_sspm_infra_req_mask_b = 0x1,
	.reg_sspm_pmic_req_mask_b = 0x1,
	.reg_sspm_srcclkena_mask_b = 0x1,
	.reg_sspm_vrf18_req_mask_b = 0x1,
	.reg_ssr_apsrc_req_mask_b = 0x0,
	.reg_ssr_ddren_req_mask_b = 0x0,
	.reg_ssr_emi_req_mask_b = 0x0,
	.reg_ssr_infra_req_mask_b = 0x0,
	.reg_ssr_pmic_req_mask_b = 0x0,
	.reg_ssr_srcclkena_mask_b = 0x0,
	.reg_ssr_vrf18_req_mask_b = 0x0,
	.reg_ufs_apsrc_req_mask_b = 0x1,
	.reg_ufs_ddren_req_mask_b = 0x1,
	.reg_ufs_emi_req_mask_b = 0x1,
	.reg_ufs_infra_req_mask_b = 0x1,
	.reg_ufs_pmic_req_mask_b = 0x1,

	/* SPM_SRC_MASK_12 */
	.reg_ufs_srcclkena_mask_b = 0x1,
	.reg_ufs_vrf18_req_mask_b = 0x1,
	.reg_vdec_apsrc_req_mask_b = 0x1,
	.reg_vdec_ddren_req_mask_b = 0x1,
	.reg_vdec_emi_req_mask_b = 0x1,
	.reg_vdec_infra_req_mask_b = 0x0,
	.reg_vdec_pmic_req_mask_b = 0x0,
	.reg_vdec_srcclkena_mask_b = 0x0,
	.reg_vdec_vrf18_req_mask_b = 0x0,
	.reg_venc_apsrc_req_mask_b = 0x1,
	.reg_venc_ddren_req_mask_b = 0x1,
	.reg_venc_emi_req_mask_b = 0x1,
	.reg_venc_infra_req_mask_b = 0x0,
	.reg_venc_pmic_req_mask_b = 0x0,
	.reg_venc_srcclkena_mask_b = 0x0,
	.reg_venc_vrf18_req_mask_b = 0x0,
	.reg_ipe_apsrc_req_mask_b = 0x1,
	.reg_ipe_ddren_req_mask_b = 0x1,
	.reg_ipe_emi_req_mask_b = 0x1,
	.reg_ipe_infra_req_mask_b = 0x1,
	.reg_ipe_pmic_req_mask_b = 0x1,
	.reg_ipe_srcclkena_mask_b = 0x1,
	.reg_ipe_vrf18_req_mask_b = 0x1,
	.reg_ufs_vcore_req_mask_b = 0x1,

	/* SPM_EVENT_CON_MISC */
	.reg_srcclken_fast_resp = 0,
	.reg_csyspwrup_ack_mask = 0,

	/* Auto-gen End */

	/* SPM_WAKEUP_EVENT_MASK */
	.reg_wakeup_event_mask = 0xEFFFFFFF,

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	.reg_ext_wakeup_event_mask = 0xFFFFFFFF,
};

#define SPM_ACK_CHK_3_CON_HW_MODE_TRIG	(0x800)
/* BIT[0]: SW_EN, BIT[4]: STA_EN, BIT[8]: HW_EN */
#define SPM_ACK_CHK_3_CON_EN		(0x110)
#define SPM_ACK_CHK_3_CON_CLR_ALL	(0x2)

static void spm_hw_s1_state_monitor_pause(void)
{
	mmio_clrsetbits_32(SPM_ACK_CHK_CON_3, SPM_ACK_CHK_3_CON_EN,
			   SPM_ACK_CHK_3_CON_HW_MODE_TRIG |
			   SPM_ACK_CHK_3_CON_CLR_ALL);
}

static void spm_set_power_control(const struct pwr_ctrl *pwrctrl)
{
	/* SPM_SRC_REQ */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_req,
		((pwrctrl->reg_spm_adsp_mailbox_req & 0x1) << 0) |
		(((pwrctrl->reg_spm_apsrc_req) & 0x1) << 1) |
		(((pwrctrl->reg_spm_ddren_req) & 0x1) << 2) |
		((pwrctrl->reg_spm_dvfs_req & 0x1) << 3) |
		(((pwrctrl->reg_spm_emi_req) & 0x1) << 4) |
		(((pwrctrl->reg_spm_f26m_req) & 0x1) << 5) |
		(((pwrctrl->reg_spm_infra_req) & 0x1) << 6) |
		(((pwrctrl->reg_spm_pmic_req) & 0x1) << 7) |
		(((uint32_t)pwrctrl->reg_spm_scp_mailbox_req & 0x1) << 8) |
		(((uint32_t)pwrctrl->reg_spm_sspm_mailbox_req & 0x1) << 9) |
		(((uint32_t)pwrctrl->reg_spm_sw_mailbox_req & 0x1) << 10) |
		((((uint32_t)pwrctrl->reg_spm_vcore_req) & 0x1) << 11) |
		((((uint32_t)pwrctrl->reg_spm_vrf18_req) & 0x1) << 12) |
		(((uint32_t)pwrctrl->adsp_mailbox_state & 0x1) << 16) |
		(((uint32_t)pwrctrl->apsrc_state & 0x1) << 17) |
		(((uint32_t)pwrctrl->ddren_state & 0x1) << 18) |
		(((uint32_t)pwrctrl->dvfs_state & 0x1) << 19) |
		(((uint32_t)pwrctrl->emi_state & 0x1) << 20) |
		(((uint32_t)pwrctrl->f26m_state & 0x1) << 21) |
		(((uint32_t)pwrctrl->infra_state & 0x1) << 22) |
		(((uint32_t)pwrctrl->pmic_state & 0x1) << 23) |
		(((uint32_t)pwrctrl->scp_mailbox_state & 0x1) << 24) |
		(((uint32_t)pwrctrl->sspm_mailbox_state & 0x1) << 25) |
		(((uint32_t)pwrctrl->sw_mailbox_state & 0x1) << 26) |
		(((uint32_t)pwrctrl->vcore_state & 0x1) << 27) |
		(((uint32_t)pwrctrl->vrf18_state & 0x1) << 28));

	/* SPM_SRC_MASK_0 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_0,
		(((uint32_t)pwrctrl->reg_apu_apsrc_req_mask_b & 0x1) << 0) |
		(((uint32_t)pwrctrl->reg_apu_ddren_req_mask_b & 0x1) << 1) |
		(((uint32_t)pwrctrl->reg_apu_emi_req_mask_b & 0x1) << 2) |
		(((uint32_t)pwrctrl->reg_apu_infra_req_mask_b & 0x1) << 3) |
		(((uint32_t)pwrctrl->reg_apu_pmic_req_mask_b & 0x1) << 4) |
		(((uint32_t)pwrctrl->reg_apu_srcclkena_mask_b & 0x1) << 5) |
		(((uint32_t)pwrctrl->reg_apu_vrf18_req_mask_b & 0x1) << 6) |
		(((uint32_t)pwrctrl->reg_audio_dsp_apsrc_req_mask_b & 0x1) << 7) |
		(((uint32_t)pwrctrl->reg_audio_dsp_ddren_req_mask_b & 0x1) << 8) |
		(((uint32_t)pwrctrl->reg_audio_dsp_emi_req_mask_b & 0x1) << 9) |
		(((uint32_t)pwrctrl->reg_audio_dsp_infra_req_mask_b & 0x1) << 10) |
		(((uint32_t)pwrctrl->reg_audio_dsp_pmic_req_mask_b & 0x1) << 11) |
		(((uint32_t)pwrctrl->reg_audio_dsp_srcclkena_mask_b & 0x1) << 12) |
		(((uint32_t)pwrctrl->reg_audio_dsp_vcore_req_mask_b & 0x1) << 13) |
		(((uint32_t)pwrctrl->reg_audio_dsp_vrf18_req_mask_b & 0x1) << 14) |
		(((uint32_t)pwrctrl->reg_cam_apsrc_req_mask_b & 0x1) << 15) |
		(((uint32_t)pwrctrl->reg_cam_ddren_req_mask_b & 0x1) << 16) |
		(((uint32_t)pwrctrl->reg_cam_emi_req_mask_b & 0x1) << 17) |
		(((uint32_t)pwrctrl->reg_cam_infra_req_mask_b & 0x1) << 18) |
		(((uint32_t)pwrctrl->reg_cam_pmic_req_mask_b & 0x1) << 19) |
		(((uint32_t)pwrctrl->reg_cam_srcclkena_mask_b & 0x1) << 20) |
		(((uint32_t)pwrctrl->reg_cam_vrf18_req_mask_b & 0x1) << 21) |
		(((uint32_t)pwrctrl->reg_mdp_emi_req_mask_b & 0x1) << 22));

	/* SPM_SRC_MASK_1 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_1,
		(((uint32_t)pwrctrl->reg_ccif_apsrc_req_mask_b & 0xfff) << 0) |
		(((uint32_t)pwrctrl->reg_ccif_emi_req_mask_b & 0xfff) << 12));

	/* SPM_SRC_MASK_2 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_2,
		(((uint32_t)pwrctrl->reg_ccif_infra_req_mask_b & 0xfff) << 0) |
		(((uint32_t)pwrctrl->reg_ccif_pmic_req_mask_b & 0xfff) << 12));

	/* SPM_SRC_MASK_3 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_3,
		(((uint32_t)pwrctrl->reg_ccif_srcclkena_mask_b & 0xfff) << 0) |
		(((uint32_t)pwrctrl->reg_ccif_vrf18_req_mask_b & 0xfff) << 12) |
		(((uint32_t)pwrctrl->reg_ccu_apsrc_req_mask_b & 0x1) << 24) |
		(((uint32_t)pwrctrl->reg_ccu_ddren_req_mask_b & 0x1) << 25) |
		(((uint32_t)pwrctrl->reg_ccu_emi_req_mask_b & 0x1) << 26) |
		(((uint32_t)pwrctrl->reg_ccu_infra_req_mask_b & 0x1) << 27) |
		(((uint32_t)pwrctrl->reg_ccu_pmic_req_mask_b & 0x1) << 28) |
		(((uint32_t)pwrctrl->reg_ccu_srcclkena_mask_b & 0x1) << 29) |
		(((uint32_t)pwrctrl->reg_ccu_vrf18_req_mask_b & 0x1) << 30) |
		(((uint32_t)pwrctrl->reg_cg_check_apsrc_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_4 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_4,
		(((uint32_t)pwrctrl->reg_cg_check_ddren_req_mask_b & 0x1) << 0) |
		(((uint32_t)pwrctrl->reg_cg_check_emi_req_mask_b & 0x1) << 1) |
		(((uint32_t)pwrctrl->reg_cg_check_infra_req_mask_b & 0x1) << 2) |
		(((uint32_t)pwrctrl->reg_cg_check_pmic_req_mask_b & 0x1) << 3) |
		(((uint32_t)pwrctrl->reg_cg_check_srcclkena_mask_b & 0x1) << 4) |
		(((uint32_t)pwrctrl->reg_cg_check_vcore_req_mask_b & 0x1) << 5) |
		(((uint32_t)pwrctrl->reg_cg_check_vrf18_req_mask_b & 0x1) << 6) |
		(((uint32_t)pwrctrl->reg_conn_apsrc_req_mask_b & 0x1) << 7) |
		(((uint32_t)pwrctrl->reg_conn_ddren_req_mask_b & 0x1) << 8) |
		(((uint32_t)pwrctrl->reg_conn_emi_req_mask_b & 0x1) << 9) |
		(((uint32_t)pwrctrl->reg_conn_infra_req_mask_b & 0x1) << 10) |
		(((uint32_t)pwrctrl->reg_conn_pmic_req_mask_b & 0x1) << 11) |
		(((uint32_t)pwrctrl->reg_conn_srcclkena_mask_b & 0x1) << 12) |
		(((uint32_t)pwrctrl->reg_conn_srcclkenb_mask_b & 0x1) << 13) |
		(((uint32_t)pwrctrl->reg_conn_vcore_req_mask_b & 0x1) << 14) |
		(((uint32_t)pwrctrl->reg_conn_vrf18_req_mask_b & 0x1) << 15) |
		(((uint32_t)pwrctrl->reg_cpueb_apsrc_req_mask_b & 0x1) << 16) |
		(((uint32_t)pwrctrl->reg_cpueb_ddren_req_mask_b & 0x1) << 17) |
		(((uint32_t)pwrctrl->reg_cpueb_emi_req_mask_b & 0x1) << 18) |
		(((uint32_t)pwrctrl->reg_cpueb_infra_req_mask_b & 0x1) << 19) |
		(((uint32_t)pwrctrl->reg_cpueb_pmic_req_mask_b & 0x1) << 20) |
		(((uint32_t)pwrctrl->reg_cpueb_srcclkena_mask_b & 0x1) << 21) |
		(((uint32_t)pwrctrl->reg_cpueb_vrf18_req_mask_b & 0x1) << 22) |
		(((uint32_t)pwrctrl->reg_disp0_apsrc_req_mask_b & 0x1) << 23) |
		(((uint32_t)pwrctrl->reg_disp0_ddren_req_mask_b & 0x1) << 24) |
		(((uint32_t)pwrctrl->reg_disp0_emi_req_mask_b & 0x1) << 25) |
		(((uint32_t)pwrctrl->reg_disp0_infra_req_mask_b & 0x1) << 26) |
		(((uint32_t)pwrctrl->reg_disp0_pmic_req_mask_b & 0x1) << 27) |
		(((uint32_t)pwrctrl->reg_disp0_srcclkena_mask_b & 0x1) << 28) |
		(((uint32_t)pwrctrl->reg_disp0_vrf18_req_mask_b & 0x1) << 29) |
		(((uint32_t)pwrctrl->reg_disp1_apsrc_req_mask_b & 0x1) << 30) |
		(((uint32_t)pwrctrl->reg_disp1_ddren_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_5 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_5,
		(((uint32_t)pwrctrl->reg_disp1_emi_req_mask_b & 0x1) << 0) |
		(((uint32_t)pwrctrl->reg_disp1_infra_req_mask_b & 0x1) << 1) |
		(((uint32_t)pwrctrl->reg_disp1_pmic_req_mask_b & 0x1) << 2) |
		(((uint32_t)pwrctrl->reg_disp1_srcclkena_mask_b & 0x1) << 3) |
		(((uint32_t)pwrctrl->reg_disp1_vrf18_req_mask_b & 0x1) << 4) |
		(((uint32_t)pwrctrl->reg_dpm_apsrc_req_mask_b & 0xf) << 5) |
		(((uint32_t)pwrctrl->reg_dpm_ddren_req_mask_b & 0xf) << 9) |
		(((uint32_t)pwrctrl->reg_dpm_emi_req_mask_b & 0xf) << 13) |
		(((uint32_t)pwrctrl->reg_dpm_infra_req_mask_b & 0xf) << 17) |
		(((uint32_t)pwrctrl->reg_dpm_pmic_req_mask_b & 0xf) << 21) |
		(((uint32_t)pwrctrl->reg_dpm_srcclkena_mask_b & 0xf) << 25));

	/* SPM_SRC_MASK_6 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_6,
		(((uint32_t)pwrctrl->reg_dpm_vcore_req_mask_b & 0xf) << 0) |
		(((uint32_t)pwrctrl->reg_dpm_vrf18_req_mask_b & 0xf) << 4) |
		(((uint32_t)pwrctrl->reg_dpmaif_apsrc_req_mask_b & 0x1) << 8) |
		(((uint32_t)pwrctrl->reg_dpmaif_ddren_req_mask_b & 0x1) << 9) |
		(((uint32_t)pwrctrl->reg_dpmaif_emi_req_mask_b & 0x1) << 10) |
		(((uint32_t)pwrctrl->reg_dpmaif_infra_req_mask_b & 0x1) << 11) |
		(((uint32_t)pwrctrl->reg_dpmaif_pmic_req_mask_b & 0x1) << 12) |
		(((uint32_t)pwrctrl->reg_dpmaif_srcclkena_mask_b & 0x1) << 13) |
		(((uint32_t)pwrctrl->reg_dpmaif_vrf18_req_mask_b & 0x1) << 14) |
		(((uint32_t)pwrctrl->reg_dvfsrc_level_req_mask_b & 0x1) << 15) |
		(((uint32_t)pwrctrl->reg_emisys_apsrc_req_mask_b & 0x1) << 16) |
		(((uint32_t)pwrctrl->reg_emisys_ddren_req_mask_b & 0x1) << 17) |
		(((uint32_t)pwrctrl->reg_emisys_emi_req_mask_b & 0x1) << 18) |
		(((uint32_t)pwrctrl->reg_gce_d_apsrc_req_mask_b & 0x1) << 19) |
		(((uint32_t)pwrctrl->reg_gce_d_ddren_req_mask_b & 0x1) << 20) |
		(((uint32_t)pwrctrl->reg_gce_d_emi_req_mask_b & 0x1) << 21) |
		(((uint32_t)pwrctrl->reg_gce_d_infra_req_mask_b & 0x1) << 22) |
		(((uint32_t)pwrctrl->reg_gce_d_pmic_req_mask_b & 0x1) << 23) |
		(((uint32_t)pwrctrl->reg_gce_d_srcclkena_mask_b & 0x1) << 24) |
		(((uint32_t)pwrctrl->reg_gce_d_vrf18_req_mask_b & 0x1) << 25) |
		(((uint32_t)pwrctrl->reg_gce_m_apsrc_req_mask_b & 0x1) << 26) |
		(((uint32_t)pwrctrl->reg_gce_m_ddren_req_mask_b & 0x1) << 27) |
		(((uint32_t)pwrctrl->reg_gce_m_emi_req_mask_b & 0x1) << 28) |
		(((uint32_t)pwrctrl->reg_gce_m_infra_req_mask_b & 0x1) << 29) |
		(((uint32_t)pwrctrl->reg_gce_m_pmic_req_mask_b & 0x1) << 30) |
		(((uint32_t)pwrctrl->reg_gce_m_srcclkena_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_7 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_7,
		(((uint32_t)pwrctrl->reg_gce_m_vrf18_req_mask_b & 0x1) << 0) |
		(((uint32_t)pwrctrl->reg_gpueb_apsrc_req_mask_b & 0x1) << 1) |
		(((uint32_t)pwrctrl->reg_gpueb_ddren_req_mask_b & 0x1) << 2) |
		(((uint32_t)pwrctrl->reg_gpueb_emi_req_mask_b & 0x1) << 3) |
		(((uint32_t)pwrctrl->reg_gpueb_infra_req_mask_b & 0x1) << 4) |
		(((uint32_t)pwrctrl->reg_gpueb_pmic_req_mask_b & 0x1) << 5) |
		(((uint32_t)pwrctrl->reg_gpueb_srcclkena_mask_b & 0x1) << 6) |
		(((uint32_t)pwrctrl->reg_gpueb_vrf18_req_mask_b & 0x1) << 7) |
		(((uint32_t)pwrctrl->reg_hwccf_apsrc_req_mask_b & 0x1) << 8) |
		(((uint32_t)pwrctrl->reg_hwccf_ddren_req_mask_b & 0x1) << 9) |
		(((uint32_t)pwrctrl->reg_hwccf_emi_req_mask_b & 0x1) << 10) |
		(((uint32_t)pwrctrl->reg_hwccf_infra_req_mask_b & 0x1) << 11) |
		(((uint32_t)pwrctrl->reg_hwccf_pmic_req_mask_b & 0x1) << 12) |
		(((uint32_t)pwrctrl->reg_hwccf_srcclkena_mask_b & 0x1) << 13) |
		(((uint32_t)pwrctrl->reg_hwccf_vcore_req_mask_b & 0x1) << 14) |
		(((uint32_t)pwrctrl->reg_hwccf_vrf18_req_mask_b & 0x1) << 15) |
		(((uint32_t)pwrctrl->reg_img_apsrc_req_mask_b & 0x1) << 16) |
		(((uint32_t)pwrctrl->reg_img_ddren_req_mask_b & 0x1) << 17) |
		(((uint32_t)pwrctrl->reg_img_emi_req_mask_b & 0x1) << 18) |
		(((uint32_t)pwrctrl->reg_img_infra_req_mask_b & 0x1) << 19) |
		(((uint32_t)pwrctrl->reg_img_pmic_req_mask_b & 0x1) << 20) |
		(((uint32_t)pwrctrl->reg_img_srcclkena_mask_b & 0x1) << 21) |
		(((uint32_t)pwrctrl->reg_img_vrf18_req_mask_b & 0x1) << 22) |
		(((uint32_t)pwrctrl->reg_infrasys_apsrc_req_mask_b & 0x1) << 23) |
		(((uint32_t)pwrctrl->reg_infrasys_ddren_req_mask_b & 0x1) << 24) |
		(((uint32_t)pwrctrl->reg_infrasys_emi_req_mask_b & 0x1) << 25) |
		(((uint32_t)pwrctrl->reg_ipic_infra_req_mask_b & 0x1) << 26) |
		(((uint32_t)pwrctrl->reg_ipic_vrf18_req_mask_b & 0x1) << 27) |
		(((uint32_t)pwrctrl->reg_mcu_apsrc_req_mask_b & 0x1) << 28) |
		(((uint32_t)pwrctrl->reg_mcu_ddren_req_mask_b & 0x1) << 29) |
		(((uint32_t)pwrctrl->reg_mcu_emi_req_mask_b & 0x1) << 30));

	/* SPM_SRC_MASK_8 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_8,
		(((uint32_t)pwrctrl->reg_mcusys_apsrc_req_mask_b & 0xff) << 0) |
		(((uint32_t)pwrctrl->reg_mcusys_ddren_req_mask_b & 0xff) << 8) |
		(((uint32_t)pwrctrl->reg_mcusys_emi_req_mask_b & 0xff) << 16) |
		(((uint32_t)pwrctrl->reg_mcusys_infra_req_mask_b & 0xff) << 24));

	/* SPM_SRC_MASK_9 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_9,
		(((uint32_t)pwrctrl->reg_mcusys_pmic_req_mask_b & 0xff) << 0) |
		(((uint32_t)pwrctrl->reg_mcusys_srcclkena_mask_b & 0xff) << 8) |
		(((uint32_t)pwrctrl->reg_mcusys_vrf18_req_mask_b & 0xff) << 16) |
		(((uint32_t)pwrctrl->reg_md_apsrc_req_mask_b & 0x1) << 24) |
		(((uint32_t)pwrctrl->reg_md_ddren_req_mask_b & 0x1) << 25) |
		(((uint32_t)pwrctrl->reg_md_emi_req_mask_b & 0x1) << 26) |
		(((uint32_t)pwrctrl->reg_md_infra_req_mask_b & 0x1) << 27) |
		(((uint32_t)pwrctrl->reg_md_pmic_req_mask_b & 0x1) << 28) |
		(((uint32_t)pwrctrl->reg_md_srcclkena_mask_b & 0x1) << 29) |
		(((uint32_t)pwrctrl->reg_md_srcclkena1_mask_b & 0x1) << 30) |
		(((uint32_t)pwrctrl->reg_md_vcore_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_10 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_10,
		(((uint32_t)pwrctrl->reg_md_vrf18_req_mask_b & 0x1) << 0) |
		(((uint32_t)pwrctrl->reg_mdp_apsrc_req_mask_b & 0x1) << 1) |
		(((uint32_t)pwrctrl->reg_mdp_ddren_req_mask_b & 0x1) << 2) |
		(((uint32_t)pwrctrl->reg_mm_proc_apsrc_req_mask_b & 0x1) << 3) |
		(((uint32_t)pwrctrl->reg_mm_proc_ddren_req_mask_b & 0x1) << 4) |
		(((uint32_t)pwrctrl->reg_mm_proc_emi_req_mask_b & 0x1) << 5) |
		(((uint32_t)pwrctrl->reg_mm_proc_infra_req_mask_b & 0x1) << 6) |
		(((uint32_t)pwrctrl->reg_mm_proc_pmic_req_mask_b & 0x1) << 7) |
		(((uint32_t)pwrctrl->reg_mm_proc_srcclkena_mask_b & 0x1) << 8) |
		(((uint32_t)pwrctrl->reg_mm_proc_vrf18_req_mask_b & 0x1) << 9) |
		(((uint32_t)pwrctrl->reg_mmsys_apsrc_req_mask_b & 0x1) << 10) |
		(((uint32_t)pwrctrl->reg_mmsys_ddren_req_mask_b & 0x1) << 11) |
		(((uint32_t)pwrctrl->reg_mmsys_vrf18_req_mask_b & 0x1) << 12) |
		(((uint32_t)pwrctrl->reg_pcie0_apsrc_req_mask_b & 0x1) << 13) |
		(((uint32_t)pwrctrl->reg_pcie0_ddren_req_mask_b & 0x1) << 14) |
		(((uint32_t)pwrctrl->reg_pcie0_infra_req_mask_b & 0x1) << 15) |
		(((uint32_t)pwrctrl->reg_pcie0_srcclkena_mask_b & 0x1) << 16) |
		(((uint32_t)pwrctrl->reg_pcie0_vrf18_req_mask_b & 0x1) << 17) |
		(((uint32_t)pwrctrl->reg_pcie1_apsrc_req_mask_b & 0x1) << 18) |
		(((uint32_t)pwrctrl->reg_pcie1_ddren_req_mask_b & 0x1) << 19) |
		(((uint32_t)pwrctrl->reg_pcie1_infra_req_mask_b & 0x1) << 20) |
		(((uint32_t)pwrctrl->reg_pcie1_srcclkena_mask_b & 0x1) << 21) |
		(((uint32_t)pwrctrl->reg_pcie1_vrf18_req_mask_b & 0x1) << 22) |
		(((uint32_t)pwrctrl->reg_perisys_apsrc_req_mask_b & 0x1) << 23) |
		(((uint32_t)pwrctrl->reg_perisys_ddren_req_mask_b & 0x1) << 24) |
		(((uint32_t)pwrctrl->reg_perisys_emi_req_mask_b & 0x1) << 25) |
		(((uint32_t)pwrctrl->reg_perisys_infra_req_mask_b & 0x1) << 26) |
		(((uint32_t)pwrctrl->reg_perisys_pmic_req_mask_b & 0x1) << 27) |
		(((uint32_t)pwrctrl->reg_perisys_srcclkena_mask_b & 0x1) << 28) |
		(((uint32_t)pwrctrl->reg_perisys_vcore_req_mask_b & 0x1) << 29) |
		(((uint32_t)pwrctrl->reg_perisys_vrf18_req_mask_b & 0x1) << 30) |
		(((uint32_t)pwrctrl->reg_scp_apsrc_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_11 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_11,
		(((uint32_t)pwrctrl->reg_scp_ddren_req_mask_b & 0x1) << 0) |
		(((uint32_t)pwrctrl->reg_scp_emi_req_mask_b & 0x1) << 1) |
		(((uint32_t)pwrctrl->reg_scp_infra_req_mask_b & 0x1) << 2) |
		(((uint32_t)pwrctrl->reg_scp_pmic_req_mask_b & 0x1) << 3) |
		(((uint32_t)pwrctrl->reg_scp_srcclkena_mask_b & 0x1) << 4) |
		(((uint32_t)pwrctrl->reg_scp_vcore_req_mask_b & 0x1) << 5) |
		(((uint32_t)pwrctrl->reg_scp_vrf18_req_mask_b & 0x1) << 6) |
		(((uint32_t)pwrctrl->reg_srcclkeni_infra_req_mask_b & 0x3) << 7) |
		(((uint32_t)pwrctrl->reg_srcclkeni_pmic_req_mask_b & 0x3) << 9) |
		(((uint32_t)pwrctrl->reg_srcclkeni_srcclkena_mask_b & 0x3) << 11) |
		(((uint32_t)pwrctrl->reg_sspm_apsrc_req_mask_b & 0x1) << 13) |
		(((uint32_t)pwrctrl->reg_sspm_ddren_req_mask_b & 0x1) << 14) |
		(((uint32_t)pwrctrl->reg_sspm_emi_req_mask_b & 0x1) << 15) |
		(((uint32_t)pwrctrl->reg_sspm_infra_req_mask_b & 0x1) << 16) |
		(((uint32_t)pwrctrl->reg_sspm_pmic_req_mask_b & 0x1) << 17) |
		(((uint32_t)pwrctrl->reg_sspm_srcclkena_mask_b & 0x1) << 18) |
		(((uint32_t)pwrctrl->reg_sspm_vrf18_req_mask_b & 0x1) << 19) |
		(((uint32_t)pwrctrl->reg_ssr_apsrc_req_mask_b & 0x1) << 20) |
		(((uint32_t)pwrctrl->reg_ssr_ddren_req_mask_b & 0x1) << 21) |
		(((uint32_t)pwrctrl->reg_ssr_emi_req_mask_b & 0x1) << 22) |
		(((uint32_t)pwrctrl->reg_ssr_infra_req_mask_b & 0x1) << 23) |
		(((uint32_t)pwrctrl->reg_ssr_pmic_req_mask_b & 0x1) << 24) |
		(((uint32_t)pwrctrl->reg_ssr_srcclkena_mask_b & 0x1) << 25) |
		(((uint32_t)pwrctrl->reg_ssr_vrf18_req_mask_b & 0x1) << 26) |
		(((uint32_t)pwrctrl->reg_ufs_apsrc_req_mask_b & 0x1) << 27) |
		(((uint32_t)pwrctrl->reg_ufs_ddren_req_mask_b & 0x1) << 28) |
		(((uint32_t)pwrctrl->reg_ufs_emi_req_mask_b & 0x1) << 29) |
		(((uint32_t)pwrctrl->reg_ufs_infra_req_mask_b & 0x1) << 30) |
		(((uint32_t)pwrctrl->reg_ufs_pmic_req_mask_b & 0x1) << 31));

	/* SPM_SRC_MASK_12 */
	mmio_write_32((uintptr_t)&mtk_spm->spm_src_mask_12,
		(((uint32_t)pwrctrl->reg_ufs_srcclkena_mask_b & 0x1) << 0) |
		(((uint32_t)pwrctrl->reg_ufs_vrf18_req_mask_b & 0x1) << 1) |
		(((uint32_t)pwrctrl->reg_vdec_apsrc_req_mask_b & 0x1) << 2) |
		(((uint32_t)pwrctrl->reg_vdec_ddren_req_mask_b & 0x1) << 3) |
		(((uint32_t)pwrctrl->reg_vdec_emi_req_mask_b & 0x1) << 4) |
		(((uint32_t)pwrctrl->reg_vdec_infra_req_mask_b & 0x1) << 5) |
		(((uint32_t)pwrctrl->reg_vdec_pmic_req_mask_b & 0x1) << 6) |
		(((uint32_t)pwrctrl->reg_vdec_srcclkena_mask_b & 0x1) << 7) |
		(((uint32_t)pwrctrl->reg_vdec_vrf18_req_mask_b & 0x1) << 8) |
		(((uint32_t)pwrctrl->reg_venc_apsrc_req_mask_b & 0x1) << 9) |
		(((uint32_t)pwrctrl->reg_venc_ddren_req_mask_b & 0x1) << 10) |
		(((uint32_t)pwrctrl->reg_venc_emi_req_mask_b & 0x1) << 11) |
		(((uint32_t)pwrctrl->reg_venc_infra_req_mask_b & 0x1) << 12) |
		(((uint32_t)pwrctrl->reg_venc_pmic_req_mask_b & 0x1) << 13) |
		(((uint32_t)pwrctrl->reg_venc_srcclkena_mask_b & 0x1) << 14) |
		(((uint32_t)pwrctrl->reg_venc_vrf18_req_mask_b & 0x1) << 15) |
		(((uint32_t)pwrctrl->reg_ipe_apsrc_req_mask_b & 0x1) << 16) |
		(((uint32_t)pwrctrl->reg_ipe_ddren_req_mask_b & 0x1) << 17) |
		(((uint32_t)pwrctrl->reg_ipe_emi_req_mask_b & 0x1) << 18) |
		(((uint32_t)pwrctrl->reg_ipe_infra_req_mask_b & 0x1) << 19) |
		(((uint32_t)pwrctrl->reg_ipe_pmic_req_mask_b & 0x1) << 20) |
		(((uint32_t)pwrctrl->reg_ipe_srcclkena_mask_b & 0x1) << 21) |
		(((uint32_t)pwrctrl->reg_ipe_vrf18_req_mask_b & 0x1) << 22) |
		(((uint32_t)pwrctrl->reg_ufs_vcore_req_mask_b & 0x1) << 23));

	/* SPM_EVENT_CON_MISC */
	mmio_write_32((uintptr_t)&mtk_spm->spm_event_con_misc,
		(((uint32_t)pwrctrl->reg_srcclken_fast_resp & 0x1) << 0) |
		(((uint32_t)pwrctrl->reg_csyspwrup_ack_mask & 0x1) << 1));

	/* SPM_WAKEUP_EVENT_MASK */
	mmio_write_32((uintptr_t)&mtk_spm->spm_wakeup_event_mask,
		(((uint32_t)pwrctrl->reg_wakeup_event_mask & 0xffffffff) << 0));

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	mmio_write_32((uintptr_t)&mtk_spm->spm_wakeup_event_ext_mask,
		(((uint32_t)pwrctrl->reg_ext_wakeup_event_mask & 0xffffffff) << 0));
}

static void spm_register_init(void)
{
	/* SPM related clk setting  */
	mmio_setbits_32(INFRA_BUS_DCM_CTRL,
		  RG_AXI_DCM_DIS_EN | RG_PLLCK_SEL_NO_SPM);

	/* Enable scpsys(SPM) clock off control, 0: Topck AO */
	mmio_setbits_32(CLK_SCP_CFG_0, GENMASK(9, 0));
	mmio_clrsetbits_32(CLK_SCP_CFG_1, BIT(12) | GENMASK(3, 2), GENMASK(1, 0));

	/* enable register control */
	mmio_write_32((uintptr_t)(uintptr_t)&mtk_spm->poweron_config_set,
		      SPM_REGWR_CFG_KEY | BCLK_CG_EN_LSB);

	/* Power on spm sram */
	mmio_clrbits_32((uintptr_t)&mtk_spm->spm_sram_con, GENMASK(19, 16));

	/* SRAM Workaround for Sleep FSM */
	mmio_setbits_32((uintptr_t)&mtk_spm->spm_sram_sleep_ctrl, BIT(9));

	/* init power control register */
	/* dram will set this register */
	mmio_write_32((uintptr_t)&mtk_spm->spm_power_on_val1, POWER_ON_VAL1_DEF);

	/* reset PCM */
	mmio_write_32((uintptr_t)&mtk_spm->pcm_con0,
		      SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | PCM_SW_RESET_LSB);
	mmio_write_32((uintptr_t)&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);
	mmio_write_32((uintptr_t)&mtk_spm->pcm_con1, SPM_REGWR_CFG_KEY |
		REG_SPM_APB_INTERNAL_EN_LSB | REG_SSPM_APB_P2P_EN_LSB);
	/* initial SPM CLK control register */
	mmio_clrsetbits_32((uintptr_t)&mtk_spm->spm_clk_con, GENMASK(23, 16),
			   BIT(28) | GENMASK(6, 4));
	mmio_setbits_32((uintptr_t)&mtk_spm->spm_clk_con1, BIT(0));

	/* clean wakeup event raw status */
	mmio_write_32((uintptr_t)&mtk_spm->spm_wakeup_event_mask, SPM_WAKEUP_EVENT_MASK_DEF);

	/* clean ISR status */
	mmio_write_32((uintptr_t)&mtk_spm->spm_irq_mask, ISRM_ALL);
	mmio_write_32((uintptr_t)&mtk_spm->spm_irq_sta, ISRC_ALL);
	mmio_write_32((uintptr_t)&mtk_spm->spm_swint_clr, PCM_SW_INT_ALL);

	/* set SPM_AP_STANDBY_CON[5] csyspower_req_mask = 1 */
	mmio_setbits_32((uintptr_t)&mtk_spm->spm_ap_standby_con, BIT(5));

	/* disable r0 and r7 to control power */
	mmio_write_32((uintptr_t)&mtk_spm->pcm_pwr_io_en, 0);

	/* DDR EN de-bounce length to 5us */
	mmio_write_32((uintptr_t)&mtk_spm->ddren_dbc_con, 0x154 | REG_DDREN_DBC_EN_LSB);

	/* Init for SPM Resource ACK */
	mmio_write_32((uintptr_t)&mtk_spm->spm_resource_ack_con0, 0xCC4E4ECC);
	mmio_write_32((uintptr_t)&mtk_spm->spm_resource_ack_con1, 0x00CCCCCC);
	mmio_write_32((uintptr_t)&mtk_spm->spm_resource_ack_mask0, 0x00000000);
	mmio_write_32((uintptr_t)&mtk_spm->spm_resource_ack_mask1, 0x00000000);
	mmio_write_32((uintptr_t)&mtk_spm->spm_resource_ack_mask2, 0x00000000);
	mmio_write_32((uintptr_t)&mtk_spm->spm_resource_ack_mask3, 0x00000000);
	mmio_write_32((uintptr_t)&mtk_spm->spm_resource_ack_mask4, 0x00000000);
	mmio_write_32((uintptr_t)&mtk_spm->spm_resource_ack_mask5, 0x00000000);
	mmio_write_32((uintptr_t)&mtk_spm->spm_resource_ack_mask6, 0x00000000);

	/* Init VCORE DVFS Status */
	mmio_clrsetbits_32((uintptr_t)&mtk_spm->spm_dvfs_con,
			   SPM_DVFS_FORCE_ENABLE_LSB, SPM_DVFSRC_ENABLE_LSB);
	mmio_write_32((uintptr_t)&mtk_spm->spm_dvfs_level, 0x00000001);
	mmio_write_32((uintptr_t)&mtk_spm->spm_dvs_dfs_level, 0x00010001);

	mmio_write_32((uintptr_t)&mtk_spm->spm_ack_chk_sel_3, SPM_ACK_CHK_3_SEL_HW_S1);
	mmio_write_32((uintptr_t)&mtk_spm->spm_ack_chk_timer_3, SPM_ACK_CHK_3_HW_S1_CNT);
	spm_hw_s1_state_monitor_pause();
}

static void spm_parse_firmware(struct mtk_mcu *mcu)
{
	size_t file_size, copy_size;
	int offset;
	uint16_t firmware_size;
	struct dyna_load_pcm *pcm = (struct dyna_load_pcm *)mcu->priv;

	file_size = mcu->run_size;

	/*
	 * spmfw layout:
	 *   uint16_t firmware_size
	 *   uint32_t binary[firmware_size]
	 *   struct pcm_desc descriptor
	 *   char *version
	 */

	/* Firmware size */
	offset = 0;
	copy_size = sizeof(firmware_size);
	memcpy(&firmware_size, mcu->load_buffer + offset, copy_size);
	NOTICE("SPM: binary array size = 0x%x\n", firmware_size);

	/* Binary */
	offset = SPMFW_HEADER_SIZE;	/* binary start offset */
	copy_size = firmware_size * sizeof(uint32_t);
	assert(offset < file_size);
	pcm->buf = (uint8_t *) (mcu->load_buffer + offset);

	/* Descriptor */
	offset += copy_size;
	assert(offset < file_size);
	copy_size = sizeof(pcm->desc);
	memcpy(&pcm->desc, mcu->load_buffer + offset, copy_size);

	/* Firmware size and total words need to be the same */
	assert(firmware_size == pcm->desc.total_words);

	/* Version */
	offset += copy_size;
	assert(offset < file_size);

	NOTICE("SPM: spmfw version: %s\n", (char *)mcu->load_buffer + offset);
}

static void spm_code_swapping(void)
{
	uint32_t con1;
	/* int retry = 0, timeout = 5000; */

	con1 = mmio_read_32(SPM_WAKEUP_EVENT_MASK);

	mmio_write_32(SPM_WAKEUP_EVENT_MASK, (con1 & ~(0x1)));
	mmio_write_32(SPM_CPU_WAKEUP_EVENT, 1);
	mmio_write_32(SPM_CPU_WAKEUP_EVENT, 0);
	mmio_write_32(SPM_WAKEUP_EVENT_MASK, con1);
}

static void spm_reset_and_init_pcm(void)
{
	uint32_t con1;
	unsigned char first_load_fw = true;

	/* check the SPM FW is run or not */
	if (mmio_read_32((uintptr_t)&mtk_spm->md32pcm_cfgreg_sw_rstn) & 0x1)
		first_load_fw = false;

	if (!first_load_fw) {
		/* SPM code swapping */
		spm_code_swapping();

		/* Backup PCM r0 -> SPM_POWER_ON_VAL0 before `reset PCM` */
		mmio_write_32(SPM_POWER_ON_VAL0, mmio_read_32(MD32PCM_SCU_CTRL0));
	}

	/* disable r0 and r7 to control power */
	mmio_write_32(PCM_PWR_IO_EN, 0);

	/* disable pcm timer after leaving FW */
	mmio_clrsetbits_32(PCM_CON1, REG_PCM_TIMER_EN_LSB, SPM_REGWR_CFG_KEY);

	/* reset PCM */
	mmio_write_32(PCM_CON0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | PCM_SW_RESET_LSB);
	mmio_write_32(PCM_CON0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	/* init PCM_CON1 (disable PCM timer but keep PCM WDT setting) */
	con1 = mmio_read_32(PCM_CON1) & (REG_PCM_WDT_WAKE_LSB);
	mmio_write_32(PCM_CON1, con1 | SPM_REGWR_CFG_KEY |
			REG_SPM_APB_INTERNAL_EN_LSB | REG_SSPM_APB_P2P_EN_LSB);
}

static void spm_kick_im_to_fetch(const struct dyna_load_pcm *pcm)
{
	uintptr_t ptr;
	uint32_t dmem_words;
	uint32_t pmem_words;
	uint32_t total_words;
	uint32_t pmem_start;
	uint32_t dmem_start;

	ptr = (uintptr_t) pcm->buf + 0x40000000;
	pmem_words = pcm->desc.pmem_words;
	total_words = pcm->desc.total_words;
	dmem_words = total_words - pmem_words;
	pmem_start = pcm->desc.pmem_start;
	dmem_start = pcm->desc.dmem_start;

	NOTICE("%s: ptr = 0x%lx, pmem/dmem words = 0x%x/0x%x\n",
	       __func__, (long)ptr, pmem_words, dmem_words);

	/* DMA needs 16-byte aligned source data. */
	assert(ptr % 16 == 0);

	if (mmio_read_32(MD32PCM_DMA0_SRC) != ptr
	    || mmio_read_32(MD32PCM_DMA0_DST) != pmem_start
	    || mmio_read_32(MD32PCM_DMA0_WPPT) != pmem_words
	    || mmio_read_32(MD32PCM_DMA0_WPTO) != dmem_start
	    || mmio_read_32(MD32PCM_DMA0_COUNT) != total_words
	    || mmio_read_32(MD32PCM_DMA0_CON) != 0x0003820E) {
		mmio_write_32(MD32PCM_DMA0_SRC, ptr);
		mmio_write_32(MD32PCM_DMA0_DST, pmem_start);
		mmio_write_32(MD32PCM_DMA0_WPPT, pmem_words);
		mmio_write_32(MD32PCM_DMA0_WPTO, dmem_start);
		mmio_write_32(MD32PCM_DMA0_COUNT, total_words);
		mmio_write_32(MD32PCM_DMA0_CON, 0x0003820E);
		mmio_write_32(MD32PCM_DMA0_START, 0x00008000);
	}

	/* kick IM to fetch (only toggle IM_KICK) */
	mmio_setbits_32(PCM_CON0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);
}

static void spm_init_pcm_register(void)
{
	mmio_write_32((uintptr_t)&mtk_spm->pcm_pwr_io_en, 0);
}

static void spm_set_wakeup_event(const struct pwr_ctrl *pwrctrl)
{
	uint32_t val, mask;

	/* toggle event counter clear */
	mmio_write_32((uintptr_t)&mtk_spm->spm_event_counter_clear, REG_SPM_EVENT_COUNTER_CLR_LSB);
	/* toggle for reset SYS TIMER start point */
	mmio_setbits_32((uintptr_t)&mtk_spm->sys_timer_con, SYS_TIMER_START_EN_LSB);

	if (pwrctrl->timer_val_cust == 0)
		val = pwrctrl->timer_val ? pwrctrl->timer_val : PCM_TIMER_SUSPEND;
	else
		val = pwrctrl->timer_val_cust;

	mmio_write_32((uintptr_t)&mtk_spm->pcm_timer_val, val);
	mmio_setbits_32((uintptr_t)&mtk_spm->pcm_con1, SPM_REGWR_CFG_KEY | REG_PCM_TIMER_EN_LSB);

	/* unmask AP wakeup source */
	if (pwrctrl->wake_src_cust == 0)
		mask = pwrctrl->wake_src;
	else
		mask = pwrctrl->wake_src_cust;

	if (pwrctrl->reg_csyspwrup_ack_mask)
		mask &= ~R12_CSYSPWREQ_B;
	mmio_write_32((uintptr_t)&mtk_spm->spm_wakeup_event_mask, ~mask);

	/* unmask SPM ISR (keep TWAM setting) */
	mmio_setbits_32((uintptr_t)&mtk_spm->spm_irq_mask, ISRM_RET_IRQ_AUX);

	/* toggle event counter clear */
	mmio_write_32((uintptr_t)&mtk_spm->spm_event_counter_clear, 0);
	/* toggle for reset SYS TIMER start point */
	mmio_clrbits_32((uintptr_t)&mtk_spm->sys_timer_con, SYS_TIMER_START_EN_LSB);
}

void spm_set_pcm_flags(const struct pwr_ctrl *pwrctrl)
{
	uint32_t pcm_flags = pwrctrl->pcm_flags, pcm_flags1 = pwrctrl->pcm_flags1;

	/* Set PCM flags and data */
	if (pwrctrl->pcm_flags_cust_clr != 0)
		pcm_flags &= ~pwrctrl->pcm_flags_cust_clr;
	if (pwrctrl->pcm_flags_cust_set != 0)
		pcm_flags |= pwrctrl->pcm_flags_cust_set;
	if (pwrctrl->pcm_flags1_cust_clr != 0)
		pcm_flags1 &= ~pwrctrl->pcm_flags1_cust_clr;
	if (pwrctrl->pcm_flags1_cust_set != 0)
		pcm_flags1 |= pwrctrl->pcm_flags1_cust_set;

	mmio_write_32((uintptr_t)&mtk_spm->spm_sw_flag_0, pcm_flags);
	mmio_write_32((uintptr_t)&mtk_spm->spm_sw_flag_1, pcm_flags1);
	mmio_write_32((uintptr_t)&mtk_spm->spm_sw_rsv[7], pcm_flags);
	mmio_write_32((uintptr_t)&mtk_spm->spm_sw_rsv[8], pcm_flags1);
}

static void spm_kick_pcm_to_run(const struct pwr_ctrl *pwrctrl)
{
	/* Waiting for loading SPMFW done*/
	while (mmio_read_32((uintptr_t)&mtk_spm->md32pcm_dma0_rlct) != 0x0)
		;

	/* In the new SOC design, this part has been simplified */
	spm_set_pcm_flags(pwrctrl);

	/* Kick PCM to run (only toggle PCM_KICK) */
	mmio_setbits_32((uintptr_t)&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	/* Reset md32pcm */
	mmio_setbits_32((uintptr_t)&mtk_spm->md32pcm_cfgreg_sw_rstn, 0x1);

	/* Waiting for SPM init done */
	udelay(SPM_INIT_DONE_US);
}

static void reset_spm(struct mtk_mcu *mcu)
{
	struct dyna_load_pcm *pcm = (struct dyna_load_pcm *)mcu->priv;

	INFO("SPM: %s done\n", __func__);
	spm_parse_firmware(mcu);
	spm_reset_and_init_pcm();
	spm_kick_im_to_fetch(pcm);
	spm_init_pcm_register();
	spm_set_wakeup_event(&spm_init_ctrl);
	spm_kick_pcm_to_run(&spm_init_ctrl);
}

static struct mtk_mcu spm = {
	.firmware_name = "spm_firmware.bin",
	.reset = reset_spm,
	.load_buffer = spm_bin,
};

static void spm_set_sysclk_settle(void)
{
	uint32_t settle;

	mmio_write_32(SPM_CLK_SETTLE, SPM_SYSCLK_SETTLE);
	settle = mmio_read_32(SPM_CLK_SETTLE);

	INFO("md_settle = %u, settle = %u\n", SPM_SYSCLK_SETTLE, settle);
}

int spmfw_init(void)
{
	struct dyna_load_pcm pcm;

	spm_register_init();
	spm_set_power_control(&spm_init_ctrl);
	spm_set_sysclk_settle();

	spm.priv = (void *)&pcm;
	spm.run_size = spm_bin_len;
	if (mtk_init_mcu(&spm))
		ERROR("SPM: %s: failed in mtk_init_mcu\n", __func__);

	INFO("SPM: %s done, spm pc = 0x%x\n", __func__,
	     mmio_read_32((uintptr_t)&mtk_spm->md32pcm_pc));

	return 0;
}

MTK_ARCH_INIT(spmfw_init);
