/*
 * Copyright (c) 2021, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* TF-A system header */
#include <common/debug.h>
#include <lib/utils_def.h>

/* Vendor header */
#include <mtk_mmap_pool.h>
#include <platform_def.h>
#include "apusys_devapc_def.h"

#define DUMP_APUSYS_DAPC       (0)

#if 0
static const mmap_region_t devapc_apusys_mmap[] MTK_MMAP_SECTION = {
	MAP_REGION_FLAT(APUSYS_CTRL_DAPC_AO_BASE, DEVAPC_REG_SIZE,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APUSYS_CTRL_DAPC_RCX_BASE, DEVAPC_REG_SIZE,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APUSYS_NOC_DAPC_RCX_BASE, DEVAPC_REG_SIZE,
		MT_DEVICE | MT_RW | MT_SECURE),
	{0}
};
DECLARE_MTK_MMAP_REGIONS(devapc_apusys_mmap);
#endif

/* AO NOC DAPC - rcx_devapc_ao_wrapper_u_device_apc_ao_device_apc_ao */
static const struct apc_dom_16 APUSYS_NOC_DAPC_RCX[] = {
/* ctrl index = 0 */
SLAVE_MD32_SRAM("slv16-0"),  /* RCX: slv16: MD32 */
SLAVE_MD32_SRAM("slv16-1"),
SLAVE_MD32_SRAM("slv16-2"),
SLAVE_MD32_SRAM("slv16-3"),
SLAVE_MD32_SRAM("slv16-4"),
};

/* AO CONTROL DEVAPC - apu_rcx_ao_infra_dapc_con */
static const struct apc_dom_16 APUSYS_CTRL_DAPC_AO[] = {
/* ctrl index = 0 */
SLAVE_VCORE("apu_ao_ctl_o-0"),
SLAVE_RPC("apu_ao_ctl_o-2"),
SLAVE_PCU("apu_ao_ctl_o-3"),
SLAVE_AO_CTRL("apu_ao_ctl_o-4"),
SLAVE_PLL("apu_ao_ctl_o-5"),
SLAVE_ACC("apu_ao_ctl_o-6"),
SLAVE_SEC("apu_ao_ctl_o-7"),  /* apu_sec_con */
SLAVE_ARE0("apu_ao_ctl_o-8"), /* APU_ARETOP_are0 */
SLAVE_ARE1("apu_ao_ctl_o-9"),
SLAVE_ARE2("apu_ao_ctl_o-10"),

/* ctrl index = 10 */
SLAVE_UNKNOWN("apu_ao_ctl_o-11"),
SLAVE_AO_BCRM("apu_ao_ctl_o-12"),
SLAVE_AO_DAPC_WRAP("apu_ao_ctl_o-13"),
SLAVE_AO_DAPC_CON("apu_ao_ctl_o-14"),
SLAVE_RCX_ACX_BULK("apu_ao_ctl_o-15"),
SLAVE_UNKNOWN("apu_ao_ctl_o-16"),
SLAVE_UNKNOWN("apu_ao_ctl_o-17"),
SLAVE_APU_BULK("apu_ao_ctl_o-18"),
SLAVE_ACX0_BCRM("apu_ao_ctl_o-20"), /* Covers apu_noc_mni_acx0 */
SLAVE_RPCTOP_LITE_ACX0("apu_ao_ctl_o-21"),

/* ctrl index = 20 */
SLAVE_ACX1_BCRM("apu_ao_ctl_o-22"), /* Covers apu_noc_mni_acx1 */
SLAVE_RPCTOP_LITE_ACX1("apu_ao_ctl_o-23"),
SLAVE_RCX_TO_ACX0_0("apu_rcx2acx0_o-0"),
SLAVE_RCX_TO_ACX0_1("apu_rcx2acx0_o-1"),
SLAVE_SAE_TO_ACX0_0("apu_sae2acx0_o-0"),
SLAVE_SAE_TO_ACX0_1("apu_sae2acx0_o-1"),
SLAVE_RCX_TO_ACX1_0("apu_rcx2acx1_o-0"),
SLAVE_RCX_TO_ACX1_1("apu_rcx2acx1_o-1"),
SLAVE_SAE_TO_ACX1_0("apu_sae2acx1_o-0"),
SLAVE_SAE_TO_ACX1_1("apu_sae2acx1_o-1"),
};

