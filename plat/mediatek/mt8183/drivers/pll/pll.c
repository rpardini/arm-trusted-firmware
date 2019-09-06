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

#include <drivers/delay_timer.h>
#include <stdio.h>
#include <lib/mmio.h>

#include "pll.h"

#define print printf

#define DRV_WriteReg32(addr, data)  mmio_write_32(addr, data)
#define DRV_Reg32(addr) mmio_read_32(addr)

unsigned int mt_get_abist_freq(unsigned int ID)
{
	int output = 0, i = 0;
    unsigned int temp, clk26cali_0, clk_dbg_cfg, clk_misc_cfg_0, clk26cali_1;

    clk_dbg_cfg = DRV_Reg32(CLK_DBG_CFG);
    DRV_WriteReg32(CLK_DBG_CFG, (clk_dbg_cfg & 0xFFC0FFFC)|(ID << 16)); //sel abist_cksw and enable freq meter sel abist
    clk_misc_cfg_0 = DRV_Reg32(CLK_MISC_CFG_0);
    DRV_WriteReg32(CLK_MISC_CFG_0, (clk_misc_cfg_0 & 0x00FFFFFF)); // select divider, WAIT CONFIRM
    clk26cali_0 = DRV_Reg32(CLK26CALI_0);
    clk26cali_1 = DRV_Reg32(CLK26CALI_1);
    DRV_WriteReg32(CLK26CALI_0, 0x1000);
    DRV_WriteReg32(CLK26CALI_0, 0x1010);

    /* wait frequency meter finish */
    while (DRV_Reg32(CLK26CALI_0) & 0x10)
    {
        mdelay(10);
        i++;
        if(i > 10)
        	break;
    }

    temp = DRV_Reg32(CLK26CALI_1) & 0xFFFF;

    output = ((temp * 26000) ) / 1024; // Khz

    DRV_WriteReg32(CLK_DBG_CFG, clk_dbg_cfg);
    DRV_WriteReg32(CLK_MISC_CFG_0, clk_misc_cfg_0);
    DRV_WriteReg32(CLK26CALI_0, clk26cali_0);
    DRV_WriteReg32(CLK26CALI_1, clk26cali_1);

    //print("abist meter[%d] = %d Khz\n", ID, output);
    return output;
}

void mt_pll_post_init(void)
{
}

