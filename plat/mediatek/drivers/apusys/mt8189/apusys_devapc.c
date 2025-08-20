/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
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

/* AO CONTROL DEVAPC - apu_rcx_ao_infra_dapc_con */
static const struct apc_dom_16 APUSYS_CTRL_DAPC_AO[] = {
/* ctrl index = 0 */
SLAVE_RCX_BULK0("apu_ao_ctl_o-0"),
SLAVE_MD32_APB("apu_ao_ctl_o-1"),
SLAVE_ACP_TCU_SSC("apu_ao_ctl_o-2"),
SLAVE_PTP_THM("apu_ao_ctl_o-3"),
SLAVE_VCORE("apu_ao_ctl_o-4"),
SLAVE_IOMMU0_BANK0("apu_ao_ctl_o-5"),
SLAVE_IOMMU0_BANK1("apu_ao_ctl_o-6"),
SLAVE_IOMMU0_BANK2("apu_ao_ctl_o-7"),
SLAVE_IOMMU0_BANK3("apu_ao_ctl_o-8"),
SLAVE_IOMMU0_BANK4("apu_ao_ctl_o-9"),

/* ctrl index = 10 */
SLAVE_IOMMU1_BANK0("apu_ao_ctl_o-10"),
SLAVE_IOMMU1_BANK1("apu_ao_ctl_o-11"),
SLAVE_IOMMU1_BANK2("apu_ao_ctl_o-12"),
SLAVE_IOMMU1_BANK3("apu_ao_ctl_o-13"),
SLAVE_IOMMU1_BANK4("apu_ao_ctl_o-14"),
SLAVE_S0_SSC("apu_ao_ctl_o-15"),
SLAVE_N0_SSC("apu_ao_ctl_o-16"),
SLAVE_S1_SSC("apu_ao_ctl_o-17"),
SLAVE_N1_SSC("apu_ao_ctl_o-18"),
SLAVE_ACP_SSC("apu_ao_ctl_o-19"),

/* ctrl index = 20 */
SLAVE_WDEC("apu_ao_ctl_o-20"),
SLAVE_SMMU("apu_ao_ctl_o-21"),
SLAVE_ARE0("apu_ao_ctl_o-22"),
SLAVE_ARE1("apu_ao_ctl_o-23"),
SLAVE_RPC("apu_ao_ctl_o-25"),
SLAVE_PCU("apu_ao_ctl_o-26"),
SLAVE_AO_CTRL("apu_ao_ctl_o-27"),
SLAVE_ACC("apu_ao_ctl_o-28"),
SLAVE_SEC("apu_ao_ctl_o-29"),
SLAVE_PLL("apu_ao_ctl_o-30"),

/* ctrl index = 30 */
SLAVE_RPC_MDLA("apu_ao_ctl_o-31"),
SLAVE_TOP_PMU("apu_ao_ctl_o-32"),
SLAVE_AO_BCRM("apu_ao_ctl_o-33"),
SLAVE_AO_DAPC_WRAP("apu_ao_ctl_o-34"),
SLAVE_AO_DAPC_CON("apu_ao_ctl_o-35"),
SLAVE_UNDEFINE0("apu_ao_ctl_o-36"),
SLAVE_UNDEFINE1("apu_ao_ctl_o-37"),
SLAVE_RCX_BULK1("apu_ao_ctl_o-38"),
SLAVE_UNDEFINE2("apu_ao_ctl_o-39"),
SLAVE_UNDEFINE3("apu_ao_ctl_o-40"),

/* ctrl index = 40 */
SLAVE_UNDEFINE4("apu_ao_ctl_o-41"),
SLAVE_UNDEFINE5("apu_ao_ctl_o-42"),
SLAVE_UNDEFINE6("apu_ao_ctl_o-43"),
SLAVE_DATA_BULK("apu_ao_ctl_o-44"),
SLAVE_ACX0_BULK("apu_ao_ctl_o-45"),
SLAVE_ACX0_AO("apu_ao_ctl_o-46"),
SLAVE_ACX1_BULK("apu_ao_ctl_o-47"),
SLAVE_ACX1_AO("apu_ao_ctl_o-48"),
SLAVE_NCX_BULK("apu_ao_ctl_o-49"),
SLAVE_NCX_AO("apu_ao_ctl_o-50"),

/* ctrl index = 50 */
SLAVE_ACX0_BULK("apu_rcx2acx0_o-0"),
SLAVE_ACX0_AO("apu_rcx2acx0_o-1"),
SLAVE_ACX0_BULK("apu_sae2acx0_o-0"),
SLAVE_ACX0_AO("apu_sae2acx0_o-1"),
SLAVE_ACX1_BULK("apu_rcx2acx1_o-0"),
SLAVE_ACX1_AO("apu_rcx2acx1_o-1"),
SLAVE_ACX1_BULK("apu_sae2acx1_o-0"),
SLAVE_ACX1_AO("apu_sae2acx1_o-1"),
SLAVE_NCX_BULK("apu_rcx2ncx_o-0"),
SLAVE_NCX_AO("apu_rcx2ncx_o-1"),

/* ctrl index = 60 */
SLAVE_NCX_BULK("apu_sae2ncx_o-0"),
SLAVE_NCX_AO("apu_sae2ncx_o-1"),
};
/* RCX CONTROL DEVAPC - apu_rcx_infra_dapc_con */
static const struct apc_dom_16 APUSYS_CTRL_DAPC_RCX[] = {
/* ctrl index = 0 */
SLAVE_MD32_SYSCTRL("md32_apb_s-0"),
SLAVE_MD32_PMU("md32_apb_s-1"),
SLAVE_MD32_WDT("md32_apb_s-2"),
SLAVE_MD32_CACHE("md32_apb_s-3"),
SLAVE_ARE0("apusys_ao-0"),
SLAVE_ARE1("apusys_ao-1"),
SLAVE_RPC("apusys_ao-2"),
SLAVE_PCU("apusys_ao-3"),
SLAVE_AO_CTRL("apusys_ao-4"),
SLAVE_SEC("apusys_ao-5"),

/* ctrl index = 10 */
SLAVE_PLL("apusys_ao-6"),
SLAVE_RPC_MDLA("apusys_ao-7"),
SLAVE_TOP_PMU("apusys_ao-8"),
SLAVE_AO_BCRM("apusys_ao-9"),
SLAVE_AO_DAPC_WRAP("apusys_ao-10"),
SLAVE_AO_DAPC_CON("apusys_ao-11"),
SLAVE_VCORE("apusys_ao-12"),
SLAVE_IOMMU0_BANK0("apu_ao_ctl_o-13"),
SLAVE_IOMMU0_BANK1("apu_ao_ctl_o-14"),
SLAVE_IOMMU0_BANK2("apu_ao_ctl_o-15"),

/* ctrl index = 20 */
SLAVE_IOMMU0_BANK3("apu_ao_ctl_o-16"),
SLAVE_IOMMU0_BANK4("apu_ao_ctl_o-17"),
SLAVE_IOMMU1_BANK0("apu_ao_ctl_o-18"),
SLAVE_IOMMU1_BANK1("apu_ao_ctl_o-19"),
SLAVE_IOMMU1_BANK2("apu_ao_ctl_o-20"),
SLAVE_IOMMU1_BANK3("apu_ao_ctl_o-21"),
SLAVE_IOMMU1_BANK4("apu_ao_ctl_o-22"),
SLAVE_S0_SSC("apu_ao_ctl_o-23"),
SLAVE_N0_SSC("apu_ao_ctl_o-24"),
SLAVE_S1_SSC("apu_ao_ctl_o-25"),

/* ctrl index = 30 */
SLAVE_N1_SSC("apu_ao_ctl_o-26"),
SLAVE_ACP_SSC("apu_ao_ctl_o-27"),
SLAVE_ACP_TCU_SSC("apu_ao_ctl_o-28"),
SLAVE_PTP_THM("apu_ao_ctl_o-29"),
SLAVE_WDEC("apu_ao_ctl_o-30"),
SLAVE_SMMU("apu_ao_ctl_o-31"),
SLAVE_ACX0_BULK("apu_ao_ctl_o-33"),
SLAVE_ACX1_BULK("apu_ao_ctl_o-34"),
SLAVE_NCX_BULK("apu_ao_ctl_o-35"),
SLAVE_DATA_BULK("noc_axi"),

/* ctrl index = 40 */
SLAVE_MD32_DBG("md32_dbg"),
SLAVE_MDLA_DBG("mdla_dbg"),
SLAVE_INFRA_DBG("apb_infra_dbg"),
SLAVE_LOG_TOP0("apu_logtop-0"),
SLAVE_LOG_TOP1("apu_logtop-1"),
SLAVE_RCX_CFG("apu_rcx_cfg"),
SLAVE_ACX_IPS("apu_acx_ips"),
SLAVE_SEMA_STIMER("apu_sema_stimer"),
SLAVE_EMI_CFG("apu_emi_cfg"),
SLAVE_CPE_SENSOR("apu_cpe_sensor"),

/* ctrl index = 50 */
SLAVE_CPE_COEF("apu_cpe_coef"),
SLAVE_CPE_CTRL("apu_cpe_ctrl"),
SLAVE_DFD("apu_dfd"),
SLAVE_SENSOR_ACX0_DLA0("apu_sen_acx0_dla0"),
SLAVE_SENSOR_ACX0_VPU("apu_sen_acx0_vpu"),
SLAVE_SENSOR_ACX1_DLA0("apu_sen_acx1_dla0"),
SLAVE_SENSOR_ACX1_VPU("apu_sen_acx1_vpu"),
SLAVE_SENSOR_NCX_DLA0("apu_sen_ncx_dla0"),
SLAVE_SENSOR_NCX_NVE("apu_sen_ncx_vpu"),
SLAVE_RCX_TCU0("noc_cfg-0"),

/* ctrl index = 60 */
SLAVE_RCX_TCU1("noc_cfg-1"),
SLAVE_RCX_TCU2("noc_cfg-2"),
SLAVE_RCX_TCU3("noc_cfg-3"),
SLAVE_RCX_TCU4("noc_cfg-4"),
SLAVE_RCX_TCU5("noc_cfg-5"),
SLAVE_RCX_TCU6("noc_cfg-6"),
SLAVE_RCX_NOC_CFG("noc_cfg-7"),
SLAVE_MDLA_CORE_CTRL("mdla_cfg-0"),
SLAVE_MDLA_BIU("mdla_cfg-1"),
SLAVE_MDLA_PMU("mdla_cfg-2"),

/* ctrl index = 70 */
SLAVE_MDLA_CMDE("mdla_cfg-3"),
SLAVE_EDPA0("apu_edpa-0"),
SLAVE_EDPA1("apu_edpa-1"),
SLAVE_RCX_BCRM("infra_bcrm"),
SLAVE_RCX_DAPC_WRAP("infra_dpac_wrap"),
SLAVE_RCX_DAPC_CON("infra_dapc_con"),
SLAVE_RCX_CMU("rcx_cmu"),
SLAVE_RCX_ACS("apu_rcx_acx"),
SLAVE_HSE("apu_hse"),
SLAVE_RCX_CBFC("rcx_cbfc"),

/* ctrl index = 80 */
SLAVE_ACC("acc"),
};

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
	apuapc_writel(APUSYS_DAPC_CON_VIO_MASK, APUSYS_DAPC_CON(base));
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

#ifdef DUMP_CFG
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
		INFO("[APUAPC_RCX] %s !\n", "init more than once");
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

#ifdef DUMP_CFG
	/* Dump Control DAPC Permission */
	dump_apusys_dapc_v1(APUSYS_CTRL_DAPC_RCX);
#endif

	INFO("[APUAPC_RCX] %s done\n", __func__);

	return ret;
}