/* RCX CONTROL DEVAPC - apu_rcx_infra_dapc_con */
static const struct apc_dom_16 APUSYS_CTRL_DAPC_RCX[] = {
/* ctrl index = 0 */
SLAVE_MD32_SYSCTRL0("md32_apb_s-0"),
SLAVE_MD32_SYSCTRL1("md32_apb_s-1"),
SLAVE_MD32_WDT("md32_apb_s-2"),
SLAVE_MD32_CACHE("md32_apb_s-3"),
SLAVE_RPC("apusys_ao-0"),
SLAVE_PCU("apusys_ao-1"),
SLAVE_AO_CTRL("apusys_ao-2"),
SLAVE_PLL("apusys_ao-3"),
SLAVE_ACC("apusys_ao-4"),
SLAVE_SEC("apusys_ao-5"),  /* apu_sec_con */

/* ctrl index = 10 */
SLAVE_ARE0("apusys_ao-6"),  /* APU_ARETOP_are0 */
SLAVE_ARE1("apusys_ao-7"),
SLAVE_ARE2("apusys_ao-8"),
SLAVE_UNKNOWN("apusys_ao-9"),
SLAVE_AO_BCRM("apusys_ao-10"),
SLAVE_AO_DAPC_WRAP("apusys_ao-11"),
SLAVE_AO_DAPC_CON("apusys_ao-12"),  /* apu_rcx_ao_infra_dapc_con */
SLAVE_VCORE("apusys_ao-13"),
SLAVE_ACX0_BCRM("apusys_ao-15"),
SLAVE_ACX1_BCRM("apusys_ao-16"),

/* ctrl index = 20 */
SLAVE_NOC_AXI("noc_axi"),
SLAVE_MD32_DBG("md32_dbg"),
SLAVE_DBG_CRTL("apb_infra_dbg"),
SLAVE_IOMMU0_BANK0("apu_n_mmu_r0"),
SLAVE_IOMMU0_BANK1("apu_n_mmu_r1"),
SLAVE_IOMMU0_BANK2("apu_n_mmu_r2"),
SLAVE_IOMMU0_BANK3("apu_n_mmu_r3"),
SLAVE_IOMMU0_BANK4("apu_n_mmu_r4"),
SLAVE_IOMMU1_BANK0("apu_s_mmu_r0"),
SLAVE_IOMMU1_BANK1("apu_s_mmu_r1"),

/* ctrl index = 30 */
SLAVE_IOMMU1_BANK2("apu_s_mmu_r2"),
SLAVE_IOMMU1_BANK3("apu_s_mmu_r3"),
SLAVE_IOMMU1_BANK4("apu_s_mmu_r4"),
SLAVE_S0_SSC("apu_s0_ssc_cfg"),
SLAVE_N0_SSC("apu_n0_ssc_cfg"),
SLAVE_ACP_SSC("apu_acp_ssc_cfg"),
SLAVE_S1_SSC("apu_s1_ssc_cfg"),
SLAVE_N1_SSC("apu_n1_ssc_cfg"),
SLAVE_CFG("apu_rcx_cfg"),
SLAVE_SEMA_STIMER("apu_sema_stimer"),

/* ctrl index = 40 */
SLAVE_EMI_CFG("apu_emi_cfg"),
SLAVE_LOG("apu_logtop"),
SLAVE_CPE_SENSOR("apu_cpe_sensor"),
SLAVE_CPE_COEF("apu_cpe_coef"),
SLAVE_CPE_CTRL("apu_cpe_ctrl"),
SLAVE_UNKNOWN("apu_xpu_rsi"),
SLAVE_DFD_REG_SOC("apu_dfd"),
SLAVE_SENSOR_WRAP_ACX0_DLA0("apu_sen_ac0_dla0"),
SLAVE_SENSOR_WRAP_ACX0_DLA1("apu_sen_ac0_dla1"),
SLAVE_SENSOR_WRAP_ACX0_VPU0("apu_sen_ac0_vpu"),

/* ctrl index = 50 */
SLAVE_SENSOR_WRAP_ACX1_DLA0("apu_sen_ac1_dla0"),
SLAVE_SENSOR_WRAP_ACX1_DLA1("apu_sen_ac1_dla1"),
SLAVE_SENSOR_WRAP_ACX1_VPU0("apu_sen_ac1_vpu"),
SLAVE_REVISER("noc_cfg-0"),
SLAVE_NOC("noc_cfg-1"),
SLAVE_BCRM("infra_bcrm"),
SLAVE_DAPC_WRAP("infra_dapc_wrap"),
SLAVE_DAPC_CON("infra_dapc_con"),
SLAVE_NOC_DAPC_WRAP("noc_dapc_wrap"),
SLAVE_NOC_DAPC_CON("noc_dapc_con"),

/* ctrl index = 60 */
SLAVE_NOC_BCRM("noc_bcrm"),
SLAVE_ACS("apu_rcx_acs"),
SLAVE_HSE("apu_hse"),
};

