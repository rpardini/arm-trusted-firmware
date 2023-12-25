/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "mt8195.h"
#include "pll.h"
#include "spm_mtcmos.h"

#include <stdio.h>
#include <drivers/delay_timer.h>
#include <stdio.h>
#include <lib/mmio.h>
#include <stdbool.h>

#define print printf

#define DRV_WriteReg32(addr, data)  mmio_write_32(addr, data)
#define DRV_Reg32(addr) mmio_read_32(addr)

#define CLKMGR_BRINGUP	0

unsigned int mt_get_abist_freq(unsigned int ID)
{
    int output = 0, i = 0;
    unsigned int temp, clk26cali_0, clk_dbg_cfg, clk_misc_cfg_0, clk26cali_1;
    bool timeout = false;

    clk_dbg_cfg = DRV_Reg32(CLK_DBG_CFG);
    DRV_WriteReg32(CLK_DBG_CFG, (clk_dbg_cfg & 0xFEFF80FC) | (ID << 8));
    clk_misc_cfg_0 = DRV_Reg32(CLK_MISC_CFG_0);
    DRV_WriteReg32(CLK_MISC_CFG_0, (clk_misc_cfg_0 & 0x00FFFFFF) | (3 << 24)); /* divider = 4 */
    clk26cali_0 = DRV_Reg32(CLK26CALI_0);
    clk26cali_1 = DRV_Reg32(CLK26CALI_1);
    DRV_WriteReg32(CLK26CALI_0, 0x80);
    DRV_WriteReg32(CLK26CALI_0, 0x90);

    /* wait frequency meter finish */
    while (DRV_Reg32(CLK26CALI_0) & 0x10)
    {
        udelay(10);
        i++;
        if(i > 10000) {
            timeout = true;
            print("abist meter[%d], timeout\n", ID);
            break;
        }
    }

    if (!timeout) {
        temp = DRV_Reg32(CLK26CALI_1) & 0xFFFF;
        output = (temp * 26000) / 1024; /* KHz */
    } else {
        output = 0;
    }

    DRV_WriteReg32(CLK_DBG_CFG, clk_dbg_cfg);
    DRV_WriteReg32(CLK_MISC_CFG_0, clk_misc_cfg_0);
    DRV_WriteReg32(CLK26CALI_0, clk26cali_0);
    DRV_WriteReg32(CLK26CALI_1, clk26cali_1);

    /* print("abist meter[%d] = %d KHz\n", ID, output * 4); */
    return output * 4;
}

unsigned int mt_get_cpu_freq(void)
{
    unsigned int result = mt_get_abist_freq(52);

    print("ARMPLL_LL Freq = %d KHz\n", result);
	
	result = mt_get_abist_freq(53);

    print("ARMPLL_L Freq = %d KHz\n", result);

	result = mt_get_abist_freq(54);

    print("CCIPLL Freq = %d KHz\n", result);	
    return result;
}

void mt_set_topck_default(void)
{
    DRV_WriteReg32(CLK_CFG_0_CLR, 0xFFFFFF00);
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
    DRV_WriteReg32(CLK_CFG_20_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_21_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_22_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_23_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_24_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_25_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_26_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_27_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_28_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_29_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_30_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_31_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_32_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_33_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_34_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_35_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_36_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_37_CLR, 0xFFFFFF00);

    DRV_WriteReg32(CLK_CFG_UPDATE, 0xFFFFFFFE); /* except "axi" */
    DRV_WriteReg32(CLK_CFG_UPDATE1, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_UPDATE2, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_UPDATE3, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_UPDATE4, 0x000EFFFF); /* except "axi_west" */
}

void set_armpll_ll_rate(enum cpu_opp opp)
{
    static unsigned int opp_freq[] = {
        [CPU_OPP0] = 0x80133B13,	/* 2000MHz */
        [CPU_OPP1] = 0x80124EC4,	/* 1904MHz */
        [CPU_OPP2] = 0x80116000,	/* 1807MHz */
        [CPU_OPP3] = 0x801073B1,	/* 1711MHz */
        [CPU_OPP4] = 0x800F84EC,	/* 1614MHz */
        [CPU_OPP5] = 0x811D313B,	/* 1518MHz */
    };

    unsigned int temp;

    if (opp < 0 && opp >= CPU_OPP_NUM)
        return;

    /* switch to 26MHz first */
    print("switch to 26MHz\n");
    temp = DRV_Reg32(CPU_PLLDIV_CFG0);
    DRV_WriteReg32(CPU_PLLDIV_CFG0, (temp & 0xFFFFF9FF) | (0x00 << 9));

    /* disable armpll_ll */
    temp = DRV_Reg32(ARMPLL_LL_CON0);
    DRV_WriteReg32(ARMPLL_LL_CON0, temp & 0xFFFFFDFF);

    /* set CPU to new freq */
    print("ARMPLL_LL switch to OPP %d\n", opp);
    DRV_WriteReg32(ARMPLL_LL_CON2, opp_freq[opp]);

    /* enable armpll_ll */
    temp = DRV_Reg32(ARMPLL_LL_CON0);
    DRV_WriteReg32(ARMPLL_LL_CON0, temp | 0x200);

    udelay(20);

    /* switch back to ARMPLL_LL */
    print("switch to ARMPLL_LL\n");
    temp = DRV_Reg32(CPU_PLLDIV_CFG0);
    DRV_WriteReg32(CPU_PLLDIV_CFG0, (temp & 0xFFFFF9FF) | (0x01 << 9));

    print("ARMPLL_LL_CON2: 0x%x\n", DRV_Reg32(ARMPLL_LL_CON2));
}

