/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <lib/mmio.h>
#include <common/debug.h>
#include <drivers/delay_timer.h>
#include "apupw.h"

#define LOCAL_TRACE 0
#define APU_ARE_NOT_ENABLE  0
#define ENABLE_VSRAM_LP 1

static const unsigned int reg_addr[APUPW_MAX_REGS] = {
	SYS_VLP, SYS_SPM, SYS_APMIXEDSYS, APU_RCX, APU_RCX_DLA, APU_ARE,
	APU_ARE_REG, APU_VCORE, APU_MD32_MBOX, APU_RPC, APU_PCU,
	APU_AO_CTL, APU_ACC, APU_PLL, APU_RPCTOP_MDLA, APU_TOP_PMU
};

static struct apu_power apupw;

static uint32_t vapu_en_set_offset = BUCK_VAPU_PMIC_REG_EN_SET_ADDR;
static uint32_t vapu_en_clr_offset = BUCK_VAPU_PMIC_REG_EN_CLR_ADDR;
static uint32_t vapu_en_shift = BUCK_VAPU_PMIC_REG_EN_SHIFT;
static uint32_t vapu_vosel_offset = BUCK_VAPU_PMIC_REG_VOSEL_ADDR;

//static uint32_t vsram_pmic_slave_id = LDO_VSRAM_PMIC_ID;
static uint32_t vsram_en_offset = LDO_VSRAM_PMIC_REG_EN_ADDR;
static uint32_t vsram_en_shift = LDO_VSRAM_PMIC_REG_EN_SHIFT;
static uint32_t vsram_lp_shift = LDO_VSRAM_PMIC_REG_LP_SHIFT;


static void _apu_w_are(int entry, unsigned long reg, unsigned long data)
{
	unsigned long are_entry_addr;

	/* (address of entry) = register */
	are_entry_addr = (unsigned long)apupw.regs[apu_are] + 4 * ARE_ENTRY(entry);
	apu_writel(reg, are_entry_addr);
	apu_writel(data, (are_entry_addr + 4));
#if CFG_DBG_DUMP_ARE_ENTRY
	INFO("%s (reg 0x%08lx:0x%08lx, data 0x%08lx:0x%08lx)\n",
		__func__, are_entry_addr, reg, (are_entry_addr + 4), data);
#endif
}

#if CFG_SECURE_CTL
static void apupw_secure_init(uint32_t op)
{
	struct __smccc_res smccc_res;

	INFO("%s initial start\n", __func__);

	__smc_conduit(MTK_SIP_BL_APUSYS_CONTROL, MTK_APUSYS_SMC_OP_APUSYS_PWR_INIT,
		op, 0, 0, 0, 0, 0, &smccc_res);

	INFO("%s initial done\n", __func__);
}
#endif

static void get_pll_pcw(uint32_t clk_rate, uint32_t *r1, uint32_t *r2)
{
	unsigned int fvco = clk_rate;
	unsigned int pcw_val;
	unsigned int postdiv_val = 1;
	unsigned int postdiv_reg = 0;

	while (fvco <= 1500) {
		postdiv_val = postdiv_val << 1;
		postdiv_reg = postdiv_reg + 1;
		fvco = fvco << 1;
	}

	pcw_val = (fvco * 1 << 14) / 26;

	if (postdiv_reg == 0) { //Fvco * 2 with post_divider = 2
		pcw_val = pcw_val * 2;
		postdiv_val = postdiv_val << 1;
		postdiv_reg = postdiv_reg + 1;
	} //Post divider is 1 is not available

	*r1 = postdiv_reg;
	*r2 = pcw_val;
}

