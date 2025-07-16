// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#include <lib/mmio.h>
#include <common/debug.h>
#include <platform_def.h>
#include <drivers/delay_timer.h>
#include <spm_mtcmos.h>
#include "include/pll.h"
#include "libdram.h"

#define CLKMGR_BRINGUP			0

#define DRV_WriteReg32(addr, data)  mmio_write_32(addr, data)
#define DRV_Reg32(addr)             mmio_read_32(addr)

/* fmeter concern */
#define FMETER_CHK			1

#define VLP_FM_WAIT_TIME		40 /* ~= 38.64ns * 1023 */

#define BITS_PER_LONG			32

#ifndef GENMASK
#define GENMASK(h, l) \
	((0xffffffff << (l)) & (0xffffffff >> (BITS_PER_LONG - 1 - (h))))
#endif

#define do_div(a, b)			(a/b)

#define BITS(nr)			(1UL << (nr))

#define MHZ				(1000)
#define FMIN				(1500UL * MHZ)
#define FMAX				(3800UL * MHZ)
#define FIN_RATE			(26 * MHZ)
#define CON1_PCW_CHG			BITS(31)
#define POSTDIV_MASK			0x7
#define POSTDIV_SHIFT			24
#define PCW_FBITS			14

#define FM_PLL_CK			0
#define FM_PLL_CKDIV_CK			1
#define FM_CKDIV_SHIFT			(7)
#define FM_CKDIV_MASK			GENMASK(10, 7)
#define FM_POSTDIV_SHIFT		(24)
#define FM_POSTDIV_MASK			GENMASK(26, 24)

#define EFUSE_MTCMOS_DPTX_DIS_S1B10 BITS(10)
#define EFUSE_MTCMOS_ADSP_TOP_DIS_S1B15 BITS(15)
#define EFUSE_MTCMOS_ADSP_INFRA_DIS_S1B16 BITS(16)
#define EFUSE_MTCMOS_ADSP_AO_DIS_S1B17 BITS(17)

#define EFUSE_MTCMOS_MFG0_DIS_S2B0 BITS(0)
#define EFUSE_MTCMOS_MFG1_DIS_S2B1 BITS(1)
#define EFUSE_MTCMOS_MFG2_DIS_S2B2 BITS(2)
#define EFUSE_MTCMOS_MFG3_DIS_S2B3 BITS(3)

#define EFUSE_MTCMOS_EDPTX_DIS_S2B15 BITS(15)
#define EFUSE_MTCMOS_ISP_IMG1_DIS_S2B18 BITS(18)
#define EFUSE_MTCMOS_ISP_IMG2_DIS_S2B19 BITS(19)
#define EFUSE_MTCMOS_ISP_IPE_DIS_S2B20 BITS(20)
#define EFUSE_MTCMOS_CAM_RAWA_DIS_S2B21 BITS(21)
#define EFUSE_MTCMOS_CAM_RAWB_DIS_S2B22 BITS(22)
#define EFUSE_MTCMOS_CAM_MAIN_DIS_S2B23 BITS(23)

#define EFUSE_MTCMOS_PCIE_DIS_S3B24 BITS(24)

#define EFUSE_MTCMOS_SSUSB_DIS_S4B26 BITS(26)
#define EFUSE_MTCMOS_CSI_DIS_S4B27 BITS(27)

#if FMETER_CHK
struct fmeter_data {
    enum fmeter_type type;
    const char *name;
    uint64_t pll_con0;
    uint64_t pll_con1;
    uint64_t con0;
    uint64_t con1;
};

static struct fmeter_data fmd[] = {
    [APLL1_CTRL] = {APLL1_CTRL, "apll1_ctrl",
            APLL1_CON0, 0, 0, 0},
    [APLL2_CTRL] = {APLL2_CTRL, "apll2_ctrl",
            APLL2_CON0, 0, 0, 0},
    [ARMPLL_BL_CTRL] = {ARMPLL_BL_CTRL, "armpll_bl_ctrl",
            ARMPLL_BL_CON0, 0, 0, 0},
    [ARMPLL_LL_CTRL] = {ARMPLL_LL_CTRL, "armpll_ll_ctrl",
            ARMPLL_LL_CON0, 0, 0, 0},
    [CCIPLL_CTRL] = {CCIPLL_CTRL, "ccipll_ctrl",
            CCIPLL_CON0, 0, 0, 0},
    [MAINPLL_CTRL] = {MAINPLL_CTRL, "mainpll_ctrl",
            MAINPLL_CON0, 0, 0, 0},
    [MMPLL_CTRL] = {MMPLL_CTRL, "mmpll_ctrl",
            MMPLL_CON0, 0, 0, 0},
    [MSDCPLL_CTRL] = {MSDCPLL_CTRL, "msdcpll_ctrl",
            MSDCPLL_CON0, 0, 0, 0},
    [UFSPLL_CTRL] = {UFSPLL_CTRL, "ufspll_ctrl",
            UFSPLL_CON0, 0, 0, 0},
    [UNIVPLL_CTRL] = {UNIVPLL_CTRL, "univpll_ctrl",
            UNIVPLL_CON0, 0, 0, 0},
    [EMIPLL_CTRL] = {EMIPLL_CTRL, "emipll_ctrl",
            EMIPLL_CON0, 0, 0, 0},
    [TVDPLL1_CTRL] = {TVDPLL1_CTRL, "tvdpll1_ctrl",
            TVDPLL1_CON0, 0, 0, 0},
    [TVDPLL2_CTRL] = {TVDPLL2_CTRL, "tvdpll2_ctrl",
            TVDPLL2_CON0, 0, 0, 0},
    [MFGPLL_CTRL] = {MFGPLL_CTRL, "mfgpll_ctrl",
            MFGPLL_CON0, 0, 0, 0},
    [ETHPLL_CTRL] = {ETHPLL_CTRL, "ethpll_ctrl",
            ETHPLL_CON0, 0, 0, 0},
    [APUPLL_CTRL] = {APUPLL_CTRL, "apupll_ctrl",
            APUPLL_CON0, 0, 0, 0},
    [APUPLL2_CTRL] = {APUPLL2_CTRL, "apupll2_ctrl",
            APUPLL2_CON0, 0, 0, 0},
    [VLP_CKSYS_TOP_CTRL] = {VLP_CKSYS_TOP_CTRL, "VLP_CKSYS_TOP_CTRL",
            0, 0, VLP_FQMTR_CON0, VLP_FQMTR_CON1},
};

const int ckgen_postdiv[] =
{
    /* 00 - 04 */	0, 1, 1, 1, 1,
    /* 05 - 09 */	1, 1, 1, 1, 1,
    /* 10 - 14 */	1, 1, 1, 1, 1,
    /* 15 - 19 */	1, 1, 1, 1, 1,
    /* 20 - 24 */	1, 1, 1, 1, 1,
    /* 25 - 29 */	1, 1, 1, 1, 1,
    /* 30 - 34 */	1, 1, 1, 1, 1,
    /* 35 - 39 */	1, 1, 1, 1, 1,
    /* 40 - 44 */	1, 1, 1, 1, 1,
    /* 45 - 49 */	1, 1, 1, 1, 1,
    /* 50 - 54 */	1, 1, 1, 1, 1,
    /* 55 - 59 */	1, 1, 1, 1, 1,
    /* 60 - 64 */	1, 1, 1, 1, 1,
    /* 65 - 69 */	1, 1, 1, 1, 1,
    /* 70 - 74 */	1, 1, 1, 1, 1,
    /* 75 - 78 */	1, 1, 1, 1,
};

const int ckgen_clkdiv[] =
{
    /* 00 - 04 */	0, 0, 0, 0, 0,
    /* 05 - 09 */	0, 0, 0, 0, 0,
    /* 10 - 14 */	0, 0, 0, 0, 0,
    /* 15 - 19 */	0, 0, 0, 0, 0,
    /* 20 - 24 */	0, 0, 0, 0, 0,
    /* 25 - 29 */	0, 0, 0, 0, 0,
    /* 30 - 34 */	0, 0, 0, 0, 0,
    /* 35 - 39 */	0, 0, 0, 0, 0,
    /* 40 - 44 */	0, 0, 0, 0, 0,
    /* 45 - 49 */	0, 0, 0, 0, 0,
    /* 50 - 54 */	0, 0, 0, 0, 0,
    /* 55 - 59 */	0, 0, 0, 0, 0,
    /* 60 - 64 */	0, 0, 0, 0, 0,
    /* 65 - 69 */	0, 0, 0, 0, 0,
    /* 70 - 74 */	0, 0, 0, 0, 0,
    /* 75 - 78 */	0, 0, 0, 0,
};

const int ckgen_needCheck[] =
{
    /* 00 - 04 */	0, 1, 1, 1, 1,
    /* 05 - 09 */	1, 1, 1, 1, 1,
    /* 10 - 14 */	1, 1, 1, 1, 1,
    /* 15 - 19 */	1, 1, 1, 1, 1,
    /* 20 - 24 */	1, 1, 1, 1, 1,
    /* 25 - 29 */	1, 1, 1, 1, 1,
    /* 30 - 34 */	1, 1, 1, 1, 1,
    /* 35 - 39 */	1, 1, 1, 1, 1,
    /* 40 - 44 */	1, 1, 1, 1, 1,
    /* 45 - 49 */	1, 1, 1, 1, 1,
    /* 50 - 54 */	1, 1, 1, 1, 1,
    /* 55 - 59 */	1, 1, 1, 1, 1,
    /* 60 - 64 */	1, 1, 1, 1, 1,
    /* 65 - 69 */	1, 1, 1, 1, 1,
    /* 70 - 74 */	1, 1, 1, 1, 1,
    /* 75 - 78 */	1, 1, 1, 1,
};