/* NOC DAPC of RCX (at AO Power domain) */
static int32_t set_slave_noc_dapc_rcx(uint32_t slave,
		enum apusys_apc_domain_id domain_id,
		enum apusys_apc_perm_type perm)
{
	uint32_t apc_register_index;
	uint32_t apc_set_index;
	uint32_t *base = NULL;
	uint32_t clr_bit;
	uint32_t set_bit;

	if (perm >= PERM_NUM || perm < 0) {
		ERROR("[APUAPC_NOC_RCX] permission type:0x%x is not supported!\n",
				perm);
		return APUSYS_APC_ERR_PERMISSION_NOT_SUPPORTED;
	}

	apc_register_index = slave / APUSYS_NOC_DAPC_RCX_SLAVE_NUM_IN_1_DOM;
	apc_set_index = slave % APUSYS_NOC_DAPC_RCX_SLAVE_NUM_IN_1_DOM;

	clr_bit = 0xFFFFFFFF ^ (0x3U << (apc_set_index * 2));
	set_bit = ((uint32_t)perm) << (apc_set_index * 2);

	if (slave < APUSYS_NOC_DAPC_RCX_SLAVE_NUM &&
			domain_id < (uint32_t)APUSYS_NOC_DAPC_RCX_DOM_NUM)
		base = (uint32_t *)((size_t)APUSYS_NOC_DAPC_RCX_BASE +
				domain_id * 0x40 + apc_register_index * 4);
	else {
		ERROR("[APUAPC_NOC_RCX] %s: %s, %s:0x%x, %s:0x%x\n",
				__func__, "out of boundary",
				"slave", slave,
				"domain_id", domain_id);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	if (base) {
		apuapc_writel(apuapc_readl(base) & clr_bit, base);
		apuapc_writel(apuapc_readl(base) | set_bit, base);

		return APUSYS_APC_OK;
	}

	return APUSYS_APC_ERR_GENERIC;
}

/* Control DAPC at AO Power domain */
static int32_t set_slave_ao_ctrl_apc(uint32_t slave,
		enum apusys_apc_domain_id domain_id,
		enum apusys_apc_perm_type perm)
{
	uint32_t apc_register_index;
	uint32_t apc_set_index;
	uint32_t *base = NULL;
	uint32_t clr_bit;
	uint32_t set_bit;