void mt_pll_init(void)
{
    unsigned int temp;

    print("Pll init start...\n");
#if 1
    temp = DRV_Reg32(AP_PLL_CON2);
    DRV_WriteReg32(AP_PLL_CON2, temp | 0x1FFE0000);//need confirm
#endif
    //step 1
    temp = DRV_Reg32(AP_PLL_CON0);
    DRV_WriteReg32(AP_PLL_CON0, temp | 0x01);// [0]
    
    //step 2
    udelay(200);
	
    //step 3
    temp = DRV_Reg32(AP_PLL_CON0);
    DRV_WriteReg32(AP_PLL_CON0, temp | 0x2); //[1]?
    /*************
 	   * xPLL PWR ON
 	   **************/
    //step 4
    temp = DRV_Reg32(MFGPLL_PWR_CON0);
    DRV_WriteReg32(MFGPLL_PWR_CON0, temp | 0x1);

    //step 5

    //step 6
    temp = DRV_Reg32(MAINPLL_PWR_CON0);
    DRV_WriteReg32(MAINPLL_PWR_CON0, temp | 0x1);

    //step 7
    temp = DRV_Reg32(UNIVPLL_PWR_CON0);
    DRV_WriteReg32(UNIVPLL_PWR_CON0, temp | 0x1);

    //step 8
    temp = DRV_Reg32(MSDCPLL_PWR_CON0);
    DRV_WriteReg32(MSDCPLL_PWR_CON0, temp | 0x1);

    //step 9
    temp = DRV_Reg32(MMPLL_PWR_CON0);
    DRV_WriteReg32(MMPLL_PWR_CON0, temp | 0x1);

    //step 10
    temp = DRV_Reg32(TVDPLL_PWR_CON0);
    DRV_WriteReg32(TVDPLL_PWR_CON0, temp | 0x1);

    //step 11
    temp = DRV_Reg32(APLL1_PWR_CON0);
    DRV_WriteReg32(APLL1_PWR_CON0, temp | 0x1);
    
    //step 12
    temp = DRV_Reg32(APLL2_PWR_CON0);
    DRV_WriteReg32(APLL2_PWR_CON0, temp | 0x1);
    
    //step 13
    temp = DRV_Reg32(ARMPLL_LL_PWR_CON0);
    DRV_WriteReg32(ARMPLL_LL_PWR_CON0, temp | 0x1);    
    
    //step 14
    temp = DRV_Reg32(ARMPLL_L_PWR_CON0);
    DRV_WriteReg32(ARMPLL_L_PWR_CON0, temp | 0x1);

    //step 15
    temp = DRV_Reg32(CCIPLL_PWR_CON0);
    DRV_WriteReg32(CCIPLL_PWR_CON0, temp | 0x1);

    //step 16
    udelay(100);	
    /******************
    	* xPLL ISO Disable
    	*******************/
    //step 17
     temp = DRV_Reg32(MFGPLL_PWR_CON0);
    DRV_WriteReg32(MFGPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 18

    //step 19
    temp = DRV_Reg32(MAINPLL_PWR_CON0);
    DRV_WriteReg32(MAINPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 20
    temp = DRV_Reg32(UNIVPLL_PWR_CON0);
    DRV_WriteReg32(UNIVPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 21
    temp = DRV_Reg32(MSDCPLL_PWR_CON0);
    DRV_WriteReg32(MSDCPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 22
    temp = DRV_Reg32(MMPLL_PWR_CON0);
    DRV_WriteReg32(MMPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 23
    temp = DRV_Reg32(TVDPLL_PWR_CON0);
    DRV_WriteReg32(TVDPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 24
    temp = DRV_Reg32(APLL1_PWR_CON0);
    DRV_WriteReg32(APLL1_PWR_CON0, temp & 0xFFFFFFFD);

    //step 25
    temp = DRV_Reg32(APLL2_PWR_CON0);
    DRV_WriteReg32(APLL2_PWR_CON0, temp & 0xFFFFFFFD);

    //step 26
    temp = DRV_Reg32(ARMPLL_LL_PWR_CON0);
    DRV_WriteReg32(ARMPLL_LL_PWR_CON0, temp & 0xFFFFFFFD);
    
    //step 27
    temp = DRV_Reg32(ARMPLL_L_PWR_CON0);
    DRV_WriteReg32(ARMPLL_L_PWR_CON0, temp & 0xFFFFFFFD);

    //step 28
    temp = DRV_Reg32(CCIPLL_PWR_CON0);
    DRV_WriteReg32(CCIPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 29
    udelay(50);
    
    //step 30
    temp = DRV_Reg32(APLL1_CON0);
    DRV_WriteReg32(APLL1_CON0, temp | 0x00000000);

    //step 31
    temp = DRV_Reg32(APLL2_CON0);
    DRV_WriteReg32(APLL2_CON0, temp | 0x00000000);
    /********************
       * xPLL Frequency Set
       *********************/
    //step 32
    DRV_WriteReg32(MFGPLL_CON1, 0x8213B13B); //512MHz

    //step 34
    DRV_WriteReg32(MAINPLL_CON1, 0x81150000); //1092MHz

    //step 35
    DRV_WriteReg32(UNIVPLL_CON1, 0x80180000); //1248MHz
    
    //step 36
    DRV_WriteReg32(MSDCPLL_CON1, 0x831D89D8); //384MHz

    //step 37
    DRV_WriteReg32(MMPLL_CON1, 0x801E49D9); // 3150MHz
    //step 38
    DRV_WriteReg32(TVDPLL_CON1, 0x8216D89E); //594MHz
    
    //step 39
    DRV_WriteReg32(APLL1_CON1, 0x6F28BD4C); //180.6336MHz
    DRV_WriteReg32(APLL1_CON2, 0x6F28BD4D); //180.6336MHz
    //step 40
    DRV_WriteReg32(APLL2_CON1, 0x78FD5264); //196.608MHz
    DRV_WriteReg32(APLL2_CON2, 0x78FD5265); //196.608MHz
    //step 41
    DRV_WriteReg32(APLL1_CON0, 0x80000188);
    
    //step 42
    DRV_WriteReg32(APLL2_CON0, 0x80000188);
     
    //step 43: CPU SPEED*****
    DRV_WriteReg32(ARMPLL_LL_CON1, 0x81152762); // 1100MHz, pos div = 2, arm div = 1
    //step 44
    DRV_WriteReg32(ARMPLL_L_CON1, 0x811713B1); // 1200MHz, pss div = 2, arm div = 1
    
    //step 45
    DRV_WriteReg32(CCIPLL_CON1, 0x81170000); // 1196/2MHz, pos div = 2, arm div = 2

    /***********************
      * xPLL Frequency Enable
      ************************/
     //step 46
     temp = DRV_Reg32(MFGPLL_CON0);
     DRV_WriteReg32(MFGPLL_CON0, temp | 0x1);
 
    //step 48
    temp = DRV_Reg32(MAINPLL_CON0);
    DRV_WriteReg32(MAINPLL_CON0, temp | 0x1);

    //step 49
    temp = DRV_Reg32(UNIVPLL_CON0);
    DRV_WriteReg32(UNIVPLL_CON0, temp | 0x1);

    //step 50
    temp = DRV_Reg32(MSDCPLL_CON0);
    DRV_WriteReg32(MSDCPLL_CON0, temp | 0x1);

    //step 51
    temp = DRV_Reg32(MMPLL_CON0);
    DRV_WriteReg32(MMPLL_CON0, temp | 0x1);

    //step 52
    temp = DRV_Reg32(TVDPLL_CON0);
    DRV_WriteReg32(TVDPLL_CON0, temp | 0x1);

    //step 53
    temp = DRV_Reg32(APLL1_CON0);
    DRV_WriteReg32(APLL1_CON0, temp | 0x1);

    //step 54
    temp = DRV_Reg32(APLL2_CON0);
    DRV_WriteReg32(APLL2_CON0, temp | 0x1);

    //step 55
    temp = DRV_Reg32(ARMPLL_LL_CON0);
    DRV_WriteReg32(ARMPLL_LL_CON0, temp | 0x1);

    //step 56
    temp = DRV_Reg32(ARMPLL_L_CON0);
    DRV_WriteReg32(ARMPLL_L_CON0, temp | 0x1);

    //step 57
    temp = DRV_Reg32(CCIPLL_CON0);
    DRV_WriteReg32(CCIPLL_CON0, temp | 0x1);

    // step 58
    udelay(50); // wait for PLL stable (min delay is 20us)

    /***************
      * xPLL DIV RSTB
      ****************/
    //step 59
    temp = DRV_Reg32(MAINPLL_CON0);
    DRV_WriteReg32(MAINPLL_CON0, temp | 0x01000000);
	
    //step 60
    temp = DRV_Reg32(UNIVPLL_CON0);
    DRV_WriteReg32(UNIVPLL_CON0, temp | 0x01000000);

    //step 61
    temp = DRV_Reg32(MMPLL_CON0);
    DRV_WriteReg32(MMPLL_CON0, temp | 0x00800000);
    /*****************
	* xPLL HW Control
    	******************/

    /**************
    	* INFRA CPU CLKMUX, CLK Div
    	***************/

//8: div1, A: div2, B: div4, 1D: div6
    temp = DRV_Reg32(MP0_PLL_DIV_CFG) ;
    DRV_WriteReg32(MP0_PLL_DIV_CFG, (temp & 0xFFE1FFFF) | (0x08 << 17)); // [21:17] divsel: CPU clock divide by 1

    temp = DRV_Reg32(MP2_PLL_DIV_CFG) ;
    DRV_WriteReg32(MP2_PLL_DIV_CFG, (temp & 0xFFE1FFFF) | (0x08 << 17)); // [21:17] divsel: CPU clock divide by 1

    temp = DRV_Reg32(BUS_PLL_DIV_CFG) ;
    DRV_WriteReg32(BUS_PLL_DIV_CFG, (temp & 0xFFE1FFFF) | (0x0A << 17)); // [21:17] divsel: CPU clock divide by 1

    //0: 26M,  1: armpll, 2:  Mainpll, 3:  Unipll
    temp = DRV_Reg32(MP0_PLL_DIV_CFG);
    DRV_WriteReg32(MP0_PLL_DIV_CFG, (temp & 0xFFFFF9FF) | (0x01<<9)); // [10:9] muxsel: switch to PLL speed

    temp = DRV_Reg32(MP2_PLL_DIV_CFG);
    DRV_WriteReg32(MP2_PLL_DIV_CFG, (temp & 0xFFFFF9FF) | (0x01<<9)); // [10:9] muxsel: switch to PLL speed

    temp = DRV_Reg32(BUS_PLL_DIV_CFG);
    DRV_WriteReg32(BUS_PLL_DIV_CFG, (temp & 0xFFFFF9FF) | (0x01<<9)); // [10:9] muxsel: switch to PLL speed

    /************
  	  * TOP CLKMUX
  	  *************/
	DRV_WriteReg32(INFRA_BUS_DCM_CTRL, DRV_Reg32(INFRA_BUS_DCM_CTRL) | (1 << 21) | (1 << 22));

    // config AXI clock first
    DRV_WriteReg32(CLK_CFG_0, 0x00000002);      // axi=syspll2_d2
    DRV_WriteReg32(CLK_CFG_UPDATE, 0x00000001); //update AXI clock

    DRV_WriteReg32(CLK_CFG_20_SET, 0x00030000); //set CAM[16] & MM[17] fast clock transition
    // config other clocks
    DRV_WriteReg32(CLK_CFG_0, 0x01010102);
    DRV_WriteReg32(CLK_CFG_1, 0x01010101);
    DRV_WriteReg32(CLK_CFG_2, 0x01010301); // mfgpll_ck
    DRV_WriteReg32(CLK_CFG_3, 0x01000101);
    DRV_WriteReg32(CLK_CFG_4, 0x01040101);
    DRV_WriteReg32(CLK_CFG_5, 0x00000100);
    DRV_WriteReg32(CLK_CFG_6, 0x01010201);
    DRV_WriteReg32(CLK_CFG_7, 0x01030300);
    DRV_WriteReg32(CLK_CFG_8, 0x01020102);
    DRV_WriteReg32(CLK_CFG_9, 0x01030303);
    DRV_WriteReg32(CLK_CFG_10, 0x00000101);

    DRV_WriteReg32(CLK_CFG_UPDATE, 0x7FFFFFFE);    //update all clocks except "axi"
    DRV_WriteReg32(CLK_CFG_UPDATE1, 0x000007FF);

#if 1
    temp = DRV_Reg32(CLK_MISC_CFG_0);   // FIX BY HK
    DRV_WriteReg32(CLK_MISC_CFG_0, ( temp | 0x0004000)); // enable [14] dramc_pll104m_ck
#endif

    /************
    	* MTCMOS 
    	*************/

    print("mtcmos Start..\n");
    spm_mtcmos_ctrl_dis(1);	

    print("mtcmos Done!\n");

#if 1
    /*for CG*/
    DRV_WriteReg32(INFRA_PDN_CLR0, 0xDBEFFF7F);//7, 20, 26, 29
    DRV_WriteReg32(INFRA_PDN_CLR1, 0xDF9F7FF7);//3, 15, 21, 22, 29
    DRV_WriteReg32(INFRA_PDN_SET1, 0x18000000);//27, 28: dxcc
    DRV_WriteReg32(INFRA_PDN_CLR2, 0x7FFC07DD);//1, 5, 11~17, 31
    DRV_WriteReg32(INFRA_PDN_CLR3, 0x01FF01FF);//9~15, 25~31
#endif

    print("Pll init Done!\n");
}