const int ckgen_golden[] =
{
    /* 00 - 04 */	/* na */0, /* hd_faxi_ck */156000, /* hd_faxi_peri_ck */156000, /* hd_faxi_ufs_ck */78000, /* hd_fbus_aximem_ck */218400,
    /* 05 - 09 */	/* hf_fdisp0_ck */687500, /* f_fmminfra_ck */687500, /* hf_fuart_ck */52000, /* hf_fspi0_ck */208000, /* hf_fspi1_ck */208000,
    /* 10 - 14 */	/* hf_fspi2_ck */208000, /* hf_fspi3_ck */208000, /* hf_fspi4_ck */208000, /* hf_fspi5_ck */208000, /* hf_fmsdc_macro_0p_ck */416000,
    /* 15 - 19 */	/* hf_fmsdc50_0_hclk_ck */273000, /* hf_fmsdc50_0_ck */416000, /* hf_faes_msdcfde_ck */416000, /* hf_fmsdc_macro_1p_ck */416000, /* hf_fmsdc30_1_ck */208000,
    /* 20 - 24 */	/* hf_fmsdc30_1_hclk_ck */208000, /* hf_fmsdc_macro_2p_ck */416000, /* hf_fmsdc30_2_ck */208000, /* hf_fmsdc30_2_ck_2 */208000, /* hf_faud_intbus_ck */136500,
    /* 25 - 29 */	/* hf_fatb_ck */273000, /* f_fdisp_pwm_ck */136500, /* hf_fusb_top_p0_ck */124800, /* hf_fssusb_xhci_p0_ck */124800, /* hf_fusb_top_p1_ck */124800,
    /* 30 - 34 */	/* hf_fssusb_xhci_p1_ck */124800, /* hf_fusb_top_p2_ck */124800, /* hf_fssusb_xhci_p2_ck */124800, /* hf_fusb_top_p3_ck */124800, /* hf_fssusb_xhci_p3_ck */124800,
    /* 35 - 39 */	/* hf_fusb_top_p4_ck */124800, /* hf_fssusb_xhci_p4_ck */124800, /* hf_fi2c_ck */124800, /* f_fseninf_ck */499200, /* f_fseninf1_ck */499200,
    /* 40 - 44 */	/* hf_faud_engen1_ck */45158, /* hf_faud_engen2_ck */49152, /* hf_faes_ufsfde_ck */546000, /* hf_fufs_ck */208000, /* hf_fufs_mbist_ck */297000,
    /* 45 - 49 */	/* hf_faud_1_ck */180634, /* hf_faud_2_ck */196608, /* hf_fvenc_ck */687500, /* hf_fvdec_ck */624000, /* hf_fpwm_ck */26000,
    /* 50 - 54 */	/* hf_faudio_h_ck */196608, /* hg_fmcupm_ck */218400, /* hf_fmem_sub_ck */546000, /* hf_fmem_sub_peri_ck */546000, /* hf_fmem_sub_ufs_ck */546000,
    /* 55 - 59 */	/* hf_femi_n_ck */26000, /* hf_fdsi_occ_ck */312000, /* f_fap2conn_host_ck */78000, /* hf_fimg1_ck */624000, /* hf_fipe_ck */546000,
    /* 60 - 64 */	/* hf_fcam_ck */624000, /* f_fcamtm_ck */208000, /* hf_fdsp_ck */208000, /* hf_fsr_pka_ck */136500, /* hf_fdxcc_ck */68250,
    /* 65 - 69 */	/* hf_fmfg_ref_ck */364000, /* hf_fmdp0_ck */687500, /* hf_fdp_ck */297000, /* hf_fedp_ck */297000, /* hf_fedp_favt_ck */297000,
    /* 70 - 74 */	/* hf_fsnps_eth_250m_ck */250000, /* hf_fsnps_eth_62p4m_ptp_ck */62500, /* hf_fsnps_eth_50m_rmii_ck */50000, /* hf_fsflash_ck */26000, /* hf_fgcpu_ck */416000,
    /* 75 - 78 */	/* hf_fcie_mac_tl_ck */136500, /* hf_fvdstx_clkdig_cts_ck */118900, /* hf_fpll_dpix_ck */171900, /* hf_fecc_ck */624000,
};

const int abist_postdiv[] =
{
    /* 00 - 04 */	0, 0, 1, 1, 0,
    /* 05 - 09 */	0, 0, 1, 0, 1,
    /* 10 - 14 */	0, 2, 0, 0, 0,
    /* 15 - 19 */	0, 0, 0, 0, 0,
    /* 20 - 24 */	0, 0, 0, 1, 1,
    /* 25 - 29 */	0, 1, 1, 1, 0,
    /* 30 - 34 */	1, 0, 0, 0, 0,
    /* 35 - 39 */	1, 0, 0, 1, 0,
    /* 40 - 44 */	13, 0, 0, 0, 0,
    /* 45 - 49 */	0, 0, 0, 0, 0,
    /* 50 - 54 */	0, 0, 0, 0, 0,
    /* 55 - 59 */	0, 0, 0, 0, 0,
    /* 60 - 64 */	0, 0, 0, 0, 0,
    /* 65 - 69 */	0, 0, 0, 0, 0,
    /* 70 - 74 */	0, 16, 16, 0, 4,
    /* 75 - 79 */	0, 0, 8, 1, 1,
    /* 80 - 84 */	1, 0, 1, 1, 0,
    /* 85 - 84 */
};

const int abist_clkdiv[] =
{
    /* 00 - 04 */	0, 0, 0, 0, 0,
    /* 05 - 09 */	0, 0, 3, 0, 3,
    /* 10 - 14 */	0, 3, 0, 0, 0,
    /* 15 - 19 */	0, 0, 0, 0, 0,
    /* 20 - 24 */	0, 0, 0, 3, 0,
    /* 25 - 29 */	0, 3, 0, 0, 0,
    /* 30 - 34 */	0, 0, 0, 0, 0,
    /* 35 - 39 */	0, 0, 0, 0, 0,
    /* 40 - 44 */	0, 0, 0, 0, 0,
    /* 45 - 49 */	0, 0, 0, 0, 0,
    /* 50 - 54 */	0, 0, 0, 0, 0,
    /* 55 - 59 */	0, 0, 0, 0, 0,
    /* 60 - 64 */	0, 0, 0, 0, 0,
    /* 65 - 69 */	0, 0, 0, 0, 0,
    /* 70 - 74 */	0, 3, 3, 0, 3,
    /* 75 - 79 */	0, 0, 3, 0, 0,
    /* 80 - 84 */	0, 0, 0, 0, 0,
    /* 85 - 84 */
};

const int abist_needCheck[] =
{
    /* 00 - 04 */	0, 0, 1, 1, 0,
    /* 05 - 09 */	0, 0, 1, 0, 1,
    /* 10 - 14 */	0, 1, 0, 0, 0,
    /* 15 - 19 */	0, 0, 0, 0, 0,
    /* 20 - 24 */	0, 0, 0, 1, 1,
    /* 25 - 29 */	0, 1, 1, 1, 0,
    /* 30 - 34 */	1, 0, 0, 0, 0,
    /* 35 - 39 */	1, 0, 0, 1, 0,
    /* 40 - 44 */	1, 0, 0, 0, 0,
    /* 45 - 49 */	0, 0, 0, 0, 0,
    /* 50 - 54 */	0, 0, 0, 0, 0,
    /* 55 - 59 */	0, 0, 0, 0, 0,
    /* 60 - 64 */	0, 0, 0, 0, 0,
    /* 65 - 69 */	0, 0, 0, 0, 0,
    /* 70 - 74 */	0, 1, 1, 0, 1,
    /* 75 - 79 */	0, 0, 1, 1, 1,
    /* 80 - 84 */	1, 0, 1, 1, 0,
    /* 85 - 84 */
};

const int abist_golden[] =
{
    /* 00 - 04 */	/* na */0, /* na */0, /* ad_apll1_ck */180600, /* ad_apll2_ck */196600, /* na */0,
    /* 05 - 09 */	/* na */0, /* ad_armpll_bl_ck */0, /* ad_armpll_bl_ckdiv_ck */2200000, /* ad_armpll_ll_ck */0, /* ad_armpll_ll_ckdiv_ck */2000000,
    /* 10 - 14 */	/* ad_ccipll_ck */0, /* ad_ccipll_ckdiv_ck */1410000, /* na */0, /* na */0, /* na */0,
    /* 15 - 19 */	/* na */0, /* na */0, /* na */0, /* na */0, /* na */0,
    /* 20 - 24 */	/* na */0, /* na */0, /* na */0, /* ad_mainpll_ckdiv_ck */2184000, /* ad_mainpll_ck */2184000,
    /* 25 - 29 */	/* na */0, /* ad_mmpll_ckdiv_ck */2750000, /* ad_mmpll_ck */2750000, /* ad_mmpll_d3_ck */916700, /* na */0,
    /* 30 - 34 */	/* ad_msdcpll_ck */416000, /* na */0, /* na */0, /* na */0, /* na */0,
    /* 35 - 39 */	/* ad_ufspll_ck */594000, /* na */0, /* na */0, /* ad_univpll_ck */2496000, /* na */0,
    /* 40 - 44 */	/* ad_univpll_192m_ck */192000, /* na */0, /* na */0, /* na */0, /* na */0,
    /* 45 - 49 */	/* na */0, /* na */0, /* na */0, /* na */0, /* na */0,
    /* 50 - 54 */	/* na */0, /* na */0, /* na */0, /* na */0, /* na */0,
    /* 55 - 59 */	/* na */0, /* na */0, /* na */0, /* na */0, /* na */0,
    /* 60 - 64 */	/* na */0, /* na */0, /* na */0, /* na */0, /* na */0,
    /* 65 - 69 */	/* na */0, /* na */0, /* na */0, /* na */0, /* na */0,
    /* 70 - 74 */	/* na */0, /* ad_apll1_ckdiv_ck */180633, /* ad_apll2_ckdiv_ck */196608, /* na */0, /* ad_ufspll_ckdiv_ck */594000,
    /* 75 - 79 */	/* na */0, /* na */0, /* ad_msdcpll_ckdiv_ck */416000, /* ad_emipll_ck */688000, /* ad_tvdpll1_ck */594000,
    /* 80 - 84 */	/* ad_tvdpll2_ck */594000, /* ad_mfgpll_opp_ck */0, /* ad_ethpll_ck */500000, /* ad_apupll_ck */750000, /* ad_apupll2_ck */0,
    /* 85 - 84 */
};

const int abist_fmd[] =
{
    /* 00 - 04 */	0, 0, APLL1_CTRL, APLL2_CTRL, 0,
    /* 05 - 09 */	0, 0, ARMPLL_BL_CTRL, 0, ARMPLL_LL_CTRL,
    /* 10 - 14 */	0, CCIPLL_CTRL, 0, 0, 0,
    /* 15 - 19 */	0, 0, 0, 0, 0,
    /* 20 - 24 */	0, 0, 0, MAINPLL_CTRL, MAINPLL_CTRL,
    /* 25 - 29 */	0, MMPLL_CTRL, MMPLL_CTRL, MMPLL_CTRL, 0,
    /* 30 - 34 */	MSDCPLL_CTRL, 0, 0, 0, 0,
    /* 35 - 39 */	UFSPLL_CTRL, 0, 0, UNIVPLL_CTRL, 0,
    /* 40 - 44 */	UNIVPLL_CTRL, 0, 0, 0, 0,
    /* 45 - 49 */	0, 0, 0, 0, 0,
    /* 50 - 54 */	0, 0, 0, 0, 0,
    /* 55 - 59 */	0, 0, 0, 0, 0,
    /* 60 - 64 */	0, 0, 0, 0, 0,
    /* 65 - 69 */	0, 0, 0, 0, 0,
    /* 70 - 74 */	0, APLL1_CTRL, APLL2_CTRL, 0, UFSPLL_CTRL,
    /* 75 - 79 */	0, 0, MSDCPLL_CTRL, EMIPLL_CTRL, TVDPLL1_CTRL,
    /* 80 - 84 */	TVDPLL2_CTRL, 0, ETHPLL_CTRL, APUPLL_CTRL, 0,
};

const int vlpck_postdiv[] =
{
    /* 00 - 04 */	0, 0, 0, 0, 0,
    /* 05 - 09 */	0, 0, 0, 0, 0,
    /* 10 - 14 */	0, 0, 0, 0, 0,
    /* 15 - 19 */	0, 0, 0, 0, 0,
    /* 20 - 24 */	0, 0, 0, 0, 0,
    /* 25 - 24 */
};

const int vlpck_clkdiv[] =
{
    /* 00 - 04 */	0, 0, 0, 0, 0,
    /* 05 - 09 */	0, 0, 0, 0, 0,
    /* 10 - 14 */	0, 0, 0, 0, 0,
    /* 15 - 19 */	0, 0, 0, 0, 0,
    /* 20 - 24 */	0, 0, 0, 0, 0,
    /* 25 - 24 */
};

