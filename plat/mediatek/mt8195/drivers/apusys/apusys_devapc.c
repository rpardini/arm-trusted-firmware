/*
 * Copyright (c) 2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* TF-A system header */
#include <common/debug.h>
#include <lib/utils_def.h>
#include <platform_def.h>
#include <mtk_plat_common.h>
#include <apusys_devapc.h>
#include <apusys_devapc_def.h>
#include <lib/spinlock.h>

spinlock_t vpu_domain_ns_lock;

static int init_flag;

#define SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW(domain) \
	APUSYS_APC_AO_ATTR(domain, \
	SEC_RW_ONLY, FORBIDDEN,   FORBIDDEN, FORBIDDEN, \
	FORBIDDEN,   SEC_RW_ONLY, FORBIDDEN, FORBIDDEN, \
	FORBIDDEN,   FORBIDDEN,   FORBIDDEN, FORBIDDEN, \
	FORBIDDEN,   FORBIDDEN,   FORBIDDEN, FORBIDDEN)

/* For AO-0, AO-1 */
#define SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW(domain) \
	APUSYS_APC_AO_ATTR(domain, \
	NO_PROTECTION, FORBIDDEN, FORBIDDEN, FORBIDDEN, \
	FORBIDDEN, SEC_RW_ONLY, FORBIDDEN, FORBIDDEN, \
	FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN, \
	FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN)

#define SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW(domain) \
	APUSYS_APC_AO_ATTR(domain, \
	SEC_RW_NS_R, FORBIDDEN,   FORBIDDEN, FORBIDDEN, \
	FORBIDDEN,   SEC_RW_ONLY, FORBIDDEN, FORBIDDEN, \
	FORBIDDEN,   FORBIDDEN,   FORBIDDEN, FORBIDDEN, \
	FORBIDDEN,   FORBIDDEN,   FORBIDDEN, FORBIDDEN)



/* NOC DAPC */
static const struct APC_DOM_16 APUSYS_NOC_DAPC_AO[] = {
/* 0 */
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv00-0"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv00-1"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv00-2"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv01-0"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv01-1"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv01-2"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv03-0"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv03-1"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv03-2"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv03-3"),
/* 10 */
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv03-4"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv04_05_06_07-0"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv04_05_06_07-1"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv04_05_06_07-2"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("slv04_05_06_07-3"),
};

