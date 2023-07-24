/*
 * Copyright (c) 2021, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* TF-A system header */
#include <common/debug.h>
#include <lib/mmio.h>
#include <lib/utils_def.h>
#include <lib/xlat_tables/xlat_tables_v2.h>

/* Vendor header */
//#include <mtk_mmap_pool.h>
#include "apusys_power.h"

#define LOCAL_DEBUG	(0)
#define MODULE_TAG	"[APUSYS]"

static const unsigned int reg_addr[APUPW_MAX_REGS] = {
	SPM_BASE, BCRM_FMEM_PDN, APU_RCX_BASE, APU_VCORE_BASE, APU_MBOX0_BASE,
	APU_RPC_BASE, APU_PCU_BASE, APU_AO_CTL_BASE, APU_PLL_BASE, APU_ACC_BASE,
	APU_ARE0_BASE, APU_ARE1_BASE, APU_ARE2_BASE,
	APU_ACX0_BASE, APU_ACX0_RPC_LITE_BASE,
};

static struct apu_power apupw;

#if 0
static const mmap_region_t apusys_power_mmap[] MTK_MMAP_SECTION = {
//	MAP_REGION_FLAT(APU_RPC_BASE, APU_RPCTOP_SZ,
//		MT_DEVICE | MT_RW | MT_SECURE),
#if 0
	MAP_REGION_FLAT(APU_ACC_BASE, APU_REG_RNG_SIZE,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APU_ACX0_BASE, APU_REG_RNG_SIZE,
		MT_DEVICE | MT_RW | MT_SECURE),
#endif
	{0}
};
DECLARE_MTK_MMAP_REGIONS(apusys_power_mmap);
#endif

int apusys_kernel_apusys_pwr_rcx(uint32_t op)
{
	switch (op) {
	case SMC_RCX_PWR_AFC_EN:
		mmio_write_32(APU_RPC_BASE + APU_RPC_TOP_SEL_1,
				(mmio_read_32(APU_RPC_BASE + APU_RPC_TOP_SEL_1)
								| (0x1 << 16)));
		break;
	case SMC_RCX_PWR_WAKEUP_RPC:
		mmio_write_32(APU_RPC_BASE + APU_RPC_TOP_CON, 0x00000100);
		break;
	case SMC_RCX_PWR_CG_EN:
		mmio_write_32(APU_VCORE_BASE + APUSYS_VCORE_CG_CLR, 0xFFFFFFFF);
		mmio_write_32(APU_RCX_BASE + APU_RCX_CG_CLR, 0xFFFFFFFF);

		break;
	default:
		ERROR("%s invalid op:%d\n", __func__, op);
	}

	return 0;
}

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