const int vlpck_needCheck[] =
{
    /* 00 - 04 */	0, 0, 0, 0, 0,
    /* 05 - 09 */	0, 0, 0, 0, 0,
    /* 10 - 14 */	0, 0, 0, 0, 0,
    /* 15 - 19 */	0, 0, 0, 0, 0,
    /* 20 - 24 */	0, 0, 0, 0, 0,
    /* 25 - 24 */
};

const int vlpck_golden[] =
{
    /* 00 - 04 */	/* na */0, /* hf_fscp_ck */0, /* f_fpwrap_ulposc_ck */0, /* hf_fspmi_p_mst_ck */0, /* hf_fdvfsrc_ck */0,
    /* 05 - 09 */	/* hf_fpwm_vlp_ck */0, /* hf_faxi_vlp_ck */0, /* hf_fsystimer_26m_ck */0, /* hf_fsspm_ck */0, /* f_fsspm_f26m_ck */0,
    /* 10 - 14 */	/* hf_fsrck_ck */0, /* hf_fscp_spi_ck */0, /* hf_fscp_iic_ck */0, /* hf_fscp_spi_high_spd_ck */0, /* hf_fscp_iic_high_spd_ck */0,
    /* 15 - 19 */	/* f_fsspm_ulposc_ck */0, /* hf_fapxgpt_26m_ck */0, /* hf_fvadsp_ck */0, /* hf_fvadsp_vowpll_ck */0, /* f_fvadsp_uarthub_bclk_ck */0,
    /* 20 - 24 */	/* f_fcamtg0_ck */0, /* f_fcamtg1_ck */0, /* f_fcamtg2_ck */0, /* hf_faud_adc_ck */0, /* hf_fkp_irq_gen_ck */0,
    /* 25 - 24 */
};
#endif

unsigned int mt_get_abist_freq(unsigned int ID)
{
    int output = 0, i = 0;
    unsigned int temp, clk_dbg_cfg, clk_misc_cfg_0, clk26cali_1;

    // pll con0[15] = 1, enable test clk
    if (abist_fmd[ID] != -1)
        DRV_WriteReg32(fmd[abist_fmd[ID]].pll_con0, (DRV_Reg32(fmd[abist_fmd[ID]].pll_con0) | 0x00008000));

    // CLK26CALI_0[15]: rst 1 -> 0
    DRV_WriteReg32(CLK26CALI_0, (DRV_Reg32(CLK26CALI_0) & 0xFFFF7FFF));
    // CLK26CALI_0[15]: rst 0 -> 1
    DRV_WriteReg32(CLK26CALI_0, (DRV_Reg32(CLK26CALI_0) | 0x00008000));

    // sel abist_cksw and enable freq meter sel abist
    clk_dbg_cfg = DRV_Reg32(CLK_DBG_CFG);
    DRV_WriteReg32(CLK_DBG_CFG, (clk_dbg_cfg & 0xFF80FFFC)|(ID << 16));

    // select divider, div 4
    clk_misc_cfg_0 = DRV_Reg32(CLK_MISC_CFG_0);
    DRV_WriteReg32(CLK_MISC_CFG_0,
        (clk_misc_cfg_0 & 0x00FFFFFF) | (3 << 24));

    clk26cali_1 = DRV_Reg32(CLK26CALI_1);
    DRV_WriteReg32(CLK26CALI_0, 0x9000);
    DRV_WriteReg32(CLK26CALI_0, 0x9010);

    /* wait frequency meter finish */
    while (DRV_Reg32(CLK26CALI_0) & 0x10)
    {
        udelay(10);
        i++;
        if (i > 100)
            break;
    }

    temp = DRV_Reg32(CLK26CALI_1) & 0xFFFF;

    output = ((temp * 26000) ) / 1024; // Khz

    DRV_WriteReg32(CLK_DBG_CFG, clk_dbg_cfg);
    DRV_WriteReg32(CLK_MISC_CFG_0, clk_misc_cfg_0);
    DRV_WriteReg32(CLK26CALI_0, 0x8000);
    DRV_WriteReg32(CLK26CALI_1, clk26cali_1);

    // Disable test clk
    if (abist_fmd[ID] != -1)
        DRV_WriteReg32(fmd[abist_fmd[ID]].pll_con0, (DRV_Reg32(fmd[abist_fmd[ID]].pll_con0) & 0xFFFF7FFF));

    INFO("abist meter[%d] = %d Khz\n",
            ID,
            output * 4);
    return output * 4;
}

static unsigned int mt_get_ckgen_freq(unsigned int ID)
{
    int output = 0, i = 0;
    unsigned int temp, clk_dbg_cfg, clk_misc_cfg_0, clk26cali_1;

    // CLK26CALI_0[15]: rst 1 -> 0
    DRV_WriteReg32(CLK26CALI_0, (DRV_Reg32(CLK26CALI_0) & 0xFFFF7FFF));
    // CLK26CALI_0[15]: rst 0 -> 1
    DRV_WriteReg32(CLK26CALI_0, (DRV_Reg32(CLK26CALI_0) | 0x00008000));

    //sel ckgen_cksw[22] and enable freq meter sel ckgen[21:16], 01:hd_faxi_ck
    clk_dbg_cfg = DRV_Reg32(CLK_DBG_CFG);
    DRV_WriteReg32(CLK_DBG_CFG, (clk_dbg_cfg & 0xFFFF80FC)|(ID << 8)|(0x1));

    // select divider?dvt set zero
    clk_misc_cfg_0 = DRV_Reg32(CLK_MISC_CFG_0);
    DRV_WriteReg32(CLK_MISC_CFG_0, (clk_misc_cfg_0 & 0x00FFFFFF));

    clk26cali_1 = DRV_Reg32(CLK26CALI_1);
    DRV_WriteReg32(CLK26CALI_0, 0x9000);
    DRV_WriteReg32(CLK26CALI_0, 0x9010);

    /* wait frequency meter finish */
    while (DRV_Reg32(CLK26CALI_0) & 0x10)
    {
        udelay(10);
        i++;
        if (i > 100)
            break;
    }

    temp = DRV_Reg32(CLK26CALI_1) & 0xFFFF;

    output = ((temp * 26000) ) / 1024; // Khz

    INFO("ckgen meter[%d] = %d Khz(0x%x)\n", ID, output, DRV_Reg32(CLK_DBG_CFG));

    DRV_WriteReg32(CLK_DBG_CFG, clk_dbg_cfg);
    DRV_WriteReg32(CLK_MISC_CFG_0, clk_misc_cfg_0);
    DRV_WriteReg32(CLK26CALI_0, 0x8000);
    DRV_WriteReg32(CLK26CALI_1, clk26cali_1);

    return output;
}

#if FMETER_CHK
static unsigned int mt_get_subsys_freq(struct fmeter_data fm_data, unsigned int ID)
{
    int output = 0, i = 0, ckdiv_en = 0;
    unsigned int temp, con0, con1;

    if (fm_data.pll_con0 != 0) {
        // check ckdiv_en
        if (DRV_Reg32(fm_data.pll_con0) & 0x00010000)
            ckdiv_en = 1;
        // pll con0[19] = 1, pll con0[16] = 1, pll con0[12] = 1
        // select pll_ckdiv, enable pll_ckdiv, enable test clk
        DRV_WriteReg32(fm_data.pll_con0, (DRV_Reg32(fm_data.pll_con0) | 0x00091000));
    }

    // CLK26CALI_0[15]: rst 1 -> 0
    DRV_WriteReg32(fm_data.con0, 0);
    // CLK26CALI_0[15]: rst 0 -> 1
    DRV_WriteReg32(fm_data.con0, (DRV_Reg32(fm_data.con0) | 0x00008000));

    con0 = DRV_Reg32(fm_data.con0);
    // vlp freq meter sel ckgen[20:16]
    // other subsys freq meter sel ckgen[2:0]
    if (fm_data.type == VLP_CKSYS_TOP_CTRL)
        DRV_WriteReg32(fm_data.con0, (con0 & 0xFFE0FFFF)|(ID << 16));
    else
        DRV_WriteReg32(fm_data.con0, (con0 & 0xFFFFFFF8)|(ID << 0));

    con1 = DRV_Reg32(fm_data.con1);
    DRV_WriteReg32(fm_data.con1, (con1 & 0x0000FFFF) | (0x1FF << 16));

    // select divider?dvt set zero
    con0 = DRV_Reg32(fm_data.con0);
    DRV_WriteReg32(fm_data.con0, (con0 & 0x00FFFFFF));

    DRV_WriteReg32(fm_data.con0, (DRV_Reg32(fm_data.con0) | 0x1000));
    DRV_WriteReg32(fm_data.con0, (DRV_Reg32(fm_data.con0) | 0x10));
    // fmeter con0[1:0] = 0
    // choose test clk
    DRV_WriteReg32(fm_data.con0, (DRV_Reg32(fm_data.con0) & 0xFFFFFFFC));

    /* wait frequency meter finish */
    if (fm_data.type == VLP_CKSYS_TOP_CTRL) {
        udelay(VLP_FM_WAIT_TIME);
    } else {
        while (DRV_Reg32(fm_data.con0) & 0x10)
        {
            udelay(10);
            i++;
            if (i > 100)
                break;
        }
    }

    temp = DRV_Reg32(fm_data.con1) & 0xFFFF;

    output = ((temp * 26000) ) / 512; // Khz

    if (fm_data.pll_con0 != 0) {
        // pll con0[19] = 0, pll con0[12] = 0
        if (ckdiv_en)
            DRV_WriteReg32(fm_data.pll_con0, (DRV_Reg32(fm_data.pll_con0) & 0xFFF7EFFF));
        else
            DRV_WriteReg32(fm_data.pll_con0, (DRV_Reg32(fm_data.pll_con0) & 0xFFF6EFFF));
    }

    DRV_WriteReg32(fm_data.con0, 0x8000);

    INFO("subsys meter_%s[%d] = %d Khz\n",
        fm_data.name, ID, output);

    return output;
}

unsigned int mt_get_vlpck_freq(unsigned int ID)
{
    return mt_get_subsys_freq(fmd[VLP_CKSYS_TOP_CTRL], ID);
}
#endif

unsigned int mt_get_cpu_freq(unsigned int ID)
{
    return mt_get_abist_freq(ID);
}

void mt_dump_cpu_freq(void)
{
    unsigned int result;

    result = mt_get_cpu_freq(ARMPLL_BL_ID);
    INFO("[CPU_BL_FREQ] AD_ARMPLL_BL0_CK = %d Mhz\n",
            (result + 500) / 1000);

    result = mt_get_cpu_freq(ARMPLL_LL_ID);
    INFO("[CPU_LL_FREQ] AD_ARMPLL_LL_CK = %d Mhz\n",
            (result + 500) / 1000);

    result = mt_get_cpu_freq(CCIPLL_ID);
    INFO("[BUS_FREQ] AD_CCIPLL_CK = %d Mhz\n",
            (result + 500) / 1000);
}