/* Cost 24 ARE entries */
static void __apu_pll_init(void)
{
	uint32_t pll_hfctl_cfg[] = {APMIXEDSYS_FHCTL11_CFG, APMIXEDSYS_FHCTL12_CFG};
	uint32_t pll_con0[] = {APUPLL_CON0, APUPLL2_CON0};
	uint32_t pll_con1[] = {APUPLL_CON1, APUPLL2_CON1};
	uint32_t pll_fhctl_dds[] = {APMIXEDSYS_FHCTL11_DDS, APMIXEDSYS_FHCTL12_DDS};
	int32_t pll_freq_out[] = {MDLA_DEFAULT_FREQ, APUTOP_DEFAULT_FREQ}; /* MHz */
	uint32_t pcw_val, posdiv_val;
	int pll_idx;
	uint32_t tmp = 0;

	INFO("%s initial start\n", __func__);

	/* Hopping function reset release: ofs 0xE0C */
	/*
	 * APUPLL_HP_SWRSTB     bit11
	 * APUPLL2_HP_SWRSTB    bit12
	 */
	apu_setl(0x3 << 11, apupw.regs[sys_apmixedsys] + APMIXEDSYS_FHCTL_RST_CON);

	/* PCW value always from hopping function: ofs 0xE00 */
	/*
	 * APUPLL_HP_EN     bit11
	 * APUPLL2_HP_EN    bit12
	 */
	apu_setl(0x3 << 11, apupw.regs[sys_apmixedsys] + APMIXEDSYS_FHCTL_HP_EN);

	/* Hopping function clock enable: ofs 0xE08 */
	/*
	 * APUPLL_HP_CLKEN  bit11
	 * APUPLL2_HP_CLKEN bit12
	 */
	apu_setl(0x3 << 11, apupw.regs[sys_apmixedsys] + APMIXEDSYS_FHCTL_CLK_CON);

	for (pll_idx = 0 ; pll_idx < PLL_NUM ; pll_idx++) {
		/* RG_APUPLL_GLITCH_FREE_EN */
		apu_setl((0x1 << 8), apupw.regs[sys_apmixedsys] + pll_con0[pll_idx]);

		/* Hopping function enable */
		apu_setl((0x1 << 0) | (0x1 << 2), apupw.regs[sys_apmixedsys] + pll_hfctl_cfg[pll_idx]);

		posdiv_val = 0;
		pcw_val = 0;
		get_pll_pcw(pll_freq_out[pll_idx], &posdiv_val, &pcw_val);

		/*
		 * postdiv offset (0x0608), [26:24] RG_APUPLL_POSDIV
		 * 3'b000: /1 , 3'b001: /2 , 3'b010: /4
		 * 3'b011: /8 , 3'b100: /16
		 */
		tmp = apu_readl(apupw.regs[sys_apmixedsys] + pll_con1[pll_idx]);
		tmp = (tmp & ~(7 << 24)) | (posdiv_val << 24);
		apu_writel(tmp, apupw.regs[sys_apmixedsys] + pll_con1[pll_idx]);

		/*
		 * PCW offset (0x1000CE00 + 0x0120)
		 * [31] FHCTL11_PLL_TGL_ORG
		 * [21:0] FHCTL11_PLL_ORG set to PCW value
		 */
		apu_writel(((0x1 << 31) | pcw_val), apupw.regs[sys_apmixedsys] + pll_fhctl_dds[pll_idx]);
	}

	INFO("%s initial done\n", __func__);
}