static void __apu_pll_init(void)
{
	uint32_t pll_hfctl_cfg[] = {PLL4HPLL_FHCTL0_CFG, PLL4HPLL_FHCTL1_CFG,
				    PLL4HPLL_FHCTL2_CFG, PLL4HPLL_FHCTL3_CFG};
	uint32_t pll_con1[] = {PLL4H_PLL1_CON1, PLL4H_PLL2_CON1,
			       PLL4H_PLL3_CON1, PLL4H_PLL4_CON1};
	uint32_t pll_fhctl_dds[] = {PLL4HPLL_FHCTL0_DDS, PLL4HPLL_FHCTL1_DDS,
				    PLL4HPLL_FHCTL2_DDS, PLL4HPLL_FHCTL3_DDS};
	int32_t pll_freq_out_default[] = {900, 832,
				  700, 700}; /* MHz */
	int32_t pll_freq_out_mt8370[] = {700, 832,
				  700, 700}; /* MHz */
	int32_t *pll_freq_out = NULL;
	uint32_t pcw_val, posdiv_val;
	int pll_idx;
	uint32_t tmp = 0;
	uint32_t ret_val = 0;

#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "PLL init %s %d ++\n", __func__, __LINE__);
#endif

	/* according to segment id in efuse to setup default max freq values of apusys */
	mtk_plat_get_partid(&ret_val);
	if (ret_val == 0x00008370) {
		// 8370
		pll_freq_out = &pll_freq_out_mt8370;
		NOTICE(MODULE_TAG "PLL init for mt8370, %s %d --\n", __func__, __LINE__);
	} else {
		// 8390 (aka 8188)
		pll_freq_out = &pll_freq_out_default;
		NOTICE(MODULE_TAG "PLL init for mt8390, %s %d --\n", __func__, __LINE__);
	}


	/* Step4. Initial PLL setting */

	/* Hopping function reset release: ofs 0xE0C */
	apu_setl(0xF << 0, apupw.regs[apu_pll] + PLL4HPLL_FHCTL_RST_CON);

	/* PCW value always from hopping function: ofs 0xE00 */
	apu_setl(0xF << 0, apupw.regs[apu_pll] + PLL4HPLL_FHCTL_HP_EN);

	/* Hopping function clock enable: ofs 0xE08 */
	apu_setl(0xF << 0, apupw.regs[apu_pll] + PLL4HPLL_FHCTL_CLK_CON);


	for (pll_idx = 0 ; pll_idx < PLL_NUM ; pll_idx++) {
		/* Hopping function enable */
		apu_setl((0x1 << 0) | (0x1 << 2),
				apupw.regs[apu_pll] + pll_hfctl_cfg[pll_idx]);

		posdiv_val = 0;
		pcw_val = 0;
		get_pll_pcw(pll_freq_out[pll_idx], &posdiv_val, &pcw_val);
//		LOG_DBG("[%s][%d] freq = %d, div = 0x%x, pcw = 0x%x\n",
//			__func__, __LINE__, pll_freq_out[pll_idx], posdiv_val, pcw_val);

		/*
		 * postdiv offset (0x000C), [26:24] RG_PLL_POSDIV
		 * 3'b000: /1 , 3'b001: /2 , 3'b010: /4
		 * 3'b011: /8 , 3'b100: /16
		 */
		tmp = apu_readl(apupw.regs[apu_pll] + pll_con1[pll_idx]);
		tmp = (tmp & ~(7 << 24)) | (posdiv_val << 24);
		apu_writel(tmp, apupw.regs[apu_pll] + pll_con1[pll_idx]);

		/*
		 * PCW offset (0x011C)
		 * [31] FHCTL0_PLL_TGL_ORG
		 * [21:0] FHCTL0_PLL_ORG set to PCW value
		 */
		apu_writel(((0x1 << 31) | pcw_val),
				apupw.regs[apu_pll] + pll_fhctl_dds[pll_idx]);
	}

#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "PLL init %s %d --\n", __func__, __LINE__);
#endif
}

static void __apu_acc_init(void)
{
#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "ACC init %s %d ++\n", __func__, __LINE__);
#endif

	/* Step6. Initial ACC setting (@ACC) */

	/* mnoc, uP clk setting */
//	LOG_DBG("mnoc clk setting %s %d\n", __func__, __LINE__);
	/* CGEN_SOC */
	apu_writel(0x00000004, apupw.regs[apu_acc] + APU_ACC_CONFG_CLR0);
	/* HW_CTRL_EN */
	apu_writel(0x00008000, apupw.regs[apu_acc] + APU_ACC_CONFG_SET0);

	/* iommu clk setting */
//	LOG_DBG("iommu clk setting %s %d\n", __func__, __LINE__);
	/* CGEN_SOC */
	apu_writel(0x00000004, apupw.regs[apu_acc] + APU_ACC_CONFG_CLR1);
	/* HW_CTRL_EN */
	apu_writel(0x00008000, apupw.regs[apu_acc] + APU_ACC_CONFG_SET1);

	/* mvpu clk setting */
//	LOG_DBG("mvpu clk setting %s %d\n", __func__, __LINE__);
	/* CGEN_SOC */
	apu_writel(0x00000004, apupw.regs[apu_acc] + APU_ACC_CONFG_CLR2);
	/* HW_CTRL_EN */
	apu_writel(0x00008000, apupw.regs[apu_acc] + APU_ACC_CONFG_SET2);
	/* CLK_REQ_SW_EN */
	apu_writel(0x00000100, apupw.regs[apu_acc] + APU_ACC_AUTO_CTRL_SET2);

	/* mdla clk setting */
//	LOG_DBG("mdla clk setting %s %d\n", __func__, __LINE__);
	/* CGEN_SOC */
	apu_writel(0x00000004, apupw.regs[apu_acc] + APU_ACC_CONFG_CLR3);
	/* HW_CTRL_EN */
	apu_writel(0x00008000, apupw.regs[apu_acc] + APU_ACC_CONFG_SET3);
	/* CLK_REQ_SW_EN */
	apu_writel(0x00000100, apupw.regs[apu_acc] + APU_ACC_AUTO_CTRL_SET3);

	/* clk invert setting */
//	LOG_DBG("clk invert setting %s %d\n", __func__, __LINE__);
	/*
	 * MVPU1_CLK_INV_EN MVPU3_CLK_INV_EN MVPU5_CLK_INV_EN
	 * MDLA1_CLK_INV_EN MDLA3_CLK_INV_EN
	 * MDLA5_CLK_INV_EN MDLA7_CLK_INV_EN
	 */
	apu_writel(0x0000AAA8, apupw.regs[apu_acc] + APU_ACC_CLK_INV_EN_SET);

#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "ACC init %s %d --\n", __func__, __LINE__);
#endif
}