void set_armpll_l_rate(enum cpu_opp opp)
{
    static unsigned int opp_freq[] = {
        [CPU_OPP0] = 0x80152762,	/* 2200MHz */
        [CPU_OPP1] = 0x80142000,	/* 2093MHz */
        [CPU_OPP2] = 0x80131B13,	/* 1987MHz */
        [CPU_OPP3] = 0x801213B1,	/* 1880MHz */
        [CPU_OPP4] = 0x80110C4E,	/* 1773MHz */
        [CPU_OPP5] = 0x80100762,	/* 1667MHz */
    };

    unsigned int temp;

    if (opp < 0 && opp >= CPU_OPP_NUM)
        return;

    /* switch to 26MHz first */
    print("switch to 26MHz\n");
    temp = DRV_Reg32(CPU_PLLDIV_CFG1);
    DRV_WriteReg32(CPU_PLLDIV_CFG1, (temp & 0xFFFFF9FF) | (0x00 << 9));

    /* disable armpll_l */
    temp = DRV_Reg32(ARMPLL_BL_CON0);
    DRV_WriteReg32(ARMPLL_BL_CON0, temp & 0xFFFFFDFF);

    /* set CPU to new freq */
    print("ARMPLL_L switch to OPP %d\n", opp);
    DRV_WriteReg32(ARMPLL_BL_CON2, opp_freq[opp]);

    /* enable armpll_l */
    temp = DRV_Reg32(ARMPLL_BL_CON0);
    DRV_WriteReg32(ARMPLL_BL_CON0, temp | 0x200);

    udelay(20);

    /* switch back to ARMPLL_L */
    print("switch to ARMPLL_L\n");
    temp = DRV_Reg32(CPU_PLLDIV_CFG1);
    DRV_WriteReg32(CPU_PLLDIV_CFG1, (temp & 0xFFFFF9FF) | (0x01 << 9));

    print("ARMPLL_BL_CON2: 0x%x\n", DRV_Reg32(ARMPLL_BL_CON2));
}

void set_ccipll_rate(enum cpu_opp opp)
{
    static unsigned int opp_freq[] = {
        [CPU_OPP0] = 0x8010589D,	/* 1700MHz */
        [CPU_OPP1] = 0x800F9627,	/* 1621MHz */
        [CPU_OPP2] = 0x811DA9D8,	/* 1543MHz */
        [CPU_OPP3] = 0x811C2762,	/* 1464MHz */
        [CPU_OPP4] = 0x811AA4EC,	/* 1385MHz */
        [CPU_OPP5] = 0x81192000,	/* 1307MHz */
    };

    unsigned int temp;

    if (opp < 0 && opp >= CPU_OPP_NUM)
        return;

    /* switch to 26MHz first */
    print("switch to 26MHz\n");
    temp = DRV_Reg32(BUS_PLLDIV_CFG);
    DRV_WriteReg32(BUS_PLLDIV_CFG, (temp & 0xFFFFF9FF) | (0x00 << 9));

    /* disable ccipll */
    temp = DRV_Reg32(CCIPLL_CON0);
    DRV_WriteReg32(CCIPLL_CON0, temp & 0xFFFFFDFF);

    /* set CCI to new freq */
    print("CCIPLL switch to OPP %d\n", opp);
    DRV_WriteReg32(CCIPLL_CON2, opp_freq[opp]);

    /* enable ccipll */
    temp = DRV_Reg32(CCIPLL_CON0);
    DRV_WriteReg32(CCIPLL_CON0, temp | 0x200);

    udelay(20);

    /* switch back to CCIPLL */
    print("switch to CCIPLL\n");
    temp = DRV_Reg32(BUS_PLLDIV_CFG);
    DRV_WriteReg32(BUS_PLLDIV_CFG, (temp & 0xFFFFF9FF) | (0x01 << 9));

    print("CCIPLL_CON2: 0x%x\n", DRV_Reg32(CCIPLL_CON2));
}