/* Cost 10 ARE entries, ARDCM(4*1+1=5) + ACC(2*1+3*1=5) */
static void __apu_acc_init(void)
{
	uint32_t top[] = {MNOC_ACC_BASE};
	uint32_t eng[] = {MDLA_ACC_BASE};
	int top_acc_arr_size = ARRAY_SIZE(top);
	int eng_acc_arr_size = ARRAY_SIZE(eng);
	int acc_idx;

	int are_idx = ACC_ENTRY_BEGIN;
	/* Step6. Initial ACC setting (@ACC) */

	INFO("%s initial start\n", __func__);

	/* Turn on RCX_AO ARE */
	apu_setl(1 << 21, apupw.regs[apu_are]);
	apu_setl(1 << 21, apupw.regs[apu_are_reg]);

	/*  write RCX_AO start entry/size */
	apu_writel(ARE_ENTRY(RCX_AO_BEGIN) | (ARE_ENTRIES(RCX_AO_BEGIN, RCX_AO_END) << 16),
		apupw.regs[apu_are] + ARE_RCX_AO_CONFIG);

	for (acc_idx = 0 ; acc_idx < top_acc_arr_size ; acc_idx++) {
		/* DCM_EN/DBC_EN */
		_apu_w_are(are_idx++,
			apupw.regs[apu_acc] + top[acc_idx]  + APU_ARDCM_CTRL1,
			0x00001006);
		/* APB_DCM_EN/APB_DBC_EN/APB_IDLE_FSEL_UPD_EN */
		_apu_w_are(are_idx++,
			apupw.regs[apu_acc] + top[acc_idx]  + APU_ARDCM_CTRL0,
			0x00000016);
		/* IDLE_FSEL/DBC_CNT */
		_apu_w_are(are_idx++,
			apupw.regs[apu_acc] + top[acc_idx]  + APU_ARDCM_CTRL1,
			0x07F0F006);
		/* APB_LOAD_TOG */
		_apu_w_are(are_idx++,
			apupw.regs[apu_acc] + top[acc_idx]  + APU_ARDCM_CTRL0,
			0x00000036);

		if (acc_idx == MNOC_ACC_BASE) {
			/* ARDCM_MODE: set 1 if uP clk src is using mnoc clk src */
			_apu_w_are(are_idx++,
				apupw.regs[apu_toppmu] + APU_ARDCM_CFG,
				BIT(31));
		}

		apu_writel(APU_ACC_CGEN_APU, apupw.regs[apu_acc] + top[acc_idx] + APU_ACC_CONFG_SET0);
		apu_writel(APU_ACC_SEL_APU,  apupw.regs[apu_acc] + top[acc_idx] + APU_ACC_CONFG_SET0);
		apu_writel(APU_ACC_CGEN_SOC, apupw.regs[apu_acc] + top[acc_idx] + APU_ACC_CONFG_CLR0);

		_apu_w_are(are_idx++,
			apupw.regs[apu_acc] + top[acc_idx] + APU_ACC_CONFG_SET0,
			APU_ACC_CGEN_APU);
		_apu_w_are(are_idx++,
			apupw.regs[apu_acc] + top[acc_idx] + APU_ACC_CONFG_SET0,
			APU_ACC_SEL_APU);
		_apu_w_are(are_idx++,
			apupw.regs[apu_acc] + top[acc_idx] + APU_ACC_CONFG_CLR0,
			APU_ACC_CGEN_SOC);

#if APU_ARE_NOT_ENABLE
		/*
		 * For uP power driver not enable yet, no ARE feature to restore
		 * case
		 */
		apu_writel(0x00001006, apupw.regs[apu_acc] + top[acc_idx]  + APU_ARDCM_CTRL1);
		apu_writel(0x00000016, apupw.regs[apu_acc] + top[acc_idx]  + APU_ARDCM_CTRL0);
		apu_writel(0x07F0F006, apupw.regs[apu_acc] + top[acc_idx]  + APU_ARDCM_CTRL1);
		apu_writel(0x00000036, apupw.regs[apu_acc] + top[acc_idx]  + APU_ARDCM_CTRL0);
		if (acc_idx == MNOC_ACC_BASE)
			apu_writel(BIT(31), apupw.regs[apu_acc] + APU_ARDCM_CFG);

		apu_writel(APU_ACC_CGEN_SOC, apupw.regs[apu_acc] + top[acc_idx]  + APU_ACC_CONFG_CLR0);
		apu_writel(APU_ACC_HW_CTRL_EN, apupw.regs[apu_acc] + top[acc_idx]  + APU_ACC_CONFG_SET0);
#endif
	}

	for (acc_idx = 0 ; acc_idx < eng_acc_arr_size ; acc_idx++) {
		apu_writel(APU_ACC_CGEN_APU, apupw.regs[apu_acc] + eng[acc_idx] + APU_ACC_CONFG_SET0);
		apu_writel(APU_ACC_SEL_APU,  apupw.regs[apu_acc] + eng[acc_idx] + APU_ACC_CONFG_SET0);
		apu_writel(APU_ACC_CGEN_SOC, apupw.regs[apu_acc] + eng[acc_idx] + APU_ACC_CONFG_CLR0);

		_apu_w_are(are_idx++,
			apupw.regs[apu_acc] + eng[acc_idx] + APU_ACC_CONFG_SET0,
			APU_ACC_CGEN_APU);
		_apu_w_are(are_idx++,
			apupw.regs[apu_acc] + eng[acc_idx] + APU_ACC_CONFG_SET0,
			APU_ACC_SEL_APU);
		_apu_w_are(are_idx++,
			apupw.regs[apu_acc] + eng[acc_idx] + APU_ACC_CONFG_CLR0,
			APU_ACC_CGEN_SOC);

#if APU_ARE_NOT_ENABLE
		/*
		 * For uP power driver not enable yet, no ARE feature to restore
		 * case
		 */
		apu_writel(APU_ACC_CGEN_SOC, apupw.regs[apu_acc] + eng[acc_idx]  + APU_ACC_CONFG_CLR0);
		apu_writel(APU_ACC_HW_CTRL_EN, apupw.regs[apu_acc] + eng[acc_idx]  + APU_ACC_CONFG_SET0);
		apu_writel(APU_ACC_CLK_REQ_SW_EN,
			apupw.regs[apu_acc] + eng[acc_idx] + APU_ACC_AUTO_CTRL_SET0);
#endif
	}

	INFO("%s initial done\n", __func__);
}

