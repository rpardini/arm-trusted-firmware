/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 MediaTek Inc.
 */

#pragma once

#define SPM_PROJECT_CODE      0xB16

#define STA_POWER_DOWN        0
#define STA_POWER_ON          1

#define SET_BUS_PROTECT	      1
#define RELEASE_BUS_PROTECT   0

void spm_mtcmos_ctrl_conn(int state);
void spm_mtcmos_ctrl_ufs0(int state);
void spm_mtcmos_ctrl_ufs0_phy(int state);
void spm_mtcmos_ctrl_audio(int state);
void spm_mtcmos_ctrl_adsp_top_shutdown(int state);
void spm_mtcmos_ctrl_adsp_top_dormant(int state);
void spm_mtcmos_ctrl_adsp_infra(int state);
void spm_mtcmos_ctrl_adsp_ao(int state);
void spm_mtcmos_ctrl_isp_img1(int state);
void spm_mtcmos_ctrl_isp_img2(int state);
void spm_mtcmos_ctrl_isp_ipe(int state);
void spm_mtcmos_ctrl_vde0(int state);
void spm_mtcmos_ctrl_ven0(int state);
void spm_mtcmos_ctrl_cam_main(int state);
void spm_mtcmos_ctrl_cam_suba(int state);
void spm_mtcmos_ctrl_cam_subb(int state);
void spm_mtcmos_ctrl_mdp0(int state);
void spm_mtcmos_ctrl_disp(int state);
void spm_mtcmos_ctrl_mm_infra(int state);
void spm_mtcmos_ctrl_dp_tx(int state);
void spm_mtcmos_ctrl_csi_rx(int state);
void spm_mtcmos_ctrl_ssusb(int state);
void spm_mtcmos_ctrl_mfg0(int state);
void spm_mtcmos_ctrl_mfg1(int state);
void spm_mtcmos_ctrl_mfg2(int state);
void spm_mtcmos_ctrl_mfg3(int state);
void spm_mtcmos_ctrl_edp_tx_shutdown(int state);
void spm_mtcmos_ctrl_edp_tx_dormant(int state);
void spm_mtcmos_ctrl_pcie(int state);
void spm_mtcmos_ctrl_pcie_phy(int state);
void spm_power_on(void);