static const struct APC_DOM_16 APUSYS_AO_Devices[] = {

/* 0 */
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("apusys_ao-0"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("apusys_ao-1"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("apusys_ao-2"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("apusys_ao-3"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("apusys_ao-4"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("apusys_ao-5"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("apusys_ao-6"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("apusys_ao-8"),
SLAVE_FORBID_EXCEPT_D0_D5_SEC_RW("apusys_ao-9"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("md32_apb_s-0"),

/* 10 */
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("md32_apb_s-1"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("md32_apb_s-2"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("md32_debug_apb"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_con2_config"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_con1_config"),
/**
 *  [TODO]apu_sctrl_reviscer need to change to `SLAVE_FORBID_EXCEPT_D0_SEC_RW` after
 *  setting infra dapc APU_S_S-4 to SEC_RW_NS_R
 */
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_sctrl_reviscer"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_sema_stimer"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_emi_config"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_edma0"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_edma1"),

/* 20 */
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_cpe_sensor"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_cpe_coef"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_cpe_ctrl"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_sensor_wrp_dla_0"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_sensor_wrp_dla_1"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_dapc_ao"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_dapc"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("infra_bcrm"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("infra_ao_bcrm"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("noc_dapc"),

/* 30 */
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_noc_bcrm"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_noc_config_0"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_noc_config_1"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_noc_config_2"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("vpu_core0_config-0"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("vpu_core0_config-1"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("vpu_core1_config-0"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("vpu_core1_config-1"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("mdla0_apb-0"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("mdla0_apb-1"),

/* 40 */
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("mdla0_apb-2"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("mdla0_apb-3"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("mdla1_apb-0"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("mdla1_apb-1"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("mdla1_apb-2"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("mdla1_apb-3"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_iommu0_r0"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_iommu0_r1"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_iommu0_r2"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_iommu0_r3"),

/* 50 */
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_iommu0_r4"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_iommu1_r0"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_iommu1_r1"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_iommu1_r2"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_iommu1_r3"),
SLAVE_FORBID_EXCEPT_D0_NO_PORTECT_D5_SEC_RW("apu_iommu1_r4"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_rsi2_config"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_s0_ssc_config"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_n0_ssc_config"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_acp_ssc_config"),

/* 60 */
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_s1_ssc_config"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_n1_ssc_config"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_ao_dbgapb-0"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_ao_dbgapb-1"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_ao_dbgapb-2"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_ao_dbgapb-3"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_ao_dbgapb-4"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apu_ao_dbgapb-5"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("vpu_core0_debug_apb"),
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("vpu_core1_debug_apb"),

/* 70 */
SLAVE_FORBID_EXCEPT_D0_SEC_RW_NS_R_D5_SEC_RW("apb_infra_dbg_ctl"),
};

static int32_t set_slave_noc_dapc(uint32_t slave,
		enum APUSYS_APC_DOMAIN_ID domain_id,
		enum APUSYS_APC_PERM_TYPE perm)
{
	uint32_t apc_register_index;
	uint32_t apc_set_index;
	uint32_t *base = NULL;
	uint32_t clr_bit;
	uint32_t set_bit;

	if (perm >= PERM_NUM || perm < 0) {
		ERROR("[NOC_DAPC] permission type:0x%x is not supported!\n",
				perm);
		return APUSYS_APC_ERR_PERMISSION_NOT_SUPPORTED;
	}

	apc_register_index = slave / APUSYS_NOC_DAPC_AO_SLAVE_NUM_IN_1_DOM;
	apc_set_index = slave % APUSYS_NOC_DAPC_AO_SLAVE_NUM_IN_1_DOM;

	clr_bit = 0xFFFFFFFF ^ (0x3U << (apc_set_index * 2));
	set_bit = ((uint32_t)perm) << (apc_set_index * 2);

	if (slave < APUSYS_NOC_DAPC_AO_SLAVE_NUM &&
			domain_id < (uint32_t)APUSYS_NOC_DAPC_AO_DOM_NUM)
		base = (uint32_t *)((size_t)APUSYS_NOC_DAPC_AO_BASE +
				domain_id * 0x40 + apc_register_index * 4);
	else {
		ERROR("[NOC_DAPC] %s: %s, %s:0x%x, %s:0x%x\n",
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

static void dump_apusys_noc_dapc(void)
{
//	#if 0 /* disable apusys dump dapc */
	uint32_t reg_num;
	uint32_t d, i;

	reg_num = APUSYS_NOC_DAPC_AO_SLAVE_NUM /
			APUSYS_NOC_DAPC_AO_SLAVE_NUM_IN_1_DOM;
	for (d = 0; d < APUSYS_NOC_DAPC_AO_DOM_NUM; d++)
		for (i = 0; i <= reg_num; i++)
			INFO("[NOCDAPC] D%d_APC_%d: 0x%x\n", d, i,
					apuapc_readl(APUSYS_NOC_DAPC_AO_BASE +
							d * 0x40 + i * 4)
			);

	INFO("[NOCDAPC] APC_CON: 0x%x\n",
		apuapc_readl(APUSYS_NOC_DAPC_CON));
//	#endif
}

static int32_t set_slave_apc(uint32_t slave,
		enum APUSYS_APC_DOMAIN_ID domain_id,
		enum APUSYS_APC_PERM_TYPE perm)
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

	apc_register_index = slave / APUSYS_APC_SYS0_AO_SLAVE_NUM_IN_1_DOM;
	apc_set_index = slave % APUSYS_APC_SYS0_AO_SLAVE_NUM_IN_1_DOM;

	clr_bit = 0xFFFFFFFF ^ (0x3 << (apc_set_index * 2));
	set_bit = perm << (apc_set_index * 2);

	if (slave < APUSYS_APC_SYS0_AO_SLAVE_NUM &&
			domain_id < (uint32_t)APUSYS_APC_SYS0_AO_DOM_NUM)
		base = (uint32_t *)((size_t)APUSYS_APC_AO_BASE +
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

static void dump_apusys_ao_apc(void)
{
//	#if 0 /* disable apusys dump dapc */
	uint32_t reg_num;
	uint32_t d, i;

	reg_num = APUSYS_APC_SYS0_AO_SLAVE_NUM /
			APUSYS_APC_SYS0_AO_SLAVE_NUM_IN_1_DOM;
	for (d = 0; d < APUSYS_APC_SYS0_AO_DOM_NUM; d++)
		for (i = 0; i <= reg_num; i++)
			INFO("[APUAPC] D%d_APC_%d: 0x%x\n", d, i,
					apuapc_readl(APUSYS_APC_AO_BASE +
							d * 0x40 + i * 4)
			);

	INFO("[APUAPC] APC_CON: 0x%x\n",
		apuapc_readl(APUSYS_APC_CON));
//	#endif
}

static void print_apuapc_vio(void)
{
	INFO("[APUAPC] vio %d\n",
			apuapc_readl(APUSYS_APC_CON) & 0x80000000);
}

static void apusys_devapc_init(void)
{
	INFO("[APUAPC] %s\n", __func__);

	print_apuapc_vio();

	/* TODO: check vio from 0x1906_4000 */
	/* TODO: clear violation status */
	/* TODO: print vio mask */

	INFO("[APUAPC] %s done\n", __func__);
}

static int32_t set_apusys_noc_dapc(void)
{
	uint32_t apusys_noc_dapc_size;
	int32_t ret = APUSYS_APC_OK;
	uint32_t i;

	apusys_noc_dapc_size = sizeof(APUSYS_NOC_DAPC_AO) /
		sizeof(struct APC_DOM_16);

	for (i = 0; i < apusys_noc_dapc_size; i++) {
		ret += set_slave_noc_dapc(i, DOMAIN_0,
				APUSYS_NOC_DAPC_AO[i].d0_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_1,
				APUSYS_NOC_DAPC_AO[i].d1_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_2,
				APUSYS_NOC_DAPC_AO[i].d2_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_3,
				APUSYS_NOC_DAPC_AO[i].d3_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_4,
				APUSYS_NOC_DAPC_AO[i].d4_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_5,
				APUSYS_NOC_DAPC_AO[i].d5_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_6,
				APUSYS_NOC_DAPC_AO[i].d6_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_7,
				APUSYS_NOC_DAPC_AO[i].d7_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_8,
				APUSYS_NOC_DAPC_AO[i].d8_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_9,
				APUSYS_NOC_DAPC_AO[i].d9_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_10,
				APUSYS_NOC_DAPC_AO[i].d10_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_11,
				APUSYS_NOC_DAPC_AO[i].d11_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_12,
				APUSYS_NOC_DAPC_AO[i].d12_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_13,
				APUSYS_NOC_DAPC_AO[i].d13_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_14,
				APUSYS_NOC_DAPC_AO[i].d14_permission);
		ret += set_slave_noc_dapc(i, DOMAIN_15,
				APUSYS_NOC_DAPC_AO[i].d15_permission);
	}

	if (ret != APUSYS_APC_OK)
		ret = APUSYS_APC_ERR_GENERIC;

	return ret;
}

static int32_t set_apusys_ao_apc(void)
{
	uint32_t infra_ao_size;
	int32_t ret = APUSYS_APC_OK;
	uint32_t i;

	infra_ao_size = sizeof(APUSYS_AO_Devices) /
		sizeof(struct APC_DOM_16);

	for (i = 0; i < infra_ao_size; i++) {
		/* d0: APMCU */
		ret += set_slave_apc(i, DOMAIN_0,
				APUSYS_AO_Devices[i].d0_permission);
		ret += set_slave_apc(i, DOMAIN_1,
				APUSYS_AO_Devices[i].d1_permission);
		ret += set_slave_apc(i, DOMAIN_2,
				APUSYS_AO_Devices[i].d2_permission);
		ret += set_slave_apc(i, DOMAIN_3,
				APUSYS_AO_Devices[i].d3_permission);
		ret += set_slave_apc(i, DOMAIN_4,
				APUSYS_AO_Devices[i].d4_permission);
		ret += set_slave_apc(i, DOMAIN_5,
				APUSYS_AO_Devices[i].d5_permission);
		ret += set_slave_apc(i, DOMAIN_6,
				APUSYS_AO_Devices[i].d6_permission);
		ret += set_slave_apc(i, DOMAIN_7,
				APUSYS_AO_Devices[i].d7_permission);
		ret += set_slave_apc(i, DOMAIN_8,
				APUSYS_AO_Devices[i].d8_permission);
		ret += set_slave_apc(i, DOMAIN_9,
				APUSYS_AO_Devices[i].d9_permission);
		ret += set_slave_apc(i, DOMAIN_10,
				APUSYS_AO_Devices[i].d10_permission);
		ret += set_slave_apc(i, DOMAIN_11,
				APUSYS_AO_Devices[i].d11_permission);
		ret += set_slave_apc(i, DOMAIN_12,
				APUSYS_AO_Devices[i].d12_permission);
		ret += set_slave_apc(i, DOMAIN_13,
				APUSYS_AO_Devices[i].d13_permission);
		ret += set_slave_apc(i, DOMAIN_14,
				APUSYS_AO_Devices[i].d14_permission);
		ret += set_slave_apc(i, DOMAIN_15,
				APUSYS_AO_Devices[i].d15_permission);
	}

	if (ret != APUSYS_APC_OK)
		ret = APUSYS_APC_ERR_GENERIC;

	return ret;
}

static void set_apc_lock(void)
{
	/* Lock apu_sctrl_reviser */
	uint32_t set_bit = 0x01 << APUSYS_APC_SYS0_LOCK_BIT_APU_SCTRL_REVISER;

	set_bit = set_bit | (0x01 << APUSYS_APC_SYS0_LOCK_BIT_DEVAPC_AO_WRAPPER);
	apuapc_writel(set_bit, APUSYS_SYS0_APC_LOCK_0);
}

static int32_t set_vpu_domain_ns(uint64_t vpu_ns, uint64_t domain,
							uint64_t core_num)
{
	uint32_t domain_old;
	uint32_t domain_new;
	uint32_t ctxt_old;
	uint32_t ctxt_new;
	uint32_t i;

	/* NOTICE("%s vpu_ns=0x%llx, domain=%lld, core=%lld\n", */
	/*			__func__, vpu_ns, domain, core_num); */

	if ((vpu_ns & NS_MASK) != vpu_ns) {
		ERROR("invalid VPU NS=0x%llx\n", vpu_ns);
		return APUSYS_APC_ERR_INVALID_CMD;
	}

	if ((domain & DOMAIN_MASK) != domain) {
		ERROR("invalid VPU domain=0x%llx\n", domain);
		return APUSYS_APC_ERR_INVALID_CMD;
	}

	if ((core_num < MIN_VPU_CORE) || (core_num > MAX_VPU_CORE)) {
		ERROR("invalid VPU core=0x%llx\n", core_num);
		return APUSYS_APC_ERR_INVALID_CMD;
	}

	spin_lock(&vpu_domain_ns_lock);

	for (i = 0 ; i < core_num ; i++) {
		uint32_t offset;

		offset = (i*4)*2;
		domain_old = mmio_read_32(APUSYS_SCTRL_VPU6_CORE0_DOMAIN_NS+offset);
		domain_new = bits_set(domain_old, domain, DOMAIN_MASK, DOMAIN_OFFSET);
		domain_new = bits_set(domain_new, vpu_ns, NS_MASK, NS_OFFSET);
		mmio_write_32(APUSYS_SCTRL_VPU6_CORE0_DOMAIN_NS+offset, domain_new);

		ctxt_old = mmio_read_32(APUSYS_SCTRL_VPU6_CORE0_CTXT+offset);
		if (vpu_ns)
			ctxt_new = bits_set(ctxt_old, VPU_BDY_3, BDY_MASK, BDY_OFFSET);
		else
			ctxt_new = bits_set(ctxt_old, VPU_BDY_0, BDY_MASK, BDY_OFFSET);
		mmio_write_32(APUSYS_SCTRL_VPU6_CORE0_CTXT+offset, ctxt_new);

		/* domain_new = mmio_read_32(APUSYS_SCTRL_VPU6_CORE0_DOMAIN_NS+offset); */
		/* ctxt_new = mmio_read_32(APUSYS_SCTRL_VPU6_CORE0_CTXT+offset); */

		/* NOTICE("domain pa(0x%x) ctxt pa(0x%x)\n", */
		/*		APUSYS_SCTRL_VPU6_CORE0_DOMAIN_NS+offset, */
		/*		APUSYS_SCTRL_VPU6_CORE0_CTXT+offset); */

		/* NOTICE("domain(0x%x)->(0x%x), ctxt(0x%x)->(0x%x) vpu%u\n", */
		/*		domain_old, domain_new, ctxt_old, ctxt_new, i); */
	}

	spin_unlock(&vpu_domain_ns_lock);

	return APUSYS_APC_OK;
}

int32_t start_apusys_devapc_ao(void)
{
	int32_t ret = APUSYS_APC_OK;

	if (init_flag) {
		INFO("[APUAPC] %s already init, direct return\n", __func__);
		return ret;
	}

	/* Clear all violation status */
	apusys_devapc_init();

	/* Initial Permission */
	ret = set_apusys_ao_apc();
	INFO("[APUAPC] %s - %s!\n", "set_apusys_ao_apc",
			ret ? "FAILED" : "SUCCESS");
	if (ret)
		return ret;

	/* Dump Permission */
	dump_apusys_ao_apc();

	/* Lock */
	set_apc_lock();

	/* Initial NoC Permission */
	ret = set_apusys_noc_dapc();
	INFO("[APUAPC] %s - %s!\n", "set_apusys_noc_dapc",
			ret ? "FAILED" : "SUCCESS");
	if (ret)
		return ret;

	/* Dump Permission */
	dump_apusys_noc_dapc();

	INFO("[APUAPC] %s done\n", __func__);

	init_flag = 1;
	return ret;
}

int32_t apusys_devapc_hyp_ctrl(uint64_t x1, uint64_t x2,
						uint64_t x3, uint64_t x4)
{
	int32_t ret = APUSYS_APC_ERR_INVALID_CMD;

	if (x1 == HYP_CTRL_VPU_ACCESS_PERMISSION)
		ret = apusys_devapc_vpu_ctrl_permission_switch(x2);
	else if (x1 == HYP_CTRL_VPU_DOMAIN_NS_BOUNDARY)
		ret = apusys_devapc_sdsp_domain_switch(x2, x3, x4);
	else if (x1 == HYP_CTRL_APU_POWER_ACCESS_PERMISSION)
		ret = apusys_devapc_apu_power_ctrl_permission_switch(x2);

	return ret;
}

int32_t apusys_devapc_sdsp_domain_switch(uint64_t vpu_ns, uint64_t domain,
							uint64_t core_num)
{
	return set_vpu_domain_ns(vpu_ns, domain, core_num);
}

int32_t apusys_kernel_reviser_init_ip(void)
{
	return set_vpu_domain_ns(VPU_NORMAL_NS, VPU_NORMAL_DOMAIN, MAX_VPU);
}

int32_t apusys_devapc_vpu_ctrl_permission_switch(uint64_t to_gz)
{
	int32_t ret = APUSYS_APC_OK;
	uint32_t i = 0;
	enum APUSYS_APC_PERM_TYPE perm_d0, perm_d11, perm_d13;

	/* NOTICE("%s to_gz=0x%llx\n", __func__, to_gz); */

	if (to_gz) {
		perm_d0 = SEC_RW_NS_R;
		perm_d11 = NO_PROTECTION;
		perm_d13 = NO_PROTECTION;	/* secure vpu */
	} else {
		perm_d0 = NO_PROTECTION;
		perm_d11 = FORBIDDEN;
		perm_d13 = FORBIDDEN;		/* secure vpu */
	}

	ret += set_slave_apc(APUSYS_APC_SLAVE_VPU_CORE0_CONFIG_0, DOMAIN_0,
			perm_d0);
	ret += set_slave_apc(APUSYS_APC_SLAVE_VPU_CORE0_CONFIG_1, DOMAIN_0,
			perm_d0);
	ret += set_slave_apc(APUSYS_APC_SLAVE_VPU_CORE1_CONFIG_0, DOMAIN_0,
			perm_d0);
	ret += set_slave_apc(APUSYS_APC_SLAVE_VPU_CORE1_CONFIG_1, DOMAIN_0,
			perm_d0);
	ret += set_slave_apc(APUSYS_APC_SLAVE_VPU_CORE0_CONFIG_0, DOMAIN_11,
			perm_d11);
	ret += set_slave_apc(APUSYS_APC_SLAVE_VPU_CORE0_CONFIG_1, DOMAIN_11,
			perm_d11);
	ret += set_slave_apc(APUSYS_APC_SLAVE_VPU_CORE1_CONFIG_0, DOMAIN_11,
			perm_d11);
	ret += set_slave_apc(APUSYS_APC_SLAVE_VPU_CORE1_CONFIG_1, DOMAIN_11,
			perm_d11);

	/* for cross talk between vpuX<->vpuY */
	ret += set_slave_apc(APUSYS_APC_SLAVE_VPU_CORE0_CONFIG_0, DOMAIN_13,
			perm_d13);
	ret += set_slave_apc(APUSYS_APC_SLAVE_VPU_CORE1_CONFIG_0, DOMAIN_13,
			perm_d13);

	/* for dmem access permission */
	for (i = APUSYS_NOC_DAPC_SLV00_0 ; i <= APUSYS_NOC_DAPC_SLV01_2 ; i++) {
		ret += set_slave_noc_dapc(i, DOMAIN_0, perm_d0);
		ret += set_slave_noc_dapc(i, DOMAIN_11, perm_d11);
	}

	if (ret != APUSYS_APC_OK)
		ret = APUSYS_APC_ERR_GENERIC;

	return ret;
}

int32_t apusys_kernel_reviser_set_boundary(uint32_t mdla, uint32_t vpu,
								uint32_t edma)
{
	uint32_t i, j;
	uint32_t old_value;
	uint32_t new_value;
	uint32_t boundary;

#if MAX_MDLA
	for (i = 0 ; i < MAX_MDLA ; i++) {
		boundary = (mdla >> (i*4)) & 0xF;
		if (boundary < 0xF) {
			old_value = mmio_read_32(APUSYS_SCTRL_MDLA_CORE0_CTXT+(i*4));
			new_value = bits_set(old_value, boundary, BDY_MASK, BDY_OFFSET);
			mmio_write_32(APUSYS_SCTRL_MDLA_CORE0_CTXT+(i*4), new_value);
			/* new_value = mmio_read_32(APUSYS_SCTRL_MDLA_CORE0_CTXT+(i*4)); */
			/* NOTICE("read 0x%x from pa(0x%x)\n", new_value, */
						/* APUSYS_SCTRL_MDLA_CORE0_CTXT+(i*4)); */
		}
	}
#endif
#if MAX_VPU
	for (i = 0 ; i < MAX_VPU ; i++) {
		boundary = (vpu >> (i*4)) & 0xF;
		if (boundary < 0xF) {
			old_value = mmio_read_32(APUSYS_SCTRL_VPU6_CORE0_CTXT+(i*8));
			new_value = bits_set(old_value, boundary, BDY_MASK, BDY_OFFSET);
			mmio_write_32(APUSYS_SCTRL_VPU6_CORE0_CTXT+(i*8), new_value);
			/* new_value = mmio_read_32(APUSYS_SCTRL_VPU6_CORE0_CTXT+(i*4)); */
			/* NOTICE("read 0x%x from pa(0x%x)\n", new_value, */
						/* APUSYS_SCTRL_VPU6_CORE0_CTXT+(i*4)); */
		}
	}
#endif
#if MAX_EDMA
	for (i = 0 ; i < MAX_EDMA ; i++) {
		boundary = (edma >> (i*4)) & 0xF;
		for (j = 0; j < MAX_EDMA_CH; j++) {
			if (boundary < 0xF) {
				old_value = mmio_read_32(
						APUSYS_SCTRL_EDMA0_CTXT + (i*0x20) + (j*4));
				new_value = bits_set(old_value, boundary, BDY_MASK, BDY_OFFSET);
				mmio_write_32(APUSYS_SCTRL_EDMA0_CTXT + (i*0x20) + (j*4),
						new_value);
				/* new_value = mmio_read_32(APUSYS_SCTRL_EDMA0_CTXT+(i*4)); */
				/* NOTICE("read 0x%x from pa(0x%x)\n", new_value, */
							/* APUSYS_SCTRL_EDMA0_CTXT+(i*4)); */
			}
		}
	}
#endif
	return APUSYS_APC_OK;
}

int32_t apusys_devapc_apu_power_ctrl_permission_switch(uint64_t to_gz)
{
	int32_t ret = APUSYS_APC_OK;
	enum APUSYS_APC_PERM_TYPE perm_d0, perm_d11;

	if (to_gz) {
		perm_d0 = FORBIDDEN;
		perm_d11 = NO_PROTECTION;
	} else {
		perm_d0 = NO_PROTECTION;
		perm_d11 = FORBIDDEN;
	}

	ret += set_slave_apc(APUSYS_APC_SLAVE_APU_RPCTOP, DOMAIN_0,
			perm_d0);
	ret += set_slave_apc(APUSYS_APC_SLAVE_APU_PCUTOP, DOMAIN_0,
			perm_d0);
	ret += set_slave_apc(APUSYS_APC_SLAVE_APU_RPCTOP, DOMAIN_11,
			perm_d11);
	ret += set_slave_apc(APUSYS_APC_SLAVE_APU_PCUTOP, DOMAIN_11,
			perm_d11);

	if (ret != APUSYS_APC_OK)
		ret = APUSYS_APC_ERR_GENERIC;

	return ret;
}

int32_t apusys_kernel_set_ao_dbg_sel(uint32_t val)
{
	INFO("[APUAPC] %s %d\n", __func__, val);

	uint32_t tmp = apuapc_readl(APUSYS_AO_DBG_SEL);

	tmp = (tmp & ~(0x1)) | (val & 0x1);
	apuapc_writel(tmp, APUSYS_AO_DBG_SEL);

	return APUSYS_APC_OK;
}

int32_t apusys_kernel_reviser_chk_value(uint32_t offset, uint32_t *value)
{
	if (offset & 0x3) {
		ERROR("%s not 4 byte alignment(0x%x)\n", __func__, offset);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	if (((offset >= 0x108) && (offset <= 0x188)) ||
		((offset >= 0x300) && (offset <= 0x370)) ||
		((offset >= 0x400) && (offset <= 0x424))) {
		*value = mmio_read_32(APUSYS_SCTRL_REVISER_BASE+offset);
		/* NOTICE("read 0x%x from pa(0x%x)\n", */
		/*	*value, APUSYS_SCTRL_REVISER_BASE+offset); */
	} else {
		ERROR("%s not expected offset(0x%x)\n", __func__, offset);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	return APUSYS_APC_OK;
}

int32_t apusys_kernel_reviser_set_default_iova(uint32_t set_value)
{
	uint32_t new_value;

	if (set_value != VIRLM_MVABASE_DEFAULT_VALUE)
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	mmio_write_32(APUSYS_SCTRL_VIRLM_MVABASE, set_value);
	new_value = mmio_read_32(APUSYS_SCTRL_VIRLM_MVABASE);
	/* NOTICE("%s set_value=0x%x, after set value=0x%x\n", */
	/*	__func__, set_value, new_value); */
	if (new_value != set_value) {
		ERROR("%s set_value=0x%x, after set value=0x%x\n",
			__func__, set_value, new_value);
		return APUSYS_APC_ERR_GENERIC;
	}

	/* need write a map to trigger sync default dram addr to each local reviser */
	mmio_write_32(VLM_REMAP_TABLE_0, (0 << VLM_REMAP_VALID_OFFSET));

	INFO("[APUAPC][%s]Set remap table 0 vld = 0 (0x%x)\n", __func__,
		mmio_read_32(VLM_REMAP_TABLE_0));

	return APUSYS_APC_OK;
}

int32_t apusys_kernel_reviser_get_interrupt_status(uint32_t offset,
								uint32_t *value)
{
	if (offset & 0x3) {
		ERROR("%s not 4 byte alignment(0x%x)\n", __func__, offset);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	if ((offset >= 0x400) && (offset <= 0x420)) {
		*value = mmio_read_32(APUSYS_SCTRL_REVISER_BASE+offset);
		mmio_write_32(APUSYS_SCTRL_REVISER_BASE+offset, *value);
		/* NOTICE("read 0x%x from pa(0x%x)\n", */
		/*	*value, APUSYS_SCTRL_REVISER_BASE+offset); */
	} else {
		ERROR("%s not expected offset(0x%x)\n", __func__, offset);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	return APUSYS_APC_OK;
}


int32_t apusys_kernel_reviser_set_context_id(uint32_t offset,
						uint32_t context_id)
{
	uint32_t old_value;
	uint32_t new_value;

	if (offset & 0x3) {
		ERROR("%s not 4 byte alignment(0x%x)\n", __func__, offset);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	if (context_id > 0x1F) {
		ERROR("%s context_id(0x%x) > 0x1F\n", __func__, context_id);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	if ((offset >= 0x108) && (offset <= 0x188)) {
		old_value = mmio_read_32(APUSYS_SCTRL_REVISER_BASE+offset);
		new_value = bits_set(old_value, context_id,
							CNTX_ID_MASK, CNTX_ID_OFFSET);
		mmio_write_32(APUSYS_SCTRL_REVISER_BASE+offset, new_value);
		new_value = mmio_read_32(APUSYS_SCTRL_REVISER_BASE+offset);
		/* NOTICE("read 0x%x from pa(0x%x)\n", */
		/*	new_value, APUSYS_SCTRL_REVISER_BASE+offset); */
	} else {
		ERROR("%s not expected offset(0x%x)\n", __func__, offset);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	return APUSYS_APC_OK;
}


int32_t apusys_kernel_reviser_set_remap_table(uint32_t offset,
						uint32_t need_valid,
						uint32_t set_value)
{
	if (offset & 0x3) {
		ERROR("%s not 4 byte alignment(0x%x)\n", __func__, offset);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	if (set_value & REMAP_TABLE_VALID) {
		ERROR("%s invalid bit in b'31(0x%x)\n", __func__, set_value);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	if ((offset >= 0x304) && (offset <= 0x370)) {
		mmio_write_32(APUSYS_SCTRL_REVISER_BASE+offset, set_value);
		if (need_valid)
			mmio_write_32(APUSYS_SCTRL_REVISER_BASE+offset,
						REMAP_TABLE_VALID|set_value);
	} else {
		ERROR("%s not expected offset(0x%x)\n", __func__, offset);
		return APUSYS_APC_ERR_OUT_OF_BOUNDARY;
	}

	return APUSYS_APC_OK;
}
