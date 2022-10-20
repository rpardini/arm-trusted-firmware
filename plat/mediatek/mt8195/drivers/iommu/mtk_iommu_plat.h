/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IOMMU_PLAT_H
#define IOMMU_PLAT_H

#define IMU_LOG_DBG_EN			0
#define IMU_PTBL_MAPPING_SUPPORT

#include <mtk_iommu_priv.h>

/**** iommu secure pagetable mapping ****/

#ifdef IMU_PTBL_MAPPING_SUPPORT

#include <mtk_iommu_public.h>

enum iommu_pgt_type {
	IOMMU_PGT_APU_SECURE		= 0,
	IOMMU_PGT_TYPE_NR,
};

#define APU_IOMMU_NUM			(2)
#define IOMMU_RESV_MEM_NR		(1)

extern struct mtk_iommu_data *g_mmu_data;
extern struct iommu_pgtable *g_mmu_pgt;
extern struct iommu_reserved_mem *g_mmu_resv_mem;

#endif

#endif /* IOMMU_PLAT_H */
