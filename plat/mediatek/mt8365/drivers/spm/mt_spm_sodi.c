#include <arch_helpers.h>
#include <lib/mmio.h>
#include <mt_spm.h>
#include <mt_spm_internal.h>
#include <mt_spm_pmic_wrap.h>
#include <mt_spm_reg.h>
#include <mt_spm_vcorefs.h>
#include <mtk_mcdi.h>
#include <mtk_plat_common.h>
#include <platform_def.h>
//#include <plat_mt_cirq.h>
#include <pmic_wrap_init.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static unsigned int resource_usage;
static unsigned char is_sleep_sodi;

#define MIN_GPT_TIME_IDLE	(0x9858) /* 39000 / 13M ~= 3ms */
#define LOOP_THRESHOLD		(51)

#define WAKE_SRC_FOR_SODI \
	(WAKE_SRC_R12_PCM_TIMER | \
	 WAKE_SRC_R12_KP_F32K_WAKEUP_EVENT_2_1 | \
	 WAKE_SRC_R12_WDT_32K_EVENT_B | \
	 WAKE_SRC_R12_APXGPT_EVENT_B | \
	 WAKE_SRC_R12_CONN2AP_WAKEUP_B | \
	 WAKE_SRC_R12_EINT_EVENT_B | \
	 WAKE_SRC_R12_CONN_WDT_IRQ | \
	 WAKE_SRC_R12_IRRX_WAKEUP | \
	 WAKE_SRC_R12_LOW_BATTERY_IRQ_B | \
	 WAKE_SRC_R12_DSP_WAKEUP_B | \
	 WAKE_SRC_R12_WDT_WAKEUP_EVENT_B | \
	 WAKE_SRC_R12_USB_CONNECT | \
	 WAKE_SRC_R12_USB_POWERDWN_B | \
	 WAKE_SRC_R12_SYS_TIMER_EVENT_B | \
	 WAKE_SRC_R12_EINT_SECURE | \
	 WAKE_SRC_R12_NIC_IRQ | \
	 WAKE_SRC_R12_AFE_IRQ_MCU_B | \
	 WAKE_SRC_R12_THERM_CTRL_EVENT_B | \
	 WAKE_SRC_R12_SYS_CIRQ_B | \
	 WAKE_SRC_R12_MSDC2_WAKEUP | \
	 WAKE_SRC_R12_DSPWDT_IRQ_B | \
	 WAKE_SRC_R12_SEJ)

#define SODI_PCM_FLAGS \
	(SPM_FLAG_SODI_CG_MODE | SPM_FLAG_SODI_OPTION)

static struct pwr_ctrl sodi_ctrl = {
	.wake_src = WAKE_SRC_FOR_SODI,
	.pcm_flags = SODI_PCM_FLAGS,

	/* Auto-gen Start */

	/* SPM_AP_STANDBY_CON */
	.wfi_op = 0x1,
	.mp0_cputop_idle_mask = 0,
	.mp1_cputop_idle_mask = 0,
	.mcusys_idle_mask = 0,
	.mm_mask_b = 0,
	.md_ddr_en_0_dbc_en = 0,
	.md_ddr_en_1_dbc_en = 0,
	.md_mask_b = 0,
	.sspm_mask_b = 0,
	.scp_mask_b = 0,
	.srcclkeni_mask_b = 0,
	.md_apsrc_1_sel = 0,
	.md_apsrc_0_sel = 0,
	.conn_ddr_en_dbc_en = 0,
	.conn_mask_b = 0x1,
	.conn_apsrc_sel = 0,
	.conn_srcclkena_sel_mask = 0x1,

	/* SPM_SRC_REQ */
	.spm_apsrc_req = 0,
	.spm_f26m_req = 0,
	.spm_infra_req = 0,
	.spm_vrf18_req = 0,
	.spm_ddren_req = 0,
	.spm_rsv_src_req = 0,
	.spm_ddren_2_req = 0,
	.cpu_md_dvfs_sop_force_on = 0,

	/* SPM_SRC_MASK */
	.csyspwreq_mask = 0,
	.ccif0_md_event_mask_b = 0,
	.ccif0_ap_event_mask_b = 0,
	.ccif1_md_event_mask_b = 0,
	.ccif1_ap_event_mask_b = 0,
	.ccif2_md_event_mask_b = 0,
	.ccif2_ap_event_mask_b = 0,
	.ccif3_md_event_mask_b = 0,
	.ccif3_ap_event_mask_b = 0,
	.md_srcclkena_0_infra_mask_b = 0,
	.md_srcclkena_1_infra_mask_b = 0,
	.conn_srcclkena_infra_mask_b = 0x1,
	.ufs_infra_req_mask_b = 0,
	.srcclkeni_infra_mask_b = 0,
	.md_apsrc_req_0_infra_mask_b = 0,
	.md_apsrc_req_1_infra_mask_b = 0,
	.conn_apsrcreq_infra_mask_b = 0x1,
	.ufs_srcclkena_mask_b = 0,
	.md_vrf18_req_0_mask_b = 0,
	.md_vrf18_req_1_mask_b = 0,
	.ufs_vrf18_req_mask_b = 0,
	.gce_vrf18_req_mask_b = 0x1,
	.conn_infra_req_mask_b = 0x1,
	.gce_apsrc_req_mask_b = 0x1,
	.disp0_apsrc_req_mask_b = 0x1,
	.disp1_apsrc_req_mask_b = 0x1,
	.mfg_req_mask_b = 0,
	.vdec_req_mask_b = 0,
	.mcu_apsrcreq_infra_mask_b = 0x1,

