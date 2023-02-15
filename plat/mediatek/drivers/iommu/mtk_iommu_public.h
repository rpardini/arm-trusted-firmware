/*
 * Copyright (c) 2021, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MTK_IOMMU_PUBLIC_H
#define MTK_IOMMU_PUBLIC_H

#include <mtk_sip_svc.h>

/******************************************************************************
 * STRUCTURE DEFINITION
 ******************************************************************************/
enum mod_id {
	MOD_CCU = 0,
	MOD_VIDEO_UP,
	MOD_GCE,
	MOD_GZ_CMDQ,
	MOD_APU_FW,
	MOD_APU_SHARE,
	MOD_DSP_FW,
	MOD_DSP_DATA,
	MOD_NUM
};

enum mem_type {
	PROTECT_MEM = 0,
	SECURE_MEM,
	MEM_NUM
};

/******************************************************************************
 * FUNCTION DEFINITION
 ******************************************************************************/
uint64_t mtk_iommu_linear_secure_map(uint64_t mem_pa, uint64_t mem_size,
		uint32_t mem_type, uint32_t mod_id);


u_register_t mtk_iommu_handler(u_register_t x1, u_register_t x2,
			       u_register_t x3, u_register_t x4,
			       void *handle, struct smccc_res *smccc_ret);
#endif /* MTK_IOMMU_PUBLIC_H */
