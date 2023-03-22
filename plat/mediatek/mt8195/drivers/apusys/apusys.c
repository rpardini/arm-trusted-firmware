/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* TF-A system header */
#include <errno.h>
#include <common/debug.h>
#include <smccc_helpers.h>
#include <lib/mtk_init/mtk_init.h>
#include "apusys_secure_boot.h"

/* Vendor header */
#include <mtk_sip_svc.h>
#include "apusys.h"

#define LOCAL_DEBUG	(1)
#define MODULE_TAG	"[APUSYS]"

/* Weak definitions can be overridden in specific platform */
#pragma weak start_apusys_devapc_ao
#pragma weak start_apusys_devapc_rcx
#pragma weak apusys_security_ctrl_init

#pragma weak apusys_kernel_apusys_rv_setup_reviser
#pragma weak apusys_kernel_apusys_rv_reset_mp
#pragma weak apusys_kernel_apusys_rv_setup_boot
#pragma weak apusys_kernel_apusys_rv_start_mp
#pragma weak apusys_kernel_apusys_rv_stop_mp
#pragma weak apusys_rv_init
#pragma weak apusys_rv_mbox_mpu_init

#pragma weak apusys_rv_setup_secure_mem
#pragma weak apusys_rv_setup_aee_coredump_mem
#pragma weak apusys_kernel_apusys_rv_disable_wdt_isr
#pragma weak apusys_kernel_apusys_rv_clear_wdt_isr
#pragma weak apusys_kernel_apusys_rv_cg_gating
#pragma weak apusys_kernel_apusys_rv_cg_ungating
#pragma weak apusys_kernel_apusys_rv_coredump_shadow_copy
#pragma weak apusys_kernel_apusys_rv_tcmdump
#pragma weak apusys_kernel_apusys_rv_ramdump
#pragma weak apusys_kernel_apusys_rv_tbufdump
#pragma weak apusys_kernel_apusys_rv_cachedump
#pragma weak apusys_kernel_apusys_rv_dbg_apb_attach
#pragma weak apusys_kernel_apusys_rv_regdump
#pragma weak apusys_kernel_apusys_pwr_dump
#pragma weak apusys_kernel_apusys_regdump
#pragma weak apusys_kernel_apusys_pwr_rcx
#pragma weak apusys_kernel_apusys_setup_secure_mem
#pragma weak apusys_kernel_apusys_rv_load_image