	/* SPM_SRC2_MASK */
	.md_ddr_en_0_mask_b = 0,
	.md_ddr_en_1_mask_b = 0,
	.conn_ddr_en_mask_b = 0x1,
	.ddren_md32_apsrc_req_mask_b = 0,
	.ddren_scp_apsrc_req_mask_b = 0,
	.disp0_ddren_mask_b = 0x1,
	.disp1_ddren_mask_b = 0x1,
	.gce_ddren_mask_b = 0x1,
	.ddren_emi_self_refresh_ch0_mask_b = 0,
	.ddren_emi_self_refresh_ch1_mask_b = 0,
	.mcu_apsrc_req_mask_b = 0,
	.mcu_ddren_mask_b = 0,

	/* SPM_SRC3_MASK */
	.md_ddr_en_2_0_mask_b = 0,
	.md_ddr_en_2_1_mask_b = 0,
	.conn_ddr_en_2_mask_b = 0x1,
	.ddren2_md32_apsrc_req_mask_b = 0,
	.ddren2_scp_apsrc_req_mask_b = 0,
	.disp0_ddren2_mask_b = 0x1,
	.disp1_ddren2_mask_b = 0x1,
	.gce_ddren2_mask_b = 0x1,
	.ddren2_emi_self_refresh_ch0_mask_b = 0x1,
	.ddren2_emi_self_refresh_ch1_mask_b = 0x1,
	.mcu_ddren_2_mask_b = 0x1,

	/* SPARE_SRC_REQ_MASK */
	.spare1_ddren_mask_b = 0x1,
	.spare1_apsrc_req_mask_b = 0x1,
	.spare1_vrf18_req_mask_b = 0x1,
	.spare1_infra_req_mask_b = 0x1,
	.spare1_srcclkena_mask_b = 0x1,
	.spare1_ddren2_mask_b = 0x1,
	.spare2_ddren_mask_b = 0x1,
	.spare2_apsrc_req_mask_b = 0,
	.spare2_vrf18_req_mask_b = 0,
	.spare2_infra_req_mask_b = 0,
	.spare2_srcclkena_mask_b = 0,
	.spare2_ddren2_mask_b = 0x1,

	/* SPM_WAKEUP_EVENT_MASK */
	.spm_wakeup_event_mask = 0xC0000000,

	/* SPM_WAKEUP_EVENT_EXT_MASK */
	.spm_wakeup_event_ext_mask = 0xFFFFFFFF,

	/* MP0_CPU0_WFI_EN */
	.mp0_cpu0_wfi_en = 1,

	/* MP0_CPU1_WFI_EN */
	.mp0_cpu1_wfi_en = 1,

	/* MP0_CPU2_WFI_EN */
	.mp0_cpu2_wfi_en = 1,

	/* MP0_CPU3_WFI_EN */
	.mp0_cpu3_wfi_en = 1,

	/* MP1_CPU0_WFI_EN */
	.mp1_cpu0_wfi_en = 0,

	/* MP1_CPU1_WFI_EN */
	.mp1_cpu1_wfi_en = 0,

	/* MP1_CPU2_WFI_EN */
	.mp1_cpu2_wfi_en = 0,

	/* MP1_CPU3_WFI_EN */
	.mp1_cpu3_wfi_en = 0,

	/* idle control */
	.idle_switch = 1,
	.infra1_idle_mask = 0x0004091e,
	.infra0_idle_mask = 0x10bf8100,
	.infra2_idle_mask = 0x08000001,
	.infra3_idle_mask = 0x00000f80,
	.infra4_idle_mask = 0x00000e70,
	.mmsys0_idle_mask = 0x7c38005f,
	.mmsys1_idle_mask = 0x0000000e,
	.cam_idle_mask = 0x00000312,
	.mfg_idle_mask = 0x00000312,
	.venc_idle_mask = 0x00000112,
	.vdec_idle_mask = 0x00000112,
	.apu_idle_mask = 0x00000312,

	/* Auto-gen End */
};

struct spm_lp_scen __spm_sodi = {
	.pwrctrl = &sodi_ctrl,
};

void spm_sodi_args(uint64_t x1, uint64_t x2, uint64_t x3)
{
	struct pwr_ctrl *pwrctrl;

	pwrctrl = __spm_sodi.pwrctrl;
	pwrctrl->pcm_flags = x1;
	pwrctrl->pcm_flags1 = x2;

	/* get spm resource request from kernel */
	resource_usage = x3;
}

int spm_is_sodi_resume(void)
{
	return is_sleep_sodi;
}