static void buck_off_by_pcu(uint32_t ofs, uint32_t shift, uint32_t slv_id)
{
	int retry = 10;

	apu_setl(0x00000004, apupw.regs[apu_pcu] + APU_PCUTOP_CTRL_SET);
	apu_writel((ofs << 16) | (0x0U << shift),
				apupw.regs[apu_pcu] + APU_PCU_PMIC_TAR_BUF1);
	apu_writel(0x00000004,
				apupw.regs[apu_pcu] + APU_PCU_PMIC_TAR_BUF2);
	apu_writel(0x00000001, apupw.regs[apu_pcu] + APU_PCU_PMIC_CMD);

	while ((apu_readl(apupw.regs[apu_pcu] + APU_PCU_PMIC_IRQ) & 0x1) == 0) {
		udelay(10);
		if (--retry < 0) {
			INFO("%s wait APU_PCU_PMIC_IRQ timeout !\n",
					__func__);
			INFO("0x%08lx = 0x%08x\n",
					(apupw.regs[apu_pcu] + APU_PCU_PMIC_IRQ),
					apu_readl(apupw.regs[apu_pcu] + APU_PCU_PMIC_IRQ));
		}
	}

	/* clear PCU irq status */
	apu_writel(0x1, apupw.regs[apu_pcu] + APU_PCU_PMIC_IRQ);
}

static void trigger_buck_off(void)
{
	// vapu buck off
	buck_off_by_pcu(vapu_en_clr_offset, 0, 0);
}

