/*
 * Copyright (c) 2023, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mtk_sip_svc.h>

enum PLAT_BINFO {
	PLAT_BINFO_PART = 0,
};

static u_register_t mtk_binfo_handler(u_register_t arg0, u_register_t arg1,
					u_register_t arg2, u_register_t arg3,
					void *handle, struct smccc_res *res)
{
	int ret = MTK_SIP_E_SUCCESS;
	uint32_t ret_val;

	switch (arg0) {
		case PLAT_BINFO_PART:
			ret = mtk_plat_get_partid(&ret_val);
			res->a1 = ret_val;
			break;
		default:
			ret = MTK_SIP_E_INVALID_PARAM;
			break;
	}
	return ret;
}
DECLARE_SMC_HANDLER(MTK_SIP_PARTNAME_ID, mtk_binfo_handler);
