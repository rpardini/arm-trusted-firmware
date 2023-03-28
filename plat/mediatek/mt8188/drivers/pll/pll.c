/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pll.h"
#include "spm_mtcmos.h"

#include <stdio.h>
#include <drivers/delay_timer.h>
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
        if(i > 50) {
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

static unsigned int mt_get_ckgen_freq(unsigned int ID)
{
    int output = 0, i = 0;
    unsigned int temp, clk26cali_0, clk_dbg_cfg, clk_misc_cfg_0, clk26cali_1;
    bool timeout = false;

    clk_dbg_cfg = DRV_Reg32(CLK_DBG_CFG);
    DRV_WriteReg32(CLK_DBG_CFG, (clk_dbg_cfg & 0xFE00FFFC) | (ID << 16) | 0x1 | (0x1 << 24));

    clk_misc_cfg_0 = DRV_Reg32(CLK_MISC_CFG_0);
    DRV_WriteReg32(CLK_MISC_CFG_0, (clk_misc_cfg_0 & 0x00FFFFFF)); /* divider = 0*/
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
            print("ckgen meter[%d], timeout\n", ID);
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

    /* print("ckgen meter[%d] = %d KHz\n", ID, output); */
    return output;
}

unsigned int mt_get_cpu_freq(void)
{
    unsigned int result;

    result = mt_get_abist_freq(1);

    print("ARMPLL_LL Freq = %d KHz\n", result);

    result = mt_get_abist_freq(2);

    print("ARMPLL_L Freq = %d KHz\n", result);

    result = mt_get_abist_freq(3);

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
    DRV_WriteReg32(CLK_CFG_22_CLR, 0x00FFFFFF);

    DRV_WriteReg32(CLK_CFG_UPDATE, 0xFFFFFFFE); /* except "axi" */
    DRV_WriteReg32(CLK_CFG_UPDATE1, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_UPDATE2, 0x07FFFFFF);
}

void set_armpll_ll_rate(enum cpu_opp opp)
{
    static unsigned int opp_freq[] = {
        [CPU_OPP0] = 0x80133B14,	/* 2000MHz */
        [CPU_OPP1] = 0x80124C4F,	/* 1903MHz */
        [CPU_OPP2] = 0x80116000,	/* 1807MHz */
        [CPU_OPP3] = 0x8010713C,	/* 1710MHz */
        [CPU_OPP4] = 0x811F09D9,	/* 1614MHz */
        [CPU_OPP5] = 0x811D2C4F,	/* 1517MHz */
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
    DRV_WriteReg32(ARMPLL_LL_CON1, opp_freq[opp]);

    /* enable armpll_ll */
    temp = DRV_Reg32(ARMPLL_LL_CON0);
    DRV_WriteReg32(ARMPLL_LL_CON0, temp | 0x200);

   udelay(20);

    /* switch back to ARMPLL_LL */
    print("switch to ARMPLL_LL\n");
    temp = DRV_Reg32(CPU_PLLDIV_CFG0);
    DRV_WriteReg32(CPU_PLLDIV_CFG0, (temp & 0xFFFFF9FF) | (0x01 << 9));

    print("ARMPLL_LL_CON1: 0x%x\n", DRV_Reg32(ARMPLL_LL_CON1));
}

void set_armpll_l_rate(enum cpu_opp opp)
{
    static unsigned int opp_freq[] = {
        [CPU_OPP0] = 0x80152763,	/* 2200MHz */
        [CPU_OPP1] = 0x801429D9,	/* 2097MHz */
        [CPU_OPP2] = 0x801329D9,	/* 1993MHz */
        [CPU_OPP3] = 0x80122C4F,	/* 1890MHz */
        [CPU_OPP4] = 0x80112EC5,	/* 1787MHz */
        [CPU_OPP5] = 0x80102EC5,	/* 1683MHz */
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
    DRV_WriteReg32(ARMPLL_BL_CON1, opp_freq[opp]);

    /* enable armpll_l */
    temp = DRV_Reg32(ARMPLL_BL_CON0);
    DRV_WriteReg32(ARMPLL_BL_CON0, temp | 0x200);

    udelay(20);

    /* switch back to ARMPLL_L */
    print("switch to ARMPLL_L\n");
    temp = DRV_Reg32(CPU_PLLDIV_CFG1);
    DRV_WriteReg32(CPU_PLLDIV_CFG1, (temp & 0xFFFFF9FF) | (0x01 << 9));

    print("ARMPLL_BL_CON1: 0x%x\n", DRV_Reg32(ARMPLL_BL_CON1));
}

void set_ccipll_rate(enum cpu_opp opp)
{
    static unsigned int opp_freq[] = {
        [CPU_OPP0] = 0x811EC4ED,	/* 1600MHz */
        [CPU_OPP1] = 0x811D6277,	/* 1528MHz */
        [CPU_OPP2] = 0x811C0000,	/* 1456MHz */
        [CPU_OPP3] = 0x811A9D8A,	/* 1384MHz */
        [CPU_OPP4] = 0x81193B14,	/* 1312MHz */
        [CPU_OPP5] = 0x8117D89E,	/* 1240MHz */
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
    DRV_WriteReg32(CCIPLL_CON1, opp_freq[opp]);

    /* enable ccipll */
    temp = DRV_Reg32(CCIPLL_CON0);
    DRV_WriteReg32(CCIPLL_CON0, temp | 0x200);

    udelay(20);

    /* switch back to CCIPLL */
    print("switch to CCIPLL\n");
    temp = DRV_Reg32(BUS_PLLDIV_CFG);
    DRV_WriteReg32(BUS_PLLDIV_CFG, (temp & 0xFFFFF9FF) | (0x01 << 9));

    print("CCIPLL_CON1: 0x%x\n", DRV_Reg32(CCIPLL_CON1));
}

void mt_pll_init(void)
{
    unsigned int temp;
    int ret = 0;
    int i;

    print("PLL init start...\n");

    /*******************
     * Turn on CLKSQ PLL
     *******************/
    temp = DRV_Reg32(AP_PLL_CON0);			/* [2]=1 (CLKSQ_EN) */
    DRV_WriteReg32(AP_PLL_CON0, temp | 0x4);

    udelay(100);

    temp = DRV_Reg32(AP_PLL_CON0);			/* [1]=1 (CLKSQ_LPF_EN) */
    DRV_WriteReg32(AP_PLL_CON0, temp | 0x2);

    temp = DRV_Reg32(AP_PLL_CON0);
    DRV_WriteReg32(AP_PLL_CON0, temp & ~((0x1) << 3));	/* [3]=0 Sleep Control (CLKSQ1_EN_SEL) */

    temp = DRV_Reg32(AP_PLL_CON0);
    DRV_WriteReg32(AP_PLL_CON0, temp & ~((0x1) << 8));	/* [8]=0 (CLKSQ2_EN) */

    /**********************************
     * Turn on PLL_GLITCH_EN (PLL output > 187.5MHz from PLLGP spec)
     **********************************/
    temp = DRV_Reg32(ETHPLL_CON0);			/* [12]=1 (GLITCH_EN) */
    DRV_WriteReg32(ETHPLL_CON0, temp | 0x00001000);

    temp = DRV_Reg32(MSDCPLL_CON0);			/* [12]=1 (GLITCH_EN) */
    DRV_WriteReg32(MSDCPLL_CON0, temp | 0x00001000);

    temp = DRV_Reg32(TVDPLL1_CON0);			/* [12]=1 (GLITCH_EN) */
    DRV_WriteReg32(TVDPLL1_CON0, temp | 0x00001000);

    temp = DRV_Reg32(TVDPLL2_CON0);			/* [12]=1 (GLITCH_EN) */
    DRV_WriteReg32(TVDPLL2_CON0, temp | 0x00001000);

    temp = DRV_Reg32(MMPLL_CON0);			/* [12]=1 (GLITCH_EN) */
    DRV_WriteReg32(MMPLL_CON0, temp | 0x00001000);

    temp = DRV_Reg32(MAINPLL_CON0);			/* [12]=1 (GLITCH_EN) */
    DRV_WriteReg32(MAINPLL_CON0, temp | 0x00001000);

    temp = DRV_Reg32(IMGPLL_CON0);			/* [12]=1 (GLITCH_EN) */
    DRV_WriteReg32(IMGPLL_CON0, temp | 0x00001000);

    temp = DRV_Reg32(UNIVPLL_CON0);			/* [12]=1 (GLITCH_EN) */
    DRV_WriteReg32(UNIVPLL_CON0, temp | 0x00001000);

    temp = DRV_Reg32(ADSPPLL_CON0);			/* [12]=1 (GLITCH_EN) */
    DRV_WriteReg32(ADSPPLL_CON0, temp | 0x00001000);

    temp = DRV_Reg32(MFGPLL_CON0);			/* [12]=1 (GLITCH_EN) */
    DRV_WriteReg32(MFGPLL_CON0, temp | 0x00001000);

    /*******************
     * SET MFGPLL CK_EN
     *******************/
    temp = DRV_Reg32(MFGPLL_CON0);			/* [20]=0 ck_en =0 */
    DRV_WriteReg32(MFGPLL_CON0, temp & 0xFFEFFFFF);

    temp = DRV_Reg32(MFGPLL_CON0);			/* [2]=1 opp_ck_en=1 */
    DRV_WriteReg32(MFGPLL_CON0, temp | 0x00000004);

    /*******************
     * set PLL frequency
     *******************/
    DRV_WriteReg32(ARMPLL_LL_CON1, 0x82133B14);		/* 500Mhz */

    DRV_WriteReg32(ARMPLL_BL_CON1, 0x82190000);		/* 650Mhz */

    DRV_WriteReg32(CCIPLL_CON1, 0x82140000);		/* 520Mhz */

    DRV_WriteReg32(ETHPLL_CON1, 0x82133B14);		/* 500Mhz */

    DRV_WriteReg32(MSDCPLL_CON1, 0x831D89D9);		/* 384Mhz */

    DRV_WriteReg32(TVDPLL1_CON1, 0x8216D89D);		/* 594Mhz */

    DRV_WriteReg32(TVDPLL2_CON1, 0x8216D89D);		/* 594Mhz */

    DRV_WriteReg32(MMPLL_CON1, 0x801A713B);		/* 2750Mhz */

    DRV_WriteReg32(MAINPLL_CON1, 0x80150000);		/* 2184Mhz */

    DRV_WriteReg32(IMGPLL_CON1, 0x82196276);		/* 660Mhz */

#ifdef _FLASH_MODE_DA_
    if (!COM_Is_USB_Download_Mode())
#endif
    {
        DRV_WriteReg32(UNIVPLL_CON1, 0x80180000);	/* 2496Mhz */
    }

    DRV_WriteReg32(ADSPPLL_CON1, 0x821EC4EC);		/* 800Mhz */

    DRV_WriteReg32(MFGPLL_CON1, 0x831E0000);		/* 390Mhz */

    DRV_WriteReg32(APLL1_CON2, 0x3C7EA932);             /* 196.608Mhz */
    DRV_WriteReg32(APLL1_CON1, 0x83000000);
    DRV_WriteReg32(APLL1_TUNER_CON0, 0x3C7EA933);

    DRV_WriteReg32(APLL2_CON2, 0x6F28BD4C);		/* 180.6336Mhz */
    DRV_WriteReg32(APLL2_CON1, 0x84000000);
    DRV_WriteReg32(APLL2_TUNER_CON0, 0x6F28BD4D);

    DRV_WriteReg32(APLL3_CON2, 0x3C7EA932);             /* 196.608Mhz */
    DRV_WriteReg32(APLL3_CON1, 0x83000000);
    DRV_WriteReg32(APLL3_TUNER_CON0, 0x3C7EA933);

    DRV_WriteReg32(APLL4_CON2, 0x3C7EA932);             /* 196.608Mhz */
    DRV_WriteReg32(APLL4_CON1, 0x83000000);
    DRV_WriteReg32(APLL4_TUNER_CON0, 0x3C7EA933);

    DRV_WriteReg32(APLL5_CON2, 0x3C7EA932);             /* 196.608Mhz */
    DRV_WriteReg32(APLL5_CON1, 0x83000000);
    DRV_WriteReg32(APLL5_TUNER_CON0, 0x3C7EA933);

    /****************
     * Turn on PLL_EN
     ****************/
    temp = DRV_Reg32(ARMPLL_LL_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(ARMPLL_LL_CON0, temp | 0x200);

    temp = DRV_Reg32(ARMPLL_BL_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(ARMPLL_BL_CON0, temp | 0x200);

    temp = DRV_Reg32(CCIPLL_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(CCIPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(ETHPLL_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(ETHPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(MSDCPLL_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(MSDCPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(TVDPLL1_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(TVDPLL1_CON0, temp | 0x200);

    temp = DRV_Reg32(TVDPLL2_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(TVDPLL2_CON0, temp | 0x200);

    temp = DRV_Reg32(MMPLL_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(MMPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(MAINPLL_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(MAINPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(IMGPLL_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(IMGPLL_CON0, temp | 0x200);

#ifdef _FLASH_MODE_DA_
    if (!COM_Is_USB_Download_Mode())
#endif
    {
        temp = DRV_Reg32(UNIVPLL_CON0);			/* [9]=1 (PLL_EN) */
        DRV_WriteReg32(UNIVPLL_CON0, temp | 0x200);
    }

    temp = DRV_Reg32(ADSPPLL_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(ADSPPLL_CON0, temp | 0x200);

    temp = DRV_Reg32(APLL1_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(APLL1_CON0, temp | 0x200);

    temp = DRV_Reg32(APLL2_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(APLL2_CON0, temp | 0x200);

    temp = DRV_Reg32(APLL3_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(APLL3_CON0, temp | 0x200);

    temp = DRV_Reg32(APLL4_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(APLL4_CON0, temp | 0x200);

    temp = DRV_Reg32(APLL5_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(APLL5_CON0, temp | 0x200);

    temp = DRV_Reg32(MFGPLL_CON0);			/* [9]=1 (PLL_EN) */
    DRV_WriteReg32(MFGPLL_CON0, temp | 0x200);

    udelay(20);

#ifdef _FLASH_MODE_DA_
    if (!COM_Is_USB_Download_Mode())
#endif
    {
        /* Enable analog divider (192m to usb ) */
        temp = DRV_Reg32(UNIVPLL_CON0);
        DRV_WriteReg32(UNIVPLL_CON0, temp | 0x80);	/* [7]=1 (UNIVPLL_CKDIV_EN) */
    }

    udelay(20);

    /**************
     * PLL DIV RSTB
     **************/
    temp = DRV_Reg32(MAINPLL_CON0);
    DRV_WriteReg32(MAINPLL_CON0, temp | 0xFF000000);	/* [31:24]=0xff (DIV_EN) */

#ifdef _FLASH_MODE_DA_
    if (!COM_Is_USB_Download_Mode())
#endif
    {
        temp = DRV_Reg32(UNIVPLL_CON0);
        DRV_WriteReg32(UNIVPLL_CON0, temp | 0xFF000000);/* [31:24]=0xff (DIV_EN) */
    }

    temp = DRV_Reg32(MMPLL_CON0);
    DRV_WriteReg32(MMPLL_CON0, temp | 0xFF000000);	/* [31:24]=0xff (DIV_EN) */

    temp = DRV_Reg32(MAINPLL_CON0);			/* [23]=1 (DIV_RSTB) */
    DRV_WriteReg32(MAINPLL_CON0, temp | 0x00800000);

#ifdef _FLASH_MODE_DA_
    if (!COM_Is_USB_Download_Mode())
#endif
    {
        temp = DRV_Reg32(UNIVPLL_CON0);			/* [23]=1 (DIV_RSTB) */
        DRV_WriteReg32(UNIVPLL_CON0, temp | 0x00800000);
    }

    temp = DRV_Reg32(MMPLL_CON0);			/* [23]=1 (DIV_RSTB) */
    DRV_WriteReg32(MMPLL_CON0, temp | 0x00800000);

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
    /* [1] pllck_sel_no_spm in ckg1, [4] pllck_sel_no_spm in ckg2 */
    DRV_WriteReg32(VDNR_DCM_TOP_INFRA_CTRL0, DRV_Reg32(VDNR_DCM_TOP_INFRA_CTRL0) | (0x1 << 1) | (0x1 << 4));

    /* switch to SPM control */
    temp = DRV_Reg32(CLK_SCP_CFG_0);
    DRV_WriteReg32(CLK_SCP_CFG_0, (temp | 0x3FF));

    /* config axi bus clock first */
    DRV_WriteReg32(CLK_CFG_0_CLR, 0x000000FF);
    DRV_WriteReg32(CLK_CFG_0_SET, 0x00000002);
    DRV_WriteReg32(CLK_CFG_UPDATE, 0x00000001);

    DRV_WriteReg32(CLK_CFG_0_CLR, 0xFFFFFF00);
    DRV_WriteReg32(CLK_CFG_0_SET, 0x01050200);

    DRV_WriteReg32(CLK_CFG_1_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_1_SET, 0x09080a02);

    DRV_WriteReg32(CLK_CFG_2_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_2_SET, 0x020b0202);

    DRV_WriteReg32(CLK_CFG_3_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_3_SET, 0x07070707);

    DRV_WriteReg32(CLK_CFG_4_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_4_SET, 0x07070707);

    DRV_WriteReg32(CLK_CFG_5_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_5_SET, 0x02020203);

    DRV_WriteReg32(CLK_CFG_6_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_6_SET, 0x01010400);

    DRV_WriteReg32(CLK_CFG_7_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_7_SET, 0x01030101);

    DRV_WriteReg32(CLK_CFG_8_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_8_SET, 0x03010102);

    DRV_WriteReg32(CLK_CFG_9_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_9_SET, 0x02010403);

    DRV_WriteReg32(CLK_CFG_10_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_10_SET, 0x01010102);

    DRV_WriteReg32(CLK_CFG_11_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_11_SET, 0x02010101);

    DRV_WriteReg32(CLK_CFG_12_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_12_SET, 0x0e030303);

    DRV_WriteReg32(CLK_CFG_13_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_13_SET, 0x07010301);

    DRV_WriteReg32(CLK_CFG_14_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_14_SET, 0x05020007);

    DRV_WriteReg32(CLK_CFG_15_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_15_SET, 0x01030401);

    DRV_WriteReg32(CLK_CFG_16_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_16_SET, 0x01030102);

    DRV_WriteReg32(CLK_CFG_17_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_17_SET, 0x03030400);

    DRV_WriteReg32(CLK_CFG_18_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_18_SET, 0x01010101);

    DRV_WriteReg32(CLK_CFG_19_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_19_SET, 0x01010101);

    DRV_WriteReg32(CLK_CFG_20_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_20_SET, 0x01010101);

    DRV_WriteReg32(CLK_CFG_21_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_21_SET, 0x01020101);

    DRV_WriteReg32(CLK_CFG_22_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_22_SET, 0x80000003); /* pdn occ[31] */

    DRV_WriteReg32(CLK_CFG_23_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_23_SET, 0x80808080); /* pdn occ */

    DRV_WriteReg32(CLK_CFG_24_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_24_SET, 0x80808080); /* pdn occ */

    DRV_WriteReg32(CLK_CFG_25_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_25_SET, 0x80808080); /* pdn occ */

    DRV_WriteReg32(CLK_CFG_26_CLR, 0xFFFFFFFF);
    DRV_WriteReg32(CLK_CFG_26_SET, 0x00808080); /* pdn rsvd */

    DRV_WriteReg32(CLK_CFG_UPDATE, 0xFFFFFFFE); /* except "axi", bit 0 */

#ifdef _FLASH_MODE_DA_
    if (COM_Is_USB_Download_Mode())
	/* except [11]: usb_top_2p_sel and [12]: ssusb_xhci_2p_sel */
        DRV_WriteReg32(CLK_CFG_UPDATE1, 0xFFFFE7FF);
    else
#endif
        DRV_WriteReg32(CLK_CFG_UPDATE1, 0xFFFFFFFF);

    DRV_WriteReg32(CLK_CFG_UPDATE2, 0x07FFFFFF);

    temp = DRV_Reg32(CLK_MISC_CFG_3);
    DRV_WriteReg32(CLK_MISC_CFG_3, temp | 0x00000100);

    /********
     * MTCMOS and SUBSYS_CG
     ********/
    print("PLL init done!\n");

    /* switch sram control to bypass mode for PEXTP_MAC_P0 to avoid inrush RG */
    DRV_WriteReg32(SPM_BASE, (0xb16 << 16) | 0x1);
    temp = DRV_Reg32(AP_MDSRC_REQ);
    DRV_WriteReg32(AP_MDSRC_REQ, temp | (0x1 << 0));

    print("MTCMOS init start...\n");
#if CLKMGR_BRINGUP
    spm_mtcmos_on_all();
#else
    /* release external buck iso */
    temp = DRV_Reg32(EXT_BUCK_ISO);
    DRV_WriteReg32(EXT_BUCK_ISO, temp & ~0x1c00);
#if 1
    /* for fastlogo */
    spm_mtcmos_on(MT8188_POWER_DOMAIN_VPPSYS0);
    spm_mtcmos_on(MT8188_POWER_DOMAIN_VDOSYS0);
    spm_mtcmos_on(MT8188_POWER_DOMAIN_VDOSYS1);
    spm_mtcmos_on(MT8188_POWER_DOMAIN_VPPSYS1);
    spm_mtcmos_on(MT8188_POWER_DOMAIN_HDMI_TX);
    /* for adsp/audio/asrc infra bus access */
    spm_mtcmos_on(MT8188_POWER_DOMAIN_ADSP_AO);
    spm_mtcmos_on(MT8188_POWER_DOMAIN_ADSP_INFRA);
#endif
#endif
    print("MTCMOS init done!\n");

#if CLKMGR_BRINGUP
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_0_CLR, 0x3fef876f);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_1_CLR, 0xa797bef7);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_2_CLR, 0x8e06865d);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_3_CLR, 0x071387a7);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_4_CLR, 0x00040003);
    temp = DRV_Reg32(AUDIO_SRC_MEM_ASRC_TOP_CON1);
    DRV_WriteReg32(AUDIO_SRC_MEM_ASRC_TOP_CON1, temp & ~0x0000000f);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_0);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_0, temp & ~0x9fbc0c14);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_4);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_4, temp & ~0xc17b01c3);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_5);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_5, temp & ~0x0dcc03bf);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_6);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_6, temp & ~0x00000fff);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_1);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_1, temp & ~0x00ff7c04);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_2);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_2, temp & ~0x00000100);
    temp = DRV_Reg32(AUDIO_AUDIO_TOP_3);
    DRV_WriteReg32(AUDIO_AUDIO_TOP_3, temp & ~0x000000a0);
    DRV_WriteReg32(PERICFG_AO_PERI_MODULE_SW_CG_0_CLR, 0x03011e3b);
    DRV_WriteReg32(IMP_IIC_WRAP_C_AP_CLOCK_CG_CEN_CLR, 0x00000007);
    DRV_WriteReg32(IMP_IIC_WRAP_W_AP_CLOCK_CG_WST_CLR, 0x00000003);
    DRV_WriteReg32(IMP_IIC_WRAP_EN_AP_CLOCK_CG_EST_NOR_CLR, 0x00000003);
    DRV_WriteReg32(MFGCFG_MFG_CG_CLR, 0x00000001);
    DRV_WriteReg32(VPP0_REG_VPPSYS0_CG0_CLR, 0x0007ed86);
    DRV_WriteReg32(VPP0_REG_VPPSYS0_CG1_CLR, 0x7fff3fff);
    DRV_WriteReg32(VPP0_REG_VPPSYS0_CG2_CLR, 0x0000000f);
    temp = DRV_Reg32(WPESYS_TOP_REG_WPESYS_RG_000);
    DRV_WriteReg32(WPESYS_TOP_REG_WPESYS_RG_000, temp | 0x01150000);
    temp = DRV_Reg32(WPE_VPP0_CTL_WPE_DCM_DIS);
    DRV_WriteReg32(WPE_VPP0_CTL_WPE_DCM_DIS, temp | 0x0003ffff);
    temp = DRV_Reg32(WPE_VPP0_CTL_DMA_DCM_DIS);
    DRV_WriteReg32(WPE_VPP0_CTL_DMA_DCM_DIS, temp | 0x0000001f);
    DRV_WriteReg32(VPPSYS1_CONFIG_VPPSYS1_CG_0_CLR, 0xffffffff);
    DRV_WriteReg32(VPPSYS1_CONFIG_VPPSYS1_CG_1_CLR, 0x043f3fbf);
    DRV_WriteReg32(IMGSYS_MAIN_IMG_MAIN_CG_CLR, 0x8000370f);
    DRV_WriteReg32(IMGSYS1_DIP_TOP_MACRO_CG_CLR, 0x00000003);
    DRV_WriteReg32(IMGSYS1_DIP_NR_MACRO_CG_CLR, 0x00000003);
    DRV_WriteReg32(IMGSYS_WPE1_MACRO_CG_CLR, 0x00000003);
    DRV_WriteReg32(IPESYS_MACRO_CG_CLR, 0x0000001f);
    DRV_WriteReg32(IMGSYS_WPE2_MACRO_CG_CLR, 0x00000003);
    DRV_WriteReg32(IMGSYS_WPE3_MACRO_CG_CLR, 0x00000003);
    DRV_WriteReg32(CAMSYS_MAIN_CAM_MAIN_CG_CLR, 0xf07fff9f);
    DRV_WriteReg32(CAMSYS_RAWA_CAMSYS_CG_CLR, 0x00000007);
    DRV_WriteReg32(CAMSYS_YUVA_CAMSYS_CG_CLR, 0x00000007);
    DRV_WriteReg32(CAMSYS_RAWB_CAMSYS_CG_CLR, 0x00000007);
    DRV_WriteReg32(CAMSYS_YUVB_CAMSYS_CG_CLR, 0x00000007);
    DRV_WriteReg32(CCU_MAIN_CCUSYS_CG_CLR, 0x00000007);
    temp = DRV_Reg32(VDEC_SOC_GCON_VDEC_CKEN);
    DRV_WriteReg32(VDEC_SOC_GCON_VDEC_CKEN, temp | 0x00000111);
    temp = DRV_Reg32(VDEC_SOC_GCON_LAT_CKEN);
    DRV_WriteReg32(VDEC_SOC_GCON_LAT_CKEN, temp | 0x00000111);
    temp = DRV_Reg32(VDEC_SOC_GCON_LARB_CKEN_CON);
    DRV_WriteReg32(VDEC_SOC_GCON_LARB_CKEN_CON, temp | 0x00000001);
    temp = DRV_Reg32(VDEC_GCON_VDEC_CKEN);
    DRV_WriteReg32(VDEC_GCON_VDEC_CKEN, temp | 0x00000111);
    temp = DRV_Reg32(VDEC_GCON_LAT_CKEN);
    DRV_WriteReg32(VDEC_GCON_LAT_CKEN, temp | 0x00000111);
    temp = DRV_Reg32(VDEC_GCON_LARB_CKEN_CON);
    DRV_WriteReg32(VDEC_GCON_LARB_CKEN_CON, temp | 0x00000001);
    DRV_WriteReg32(VENC_GCON_VENCSYS_CG_SET, 0x90011111);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL0_CG_0_CLR, 0xffea0555);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL0_CG_1_CLR, 0x0000fce3);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL0_CG_2_CLR, 0x00010101);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_0_CLR, 0x00000fff);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_1_CLR, 0xf00fefff);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_2_CLR, 0x00ff0007);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_3_CLR, 0x00030303);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_4_CLR, 0x00000100);
#else
    /* for fastlogo */
    DRV_WriteReg32(VPP0_REG_VPPSYS0_CG0_CLR, 0x0007ed86);
    DRV_WriteReg32(VPP0_REG_VPPSYS0_CG1_CLR, 0x7fff3fff);
    DRV_WriteReg32(VPP0_REG_VPPSYS0_CG2_CLR, 0x0000000f);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL0_CG_0_CLR, 0xffea0555);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL0_CG_1_CLR, 0x0000fce3);
    DRV_WriteReg32(VDOSYS0_CONFIG_GLOBAL0_CG_2_CLR, 0x00010101);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_0_CLR, 0x00000fff);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_1_CLR, 0xf00fefff);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_2_CLR, 0x00ff0007);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_3_CLR, 0x00030303);
    DRV_WriteReg32(VDOSYS1_CONFIG_VDOSYS1_CG_4_CLR, 0x00000100);
    DRV_WriteReg32(VPPSYS1_CONFIG_VPPSYS1_CG_0_CLR, 0xffffffff);
    DRV_WriteReg32(VPPSYS1_CONFIG_VPPSYS1_CG_1_CLR, 0x043f3fbf);
    /* disable unused clocks in infra_ao */
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_1_SET, 0x00004000);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_2_SET, 0xf0003802);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_3_SET, 0x30000000);
    DRV_WriteReg32(INFRACFG_AO_MODULE_SW_CG_4_SET, 0x81c20000);
#endif

    set_armpll_ll_rate(CPU_OPP0);
#if CLKMGR_BRINGUP
    mt_get_cpu_freq();
#endif
}