static void __apu_buck_off_cfg(void)
{
	INFO("%s initial start\n", __func__);

	// Step15. Roll back to Buck off stage
	apu_setl(1 << 10, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_setl(1 << 9, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_CLR);
	apu_setl(1 << 12, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_CLR);
	apu_setl(1 << 14, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_CLR);

	apu_clearl(1 << 10, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_clearl(1 << 9, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_CLR);
	apu_clearl(1 << 12, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_CLR);
	apu_clearl(1 << 14, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_CLR);
	udelay(1);

	// a. Setup Buck control signal
	//  The following setting need to in order,
	//  and wait 1uS before setup next control signal
	// APU_BUCK_PROT_REQ
	apu_writel(0x00004000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(1);
	// SRAM_AOC_LHENB
	apu_writel(0x00000010, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(1);
	// SRAM_AOC_ISO
	apu_writel(0x00000040, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(1);
	//Set 0 for PLL ISO assert
	apu_writel(1 << 8, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(1);
	// APU_BUCK_ELS_EN
	apu_writel(0x00000400, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(1);
	// APU_BUCK_RST_B
	apu_writel(0x00002000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(1);

	// b. Manually turn off Buck (by configuring register in PMIC)
	// move to probe last line
	trigger_buck_off();

	// Step12. After APUsys is finished, update the following register to 1,
	//     ARE will use this information to ensure the SRAM in ARE is
	//     trusted or not
	//     apusys_initial_done
	apu_setl(1 << 6, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	udelay(1);
	apu_setl(1 << 7, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	udelay(1);
	apu_clearl(1 << 6, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	udelay(1);
	apu_clearl(1 << 7, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	udelay(1);

	INFO("%s initial done\n", __func__);
}

/*
 * low 32-bit data for PMIC control
 *  APU_PCU_PMIC_TAR_BUF1 (or APU_PCU_BUCK_ON_DAT0_L)
 *  [31:16] offset to update
 *  [15:00] data to update
 *
 * high 32-bit data for PMIC control
 *  APU_PCU_PMIC_TAR_BUF2 (or APU_PCU_BUCK_ON_DAT0_H)
 *  [2:0] cmd_op, read:0x3 , write:0x7
 *  [3]: pmifid,
 *  [7:4]: slvid
 *  [8]: bytecnt
 */
static void __apu_pcu_init(void)
{
	INFO("%s initial start\n", __func__);

	// auto buck enable
	apu_writel((0x1 << 3), apupw.regs[apu_pcu] + APU_PCUTOP_CTRL_SET);

	/*
	 * Step1. enable cmd operation in auto buck on/off flow
	 * [0]: enable auto ON cmd0 (clear vsram ldo LP mode),
	 * [1]: enable auto ON cmd1 (set vapu voltage to 0.75v)
	 * [2]: enable auto ON cmd2 (turn vapu buck ON),
	 * [4]: enable auto OFF cmd0 (turn vapu buck OFF),
	 * [5]: enable auto OFF cmd1 (set vsram ldo LP mode),
	 */
#if ENABLE_VSRAM_LP
	apu_writel(0x37,  apupw.regs[apu_pcu] + APU_PCU_BUCK_STEP_SEL);
#else
	apu_writel(0x17,  apupw.regs[apu_pcu] + APU_PCU_BUCK_STEP_SEL);
#endif

	/* Step2. fill-in auto ON cmd0 */
	apu_writel((vsram_en_offset << 16) | (0x1U << vsram_en_shift),
		apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT0_L);
	apu_writel(0x00000004, //write
		apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT0_H);

	/* Step3. fill-in auto ON cmd1 */
	apu_writel((vapu_vosel_offset << 16) | ((750000 - 506250) / 6250),
		apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT1_L);
	apu_writel(0x00000004, //write
		apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT1_H);

	/* Step4. fill-in auto ON cmd2 */
	apu_writel((vapu_en_set_offset << 16) | (0x1U << vapu_en_shift),
		apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT2_L);
	apu_writel(0x00000004, //write
		apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT2_H);

	/* Step5. fill-in auto OFF cmd0 */
	apu_writel((vapu_en_clr_offset << 16) | (0x0U << vapu_en_shift),
		apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT0_L);
	apu_writel(0x00000004, //write
		apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT0_H);

#if ENABLE_VSRAM_LP
	/* Step6. fill-in auto OFF cmd1 */
	apu_writel((vsram_en_offset << 16) | (0x1U << vsram_lp_shift) | (0x1U << vsram_en_shift),
		apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT1_L);
	apu_writel(0x00000004, //write
		apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT1_H);
#endif

	/* Step7. fill-in settle time for auto ON/OFF cmd */
	apu_writel(0x12C, apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_SLE0); /* 300us */
	apu_writel(0x12C, apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_SLE2); /* 300us */
	apu_writel(0x12C, apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_SLE0); /* 300us */

	INFO("%s initial done\n", __func__);
}

static void __apu_rpc_mdla_init(void)
{
	INFO("%s initial start\n", __func__);

	// RPC-mdla: iTCM in MDLA need to setup to sleep type
	apu_clearl((0x1 << 0),
			apupw.regs[apu_rpctop_mdla] + 0x0200);

	INFO("%s initial done\n", __func__);
}

static void __apu_rpc_init(void)
{
	INFO("%s initial start\n", __func__);

	/* Step7. RPC: memory types (sleep or PD type) */
	/* RPC: APU TCM set PD type */
	apu_writel(0x74, apupw.regs[apu_rpc] + 0x0200);

	// Step9. RPCtop initial
	/* 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 (bit offset)
	 *  1  0  1  1  1  0  0  0  0  0  0  0  0  0  0  0 --> 0xB800
	 * 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0 (bit offset)
	 *  1  1  0  1  0  1  1  1  0  0  0  0  1  1  1  1 --> 0xD70F
	 */
	apu_setl(0xB800D70F, apupw.regs[apu_rpc] + APU_RPC_TOP_SEL);

#if !CFG_CTL_RPC_BY_CE
	/* turn off CE wake up RPC */
	apu_clearl(1 << 10, apupw.regs[apu_rpc] + APU_RPC_TOP_SEL);
#endif

	/* Disable PLL auto on/off */
	apu_clearl(1 << 12, apupw.regs[apu_rpc] + APU_RPC_TOP_SEL);

	// BUCK_PROT_SEL
	apu_setl((0x1 << 20), apupw.regs[apu_rpc] + APU_RPC_TOP_SEL_1);

	INFO("%s initial done\n", __func__);
}

static void __apu_are_init(void)
{
	int entry = 0;

	INFO("%s initial start\n", __func__);

	/*  clean all enable core as 0 */
	apu_clearl(0xFFFU<<20, apupw.regs[apu_are]);

	/* Turn on Vcroe ARE to avoid block spm supend flow */
	apu_setl(1 << 20, apupw.regs[apu_are]);
	apu_setl(1 << 20, apupw.regs[apu_are_reg]);

	/*  clean entry#1(0x190A0004) ~ #27(0x190A006C) */
	for (entry = 4; entry < 0x6C; entry += 4)
		apu_writel(0, apupw.regs[apu_are] + entry);

	INFO("%s initial done\n", __func__);
}

static void __apu_aoc_init(void)
{
	INFO("%s initial start\n", __func__);

	/*
	 * 1. Manually disable Buck els enable @SOC (disable SW mode to manually control Buck on/off)
	 */
#if CFG_SECURE_CTL
	apupw_secure_init(0);
#else
	apu_setl((0x1 << 8), SPM_BASE + 0xF30);
	apu_clearl((0x1 << 1), SPM_BASE + 0x414);
#endif

	/*
	 * 2. Vsram AO clock enable
	 */
	apu_writel(0x00000001, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_CONFIG);
	udelay(1);

	/*
	 * 3. keep AOC control from vsram ao register bit[9] bit[14] =1, bit[12] = 1
	 */
#if !CFG_CTL_RPC_BY_CE
	apu_setl(1 << 8, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_setl(1 << 11, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_setl(1 << 13, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);

	apu_clearl(1 << 8, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_clearl(1 << 11, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_clearl(1 << 13, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
#else
	apu_setl(1 << 9, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_setl(1 << 12, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_setl(1 << 14, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_setl(1 << 10, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);

	apu_clearl(1 << 9, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_clearl(1 << 12, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_clearl(1 << 14, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
	apu_clearl(1 << 10, apupw.regs[apu_ao_ctl] + APUSYS_AO_SRAM_SET);
#endif
	udelay(1);

	/*
	 * 4. Roll back to APU Buck on stage
	 * The following setting need to in order and wait 1uS before setup next
	 * control signal
	 */
	/* APU_BUCK_ELS_EN */
	apu_writel(0x00000800, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(1);

	/* APU_BUCK_RST_B */
	apu_writel(0x00001000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(1);

	/* APU_BUCK_PROT_REQ */
	apu_writel(0x00008000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(1);

	/* SRAM_AOC_ISO */
	apu_writel(0x00000080, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(1);

	/* PLL_AOC_ISO_EN */
	apu_writel(1 << 9, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(1);

	INFO("%s initial done\n", __func__);
}

static void init_hw_setting(void)
{
	__apu_aoc_init();
	__apu_pcu_init();
	__apu_rpc_init();
	__apu_rpc_mdla_init();
	__apu_are_init();
	__apu_pll_init();
	__apu_acc_init();
	__apu_buck_off_cfg();
}

/*
 * init apusys power hw here and ensure the following sequence:
 * apusys_power    - LK_INIT_LEVEL_PLATFORM
 * apusys_devapc   - LK_INIT_LEVEL_APPS - 1
 * spm_devapc      - mt_boot_init
 * vlp_devapc      - mt_boot_init
 */
int apusys_power_init(void)
{
	int idx = 0;

	INFO("%s initial +\n", __func__);

	for (idx = 0; idx <= sys_apmixedsys; idx++) {
		apupw.regs[idx] = IO_PHYS + reg_addr[idx];
		apupw.phy_addr[idx] = IO_PHYS + reg_addr[idx];
	}

	for (; idx < APUPW_MAX_REGS; idx++) {
		apupw.regs[idx] = APUSYS_BASE + reg_addr[idx];
		apupw.phy_addr[idx] = 0x10000000 + 0x09000000 + reg_addr[idx];
	}

	init_hw_setting();

	/* write 1 to close the entry of hw flag */
	/* only enable RCX wakeup/sleep */
	apu_writel(0xfffffffc, APU_ACE_HW_FLAG_DIS);

	INFO("%s initial -\n", __func__);

	return 0;
}