static void __apu_buck_off_cfg(void)
{
	/* Step11. Roll back to Buck off stage */
	NOTICE(MODULE_TAG "%s %d ++\n", __func__, __LINE__);

	/*
	 * a. Setup Buck control signal
	 * The following setting need to in order,
	 * and wait 1uS before setup next control signal
	 */
	/* APU_BUCK_PROT_REQ */
	apu_writel(0x00004000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(10);
	/* APU_BUCK_ELS_EN */
	apu_writel(0x00000400, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(10);
	/* APU_BUCK_RST_B */
	apu_writel(0x00002000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(10);

	/* b. Manually turn off Buck (by configuring register in PMIC) */
	/* move to probe last line */
	NOTICE(MODULE_TAG "%s %d --\n", __func__, __LINE__);
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
	uint32_t cmd_op_w = 0x4;
	uint32_t vapu_en_offset = BUCK_VAPU_PMIC_REG_EN_ADDR;
	uint32_t vapu_sram_en_offset = BUCK_VAPU_SRAM_PMIC_REG_EN_ADDR;

#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "PCU init %s %d ++\n", __func__, __LINE__);
#endif

	/* auto buck enable */
	apu_writel((0x1 << 16), apupw.regs[apu_pcu] + APU_PCUTOP_CTRL_SET);

	/* Step1. enable auto buck on/off function of command0/1 */
	apu_writel(0x33, apupw.regs[apu_pcu] + APU_PCU_BUCK_STEP_SEL);

	/* Step2. fill-in command0/1 for vapu_sram/vapu auto buck ON */
	apu_writel((vapu_sram_en_offset << 16) + 1,
			apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT0_L);
	apu_writel(cmd_op_w, apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT0_H);

	apu_writel((vapu_en_offset << 16) + 1,
			apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT1_L);
	apu_writel(cmd_op_w, apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT1_H);

//	LOG_DBG("%s APU_PCU_BUCK_ON_DAT0_L=0x%08x, APU_PCU_BUCK_ON_DAT0_H=0x%08x\n",
//		__func__,
//		apu_readl(apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT0_L),
//		apu_readl(apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT0_H));
//	LOG_DBG("%s APU_PCU_BUCK_ON_DAT1_L=0x%08x, APU_PCU_BUCK_ON_DAT1_H=0x%08x\n",
//		__func__,
//		apu_readl(apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT1_L),
//		apu_readl(apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_DAT1_H));

	/* Step3. fill-in command0/1 for vapu/vapu_sram auto buck OFF */
	apu_writel((vapu_en_offset << 16) + 0,
			apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT0_L);
	apu_writel(cmd_op_w, apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT0_H);

	apu_writel((vapu_sram_en_offset << 16) + 0,
			apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT1_L);
	apu_writel(cmd_op_w, apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT1_H);

//	LOG_DBG("%s APU_PCU_BUCK_OFF_DAT0_L=0x%08x, APU_PCU_BUCK_OFF_DAT0_H=0x%08x\n",
//		__func__,
//		apu_readl(apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT0_L),
//		apu_readl(apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT0_H));
//	LOG_DBG("%s APU_PCU_BUCK_OFF_DAT1_L=0x%08x, APU_PCU_BUCK_OFF_DAT1_H=0x%08x\n",
//		__func__,
//		apu_readl(apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT1_L),
//		apu_readl(apupw.regs[apu_pcu] + APU_PCU_BUCK_OFF_DAT1_H));

	/* Step4. update buck settle time for vapu_sram/vapu by SEL0/1 */
	apu_writel(APU_PCU_BUCK_ON_SETTLE_TIME,
			apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_SLE0);
	apu_writel(APU_PCU_BUCK_ON_SETTLE_TIME,
			apupw.regs[apu_pcu] + APU_PCU_BUCK_ON_SLE1);

#if LOCAL_DEBUG
	NOTICE("PCU init %s %d --\n", __func__, __LINE__);
#endif
}

static void __apu_rpclite_init(void)
{
	uint32_t sleep_type_offset[] = {0x0208, 0x020C, 0x0210, 0x0214,
					0x0218, 0x021C, 0x0220, 0x0224};
	enum apupw_reg rpc_lite_base[CLUSTER_NUM];
	int ofs_arr_size = sizeof(sleep_type_offset) / sizeof(uint32_t);
	int acx_idx, ofs_idx;

#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "RPC-Lite init %s %d ++\n", __func__, __LINE__);
#endif

	rpc_lite_base[0] = apu_acx0_rpc_lite;

	for (acx_idx = 0 ; acx_idx < CLUSTER_NUM ; acx_idx++) {
		for (ofs_idx = 0 ; ofs_idx < ofs_arr_size ; ofs_idx++) {
			/* Memory setting */
			apu_clearl((0x1 << 1),
					apupw.regs[rpc_lite_base[acx_idx]]
					+ sleep_type_offset[ofs_idx]);
		}

		/* Control setting */
		apu_setl(0x0000009E, apupw.regs[rpc_lite_base[acx_idx]]
					+ APU_RPC_TOP_SEL);
	}

#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "RPC-Lite init %s %d --\n", __func__, __LINE__);
#endif
}

static void __apu_rpc_init(void)
{
#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "RPC init %s %d ++\n", __func__, __LINE__);
#endif

	/* Step7. RPC: memory types (sleep or PD type) */
	/* RPC: iTCM in uP need to setup to sleep type */
	apu_clearl((0x1 << 1), apupw.regs[apu_rpc] + 0x0200);

	/* Step9. RPCtop initial */
	/* RPC */
#if ENABLE_SW_BUCK_CTL
	apu_setl(0x0800101E, apupw.regs[apu_rpc] + APU_RPC_TOP_SEL);
#else
	apu_setl(0x0800501E, apupw.regs[apu_rpc] + APU_RPC_TOP_SEL);
#endif

	/* BUCK_PROT_SEL */
	apu_setl((0x1 << 20), apupw.regs[apu_rpc] + APU_RPC_TOP_SEL_1);

#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "RPC init %s %d --\n", __func__, __LINE__);
#endif
}

static int __apu_are_init(void)
{
	uint32_t tmp = 0;
	int ret, val = 0;
	int are_id, idx = 0;
	uint32_t are_entry2_cfg_h[] = {0x00140000, 0x00140000, 0x00140000};
	uint32_t are_entry2_cfg_l[] = {0x004E0804, 0x004E0806, 0x004E0807};
	int polling_cnt = 0;

#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "ARE init %s %d ++\n", __func__, __LINE__);
#endif

	/* Step10. ARE initial */

	/* Turn on ARE: (8139/8188 Only) */
	tmp = apu_readl(apupw.regs[apu_ao_ctl] + CSR_DUMMY_0_ADDR) | (0x1 << 2);
	apu_writel(tmp, apupw.regs[apu_ao_ctl] + CSR_DUMMY_0_ADDR);

	/* TINFO="Wait for sare1 fsm to transition to IDLE" */
#if 0
	ret = readl_relaxed_poll_timeout_atomic(
			(apupw.regs[apu_are2] + 0x48),
			val, (val & 0x1UL), 50, 10000);
	if (ret) {
//		pr_info("%s timeout to wait sram1 fsm to idle, ret %d\n",
//				__func__, ret);
		return -1;
	}
#endif

	while ((apu_readl(apupw.regs[apu_are2] + 0x48) & 0x1) != 0x1) {
		udelay(5);
		if (polling_cnt++ > 20) {
			ret = -1;
			ERROR(MODULE_TAG "[%s][%d] ARE init timeout\n",
				__func__, __LINE__);
			break;
		}
	}

	for (are_id = apu_are0, idx = 0; are_id <= apu_are2; are_id++, idx++) {

//		LOG_DBG("%s are_id:%d\n", __func__, are_id);

		/* ARE entry 0 initial */
		apu_writel(0x01234567, apupw.regs[are_id]
						+ APU_ARE_ETRY0_SRAM_H);
		apu_writel(0x89ABCDEF, apupw.regs[are_id]
						+ APU_ARE_ETRY0_SRAM_L);

		/* ARE entry 1 initial */
		apu_writel(0xFEDCBA98, apupw.regs[are_id]
						+ APU_ARE_ETRY1_SRAM_H);
		apu_writel(0x76543210, apupw.regs[are_id]
						+ APU_ARE_ETRY1_SRAM_L);

		/* ARE entry 2 initial */
		apu_writel(are_entry2_cfg_h[idx], apupw.regs[are_id]
						+ APU_ARE_ETRY2_SRAM_H);
		apu_writel(are_entry2_cfg_l[idx], apupw.regs[are_id]
						+ APU_ARE_ETRY2_SRAM_L);

		/* dummy read ARE entry2 H/L sram */
		tmp = apu_readl(apupw.regs[are_id] + APU_ARE_ETRY2_SRAM_H);
		tmp = apu_readl(apupw.regs[are_id] + APU_ARE_ETRY2_SRAM_L);

		/* update ARE sram */
		apu_writel(0x00000004, apupw.regs[are_id] + APU_ARE_INI_CTRL);

//		dev_dbg(dev, "%s ARE entry2_H phys 0x%x = 0x%x\n",
//			__func__,
//			(u32)(apupw.phy_addr[are_id] + APU_ARE_ETRY2_SRAM_H),
//			readl(apupw.regs[are_id] + APU_ARE_ETRY2_SRAM_H));

//		dev_dbg(dev, "%s ARE entry2_L phys 0x%x = 0x%x\n",
//			__func__,
//			(u32)(apupw.phy_addr[are_id] + APU_ARE_ETRY2_SRAM_L),
//			readl(apupw.regs[are_id] + APU_ARE_ETRY2_SRAM_L));
	}

#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "ARE init %s %d --\n", __func__, __LINE__);
#endif

	return 0;
}

static void __apu_aoc_init(void)
{
#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "[%s][%d] +\n", __func__, __LINE__);
#endif

/*
	// Step1. Switch APU AOC control signal from SW register to HW path (RPC)
	//        rpc_sram_ctrl_mux_sel
	// Step2. Manually disable Buck els enable @SOC
	//        (disable SW mode to manually control Buck on/off)
	//        vapu_ext_buck_iso
	apupw_secure_init(0);
*/

	/* Step1. Switch APU AOC ctrl signal from SW reg to HW path (RPC) */
	// apu_clearl((0x1 << 5), apupw.regs[sys_spm] + 0xf30);
	// apu_setl((0x1 << 0), apupw.regs[sys_spm] + 0x414);

	/*
	* Step2. Manually disable Buck els enable @SOC
	* (disable SW mode to manually control Buck on/off)
	*/
	apu_clearl(((0x1 << 5) | (0x1 << 0)),
				0x10006000 + APUSYS_BUCK_ISOLATION);

	/*
	* Step3. Roll back to APU Buck on stage
	* The following setting need to in order and wait 1uS before setup
	* next control signal
	*/
	/* APU_BUCK_ELS_EN */
	apu_writel(0x00000800, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(10);

	/* APU_BUCK_RST_B */
	apu_writel(0x00001000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(10);

	/* APU_BUCK_PROT_REQ */
	apu_writel(0x00008000, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(10);

	/* SRAM_AOC_ISO */
	apu_writel(0x00000080, apupw.regs[apu_rpc] + APU_RPC_HW_CON);
	udelay(10);

#if LOCAL_DEBUG
	NOTICE(MODULE_TAG "[%s][%d] -\n", __func__, __LINE__);
#endif
}

static int init_hw_setting(void)
{
	__apu_aoc_init();
	__apu_pcu_init();
	__apu_rpc_init();
	__apu_rpclite_init();
	__apu_are_init();
	__apu_pll_init();
	__apu_acc_init();
	__apu_buck_off_cfg();

	return 0;
}

/*
 * init apusys power hw here and ensure the following sequence:
 * 1. apusys_power
 * 2. apusys_devapc
 */
int apusys_power_init(void)
{
	int idx = 0;

	for (idx = sys_spm; idx < APUPW_MAX_REGS; idx++) {
		apupw.regs[idx] = reg_addr[idx];
		apupw.phy_addr[idx] = reg_addr[idx];
	}

	init_hw_setting();

#if LOCAL_DEBUG
	INFO(MODULE_TAG "%s initial done\n", __func__);
#endif
	return 0;
}