	if (perm >= PERM_NUM) {
		ERROR("[APUAPC] permission type:0x%x is not supported!\n",
				perm);
		return APUSYS_APC_ERR_PERMISSION_NOT_SUPPORTED;
	}

	apc_register_index = slave / APUSYS_CTRL_DAPC_AO_SLAVE_NUM_IN_1_DOM;
	apc_set_index = slave % APUSYS_CTRL_DAPC_AO_SLAVE_NUM_IN_1_DOM;

	clr_bit = 0xFFFFFFFF ^ (0x3U << (apc_set_index * 2));
	set_bit = (uint32_t)perm << (apc_set_index * 2);

	if (slave < APUSYS_CTRL_DAPC_AO_SLAVE_NUM &&
			domain_id < (uint32_t)APUSYS_CTRL_DAPC_AO_DOM_NUM)
		base = (uint32_t *)((size_t)APUSYS_CTRL_DAPC_AO_BASE +
				domain_id * 0x40 + apc_register_index * 4);
	else {
		ERROR("[APUAPC_CTRL_AO] %s: %s, %s:0x%x, %s:0x%x\n",
				__func__, "out of boundary",
				"slave", slave,
				"domain_id", domain_id);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	if (base) {
		apuapc_writel(apuapc_readl(base) & clr_bit, base);
		apuapc_writel(apuapc_readl(base) | set_bit, base);

		return APUSYS_APC_OK;
	}

	return APUSYS_APC_ERR_GENERIC;
}

/* Control DAPC at RCX Power domain */
static int32_t set_slave_rcx_ctrl_apc(uint32_t slave,
		enum apusys_apc_domain_id domain_id,
		enum apusys_apc_perm_type perm)
{
	uint32_t apc_register_index;
	uint32_t apc_set_index;
	uint32_t *base = NULL;
	uint32_t clr_bit;
	uint32_t set_bit;

	if (perm >= PERM_NUM) {
		ERROR("[APUAPC] permission type:0x%x is not supported!\n",
				perm);
		return APUSYS_APC_ERR_PERMISSION_NOT_SUPPORTED;
	}

	apc_register_index = slave / APUSYS_CTRL_DAPC_RCX_SLAVE_NUM_IN_1_DOM;
	apc_set_index = slave % APUSYS_CTRL_DAPC_RCX_SLAVE_NUM_IN_1_DOM;

	clr_bit = 0xFFFFFFFF ^ (0x3U << (apc_set_index * 2));
	set_bit = (uint32_t)perm << (apc_set_index * 2);