int32_t start_apusys_devapc_ao(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int32_t start_apusys_devapc_rcx(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_setup_reviser(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_reset_mp(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_setup_boot(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_start_mp(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_stop_mp(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_rv_init(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_rv_mbox_mpu_init(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_rv_setup_secure_mem(uint64_t addr, uint64_t size)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_rv_setup_aee_coredump_mem(uint64_t addr, uint64_t size)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_disable_wdt_isr(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_clear_wdt_isr(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_cg_gating(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_cg_ungating(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_coredump_shadow_copy(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_tcmdump(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_ramdump(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_tbufdump(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_cachedump(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_dbg_apb_attach(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_rv_regdump(uint32_t dbg_apb_status)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_pwr_dump(uint32_t op)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_regdump(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_pwr_rcx(uint32_t op)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int32_t apusys_security_ctrl_init(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_setup_secure_mem(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int32_t apusys_kernel_apusys_rv_load_image(uint64_t res_mem_start,
			uint64_t res_mem_size, uint64_t apusys_part_size)
{
	WARN(MODULE_TAG "%s not support\n", __func__);
	return -EOPNOTSUPP;
}

u_register_t apusys_kernel_handler(u_register_t x1,
	u_register_t x2,
	u_register_t x3,
	u_register_t x4,
	struct smccc_res *smccc_ret)
{
	uint32_t request_ops;
	int32_t ret = -EIO;

#if LOCAL_DEBUG
	INFO("%s %s\n", MODULE_TAG, __func__);
#endif
	request_ops = (uint32_t)x1;

	switch (request_ops) {
	case MTK_APUSYS_KERNEL_OP_REVISER_SET_BOUNDARY:
		break;
	case MTK_APUSYS_KERNEL_OP_SET_AO_DBG_SEL:
		break;
	case MTK_APUSYS_KERNEL_OP_REVISER_SET_DEFAULT_IOVA:
		break;
	case MTK_APUSYS_KERNEL_OP_REVISER_GET_INTERRUPT_STATUS:
		break;
	case MTK_APUSYS_KERNEL_OP_REVISER_SET_CONTEXT_ID:
		break;
	case MTK_APUSYS_KERNEL_OP_REVISER_SET_REMAP_TABLE:
		break;
	case MTK_APUSYS_KERNEL_OP_REVISER_CHK_VALUE:
		break;
	case MTK_APUSYS_KERNEL_OP_DEVAPC_INIT_RCX:
		ret = start_apusys_devapc_rcx();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_SETUP_REVISER:
		ret = apusys_kernel_apusys_rv_setup_reviser();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_RESET_MP:
		ret = apusys_kernel_apusys_rv_reset_mp();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_SETUP_BOOT:
		ret = apusys_kernel_apusys_rv_setup_boot();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_START_MP:
		ret = apusys_kernel_apusys_rv_start_mp();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_STOP_MP:
		ret = apusys_kernel_apusys_rv_stop_mp();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_DISABLE_WDT_ISR:
		ret = apusys_kernel_apusys_rv_disable_wdt_isr();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_CLEAR_WDT_ISR:
		ret = apusys_kernel_apusys_rv_clear_wdt_isr();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_CG_GATING:
		ret = apusys_kernel_apusys_rv_cg_gating();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_CG_UNGATING:
		ret = apusys_kernel_apusys_rv_cg_ungating();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_COREDUMP_SHADOW_COPY:
		ret = apusys_kernel_apusys_rv_coredump_shadow_copy();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_TCMDUMP:
		ret = apusys_kernel_apusys_rv_tcmdump();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_RAMDUMP:
		ret = apusys_kernel_apusys_rv_ramdump();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_TBUFDUMP:
		ret = apusys_kernel_apusys_rv_tbufdump();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_CACHEDUMP:
		ret = apusys_kernel_apusys_rv_cachedump();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_DBG_APB_ATTACH:
		ret = apusys_kernel_apusys_rv_dbg_apb_attach();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_REGDUMP:
		ret = apusys_kernel_apusys_rv_regdump((uint32_t)x2);
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_PWR_DUMP:
		ret = apusys_kernel_apusys_pwr_dump((uint32_t)x2);
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_REGDUMP:
		ret = apusys_kernel_apusys_regdump();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_PWR_RCX:
		ret = apusys_kernel_apusys_pwr_rcx((uint32_t)x2);
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_SETUP_SECURE_MEM:
		ret = apusys_kernel_apusys_setup_secure_mem();
		break;
	case MTK_APUSYS_KERNEL_OP_APUSYS_RV_LOAD_IMAGE:
		ret = apusys_kernel_apusys_rv_load_image((uint64_t)x2,
						(uint64_t)x3, (uint64_t)x4);
		break;
	default:
		ERROR("%s unknown request_ops = %x\n", MODULE_TAG, request_ops);
		break;
	}

	return ret;
}

/* Register SiP SMC service */
DECLARE_SMC_HANDLER(MTK_SIP_APUSYS_CONTROL, apusys_kernel_handler);

/*
 * apusys_init() - Do apu init flow.
 * Hook this func to MTK_PLAT_SETUP_1_INIT stage to make sure apu init will be
 * called after i2c & pmic drv init done.
 *
 * 1. apusys_rv_init(): init apu atf resource
 * 2. apusys_devapc_init(): init apu devapc permission setting
 * 3. apusys_rv_mbox_mpu_init(): setup up mbox mpu
 */
int apusys_init(void)
{
#if LOCAL_DEBUG
	NOTICE("%s %s +\n", MODULE_TAG, __func__);
#endif

	apusys_rv_init();
	start_apusys_devapc_ao();
	apusys_rv_mbox_mpu_init();

#if LOCAL_DEBUG
	NOTICE("%s %s -\n", MODULE_TAG, __func__);
#endif
	return 0;
}

MTK_PLAT_SETUP_1_INIT(apusys_init);