#define Range 1000
#define CKGEN_CHANNEL_CNT 79
#define ABIST_CHANNEL_CNT 85
#define VLPCK_CHANNEL_CNT 25
unsigned int ret_feq_store[CKGEN_CHANNEL_CNT+ABIST_CHANNEL_CNT+VLPCK_CHANNEL_CNT];
unsigned int ret_feq_total=0;

void mt_print_pll_chcek_result(void)
{
#ifdef _FREQ_SCAN_
    unsigned int temp, ret_feq;
    unsigned int isFail = 0;
    unsigned int total = 0;

    INFO("==============================\n");
    INFO("==      Parsing Start       ==\n");
    INFO("==============================\n");
    for (temp = 0; temp <= CKGEN_CHANNEL_CNT; temp++) {
        INFO("CKGEN(%d) ", temp);
        if (!ckgen_needCheck[temp]) {
            INFO("skip:%d\n", temp);
            continue;
        }
        ret_feq = ret_feq_store[total];
        total++;
        INFO("%d",ret_feq);
        INFO("\n");
    }

    // abist
    for (temp = 0; temp <= ABIST_CHANNEL_CNT; temp++) {
        INFO("ABIST(%d) ", temp);
        if (!abist_needCheck[temp]) {
            INFO("skip:%d\n", temp);
            continue;
        }
        ret_feq = ret_feq_store[total];
        total++;
        INFO("%d", ret_feq);
        INFO("\n");
    }

    // vlpck
    for (temp = 0; temp <= VLPCK_CHANNEL_CNT; temp++) {
        INFO("VLPCK(%d) ", temp);
        if (!vlpck_needCheck[temp]) {
            INFO("skip:%d\n", temp);
            continue;
        }
        ret_feq = ret_feq_store[total];
        total++;
        INFO("%d", ret_feq);
        INFO("\n");
    }
#endif /* _FREQ_SCAN_ */
}

void mt_pll_post_init(void)
{
#ifdef _FREQ_SCAN_
    unsigned int temp, ret_feq;
    unsigned int isFail = 0;
    INFO("Pll post init start...\n");
    INFO("==============================\n");
    INFO("==      Parsing Start       ==\n");
    INFO("==============================\n");
    for (temp=0; temp <= CKGEN_CHANNEL_CNT; temp++)
    {
        if (!ckgen_needCheck[temp])
            continue;
        else
            INFO("%d:",temp);
        ret_feq = mt_get_ckgen_freq(temp);
        ret_feq_store[ret_feq_total] = ret_feq;
        ret_feq_total++;
        INFO("%d\n",ret_feq);
    }
    // abist
    for (temp=0; temp <= ABIST_CHANNEL_CNT; temp++)
    {
        if (!abist_needCheck[temp])
            continue;
        else
            INFO("%d:",temp);
        ret_feq = mt_get_abist_freq(temp);
        ret_feq_store[ret_feq_total] = ret_feq;
        ret_feq_total++;
        INFO("%d\n",ret_feq);
    }
    // vlpck
    for (temp=0; temp <= VLPCK_CHANNEL_CNT; temp++)
    {
        if (!vlpck_needCheck[temp])
            continue;
        else
            INFO("%d:",temp);
        ret_feq = mt_get_subsys_freq(fmd[VLP_CKSYS_TOP_CTRL], temp);
        ret_feq_store[ret_feq_total] = ret_feq;
        ret_feq_total++;
        INFO("%d\n",ret_feq);
    }
    mt_print_pll_chcek_result();
    INFO("Pll post init Done!\n");
#endif /* _FREQ_SCAN_ */
}

static uint32_t mtk_pll_calc_values(uint32_t khz)
{
    uint32_t _pcw;
    uint32_t pcw;
    uint32_t val;
    uint32_t dividend;
    uint32_t base = FIN_RATE;
    uint32_t postdiv = 0;
    uint32_t j = 0;

    if (khz > FMAX)
        khz = FMAX;

    for (val = 0; val < 5; val++) {
        postdiv = (1 << val);
        if (khz * postdiv >= FMIN)
            break;
    }

    /* _pcw = freq * postdiv / fin * 2^pcwfbits */
    _pcw = (khz << val);
    for (j = 0; j <= PCW_FBITS; j++) {
        dividend = _pcw << j;
        if (dividend >= 0x80000000) {
            base = FIN_RATE >> (PCW_FBITS - j);
            break;
        }
    }

    _pcw = do_div(dividend, base);
    pcw = _pcw | CON1_PCW_CHG | ((val & POSTDIV_MASK) << POSTDIV_SHIFT);

    return pcw;
}

void set_armpll_ll_rate(uint32_t khz)
{
    uint32_t pcw;
    unsigned int temp;

    /* switch to 26MHz first */
    INFO("switch to 26MHz\n");
    temp = DRV_Reg32(CPU_PLLDIV_0_CFG0);
    DRV_WriteReg32(CPU_PLLDIV_0_CFG0, (temp & 0xFFFFF9FF) | (0x00 << 9));

    /* disable armpll_ll */
    DRV_WriteReg32(PLLEN_ALL_CLR, 0x40000);

    /* set CPU to new freq */
    INFO("ARMPLL_LL switch to frequency %d\n", khz);
	pcw = mtk_pll_calc_values(khz);
    DRV_WriteReg32(ARMPLL_LL_CON1, pcw);

    /* enable armpll_ll */
    DRV_WriteReg32(PLLEN_ALL_SET, 0x40000);

    udelay(20);

    /* switch back to ARMPLL_LL */
    INFO("switch to ARMPLL_LL\n");
    temp = DRV_Reg32(CPU_PLLDIV_0_CFG0);
    DRV_WriteReg32(CPU_PLLDIV_0_CFG0, (temp & 0xFFFFF9FF) | (0x01 << 9));

    INFO("ARMPLL_LL_CON1: 0x%x\n", DRV_Reg32(ARMPLL_LL_CON1));
}

void set_armpll_bl_rate(uint32_t khz)
{
	uint32_t pcw;
    unsigned int temp;

    /* switch to 26MHz first */
    INFO("switch to 26MHz\n");
    temp = DRV_Reg32(CPU_PLLDIV_1_CFG0);
    DRV_WriteReg32(CPU_PLLDIV_1_CFG0, (temp & 0xFFFFF9FF) | (0x00 << 9));

    /* disable armpll_l */
    DRV_WriteReg32(PLLEN_ALL_CLR, 0x20000);

	/* set CPU to new freq */
    INFO("ARMPLL_BL switch to frequency %d\n", khz);
	pcw = mtk_pll_calc_values(khz);
    DRV_WriteReg32(ARMPLL_BL_CON1, pcw);

    /* enable armpll_l */
    DRV_WriteReg32(PLLEN_ALL_SET, 0x20000);

    udelay(20);

    /* switch back to ARMPLL_L */
    INFO("switch to ARMPLL_L\n");
    temp = DRV_Reg32(CPU_PLLDIV_1_CFG0);
    DRV_WriteReg32(CPU_PLLDIV_1_CFG0, (temp & 0xFFFFF9FF) | (0x01 << 9));

    INFO("ARMPLL_BL_CON1: 0x%x\n", DRV_Reg32(ARMPLL_BL_CON1));
}

void set_ccipll_rate(uint32_t khz)
{
    uint32_t pcw;
    unsigned int temp;

    /* switch to 26MHz first */
    INFO("switch to 26MHz\n");
    temp = DRV_Reg32(BUS_PLLDIV_CFG0);
    DRV_WriteReg32(BUS_PLLDIV_CFG0, (temp & 0xFFFFF9FF) | (0x00 << 9));

    /* disable ccipll */
    DRV_WriteReg32(PLLEN_ALL_CLR, 0x10000);

    /* set CCI to new freq */
	INFO("ARMPLL_BL switch to frequency %d\n", khz);
	pcw = mtk_pll_calc_values(khz);
    DRV_WriteReg32(CCIPLL_CON1, pcw);

    /* enable ccipll */
    DRV_WriteReg32(PLLEN_ALL_SET, 0x10000);

    udelay(20);

    /* switch back to CCIPLL */
    INFO("switch to CCIPLL\n");
    temp = DRV_Reg32(BUS_PLLDIV_CFG0);
    DRV_WriteReg32(BUS_PLLDIV_CFG0, (temp & 0xFFFFF9FF) | (0x01 << 9));

    INFO("CCIPLL_CON1: 0x%x\n", DRV_Reg32(CCIPLL_CON1));
}

void set_pll_rate(enum PLL_LIST pll_id, uint32_t khz)
{
    if (pll_id == ARMPLL_LL_RATE)
        set_armpll_ll_rate(khz);
    else if (pll_id == ARMPLL_BL_RATE)
        set_armpll_bl_rate(khz);
    else if (pll_id == CCIPLL_RATE)
        set_ccipll_rate(khz);
    else
		return;
}

void mt_set_topck_default(void)
{
    DRV_WriteReg32(CLK_CFG_0_CLR, 0xFFFFFFF0);
    DRV_WriteReg32(CLK_CFG_1_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_2_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_3_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_4_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_5_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_6_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_7_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_8_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_9_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_10_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_11_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_12_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_13_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_14_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_15_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_16_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_17_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_18_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_19_CLR, 0xFFFFFFFF);

    DRV_WriteReg32(CLK_CFG_UPDATE, 0x7FFFFFFE); /* except "axi" */
    DRV_WriteReg32(CLK_CFG_UPDATE1, 0x7FFFFFFF);
    DRV_WriteReg32(CLK_CFG_UPDATE2, 0x0000FFFF);
}