	if (slave < APUSYS_CTRL_DAPC_RCX_SLAVE_NUM &&
			domain_id < (uint32_t)APUSYS_CTRL_DAPC_RCX_DOM_NUM)
		base = (uint32_t *)((size_t)APUSYS_CTRL_DAPC_RCX_BASE +
				domain_id * 0x40 + apc_register_index * 4);
	else {
		ERROR("[APUAPC] %s: %s, %s:0x%x, %s:0x%x\n",
				__func__, "out of boundary",
				"slave", slave,
				"domain_id", domain_id);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	if (base) {
		apuapc_writel(apuapc_readl(base) & clr_bit, base);
		apuapc_writel(apuapc_readl(base) | set_bit, base);

		return APUSYS_APC_OK;
	}

	return APUSYS_APC_ERR_GENERIC;
}

static void apusys_devapc_init(const char *name, uint32_t base)
{
	uint32_t val;

	val = apuapc_readl(APUSYS_DAPC_CON(base)) & APUSYS_DAPC_CON_VIO_MASK;
	apuapc_writel(APUSYS_DAPC_CON_VIO_MASK, APUSYS_DAPC_CON(base));
	if (val) {
	}
}

static void apusys_devapc_ao_init(void)
{
	INFO("[APUAPC_AO] %s\n", __func__);
	apusys_devapc_init("APUAPC_CTRL_AO", APUSYS_CTRL_DAPC_AO_BASE);
	INFO("[APUAPC_AO] %s done\n", __func__);
}

static void apusys_devapc_rcx_init(void)
{
	INFO("[APUAPC_RCX] %s\n", __func__);
	apusys_devapc_init("APUAPC_CTRL_RCX", APUSYS_CTRL_DAPC_RCX_BASE);
	apusys_devapc_init("APUAPC_NOC_RCX", APUSYS_NOC_DAPC_RCX_BASE);
	INFO("[APUAPC_RCX] %s done\n", __func__);
}

/*
 * AO Power Domain DAPC(s) initialization (from LK), including
 *  Control DAPC:
 *     apu_rcx_ao_infra_dapc_con
 */

static uint32_t kernel_init;

int32_t start_apusys_devapc_ao(void)
{
	int32_t ret = APUSYS_APC_OK;

	/* Clear all violation status */
	apusys_devapc_ao_init();

	/* Initial Permission */
	ret = set_apusys_dapc_v1(APUSYS_CTRL_DAPC_AO, set_slave_ao_ctrl_apc);
	INFO("[APUAPC_AO] %s - %s!\n", "set_apusys_ao_ctrl_dapc",
			ret ? "FAILED" : "SUCCESS");
	if (ret)
		return ret;

#if DUMP_APUSYS_DAPC
	/* Dump Control DAPC Permission */
	dump_apusys_dapc_v1(APUSYS_CTRL_DAPC_AO);
#endif

	INFO("[APUAPC_AO] %s done\n", __func__);
	kernel_init = 0;

	return ret;
}

/*
 * RCX Power Domain DAPC(s) initialization (from Kernel), including
 *   Control DAPC:
 *     apu_rcx_infra_dapc_con
 *     - This's where Reviser DAPC Permission being initialized and locked.
 *   NoC DAPC:
 *     rcx_devapc_ao_wrapper_u_device_apc_ao_device_apc_ao
 *
 * Note: ACX Power Domain DAPC(s) are initialized by ARE driver at APU-uP, including
 *   Control DAPC:
 *     apu_acx0_infra_dapc_con
 *     apu_acx1_infra_dapc_con
 *   NoC DAPC:
 *     acx0_devapc_ao_wrapper_u_device_apc_ao_device_apc_ao
 *     acx1_devapc_ao_wrapper_u_device_apc_ao_device_apc_ao
 */

int32_t start_apusys_devapc_rcx(void)
{
	int32_t ret = APUSYS_APC_OK;

	if (kernel_init) {
		INFO("[APUAPC RCX] %s !\n", "init more than once");
		return APUSYS_APC_ERR_GENERIC;
	}
	kernel_init = 1;

	/* Clear all violation status */
	apusys_devapc_rcx_init();

	/* Initial Permission */
	ret = set_apusys_dapc_v1(APUSYS_CTRL_DAPC_RCX, set_slave_rcx_ctrl_apc);
	INFO("[APUAPC_RCX] %s - %s!\n", "set_slave_rcx_ctrl_apc",
			ret ? "FAILED" : "SUCCESS");
	if (ret)
		return ret;

#if DUMP_APUSYS_DAPC
	/* Dump Control DAPC Permission */
	dump_apusys_dapc_v1(APUSYS_CTRL_DAPC_RCX);
#endif

	/* Initial NoC DAPC Permission */
	ret = set_apusys_dapc_v1(APUSYS_NOC_DAPC_RCX, set_slave_noc_dapc_rcx);
	INFO("[APUAPC_RCX] %s - %s!\n", "set_slave_noc_dapc_rcx",
			ret ? "FAILED" : "SUCCESS");
	if (ret)
		return ret;

#if DUMP_APUSYS_DAPC
	/* Dump NoC DAPC Permission */
	dump_apusys_dapc_v1(APUSYS_NOC_DAPC_RCX);
#endif

	INFO("[APUAPC_RCX] %s done\n", __func__);

	return ret;
}