void mt_pll_init(void)
{
    unsigned int temp;

    print("PLL init start...\n");

    /*******************
     * Turn on CLKSQ PLL
     *******************/
    temp = DRV_Reg32(AP_PLL_CON0);
    DRV_WriteReg32(AP_PLL_CON0, temp | 0x4);	/* [2]=1 (CLKSQ_EN) */

    udelay(100);

    temp = DRV_Reg32(AP_PLL_CON0);
    DRV_WriteReg32(AP_PLL_CON0, temp | 0x2);	/* [1]=1 (CLKSQ_LPF_EN) */

    /********************
     * Turn on PLL PWR_ON
     ********************/
    temp = DRV_Reg32(ARMPLL_LL_CON4);
    DRV_WriteReg32(ARMPLL_LL_CON4, temp | 0x1);	/* [0]=1 (PWR_ON) */

    temp = DRV_Reg32(ARMPLL_BL_CON4);
    DRV_WriteReg32(ARMPLL_BL_CON4, temp | 0x1);

    temp = DRV_Reg32(CCIPLL_CON4);
    DRV_WriteReg32(CCIPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(NNAPLL_CON4);
    DRV_WriteReg32(NNAPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(RESPLL_CON4);
    DRV_WriteReg32(RESPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(ETHPLL_CON4);
    DRV_WriteReg32(ETHPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(MSDCPLL_CON4);
    DRV_WriteReg32(MSDCPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(TVDPLL1_CON4);
    DRV_WriteReg32(TVDPLL1_CON4, temp | 0x1);

    temp = DRV_Reg32(TVDPLL2_CON4);
    DRV_WriteReg32(TVDPLL2_CON4, temp | 0x1);

    temp = DRV_Reg32(MMPLL_CON4);
    DRV_WriteReg32(MMPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(MAINPLL_CON4);
    DRV_WriteReg32(MAINPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(VDECPLL_CON4);
    DRV_WriteReg32(VDECPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(IMGPLL_CON4);
    DRV_WriteReg32(IMGPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(UNIVPLL_CON4);
    DRV_WriteReg32(UNIVPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(HDMIPLL1_CON4);
    DRV_WriteReg32(HDMIPLL1_CON4, temp | 0x1);

    temp = DRV_Reg32(HDMIPLL2_CON4);
    DRV_WriteReg32(HDMIPLL2_CON4, temp | 0x1);

    temp = DRV_Reg32(HDMIRX_APLL_CON5);
    DRV_WriteReg32(HDMIRX_APLL_CON5, temp | 0x1);

    temp = DRV_Reg32(USB1PLL_CON4);
    DRV_WriteReg32(USB1PLL_CON4, temp | 0x1);

    temp = DRV_Reg32(ADSPPLL_CON4);
    DRV_WriteReg32(ADSPPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(APLL1_CON5);
    DRV_WriteReg32(APLL1_CON5, temp | 0x1);

    temp = DRV_Reg32(APLL2_CON5);
    DRV_WriteReg32(APLL2_CON5, temp | 0x1);

    temp = DRV_Reg32(APLL3_CON5);
    DRV_WriteReg32(APLL3_CON5, temp | 0x1);

    temp = DRV_Reg32(APLL4_CON5);
    DRV_WriteReg32(APLL4_CON5, temp | 0x1);

    temp = DRV_Reg32(APLL5_CON5);
    DRV_WriteReg32(APLL5_CON5, temp | 0x1);

    temp = DRV_Reg32(MFGPLL_CON4);
    DRV_WriteReg32(MFGPLL_CON4, temp | 0x1);

    temp = DRV_Reg32(DGIPLL_CON4);
    DRV_WriteReg32(DGIPLL_CON4, temp | 0x1);

    udelay(30);

    /*********************
     * Turn off PLL_ISO_EN
     *********************/
    temp = DRV_Reg32(ARMPLL_LL_CON4);
    DRV_WriteReg32(ARMPLL_LL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(ARMPLL_BL_CON4);
    DRV_WriteReg32(ARMPLL_BL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(CCIPLL_CON4);
    DRV_WriteReg32(CCIPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(NNAPLL_CON4);
    DRV_WriteReg32(NNAPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(RESPLL_CON4);
    DRV_WriteReg32(RESPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(ETHPLL_CON4);
    DRV_WriteReg32(ETHPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(MSDCPLL_CON4);
    DRV_WriteReg32(MSDCPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(TVDPLL1_CON4);
    DRV_WriteReg32(TVDPLL1_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(TVDPLL2_CON4);
    DRV_WriteReg32(TVDPLL2_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(MMPLL_CON4);
    DRV_WriteReg32(MMPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(MAINPLL_CON4);
    DRV_WriteReg32(MAINPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(VDECPLL_CON4);
    DRV_WriteReg32(VDECPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(IMGPLL_CON4);
    DRV_WriteReg32(IMGPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(UNIVPLL_CON4);
    DRV_WriteReg32(UNIVPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(HDMIPLL1_CON4);
    DRV_WriteReg32(HDMIPLL1_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(HDMIPLL2_CON4);
    DRV_WriteReg32(HDMIPLL2_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(HDMIRX_APLL_CON5);
    DRV_WriteReg32(HDMIRX_APLL_CON5, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(USB1PLL_CON4);
    DRV_WriteReg32(USB1PLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(ADSPPLL_CON4);
    DRV_WriteReg32(ADSPPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(APLL1_CON5);
    DRV_WriteReg32(APLL1_CON5, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(APLL2_CON5);
    DRV_WriteReg32(APLL2_CON5, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(APLL3_CON5);
    DRV_WriteReg32(APLL3_CON5, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(APLL4_CON5);
    DRV_WriteReg32(APLL4_CON5, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(APLL5_CON5);
    DRV_WriteReg32(APLL5_CON5, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(MFGPLL_CON4);
    DRV_WriteReg32(MFGPLL_CON4, temp & 0xFFFFFFFD);

    temp = DRV_Reg32(DGIPLL_CON4);
    DRV_WriteReg32(DGIPLL_CON4, temp & 0xFFFFFFFD);

    udelay(1);

    /**********************************
     * Turn off PLL_GLITCH_EN (<374MHz)
     **********************************/
    temp = DRV_Reg32(APLL1_CON0);
    DRV_WriteReg32(APLL1_CON0, temp & 0xFFFFEFFF);

    temp = DRV_Reg32(APLL2_CON0);
    DRV_WriteReg32(APLL2_CON0, temp & 0xFFFFEFFF);

    temp = DRV_Reg32(APLL3_CON0);
    DRV_WriteReg32(APLL3_CON0, temp & 0xFFFFEFFF);

    temp = DRV_Reg32(APLL4_CON0);
    DRV_WriteReg32(APLL4_CON0, temp & 0xFFFFEFFF);

    temp = DRV_Reg32(APLL5_CON0);
    DRV_WriteReg32(APLL5_CON0, temp & 0xFFFFEFFF);

    temp = DRV_Reg32(VDECPLL_CON0);
    DRV_WriteReg32(VDECPLL_CON0, temp & 0xFFFFEFFF);

    temp = DRV_Reg32(DGIPLL_CON0);
    DRV_WriteReg32(DGIPLL_CON0, temp & 0xFFFFEFFF);

    temp = DRV_Reg32(USB1PLL_CON0);
    DRV_WriteReg32(USB1PLL_CON0, temp & 0xFFFFEFFF);

    temp = DRV_Reg32(HDMIRX_APLL_CON0);
    DRV_WriteReg32(HDMIRX_APLL_CON0, temp & 0xFFFFEFFF);

    /*******************
     * SET MFGPLL CK_EN
     *******************/
    temp = DRV_Reg32(MFGPLL_CON0);
    DRV_WriteReg32(MFGPLL_CON0, temp & 0xFFEFFFFF);	/* [20]=0 (ck_en) */

    temp = DRV_Reg32(MFGPLL_CON1);
    DRV_WriteReg32(MFGPLL_CON1, temp | 0x00000004);	/* [2]=1 (opp_ck_en) */

    /*******************
     * set PLL frequency
     *******************/
    DRV_WriteReg32(ARMPLL_LL_CON2, 0x8113E9D8);		/* 1036 MHz */

    DRV_WriteReg32(ARMPLL_BL_CON2, 0x8113BD89);		/* 1027 MHz */

    DRV_WriteReg32(CCIPLL_CON2, 0x81100C4E);		/* 835 MHz */

    DRV_WriteReg32(NNAPLL_CON2, 0x81108A25);		/* 860.06 MHz */

    DRV_WriteReg32(RESPLL_CON2, 0x821713B2);		/* 600 MHz */

    DRV_WriteReg32(ETHPLL_CON2, 0x82133B14);		/* 500 MHz */

    DRV_WriteReg32(MSDCPLL_CON2, 0x831D89D9);		/* 384 MHz */

    DRV_WriteReg32(TVDPLL1_CON2, 0x8216DA5C);		/* 594.177 MHz */

    DRV_WriteReg32(TVDPLL2_CON2, 0x8216DA5C);		/* 594.177 MHz */

    DRV_WriteReg32(MMPLL_CON2, 0x801A715A);		/* 2750.048 MHz */

    DRV_WriteReg32(MAINPLL_CON2, 0x801500E3);		/* 2184.359 MHz */

    DRV_WriteReg32(VDECPLL_CON2, 0x8310EC4F);		/* 220 MHz */

    DRV_WriteReg32(IMGPLL_CON2, 0x82190042);		/* 650.026 MHz */

    DRV_WriteReg32(UNIVPLL_CON2, 0x80180000);		/* 2496 MHz */

    DRV_WriteReg32(HDMIPLL1_CON2, 0x81110015);		/* 884.016 MHz */

    DRV_WriteReg32(HDMIPLL2_CON2, 0x821713EE);		/* 600.024 MHz */

    DRV_WriteReg32(HDMIRX_APLL_CON2, 0x83000000);
    DRV_WriteReg32(HDMIRX_APLL_CON3, 0x5ABE3A4A);	/* 294.915 MHz */

    DRV_WriteReg32(USB1PLL_CON2, 0x830EC4ED);		/* 192 MHz */

    DRV_WriteReg32(ADSPPLL_CON2, 0x821BB13C);		/* 720 MHz */

    DRV_WriteReg32(APLL1_CON3, 0x3C7EA932);		/* 196.608 MHz */
    DRV_WriteReg32(APLL1_CON2, 0x83000000);
    DRV_WriteReg32(APLL1_TUNER_CON0, 0x3C7EA933);

    DRV_WriteReg32(APLL2_CON3, 0x6F28BD4C);		/* 180.6336 MHz */
    DRV_WriteReg32(APLL2_CON2, 0x84000000);
    DRV_WriteReg32(APLL2_TUNER_CON0, 0x6F28BD4D);

    DRV_WriteReg32(APLL3_CON3, 0x3C7EA932);		/* 196.608 MHz */
    DRV_WriteReg32(APLL3_CON2, 0x83000000);
    DRV_WriteReg32(APLL3_TUNER_CON0, 0x3C7EA933);

    DRV_WriteReg32(APLL4_CON3, 0x3C7EA932);		/* 196.608 MHz */
    DRV_WriteReg32(APLL4_CON2, 0x83000000);
    DRV_WriteReg32(APLL4_TUNER_CON0, 0x3C7EA933);

    DRV_WriteReg32(APLL5_CON3, 0x3C7EA932);		/* 196.608 MHz */
    DRV_WriteReg32(APLL5_CON2, 0x83000000);
    DRV_WriteReg32(APLL5_TUNER_CON0, 0x3C7EA933);

    DRV_WriteReg32(MFGPLL_CON2, 0x821AEC4F);		/* 700 MHz */

    DRV_WriteReg32(DGIPLL_CON2, 0x84196277);		/* 165 MHz */

    /****************
     * Turn on PLL_EN
     ****************/
    temp = DRV_Reg32(ARMPLL_LL_CON0);
    DRV_WriteReg32(ARMPLL_LL_CON0, temp | 0x200);	/* [9]=1 (PLL_EN) */

    temp = DRV_Reg32(ARMPLL_BL_CON0);
    DRV_WriteReg32(ARMPLL_BL_CON0, temp | 0x200);

    temp = DRV_Reg32(CCIPLL_CON0);
    DRV_WriteReg32(CCIPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(NNAPLL_CON0);
    DRV_WriteReg32(NNAPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(RESPLL_CON0);
    DRV_WriteReg32(RESPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(ETHPLL_CON0);
    DRV_WriteReg32(ETHPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(MSDCPLL_CON0);
    DRV_WriteReg32(MSDCPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(TVDPLL1_CON0);
    DRV_WriteReg32(TVDPLL1_CON0, temp | 0x200);

    temp = DRV_Reg32(TVDPLL2_CON0);
    DRV_WriteReg32(TVDPLL2_CON0, temp | 0x200);

    temp = DRV_Reg32(MMPLL_CON0);
    DRV_WriteReg32(MMPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(MAINPLL_CON0);
    DRV_WriteReg32(MAINPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(VDECPLL_CON0);
    DRV_WriteReg32(VDECPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(IMGPLL_CON0);
    DRV_WriteReg32(IMGPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(UNIVPLL_CON0);
    DRV_WriteReg32(UNIVPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(HDMIPLL1_CON0);
    DRV_WriteReg32(HDMIPLL1_CON0, temp | 0x200);

    temp = DRV_Reg32(HDMIPLL2_CON0);
    DRV_WriteReg32(HDMIPLL2_CON0, temp | 0x200);

    temp = DRV_Reg32(HDMIRX_APLL_CON0);
    DRV_WriteReg32(HDMIRX_APLL_CON0, temp | 0x200);

    temp = DRV_Reg32(USB1PLL_CON0);
    DRV_WriteReg32(USB1PLL_CON0, temp | 0x200);

    temp = DRV_Reg32(ADSPPLL_CON0);
    DRV_WriteReg32(ADSPPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(APLL1_CON0);
    DRV_WriteReg32(APLL1_CON0, temp | 0x200);

    temp = DRV_Reg32(APLL2_CON0);
    DRV_WriteReg32(APLL2_CON0, temp | 0x200);

    temp = DRV_Reg32(APLL3_CON0);
    DRV_WriteReg32(APLL3_CON0, temp | 0x200);

    temp = DRV_Reg32(APLL4_CON0);
    DRV_WriteReg32(APLL4_CON0, temp | 0x200);

    temp = DRV_Reg32(APLL5_CON0);
    DRV_WriteReg32(APLL5_CON0, temp | 0x200 | (0x1 << 20));

    temp = DRV_Reg32(MFGPLL_CON0);
    DRV_WriteReg32(MFGPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(DGIPLL_CON0);
    DRV_WriteReg32(DGIPLL_CON0, temp | 0x200);

    /* Enable analog divider */
    temp = DRV_Reg32(UNIVPLL_CON0);
    DRV_WriteReg32(UNIVPLL_CON0, temp | 0x8D);	/* enable[7] analog DIV=13[0:3] */

    udelay(20);

    /**************
     * PLL DIV RSTB
     **************/
    temp = DRV_Reg32(MAINPLL_CON0);
    DRV_WriteReg32(MAINPLL_CON0, temp | 0xFF000000);	/* [31:24]=0xff (DIV_EN) */

    temp = DRV_Reg32(UNIVPLL_CON0);
    DRV_WriteReg32(UNIVPLL_CON0, temp | 0xFF000000);

    temp = DRV_Reg32(MMPLL_CON0);
    DRV_WriteReg32(MMPLL_CON0, temp | 0xFF000000);

    temp = DRV_Reg32(MAINPLL_CON0);
    DRV_WriteReg32(MAINPLL_CON0, temp | 0x00800000);    /* [23]=1 (RSTB) */

    temp = DRV_Reg32(UNIVPLL_CON0);
    DRV_WriteReg32(UNIVPLL_CON0, temp | 0x00800000);

    temp = DRV_Reg32(MMPLL_CON0);
    DRV_WriteReg32(MMPLL_CON0, temp | 0x00800000);

    /*****************
     * xPLL HW Control
     *****************/

    /*********************
     * CPU CLKMUX, CLK DIV
     *********************/
    /* 8: div1, A: div2, B: div4, 1D: div6 */
    temp = DRV_Reg32(CPU_PLLDIV_CFG0) ;
    DRV_WriteReg32(CPU_PLLDIV_CFG0, (temp & 0xFFC1FFFF) | (0x08 << 17));

    temp = DRV_Reg32(CPU_PLLDIV_CFG1) ;
    DRV_WriteReg32(CPU_PLLDIV_CFG1, (temp & 0xFFC1FFFF) | (0x08 << 17));

    temp = DRV_Reg32(BUS_PLLDIV_CFG) ;
    DRV_WriteReg32(BUS_PLLDIV_CFG, (temp & 0xFFC1FFFF) | (0x08 << 17));

    /* 0: 26m,  1: armpll, 2: mainpll, 3: univpll */
    temp = DRV_Reg32(CPU_PLLDIV_CFG0);
    DRV_WriteReg32(CPU_PLLDIV_CFG0, (temp & 0xFFFFF9FF) | (0x01 << 9));

    temp = DRV_Reg32(CPU_PLLDIV_CFG1);
    DRV_WriteReg32(CPU_PLLDIV_CFG1, (temp & 0xFFFFF9FF) | (0x01 << 9));

    temp = DRV_Reg32(BUS_PLLDIV_CFG);
    DRV_WriteReg32(BUS_PLLDIV_CFG, (temp & 0xFFFFF9FF) | (0x01 << 9));

    /************
     * TOP CLKMUX
     *************/
    /* rg_axi_dcm_dis_en = 1 , rg_pllck_sel_no_spm = 1 */
    DRV_WriteReg32(INFRA_BUS_DCM_CTRL, DRV_Reg32(INFRA_BUS_DCM_CTRL) | (1 << 21) | (1 << 22));
    /* [1]enable bus dcm divider */
    DRV_WriteReg32(VDNR_DCM_TOP_INFRA_CTRL0, (DRV_Reg32(VDNR_DCM_TOP_INFRA_CTRL0) | 0x2));

    /* switch to SPM control */
    temp = DRV_Reg32(CLK_SCP_CFG_0);
    DRV_WriteReg32(CLK_SCP_CFG_0, (temp | 0x3FF));

    /* axi bus clock */
    DRV_WriteReg32(CLK_CFG_0_CLR, 0x000000FF);
    DRV_WriteReg32(CLK_CFG_0_SET, 0x00000002);	/* 156 MHz */
    DRV_WriteReg32(CLK_CFG_UPDATE, 0x00000001);

    /* west peripheral bus clock */
    DRV_WriteReg32(CLK_CFG_37_CLR, 0x000000FF);
    DRV_WriteReg32(CLK_CFG_37_SET, 0x00000002);	/* 109.2 MHz */
    DRV_WriteReg32(CLK_CFG_UPDATE4, 0x00100000);

    DRV_WriteReg32(CLK_CFG_0_CLR, 0xFFFFFF00);
    DRV_WriteReg32(CLK_CFG_0_SET, 0x03070200);

    DRV_WriteReg32(CLK_CFG_1_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_1_SET, 0x08080809);

    DRV_WriteReg32(CLK_CFG_2_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_2_SET, 0x07020B02);

    DRV_WriteReg32(CLK_CFG_3_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_3_SET, 0x07070707);

    DRV_WriteReg32(CLK_CFG_4_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_4_SET, 0x07070707);

    DRV_WriteReg32(CLK_CFG_5_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_5_SET, 0x02020203);

    DRV_WriteReg32(CLK_CFG_6_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_6_SET, 0x04000202);

    DRV_WriteReg32(CLK_CFG_7_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_7_SET, 0x01010101);

    DRV_WriteReg32(CLK_CFG_8_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_8_SET, 0x02010301);

    DRV_WriteReg32(CLK_CFG_9_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_9_SET, 0x03030101);

    DRV_WriteReg32(CLK_CFG_10_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_10_SET, 0x01010103);

    DRV_WriteReg32(CLK_CFG_11_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_11_SET, 0x01010101);

    DRV_WriteReg32(CLK_CFG_12_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_12_SET, 0x01010101);

    DRV_WriteReg32(CLK_CFG_13_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_13_SET, 0x04040402);

    DRV_WriteReg32(CLK_CFG_14_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_14_SET, 0x03010304);

    DRV_WriteReg32(CLK_CFG_15_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_15_SET, 0x00000605);

    DRV_WriteReg32(CLK_CFG_16_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_16_SET, 0x0101010E);

    DRV_WriteReg32(CLK_CFG_17_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_17_SET, 0x02000707);

    DRV_WriteReg32(CLK_CFG_18_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_18_SET, 0x04010502);

    DRV_WriteReg32(CLK_CFG_19_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_19_SET, 0x01010203);

    DRV_WriteReg32(CLK_CFG_20_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_20_SET, 0x03010200);

    DRV_WriteReg32(CLK_CFG_21_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_21_SET, 0x01010501);

    DRV_WriteReg32(CLK_CFG_22_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_22_SET, 0x03030308);

    DRV_WriteReg32(CLK_CFG_23_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_23_SET, 0x01010101);

    DRV_WriteReg32(CLK_CFG_24_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_24_SET, 0x80060601); /* de-feature: [31:24]hf_fi2so4_mck_sel */

    DRV_WriteReg32(CLK_CFG_25_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_25_SET, 0x80060680); /* de-feature: [7:0]hf_fi2so5_mck_sel, [31:24]hf_fi2si4_mck_sel */

    DRV_WriteReg32(CLK_CFG_26_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_26_SET, 0x01060680); /* de-feature: [7:0]hf_fi2si5_mck_sel */

    DRV_WriteReg32(CLK_CFG_27_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_27_SET, 0x07020101);

    DRV_WriteReg32(CLK_CFG_28_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_28_SET, 0x03030107);

    DRV_WriteReg32(CLK_CFG_29_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_29_SET, 0x00808001);	/* scp(ulposc_sel/ulposc_core_sel) */

    DRV_WriteReg32(CLK_CFG_30_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_30_SET, 0x80808080); /* occ */

    DRV_WriteReg32(CLK_CFG_31_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_31_SET, 0x80808080); /* occ */

    DRV_WriteReg32(CLK_CFG_32_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_32_SET, 0x80808080); /* occ */

    DRV_WriteReg32(CLK_CFG_33_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_33_SET, 0x80808080); /* occ */

    DRV_WriteReg32(CLK_CFG_34_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_34_SET, 0x80808080); /* occ */

    DRV_WriteReg32(CLK_CFG_35_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_35_SET, 0x80808080); /* occ */

    DRV_WriteReg32(CLK_CFG_36_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_36_SET, 0x80808080); /* occ */

    DRV_WriteReg32(CLK_CFG_37_CLR, 0xFFFFFF00);
    DRV_WriteReg32(CLK_CFG_37_SET, 0x00838300); /* rsvd_2,rsvd_3: off */

    DRV_WriteReg32(CLK_CFG_UPDATE, 0xFFFFFFFE);	/* except "axi" */
    DRV_WriteReg32(CLK_CFG_UPDATE1, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_UPDATE2, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_UPDATE3, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_UPDATE4, 0x000EFFFF); /* except "axi_west" */

    temp = DRV_Reg32(CLK_MISC_CFG_3);
    DRV_WriteReg32(CLK_MISC_CFG_3, temp | 0x00000100);

    /********
     * MTCMOS and SUBSYS_CG
     ********/
    print("PLL init Done!\n");

    print("MTCMOS init Start...\n");
    /* Switch sram control to bypass mode for PCIE_MAC_P0 */
    temp = DRV_Reg32(AP_MDSRC_REQ);
    DRV_WriteReg32(AP_MDSRC_REQ, temp | 0x1);
#if CLKMGR_BRINGUP
    spm_mtcmos_on_all();
#else
#if 1
    spm_mtcmos_on(MT8195_POWER_DOMAIN_VPPSYS0);
    spm_mtcmos_on(MT8195_POWER_DOMAIN_VDOSYS0);
    spm_mtcmos_on(MT8195_POWER_DOMAIN_VPPSYS1);
    spm_mtcmos_on(MT8195_POWER_DOMAIN_VDOSYS1);
#endif
	#endif
    print("MTCMOS init Done!\n");

#if CLKMGR_BRINGUP
    temp = DRV_Reg32(TOPCKGEN_CLK_MISC_CFG_1);
    DRV_WriteReg32(TOPCKGEN_CLK_MISC_CFG_1, temp | 0x0000be7f);
    temp = DRV_Reg32(TOPCKGEN_CLK_MISC_CFG_3);
    DRV_WriteReg32(TOPCKGEN_CLK_MISC_CFG_3, temp | 0x000000ff);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_0_CLR, 0x3fef876f);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_1_CLR, 0xa797fef7);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_2_CLR, 0xfe06be5f);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_3_CLR, 0x371387a7);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_4_CLR, 0x81c60183);
    temp = DRV_Reg32(APMIXEDSYS_AP_PLL_CON2);
    DRV_WriteReg32(APMIXEDSYS_AP_PLL_CON2, temp | 0x00000002);
    temp = DRV_Reg32(IPNNA_F26M_CK_CG);
    DRV_WriteReg32(IPNNA_F26M_CK_CG, temp | 0x00000001);
    temp = DRV_Reg32(IPNNA_AXI_CK_CG);
    DRV_WriteReg32(IPNNA_AXI_CK_CG, temp | 0x00000001);
    temp = DRV_Reg32(IPNNA_NNA0_CG_EN);
    DRV_WriteReg32(IPNNA_NNA0_CG_EN, temp | 0x00000001);
    temp = DRV_Reg32(IPNNA_NNA1_CG_EN);
    DRV_WriteReg32(IPNNA_NNA1_CG_EN, temp | 0x00000001);
    temp = DRV_Reg32(IPNNA_NNA0_EMI_CG_EN);
    DRV_WriteReg32(IPNNA_NNA0_EMI_CG_EN, temp | 0x00000011);
    temp = DRV_Reg32(IPNNA_NNA1_EMI_CG_EN);
    DRV_WriteReg32(IPNNA_NNA1_EMI_CG_EN, temp | 0x00000001);
    temp = DRV_Reg32(IPNNA_NNA0_AXI_CG_EN);
    DRV_WriteReg32(IPNNA_NNA0_AXI_CG_EN, temp | 0x00000001);
    temp = DRV_Reg32(IPNNA_NNA1_AXI_CG_EN);
    DRV_WriteReg32(IPNNA_NNA1_AXI_CG_EN, temp | 0x00000001);
    temp = DRV_Reg32(SCP_PAR_TOP_AUDIODSP_CK_CG);
    DRV_WriteReg32(SCP_PAR_TOP_AUDIODSP_CK_CG, temp & ~0x00000001);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_0);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_0, temp & ~0x9fbc0c14);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_4);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_4, temp & ~0xc17b01c3);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_5);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_5, temp & ~0x0dcc03bf);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_6);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_6, temp & ~0x000fffff);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_1);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_1, temp & ~0x000f7c04);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_3);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_3, temp & ~0x000000a0);
    temp = DRV_Reg32(AUDIO_SRC_MEM_ASRC_TOP_1);
    DRV_WriteReg32(AUDIO_SRC_MEM_ASRC_TOP_1, temp & ~0x00000fff);
    DRV_WriteReg32(PERICFG_AO_PERI_MODULE_SW_CG_0_CLR, 0x03199fab);
    DRV_WriteReg32(MFGCFG_MFG_CG_CLR, 0x00000001);
    DRV_WriteReg32(VPP0_REG_VPPSYS0_CG0_CLR, 0x0007ffff);
    DRV_WriteReg32(VPP0_REG_VPPSYS0_CG1_CLR, 0x0fffffff);
    DRV_WriteReg32(VPP0_REG_VPPSYS0_CG2_CLR, 0x0000000f);
    temp = DRV_Reg32(WPESYS_TOP_REG_WPESYS_RG_000);
    DRV_WriteReg32(WPESYS_TOP_REG_WPESYS_RG_000, temp | 0x031f0000);
    temp = DRV_Reg32(WPE_VPP0_CTL_WPE_DCM_DIS);
    DRV_WriteReg32(WPE_VPP0_CTL_WPE_DCM_DIS, temp | 0x0003ffff);
    temp = DRV_Reg32(WPE_VPP0_CTL_DMA_DCM_DIS);
    DRV_WriteReg32(WPE_VPP0_CTL_DMA_DCM_DIS, temp | 0x0000001f);
    temp = DRV_Reg32(WPE_VPP1_CTL_WPE_DCM_DIS);
    DRV_WriteReg32(WPE_VPP1_CTL_WPE_DCM_DIS, temp | 0x0003ffff);
    temp = DRV_Reg32(WPE_VPP1_CTL_DMA_DCM_DIS);
    DRV_WriteReg32(WPE_VPP1_CTL_DMA_DCM_DIS, temp | 0x0000001f);
    DRV_WriteReg32(VPPSYS1_CONFIG_VPPSYS1_CG_0_CLR, 0xffffffff);
    DRV_WriteReg32(VPPSYS1_CONFIG_VPPSYS1_CG_1_CLR, 0x0001ffff);
    DRV_WriteReg32(IMGSYS_MAIN_IMG_MAIN_CG_CLR, 0x80001f1f);
    DRV_WriteReg32(IMGSYS1_DIP_TOP_MACRO_CG_CLR, 0x00000003);
    DRV_WriteReg32(IMGSYS1_DIP_NR_MACRO_CG_CLR, 0x00000003);
    DRV_WriteReg32(IMGSYS1_WPE_MACRO_CG_CLR, 0x00000003);
    temp = DRV_Reg32(IPESYS_MACRO_CG);
    DRV_WriteReg32(IPESYS_MACRO_CG, temp & ~0x0000001f);
    DRV_WriteReg32(CAMSYS_MAIN_CAMSYS_CG_CLR, 0x07ff1ffb);
    DRV_WriteReg32(CAMSYS_RAWA_CAMSYS_CG_CLR, 0x00000007);
    DRV_WriteReg32(CAMSYS_YUVA_CAMSYS_CG_CLR, 0x00000007);
    DRV_WriteReg32(CAMSYS_RAWB_CAMSYS_CG_CLR, 0x00000007);
    DRV_WriteReg32(CAMSYS_YUVB_CAMSYS_CG_CLR, 0x00000007);
    DRV_WriteReg32(CAMSYS_MRAW_CAMSYS_CG_CLR, 0x0000007d);
    DRV_WriteReg32(CCU_MAIN_CCUSYS_CG_CLR, 0x0000000f);
    temp = DRV_Reg32(VDEC_SOC_GCON_VDEC_CKEN);
    DRV_WriteReg32(VDEC_SOC_GCON_VDEC_CKEN, temp | 0x00000001);
    temp = DRV_Reg32(VDEC_SOC_GCON_LAT_CKEN);
    DRV_WriteReg32(VDEC_SOC_GCON_LAT_CKEN, temp | 0x00000001);
    temp = DRV_Reg32(VDEC_SOC_GCON_LARB_CKEN_CON);
    DRV_WriteReg32(VDEC_SOC_GCON_LARB_CKEN_CON, temp | 0x00000001);
    temp = DRV_Reg32(VDEC_GCON_VDEC_CKEN);
    DRV_WriteReg32(VDEC_GCON_VDEC_CKEN, temp | 0x00000001);
    temp = DRV_Reg32(VDEC_GCON_LAT_CKEN);
    DRV_WriteReg32(VDEC_GCON_LAT_CKEN, temp | 0x00000001);
    temp = DRV_Reg32(VDEC_GCON_LARB_CKEN_CON);
    DRV_WriteReg32(VDEC_GCON_LARB_CKEN_CON, temp | 0x00000001);
    temp = DRV_Reg32(VDEC_CORE1_GCON_VDEC_CKEN);
    DRV_WriteReg32(VDEC_CORE1_GCON_VDEC_CKEN, temp | 0x00000001);
    temp = DRV_Reg32(VDEC_CORE1_GCON_LAT_CKEN);
    DRV_WriteReg32(VDEC_CORE1_GCON_LAT_CKEN, temp | 0x00000001);
    temp = DRV_Reg32(VDEC_CORE1_GCON_LARB_CKEN_CON);
    DRV_WriteReg32(VDEC_CORE1_GCON_LARB_CKEN_CON, temp | 0x00000001);
    DRV_WriteReg32(VENC_GCON_VENCSYS_CG_SET, 0x10011111);
    DRV_WriteReg32(VENC_CORE1_GCON_VENCSYS_CG_SET, 0x10011111);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL0_CG_0_CLR, 0xffffffff);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL0_CG_1_CLR, 0xffffffff);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL0_CG_2_CLR, 0xffffffff);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL1_CG_0_CLR, 0xffffffff);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL1_CG_1_CLR, 0xffffffff);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL1_CG_2_CLR, 0xffffffff);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_0_CLR, 0xffffffff);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_1_CLR, 0xffffffff);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_2_CLR, 0xffffffff);
#else
    /* critical clock */
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_0_CLR, 0x00000020);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_1_CLR, 0x00100000);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_3_CLR, 0x02000000);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_4_CLR, 0x00000003);
    /* logo */
    temp = DRV_Reg32(TOPCKGEN_CLK_MISC_CFG_1);
    DRV_WriteReg32(TOPCKGEN_CLK_MISC_CFG_1, temp | 0x0000000c);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_1_CLR, 0x00023001);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL0_CG_0_CLR, 0x04e80555);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL0_CG_2_CLR, 0x00000101);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_0_CLR, 0x28c72800);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_1_CLR, 0x00ff0007);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_2_CLR, 0x00000101);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_3_CLR, 0x00000100);
    DRV_WriteReg32(VPPSYS1_CONFIG_VPPSYS1_CG_1_CLR, 0x00001000);
    /* not model in kernel, make it default off */
    DRV_WriteReg32(PERICFG_AO_PERI_MODULE_SW_CG_0_SET, 0x00000010);
#endif

    set_armpll_ll_rate(CPU_OPP0);
#if CLKMGR_BRINGUP
    mt_get_cpu_freq();
#endif
}