void mt_pll_init(void)
{
    unsigned int temp;
    unsigned int ucSpare0 = 0, ucSpare1 = 0, ucSpare2 = 0, ucSpare3 = 0, ucSpare4 = 0;

    ucSpare0 = 0x440000;
    ucSpare1 = 0x1C000;
    ucSpare2 = 0x400000;
    ucSpare3 = 0x400;
    ucSpare4 = 0x3200003;

    NOTICE( "Pll init start...\n");

    INFO("ucSpare0=%x, ucSpare1=%x, ucSpare2=%x, ucSpare3=%x, ucSpare4=%x\n",
          ucSpare0, ucSpare1, ucSpare2, ucSpare3, ucSpare4);
    spm_power_on();

    INFO("pll control start...\n");
    /* [0]=1 (CLKSQ_EN) + Default Value \ prevent brom overwrite setting */
    temp = DRV_Reg32(AP_CLKSQ_CON0);
    DRV_WriteReg32(AP_CLKSQ_CON0, temp | 0x00000001);

    /* Wait 100us */
    udelay(100);

    /* [2]=1 (CLKSQ_LPF_EN) */
    temp = DRV_Reg32(AP_CLKSQ_CON0);
    DRV_WriteReg32(AP_CLKSQ_CON0, temp | 0x00000004);

    /* MAINPLL=2184Mhz */
    DRV_WriteReg32(MAINPLL_CON1, 0x80150000);

    /* UNIVPLL=2496Mhz */
    DRV_WriteReg32(UNIVPLL_CON1, 0x80180000);

    /* MSDCPLL=384Mhz */
    DRV_WriteReg32(MSDCPLL_CON1, 0x820EC4EC);

    /* MMPLL=2750Mhz */
    DRV_WriteReg32(MMPLL_CON1, 0x801A713B);

    /* UFSPLL=594Mhz */
    DRV_WriteReg32(UFSPLL_CON1, 0x8216D89D);

    /* APUPLL=1100Mhz */
    DRV_WriteReg32(APUPLL_CON1, 0x83196276);

    /* APUPLL2=750Mhz */
    DRV_WriteReg32(APUPLL2_CON1, 0x8311B13B);

    /* [21:0] (MFG_N_INFO\ 390MHz) */
    DRV_WriteReg32(MFGPLL_CON1, 0x831E0000);

    /* [8]=1(MFGPLL glitch free en=1) */
    temp = DRV_Reg32(MFGPLL_CON0);
    DRV_WriteReg32(MFGPLL_CON0, temp | 0x00000100);
#if 0 // need fix
    if (mt_get_dram_type() == TYPE_LPDDR5)
        /* EMIPLL=387Mhz */
        DRV_WriteReg32(EMIPLL_CON1, 0x820EE276);
    else
        /* EMIPLL=200Mhz */
        DRV_WriteReg32(EMIPLL_CON1, 0x830F6276);
#endif

    /* [8]=1(EMIPLL glitch free en=1) */
    temp = DRV_Reg32(EMIPLL_CON0);
    DRV_WriteReg32(EMIPLL_CON0, temp | 0x00000100);

    /* TVDPLL1=594Mhz */
    DRV_WriteReg32(TVDPLL1_CON1, 0x8216D89D);

    /* TVDPLL2=594Mhz */
    DRV_WriteReg32(TVDPLL2_CON1, 0x8216D89D);

    /* ETHPLL=500Mhz */
    DRV_WriteReg32(ETHPLL_CON1, 0x82133B13);

    /* APLL1=180.6336Mhz */
    DRV_WriteReg32(APLL1_CON2, 0x6F28BD4C);

    /* [26:24] (APLL1 posdiv=4) */
    DRV_WriteReg32(APLL1_CON1, 0x84000000);
    DRV_WriteReg32(APLL1_TUNER_CON0, 0x6F28BD4D);

    /* APLL2=196.608Mhz */
    DRV_WriteReg32(APLL2_CON2, 0x78FD5264);

    /* [26:24] (APLL2 posdiv=4) */
    DRV_WriteReg32(APLL2_CON1, 0x84000000);
    DRV_WriteReg32(APLL2_TUNER_CON0, 0x78FD5265);

    /* CCIPLL=1140Mhz */
    DRV_WriteReg32(CCIPLL_CON1, 0x8115EC4E);

    /* ARMPLL_LL=1600Mhz */
    DRV_WriteReg32(ARMPLL_LL_CON1, 0x811EC4EC);

    /* ARMPLL_BL=1700Mhz */
    DRV_WriteReg32(ARMPLL_BL_CON1, 0x8120B13B);

    /* PLL all enable */
    DRV_WriteReg32(PLLEN_ALL_SET, 0x0007FFFC);

    /* Wait PLL stable (20us) */
    udelay(20);

    /* turn on pll div en */
    DRV_WriteReg32(MAINPLL_CON0, DRV_Reg32(MAINPLL_CON0) | 0xFF000000);
    DRV_WriteReg32(UNIVPLL_CON0, DRV_Reg32(UNIVPLL_CON0) | 0xFF000000);
    udelay(20);


    /* pll all rstb */
    DRV_WriteReg32(PLL_DIV_RSTB_ALL_SET, 0x00000007);

    INFO("pll control done...\n");

    /* [10:9] muxsel: switch to PLL speed */
    temp = DRV_Reg32(CPU_PLLDIV_0_CFG0);
    DRV_WriteReg32(CPU_PLLDIV_0_CFG0, (temp & 0xFFFFF9FF) | 0x00000200);

    /* [10:9] muxsel: switch to PLL speed */
    temp = DRV_Reg32(CPU_PLLDIV_1_CFG0);
    DRV_WriteReg32(CPU_PLLDIV_1_CFG0, (temp & 0xFFFFF9FF) | 0x00000200);

    /* [10:9] muxsel: switch to PLL speed */
    temp = DRV_Reg32(BUS_PLLDIV_CFG0);
    DRV_WriteReg32(BUS_PLLDIV_CFG0, (temp & 0xFFFFF9FF) | 0x00000200);

    /* Infra DCM divider */
    temp = DRV_Reg32(VDNR_DCM_INFRA_PAR_BUS_CTRL_0);
    DRV_WriteReg32(VDNR_DCM_INFRA_PAR_BUS_CTRL_0, temp | 0x00000048);

    /* Peri DCM divider */
    temp = DRV_Reg32(VDNR_DCM_PERI_PAR_BUS_CTRL_0);
    DRV_WriteReg32(VDNR_DCM_PERI_PAR_BUS_CTRL_0, temp | 0x00000490);

    INFO("mux switch control start...\n");

    /* None */
    DRV_WriteReg32(CLK_CFG_0_CLR, 0xFFFFFFFF);

    /* axi=156Mhz(MAINPLL_D7_D2)\axi_peri=156Mhz(MAINPLL_D7_D2)\axi_ufs=78Mhz(MAINPLL_D7_D4)\bus_aximem=218.4Mhz(MAINPLL_D5_D2) */
    DRV_WriteReg32(CLK_CFG_0_SET, 0x02020202);

    /* None */
    DRV_WriteReg32(CLK_CFG_1_CLR, 0xFFFFFFFF);
    if (mt_get_dram_type() == TYPE_LPDDR5)
        /* disp0=624Mhz(UNIVPLL_D4)\mminfra=624Mhz(UNIVPLL_D4)\uart=52Mhz(UNIVPLL_D6_D8)\spi0=208Mhz(UNIVPLL_D6_D2) */
        DRV_WriteReg32(CLK_CFG_1_SET, 0x01010D0A);
    else
        /* disp0=416Mhz(UNIVPLL_D6)\mminfra=458.33Mhz(MMPLL_D6)\uart=52Mhz(UNIVPLL_D6_D8)\spi0=208Mhz(UNIVPLL_D6_D2) */
        DRV_WriteReg32(CLK_CFG_1_SET, 0x01010A06);

    INFO("%s %d\n", __func__, __LINE__);

    /* None */
    DRV_WriteReg32(CLK_CFG_2_CLR, 0xFFFFFFFF);

    /* spi1=208Mhz(UNIVPLL_D6_D2)\spi2=208Mhz(UNIVPLL_D6_D2)\spi3=208Mhz(UNIVPLL_D6_D2)\spi4=208Mhz(UNIVPLL_D6_D2) */
    DRV_WriteReg32(CLK_CFG_2_SET, 0x01010101);

    /* None */
    DRV_WriteReg32(CLK_CFG_3_CLR, 0xFFFFFFFF);

    /* spi5=208Mhz(UNIVPLL_D6_D2)\msdc_macro_0p=416Mhz(MSDCPLL_CK)\msdc50_0_hclk=273Mhz(MAINPLL_D4_D2)\msdc50_0=416Mhz(MSDCPLL_CK) */
    DRV_WriteReg32(CLK_CFG_3_SET, 0x01010101);

    INFO("%s %d\n", __func__, __LINE__);

    /* None */
    DRV_WriteReg32(CLK_CFG_4_CLR, 0xFFFFFFFF);

    /* aes_msdcfde=416Mhz(MSDCPLL_CK)\msdc_macro_1p=416Mhz(MSDCPLL_CK)\msdc30_1=208Mhz(MSDCPLL_D2)\msdc30_1_hclk=208Mhz(MSDCPLL_D2) */
    DRV_WriteReg32(CLK_CFG_4_SET, 0x01040104);

    INFO("%s %d\n", __func__, __LINE__);

    /* None */
    DRV_WriteReg32(CLK_CFG_5_CLR, 0xFFFFFFFF);

    /* msdc_macro_2p=416Mhz(MSDCPLL_CK)\msdc30_2=208Mhz(MSDCPLL_D2)\msdc30_2_hclk=208Mhz(MSDCPLL_D2)\aud_intbus=136.5Mhz(MAINPLL_D4_D4) */
    DRV_WriteReg32(CLK_CFG_5_SET, 0x01010401);

    /* None */
    DRV_WriteReg32(CLK_CFG_6_CLR, 0xFFFFFFFF);

    /* atb=273Mhz(MAINPLL_D4_D2)\disp_pwm=136.5Mhz(MAINPLL_D4_D4)\usb_top_p0=124.8Mhz(UNIVPLL_D5_D4)\ssusb_xhci_p0=124.8Mhz(UNIVPLL_D5_D4) */
    DRV_WriteReg32(CLK_CFG_6_SET, 0x01010601);

    INFO("%s %d\n", __func__, __LINE__);
    /* None */
    DRV_WriteReg32(CLK_CFG_7_CLR, 0xFFFFFFFF);

    /* usb_top_p1=124.8Mhz(UNIVPLL_D5_D4)\ssusb_xhci_p1=124.8Mhz(UNIVPLL_D5_D4)\usb_top_p2=124.8Mhz(UNIVPLL_D5_D4)\ssusb_xhci_p2=124.8Mhz(UNIVPLL_D5_D4) */
    DRV_WriteReg32(CLK_CFG_7_SET, 0x01010101);

    INFO("%s %d\n", __func__, __LINE__);
    /* None */
    DRV_WriteReg32(CLK_CFG_8_CLR, 0xFFFFFFFF);

    /* usb_top_p3=124.8Mhz(UNIVPLL_D5_D4)\ssusb_xhci_p3=124.8Mhz(UNIVPLL_D5_D4)\usb_top_p4=124.8Mhz(UNIVPLL_D5_D4)\ssusb_xhci_p4=124.8Mhz(UNIVPLL_D5_D4) */
    DRV_WriteReg32(CLK_CFG_8_SET, 0x01010101);

    INFO("%s %d\n", __func__, __LINE__);
    /* None */
    DRV_WriteReg32(CLK_CFG_9_CLR, 0xFFFFFFFF);

    /* i2c=124.8Mhz(UNIVPLL_D5_D4)\seninf=499.2Mhz(UNIVPLL_D5)\seninf1=499.2Mhz(UNIVPLL_D5)\aud_engen1=45.1584Mhz(APLL1_D4) */
    DRV_WriteReg32(CLK_CFG_9_SET, 0x02070702);

    /* None */
    DRV_WriteReg32(CLK_CFG_10_CLR, 0xFFFFFFFF);

    /* aud_engen2=49.152Mhz(APLL2_D4)\aes_ufsfde=546Mhz(MAINPLL_D4)\ufs=208Mhz(UNIVPLL_D6_D2)\ufs_mbist=297Mhz(UFSPLL_D2) */
    DRV_WriteReg32(CLK_CFG_10_SET, 0x03050102);

    INFO("%s %d\n", __func__, __LINE__);

    /* None */
    DRV_WriteReg32(CLK_CFG_11_CLR, 0xFFFFFFFF);
    if (mt_get_dram_type() == TYPE_LPDDR5)
        /* aud_1=180.6336Mhz(APLL1_CK)\aud_2=196.608Mhz(APLL2_CK)\venc=624Mhz(UNIVPLL_D4)\vdec=546Mhz(MAINPLL_D4) */
        DRV_WriteReg32(CLK_CFG_11_SET, 0x0D0C0101);
    else
        /* aud_1=180.6336Mhz(APLL1_CK)\aud_2=196.608Mhz(APLL2_CK)\venc=458.33Mhz(MMPLL_D6)\vdec=416Mhz(UNIVPLL_D6) */
        DRV_WriteReg32(CLK_CFG_11_SET, 0x0C060101);

    /* None */
    DRV_WriteReg32(CLK_CFG_12_CLR, 0xFFFFFFFF);
    if (mt_get_dram_type() == TYPE_LPDDR5)
        /* pwm=26Mhz(tck_26m_mx9_ck)\audio_h=196.608Mhz(APLL2_CK)\mcupm=218.4Mhz(MAINPLL_D5_D2)\mem_sub=546Mhz(MAINPLL_D4) */
        DRV_WriteReg32(CLK_CFG_12_SET, 0x09020300);
    else
        /* pwm=26Mhz(tck_26m_mx9_ck)\audio_h=196.608Mhz(APLL2_CK)\mcupm=218.4Mhz(MAINPLL_D5_D2)\mem_sub=436.8Mhz(MAINPLL_D5) */
        DRV_WriteReg32(CLK_CFG_12_SET, 0x07020300);

    /* None */
    DRV_WriteReg32(CLK_CFG_13_CLR, 0xFFFFFFFF);
    if (mt_get_dram_type() == TYPE_LPDDR5)
        /* mem_sub_peri=546Mhz(MAINPLL_D4)\mem_sub_ufs=546Mhz(MAINPLL_D4)\emi_n=242.667Mhz(MAINPLL_D9)\dsi_occ=312Mhz(UNIVPLL_D4_D2) */
        DRV_WriteReg32(CLK_CFG_13_SET, 0x03020707);
    else
        /* mem_sub_peri=436.8Mhz(MAINPLL_D5)\mem_sub_ufs=436.8Mhz(MAINPLL_D5)\emi_n=242.667Mhz(MAINPLL_D9)\dsi_occ=312Mhz(UNIVPLL_D4_D2) */
        DRV_WriteReg32(CLK_CFG_13_SET, 0x03020505);

    /* None */
    DRV_WriteReg32(CLK_CFG_14_CLR, 0xFFFFFFFF);
    if (mt_get_dram_type() == TYPE_LPDDR5)
        /* ap2conn_host=78Mhz(MAINPLL_D7_D4)\img1=624Mhz(UNIVPLL_D4)\ipe=546Mhz(MAINPLL_D4)\cam=624Mhz(UNIVPLL_D4) */
        DRV_WriteReg32(CLK_CFG_14_SET, 0x03020101);
    else
    /* ap2conn_host=78Mhz(MAINPLL_D7_D4)\img1=458.33Mhz(MMPLL_D6)\ipe=416Mhz(UNIVPLL_D6)\cam=546Mhz(MAINPLL_D4) */
        DRV_WriteReg32(CLK_CFG_14_SET, 0x01040301);

    INFO("%s %d\n", __func__, __LINE__);

    /* None */
    DRV_WriteReg32(CLK_CFG_15_CLR, 0xFFFFFFFF);

    /* camtm=208Mhz(UNIVPLL_D6_D2)\dsp=208Mhz(UNIVPLL_D6_D2)\sr_pka=136.5Mhz(MAINPLL_D4_D4)\dxcc=68.25Mhz(MAINPLL_D4_D8) */
    DRV_WriteReg32(CLK_CFG_15_SET, 0x01010502);

    /* None */
    DRV_WriteReg32(CLK_CFG_16_CLR, 0xFFFFFFFF);
    if (mt_get_dram_type() == TYPE_LPDDR5)
        /* mfg_ref=364Mhz(MAINPLL_D6)\mdp0=624Mhz(UNIVPLL_D4)\dp=297Mhz(TVDPLL1_D2)\edp=297Mhz(TVDPLL2_D2) */
        DRV_WriteReg32(CLK_CFG_16_SET, 0x04040A02);
    else
        /* mfg_ref=364Mhz(MAINPLL_D6)\mdp0=416Mhz(UNIVPLL_D6)\dp=148.5Mhz(TVDPLL1_D4)\edp=148.5Mhz(TVDPLL2_D4) */
        DRV_WriteReg32(CLK_CFG_16_SET, 0x03030602);

    /* None */
    DRV_WriteReg32(CLK_CFG_17_CLR, 0xFFFFFFFF);
    if (mt_get_dram_type() == TYPE_LPDDR5)
        /* edp_favt=297Mhz(TVDPLL2_D2)\snps_eth_250m=250Mhz(ETHPLL_D2)\snps_eth_62p4m_ptp=62.5Mhz(ETHPLL_D8)\snps_eth_50m_rmii=50Mhz(ETHPLL_D10) */
        DRV_WriteReg32(CLK_CFG_17_SET, 0x01010104);
    else
        /* edp_favt=148.5Mhz(TVDPLL2_D4)\snps_eth_250m=250Mhz(ETHPLL_D2)\snps_eth_62p4m_ptp=62.5Mhz(ETHPLL_D8)\snps_eth_50m_rmii=50Mhz(ETHPLL_D10) */
        DRV_WriteReg32(CLK_CFG_17_SET, 0x01010103);

    /* None */
    DRV_WriteReg32(CLK_CFG_18_CLR, 0xFFFFFFFF);
    if (mt_get_dram_type() == TYPE_LPDDR5)
        /* sflash=26Mhz(tck_26m_mx9_ck)\gcpu=416Mhz(UNIVPLL_D6)\cie_mac_tl=136.5Mhz(MAINPLL_D4_D4)\vdstx_clkdig_cts=118.9Mhz(LVDSTX_CLKDIG_CTS_CK) */
        DRV_WriteReg32(CLK_CFG_18_SET, 0x01010600);
    else
        /* sflash=26Mhz(tck_26m_mx9_ck)\gcpu=364Mhz(MAINPLL_D6)\cie_mac_tl=136.5Mhz(MAINPLL_D4_D4)\vdstx_clkdig_cts=118.9Mhz(LVDSTX_CLKDIG_CTS_CK) */
        DRV_WriteReg32(CLK_CFG_18_SET, 0x01010100);

    /* None */
    DRV_WriteReg32(CLK_CFG_19_CLR, 0x0000FFFF);
    if (mt_get_dram_type() == TYPE_LPDDR5)
        /* pll_dpix=171.9Mhz(VPLL_DPIX_CK)\ecc=546Mhz(MAINPLL_D4) */
        DRV_WriteReg32(CLK_CFG_19_SET, 0x00000401);
    else
        /* pll_dpix=171.9Mhz(VPLL_DPIX_CK)\ecc=416Mhz(UNIVPLL_D6) */
        DRV_WriteReg32(CLK_CFG_19_SET, 0x00000301);

    //__asm__    __volatile__ ("b ." : : : "memory");
    /* update mux */
    DRV_WriteReg32(CLK_CFG_UPDATE, 0x7FFFFFFF);

    /* update mux */
    DRV_WriteReg32(CLK_CFG_UPDATE1, 0x7FFFFFFF);

    INFO("%s %d\n", __func__, __LINE__);

    /* update mux */
    DRV_WriteReg32(CLK_CFG_UPDATE2, 0x0000FFFF);

    INFO("%s %d\n", __func__, __LINE__);

    //__asm__    __volatile__ ("b ." : : : "memory");

    /* femisys_dvfs_ck_gfmux_sel = 1(emipll_ck) */
    DRV_WriteReg32(CLK_MEM_DFS_CFG, 0x1);

    INFO("%s %d\n", __func__, __LINE__);

    /* None */
    DRV_WriteReg32(VLP_CLK_CFG_0_CLR, 0xFFFFFFFF);

    /* scp=26Mhz(tck_26m_mx9_ck)\pwrap_ulposc=26Mhz(tck_26m_mx9_ck)\spmi_p_mst=26Mhz(tck_26m_mx9_ck)\dvfsrc=Mhz() */
    DRV_WriteReg32(VLP_CLK_CFG_0_SET, 0x00000000);

    INFO("%s %d\n", __func__, __LINE__);

    /* None */
    DRV_WriteReg32(VLP_CLK_CFG_1_CLR, 0xFFFFFFFF);

    /* pwm_vlp=26Mhz(tck_26m_mx9_ck)\axi_vlp=156Mhz(MAINPLL_D7_D2)\systimer_26m=26Mhz(tck_26m_mx9_ck)\sspm=364Mhz(mainpll_d6) */
    DRV_WriteReg32(VLP_CLK_CFG_1_SET, 0x04000400);

    /* None */
    DRV_WriteReg32(VLP_CLK_CFG_2_CLR, 0xFFFFFFFF);

    /* sspm_f26m=Mhz()\srck=Mhz()\scp_spi=26Mhz(tck_26m_mx9_ck)\scp_iic=26Mhz(tck_26m_mx9_ck) */
    DRV_WriteReg32(VLP_CLK_CFG_2_SET, 0x00000000);

    INFO("%s %d\n", __func__, __LINE__);

    /* None */
    DRV_WriteReg32(VLP_CLK_CFG_3_CLR, 0xFFFFFFFF);

    /* scp_spi_high_spd=26Mhz(tck_26m_mx9_ck)\scp_iic_high_spd=26Mhz(tck_26m_mx9_ck)\sspm_ulposc=Mhz()\apxgpt_26m=Mhz() */
    DRV_WriteReg32(VLP_CLK_CFG_3_SET, 0x00000000);

    INFO("%s %d\n", __func__, __LINE__);

    /* None */
    DRV_WriteReg32(VLP_CLK_CFG_4_CLR, 0xFFFFFFFF);

    /* vadsp=26Mhz(tck_26m_mx9_ck)\vadsp_vowpll=26Mhz(tck_26m_mx9_ck)\vadsp_uarthub_bclk=208Mhz(univpll_d6_d2)\camtg0=24Mhz(UNIVPLL_192M_D8) */
    DRV_WriteReg32(VLP_CLK_CFG_4_SET, 0x01030000);

    INFO("%s %d\n", __func__, __LINE__);

    DRV_WriteReg32(VLP_CLK_CFG_5_CLR, 0xFFFFFFFF);

    /* camtg1_sel=24Mhz(univpll_192m_d8)\camtg2_sel=224Mhz(univpll_192m_d8)\aud_adc_sel=26Mhz\kp_irq_gen_sel=156Mhz(mainpll_d7_d2) */
    DRV_WriteReg32(VLP_CLK_CFG_5_SET, 0x04000101);

    INFO("%s %d\n", __func__, __LINE__);

    /* update mux */
    DRV_WriteReg32(VLP_CLK_CFG_UPDATE, 0x00FFFFFF);

    INFO("mux switch control done...\n");

    //mt_dump_cpu_freq();

    /************
     * MTCMOS
     *************/
    // INFO("conn mtcmos Start..\n");
    // spm_mtcmos_ctrl_conn(STA_POWER_ON);
    // INFO("conn mtcmos Done!\n");

    INFO("ufs0 mtcmos Start..\n");
    spm_mtcmos_ctrl_ufs0(STA_POWER_ON);
    INFO("ufs0 mtcmos Done!\n");

    INFO("ufs0_phy mtcmos Start..\n");
    spm_mtcmos_ctrl_ufs0_phy(STA_POWER_ON);
    INFO("ufs0_phy mtcmos Done!\n");

    INFO("audio mtcmos Start..\n");
    spm_mtcmos_ctrl_audio(STA_POWER_ON);
    INFO("audio mtcmos Done!\n");

    if (!(ucSpare1 & EFUSE_MTCMOS_ADSP_AO_DIS_S1B17)) {
        INFO("adsp_ao mtcmos Start..\n");
        spm_mtcmos_ctrl_adsp_ao(STA_POWER_ON);
        INFO("adsp_ao mtcmos Done!\n");
    }

    if (!(ucSpare1 & EFUSE_MTCMOS_ADSP_INFRA_DIS_S1B16)) {
        INFO("adsp_infra mtcmos Start..\n");
        spm_mtcmos_ctrl_adsp_infra(STA_POWER_ON);
        INFO("adsp_infra mtcmos Done!\n");
    }

    if (!(ucSpare1 & EFUSE_MTCMOS_ADSP_TOP_DIS_S1B15)) {
        INFO("adsp_top_shutdown mtcmos Start..\n");
        spm_mtcmos_ctrl_adsp_top_shutdown(STA_POWER_ON);
        INFO("adsp_top_shutdown mtcmos Done!\n");
    }

    INFO("mm_infra mtcmos Start..\n");
    spm_mtcmos_ctrl_mm_infra(STA_POWER_ON);
    INFO("mm_infra mtcmos Done!\n");

    if (!(ucSpare2 & EFUSE_MTCMOS_ISP_IMG1_DIS_S2B18)) {
        INFO("isp_img1 mtcmos Start..\n");
        spm_mtcmos_ctrl_isp_img1(STA_POWER_ON);
        INFO("isp_img1 mtcmos Done!\n");
    }

    if (!(ucSpare2 & EFUSE_MTCMOS_ISP_IMG2_DIS_S2B19)) {
        INFO("isp_img2 mtcmos Start..\n");
        spm_mtcmos_ctrl_isp_img2(STA_POWER_ON);
        INFO("isp_img2 mtcmos Done!\n");
    }

    if (!(ucSpare2 & EFUSE_MTCMOS_ISP_IPE_DIS_S2B20)) {
        INFO("isp_ipe mtcmos Start..\n");
        spm_mtcmos_ctrl_isp_ipe(STA_POWER_ON);
        INFO("isp_ipe mtcmos Done!\n");
    }

    INFO("vde0 mtcmos Start..\n");
    spm_mtcmos_ctrl_vde0(STA_POWER_ON);
    INFO("vde0 mtcmos Done!\n");

    INFO("ven0 mtcmos Start..\n");
    spm_mtcmos_ctrl_ven0(STA_POWER_ON);
    INFO("ven0 mtcmos Done!\n");

    if (!(ucSpare2 & EFUSE_MTCMOS_CAM_MAIN_DIS_S2B23)) {
        INFO("cam_main mtcmos Start..\n");
        spm_mtcmos_ctrl_cam_main(STA_POWER_ON);
        INFO("cam_main mtcmos Done!\n");
    }

    if (!(ucSpare2 & EFUSE_MTCMOS_CAM_RAWA_DIS_S2B21)) {
        INFO("cam_suba mtcmos Start..\n");
        spm_mtcmos_ctrl_cam_suba(STA_POWER_ON);
        INFO("cam_suba mtcmos Done!\n");
    }

    if (!(ucSpare2 & EFUSE_MTCMOS_CAM_RAWB_DIS_S2B22)) {
        INFO("cam_subb mtcmos Start..\n");
        spm_mtcmos_ctrl_cam_subb(STA_POWER_ON);
        INFO("cam_subb mtcmos Done!\n");
    }

    INFO("mdp0 mtcmos Start..\n");
    spm_mtcmos_ctrl_mdp0(STA_POWER_ON);
    INFO("mdp0 mtcmos Done!\n");

    INFO("disp mtcmos Start..\n");
    spm_mtcmos_ctrl_disp(STA_POWER_ON);
    INFO("disp mtcmos Done!\n");

    if (!(ucSpare1 & EFUSE_MTCMOS_DPTX_DIS_S1B10)) {
        INFO("dp_tx mtcmos Start..\n");
        spm_mtcmos_ctrl_dp_tx(STA_POWER_ON);
        INFO("dp_tx mtcmos Done!\n");
    }

    if (!(ucSpare4 & EFUSE_MTCMOS_CSI_DIS_S4B27)) {
        INFO("csi_rx mtcmos Start..\n");
        spm_mtcmos_ctrl_csi_rx(STA_POWER_ON);
        INFO("csi_rx mtcmos Done!\n");
    }

    if (!(ucSpare4 & EFUSE_MTCMOS_SSUSB_DIS_S4B26)) {
        INFO("ssusb mtcmos Start..\n");
        spm_mtcmos_ctrl_ssusb(STA_POWER_ON);
        INFO("ssusb mtcmos Done!\n");
    }

    if (!(ucSpare2 & EFUSE_MTCMOS_MFG0_DIS_S2B0)) {
        INFO("mfg0 mtcmos Start..\n");
        spm_mtcmos_ctrl_mfg0(STA_POWER_ON);
        INFO("mfg0 mtcmos Done!\n");
    }

    if (!(ucSpare2 & EFUSE_MTCMOS_MFG1_DIS_S2B1)) {
        INFO("mfg1 mtcmos Start..\n");
        spm_mtcmos_ctrl_mfg1(STA_POWER_ON);
        INFO("mfg1 mtcmos Done!\n");
    }

    if (!(ucSpare2 & EFUSE_MTCMOS_MFG2_DIS_S2B2)) {
        INFO("mfg2 mtcmos Start..\n");
        spm_mtcmos_ctrl_mfg2(STA_POWER_ON);
        INFO("mfg2 mtcmos Done!\n");
    }

    if (!(ucSpare2 & EFUSE_MTCMOS_MFG3_DIS_S2B3)) {
        INFO("mfg3 mtcmos Start..\n");
        spm_mtcmos_ctrl_mfg3(STA_POWER_ON);
        INFO("mfg3 mtcmos Done!\n");
    }

    if (!(ucSpare2 & EFUSE_MTCMOS_EDPTX_DIS_S2B15)) {
        INFO("edp_tx_shutdown mtcmos Start..\n");
        spm_mtcmos_ctrl_edp_tx_shutdown(STA_POWER_ON);
        INFO("edp_tx_shutdown mtcmos Done!\n");
    }

    if (!(ucSpare3 & EFUSE_MTCMOS_PCIE_DIS_S3B24)) {
        INFO("pcie mtcmos Start..\n");
        spm_mtcmos_ctrl_pcie(STA_POWER_ON);
        INFO("pcie mtcmos Done!\n");

        INFO("pcie_phy mtcmos Start..\n");
        spm_mtcmos_ctrl_pcie_phy(STA_POWER_ON);
        INFO("pcie_phy mtcmos Done!\n");
    }
    /*for CG*/

    INFO("subsysCG enable start...\n");
#if CLKMGR_BRINGUP
    /* TOPCKGEN CG Clear*/
    DRV_WriteReg32(TOPCKGEN_CLK_MISC_CFG_3_CLR, 0x00010000);
    DRV_WriteReg32(TOPCKGEN_CLK_MISC_CFG_3_SET, 0xDF3CFCFF);
    /* INFRACFG_AO CG Clear*/
    DRV_WriteReg32(INFRACFG_AO_MODULE_CG_0_CLR, 0x10000000);
    DRV_WriteReg32(INFRACFG_AO_MODULE_CG_1_CLR, 0x21000000);
    DRV_WriteReg32(INFRACFG_AO_MODULE_CG_2_CLR, 0x08000000);
    DRV_WriteReg32(INFRACFG_AO_MODULE_CG_3_CLR, 0x02000000);
    /* PERICFG_AO CG Clear*/
    DRV_WriteReg32(PERICFG_AO_PERI_CG_0_CLR, 0x3FFFFFFF);
    DRV_WriteReg32(PERICFG_AO_PERI_CG_1_CLR, 0x3DBDFBF6);
    DRV_WriteReg32(PERICFG_AO_PERI_CG_2_CLR, 0x0FFFFFFB);
    /* UFSCFG_AO_REG CG Clear*/
    DRV_WriteReg32(UFSCFG_AO_REG_UFS_AO_CG_0_CLR, 0x0000007F);
    /* IMP_IIC_WRAP_WS CG Clear*/
    DRV_WriteReg32(IMP_IIC_WRAP_WS_AP_CLOCK_CG_CLR, 0x00000001);
    /* IMP_IIC_WRAP_E CG Clear*/
    DRV_WriteReg32(IMP_IIC_WRAP_E_AP_CLOCK_CG_CLR, 0x00000003);
    /* IMP_IIC_WRAP_S CG Clear*/
    DRV_WriteReg32(IMP_IIC_WRAP_S_AP_CLOCK_CG_CLR, 0x0000000F);
    /* IMP_IIC_WRAP_EN CG Clear*/
    DRV_WriteReg32(IMP_IIC_WRAP_EN_AP_CLOCK_CG_CLR, 0x00000003);
    /* VLP_CK CG Clear*/
    DRV_WriteReg32(VLP_CK_VLP_CLK_CFG_30_SET, 0x00000832);
    /* SCP_IIC CG Clear*/
    DRV_WriteReg32(SCP_IIC_CCU_CLOCK_CG_SET, 0x00000003);
    /* SCP CG Clear*/
    DRV_WriteReg32(SCP_AP_SPI_CG, DRV_Reg32(SCP_AP_SPI_CG) | 0x00000003);
    /* VAD CG Clear*/
    if (!(ucSpare1 & EFUSE_MTCMOS_ADSP_AO_DIS_S1B17))
        DRV_WriteReg32(VAD_VADSYS_CK_EN, DRV_Reg32(VAD_VADSYS_CK_EN) | 0x0001003D);

    /* VLPCFG_AO_REG CG Clear*/
    DRV_WriteReg32(VLPCFG_AO_REG_DEBUGTOP_VLPAO_CTRL, DRV_Reg32(VLPCFG_AO_REG_DEBUGTOP_VLPAO_CTRL) & ~(0x00000100));
    /* VLPCFG_REG CG Clear*/
    DRV_WriteReg32(VLPCFG_REG_VLP_TEST_CK_CTRL, DRV_Reg32(VLPCFG_REG_VLP_TEST_CK_CTRL) | 0x11F43F9F);
    /* DVFSRC_TOP CG Clear*/
    DRV_WriteReg32(DVFSRC_TOP_DVFSRC_BASIC_CONTROL, DRV_Reg32(DVFSRC_TOP_DVFSRC_BASIC_CONTROL) | 0x00000001);
    /* GCE CG Clear*/
    //DRV_WriteReg32(GCE_GCE_CTL_INT0, DRV_Reg32(GCE_GCE_CTL_INT0) & ~(0x00010000));
    /* MDP_GCE CG Clear*/
    //DRV_WriteReg32(MDP_GCE_GCE_CTL_INT0, DRV_Reg32(MDP_GCE_GCE_CTL_INT0) & ~(0x00010000));

    /* DBGAO CG Clear*/
    DRV_WriteReg32(DBGAO_ATB, DRV_Reg32(DBGAO_ATB) | 0x00000001);
    /* DEM CG Clear*/
    DRV_WriteReg32(DEM_ATB, DRV_Reg32(DEM_ATB) | 0x00000001);
    DRV_WriteReg32(DEM_DBGBUSCLK_EN, DRV_Reg32(DEM_DBGBUSCLK_EN) | 0x00000001);
    DRV_WriteReg32(DEM_DBGSYSCLK_EN, DRV_Reg32(DEM_DBGSYSCLK_EN) | 0x00000001);
    /* UFSCFG_PDN_REG CG Clear*/
    DRV_WriteReg32(UFSCFG_PDN_REG_UFS_PDN_CG_0_CLR, 0x0000002B);
    /* AUDIO CG Clear*/
    DRV_WriteReg32(AUDIO_AUDIO_TOP_0, DRV_Reg32(AUDIO_AUDIO_TOP_0) & ~(0x03364F80));
    DRV_WriteReg32(AUDIO_AUDIO_TOP_1, DRV_Reg32(AUDIO_AUDIO_TOP_1) & ~(0x00F000FF));
    DRV_WriteReg32(AUDIO_AUDIO_TOP_2, DRV_Reg32(AUDIO_AUDIO_TOP_2) & ~(0x01323000));
    DRV_WriteReg32(AUDIO_AUDIO_TOP_3, DRV_Reg32(AUDIO_AUDIO_TOP_3) & ~(0x03F00000));
    DRV_WriteReg32(AUDIO_AUDIO_TOP_4, DRV_Reg32(AUDIO_AUDIO_TOP_4) & ~(0x0000301F));
    /* MMINFRA_CONFIG CG Clear*/
    DRV_WriteReg32(MMINFRA_CONFIG_MMINFRA_CG_0_CLR, 0x00000007);
    DRV_WriteReg32(MMINFRA_CONFIG_MMINFRA_CG_1_CLR, 0x00020000);

    /* MDPSYS_CONFIG CG Clear*/
    DRV_WriteReg32(MDPSYS_CONFIG_MDPSYS_CG_0_CLR, 0x337FCFFF);
    DRV_WriteReg32(MDPSYS_CONFIG_MDPSYS_CG_1_CLR, 0x00000018);
    /* GCE CG Clear*/
    DRV_WriteReg32(GCE_GCE_CTL_INT0, DRV_Reg32(GCE_GCE_CTL_INT0) & ~(0x00010000));
    /* MDP_GCE CG Clear*/
    DRV_WriteReg32(MDP_GCE_GCE_CTL_INT0, DRV_Reg32(MDP_GCE_GCE_CTL_INT0) & ~(0x00010000));

    /* IMGSYS1 CG Clear*/
    if (!(ucSpare2 & EFUSE_MTCMOS_ISP_IMG1_DIS_S2B18))
        DRV_WriteReg32(IMGSYS1_IMG_CG_CLR, 0x00001007);
    /* IMGSYS2 CG Clear*/
    if (!(ucSpare2 & EFUSE_MTCMOS_ISP_IMG2_DIS_S2B19))
        DRV_WriteReg32(IMGSYS2_IMG_CG_CLR, 0x000011C3);
    /* IPE CG Clear*/
    if (!(ucSpare2 & EFUSE_MTCMOS_ISP_IPE_DIS_S2B20))
        DRV_WriteReg32(IPE_IMG_CG_CLR, 0x0000013F);
    /* VDEC_CORE CG Clear*/
    DRV_WriteReg32(VDEC_CORE_LARB_CKEN_CON, DRV_Reg32(VDEC_CORE_LARB_CKEN_CON) | 0x00000001);
    DRV_WriteReg32(VDEC_CORE_VDEC_CKEN, DRV_Reg32(VDEC_CORE_VDEC_CKEN) | 0x00000011);
    /* VENC_GCON CG Clear*/
    DRV_WriteReg32(VENC_GCON_VENCSYS_CG_SET, 0x90011111);
    /* CAMSYS_MAIN CG Clear*/
    if (!(ucSpare2 & EFUSE_MTCMOS_CAM_MAIN_DIS_S2B23))
        DRV_WriteReg32(CAMSYS_MAIN_CAMSYS_CG_CLR, 0x003A1DC5);
    /* CAMSYS_RAWA CG Clear*/
    if (!(ucSpare2 & EFUSE_MTCMOS_CAM_RAWA_DIS_S2B21))
        DRV_WriteReg32(CAMSYS_RAWA_CAMSYS_CG_CLR, 0x00000007);
    /* CAMSYS_RAWB CG Clear*/
    if (!(ucSpare2 & EFUSE_MTCMOS_CAM_RAWB_DIS_S2B22))
        DRV_WriteReg32(CAMSYS_RAWB_CAMSYS_CG_CLR, 0x00000007);
    /* MMSYS_CONFIG CG Clear*/
    DRV_WriteReg32(MMSYS_CONFIG_MMSYS_CG_0_CLR, 0xFF7FFFFF);
    DRV_WriteReg32(MMSYS_CONFIG_MMSYS_CG_1_CLR, 0x0000007B);
    /* MFG CG Clear*/
    if (!(ucSpare2 & EFUSE_MTCMOS_MFG0_DIS_S2B0))
        DRV_WriteReg32(MFG_MFG_CG_CON_CLR, 0x00000001);
    INFO("subsysCG enable done...\n");
#else
    /* TOPCKGEN CG Clear*/
    DRV_WriteReg32(TOPCKGEN_CLK_MISC_CFG_3_CLR, 0x00010000);
    DRV_WriteReg32(TOPCKGEN_CLK_MISC_CFG_3_SET, 0xDF3CFCFF);
    /* INFRACFG_AO CG Clear*/
    DRV_WriteReg32(INFRACFG_AO_MODULE_CG_0_CLR, 0x10000000);
    DRV_WriteReg32(INFRACFG_AO_MODULE_CG_1_CLR, 0x21000000);
    DRV_WriteReg32(INFRACFG_AO_MODULE_CG_2_CLR, 0x08000000);
    DRV_WriteReg32(INFRACFG_AO_MODULE_CG_3_CLR, 0x02000000);
    /* PERICFG_AO CG Clear*/
    DRV_WriteReg32(PERICFG_AO_PERI_CG_0_CLR, 0x3FFFFFFF);
    DRV_WriteReg32(PERICFG_AO_PERI_CG_1_CLR, 0x3DBDFBF6);
    DRV_WriteReg32(PERICFG_AO_PERI_CG_2_CLR, 0x0FFFFFFB);
    /* UFSCFG_AO_REG CG Clear*/
    DRV_WriteReg32(UFSCFG_AO_REG_UFS_AO_CG_0_CLR, 0x0000007F);
    /* IMP_IIC_WRAP_WS CG Clear*/
    DRV_WriteReg32(IMP_IIC_WRAP_WS_AP_CLOCK_CG_CLR, 0x00000001);
    /* IMP_IIC_WRAP_E CG Clear*/
    DRV_WriteReg32(IMP_IIC_WRAP_E_AP_CLOCK_CG_CLR, 0x00000003);
    /* IMP_IIC_WRAP_S CG Clear*/
    DRV_WriteReg32(IMP_IIC_WRAP_S_AP_CLOCK_CG_CLR, 0x0000000F);
    /* IMP_IIC_WRAP_EN CG Clear*/
    DRV_WriteReg32(IMP_IIC_WRAP_EN_AP_CLOCK_CG_CLR, 0x00000003);
    /* VLP_CK CG Clear*/
    DRV_WriteReg32(VLP_CK_VLP_CLK_CFG_30_SET, 0x00000832);
    /* SCP_IIC CG Clear*/
    DRV_WriteReg32(SCP_IIC_CCU_CLOCK_CG_SET, 0x00000003);
    /* SCP CG Clear*/
    DRV_WriteReg32(SCP_AP_SPI_CG, DRV_Reg32(SCP_AP_SPI_CG) | 0x00000003);
    /* VLPCFG_AO_REG CG Clear*/
    DRV_WriteReg32(VLPCFG_AO_REG_DEBUGTOP_VLPAO_CTRL,
           DRV_Reg32(VLPCFG_AO_REG_DEBUGTOP_VLPAO_CTRL) & ~(0x00000100));
    /* VLPCFG_REG CG Clear*/
    DRV_WriteReg32(VLPCFG_REG_VLP_TEST_CK_CTRL,
           DRV_Reg32(VLPCFG_REG_VLP_TEST_CK_CTRL) | 0x11F43F9F);
    /* DVFSRC_TOP CG Clear*/
    DRV_WriteReg32(DVFSRC_TOP_DVFSRC_BASIC_CONTROL,
           DRV_Reg32(DVFSRC_TOP_DVFSRC_BASIC_CONTROL) | 0x00000001);
    /* DBGAO CG Clear*/
    DRV_WriteReg32(DBGAO_ATB, DRV_Reg32(DBGAO_ATB) | 0x00000001);
    /* DEM CG Clear*/
    DRV_WriteReg32(DEM_ATB, DRV_Reg32(DEM_ATB) | 0x00000001);
    DRV_WriteReg32(DEM_DBGBUSCLK_EN, DRV_Reg32(DEM_DBGBUSCLK_EN) | 0x00000001);
    DRV_WriteReg32(DEM_DBGSYSCLK_EN, DRV_Reg32(DEM_DBGSYSCLK_EN) | 0x00000001);
    /* UFSCFG_PDN_REG CG Clear*/
    DRV_WriteReg32(UFSCFG_PDN_REG_UFS_PDN_CG_0_CLR, 0x0000002B);
    /* MMINFRA_CONFIG CG Clear*/
    DRV_WriteReg32(MMINFRA_CONFIG_MMINFRA_CG_0_CLR, 0x00000007);
    DRV_WriteReg32(MMINFRA_CONFIG_MMINFRA_CG_1_CLR, 0x00020000);
    /* GCE CG Clear*/
    DRV_WriteReg32(GCE_GCE_CTL_INT0, DRV_Reg32(GCE_GCE_CTL_INT0) & ~(0x00010000));

    /* MMSYS_CONFIG CG Clear*/
    DRV_WriteReg32(MMSYS_CONFIG_MMSYS_CG_0_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(MMSYS_CONFIG_MMSYS_CG_1_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(MMSYS_CONFIG_MMSYS_CG_0_SET, 0xCF8FFFcc);
    DRV_WriteReg32(MMSYS_CONFIG_MMSYS_CG_1_SET, 0x4);
#endif
    /* pwm clock source switch */
    DRV_WriteReg32(PERICFG_AO_CLOCK_CON, DRV_Reg32(PERICFG_AO_CLOCK_CON) | 0x1550);

    /* disable display secure register settings */
    DRV_WriteReg32(MMSYS_DIS_SHADOW, 0x00000001);
    DRV_WriteReg32(MMSYS_CROSS_SHADOW, 0x00FF0000);
    DRV_WriteReg32(MMSYS_SECURE, 0xFFFFFFFF);
    DRV_WriteReg32(MMSYS_SECURE_1, 0xFFFFFFFF);
    DRV_WriteReg32(MMSYS_SECURE_2, 0xFFFFFFFF);
    /* set gce select register settings */
    DRV_WriteReg32(MMSYS_GCE_EVENT_SEL, 0x3);

    NOTICE( "Pll init Done!!\n");
}
