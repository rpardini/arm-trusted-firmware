/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IOMMU_PLAT_H
#define IOMMU_PLAT_H

#define IMU_LOG_DBG_EN			1
#define IMU_PTBL_MAPPING_SUPPORT

#include <mtk_iommu_priv.h>

/* mm iommu */
#define SMI_LARB_NUM	(26)
extern struct mtk_smi_larb_config *g_larb_cfg;

/* infra iommu */
#define MMU_DEV_NUM	(1)
extern struct mtk_ifr_mst_config *g_ifr_mst_cfg;
extern uint32_t *g_ifr_mst_cfg_base;
extern uint32_t *g_ifr_mst_cfg_offs;

extern void mtk_infra_iommu_enable_protect(void);

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
