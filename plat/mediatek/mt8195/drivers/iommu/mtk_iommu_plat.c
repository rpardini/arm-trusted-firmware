/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mtk_iommu_priv.h>
#include <platform_def.h>

/* secure iommu */
static struct mtk_secure_iommu_config mt8195_secure_iommu_config[] = {
	SEC_IOMMU_CFG_ENTRY(SEC_APU_IOMMU_0_BASE),
	SEC_IOMMU_CFG_ENTRY(SEC_APU_IOMMU_1_BASE),
};

struct mtk_secure_iommu_config *g_sec_iommu_cfg = &mt8195_secure_iommu_config[0];
const unsigned int g_sec_iommu_num = ARRAY_SIZE(mt8195_secure_iommu_config);

/**** iommu mapping ****/

#ifdef ATF_MTK_IOMMU_PTBL_MAPPING_SUPPORT

#include <mtk_iommu_public.h>

#define APU_IOMMU_0_ID		0
#define APU_IOMMU_1_ID		1

enum iommu_pgt_type {
	IOMMU_PGT_APU_SECURE		= 0,
	IOMMU_PGT_TYPE_NR,
};

static struct mtk_iommu_data mt8195_mmu_data[] = {
	[APU_IOMMU_0_ID] = IOMMU_DATA_ENTRY(APU_IOMMU_0_BASE, 5),
	[APU_IOMMU_1_ID] = IOMMU_DATA_ENTRY(APU_IOMMU_1_BASE, 5),
};

struct mtk_iommu_data *g_mmu_data = &mt8195_mmu_data[0];
const unsigned int g_mmu_data_num = ARRAY_SIZE(mt8195_mmu_data);

static uint32_t iommu_pgt_bank_info[] = {
	/* use IOMMU_PGT_APU_SECURE */
	IOMMU_PGT_BANK_INFO_SET(APU_IOMMU_0_ID, 4),
	IOMMU_PGT_BANK_INFO_SET(APU_IOMMU_1_ID, 4),
};

static struct iommu_pgtable iommu_pgt[] = {
	[IOMMU_PGT_APU_SECURE] = {
		.pgd_size	= 0x4000,
		.mmu_bank_msk	= &iommu_pgt_bank_info[0],
		.mmu_bank_nr	= ARRAY_SIZE(iommu_pgt_bank_info),
	},
};
struct iommu_pgtable *g_mmu_pgt = &iommu_pgt[0];
const unsigned int g_mmu_pgt_num = ARRAY_SIZE(iommu_pgt);

static struct iommu_reserved_mem iommu_resv_mem[] = {
	{	.type		= IOMMU_RESV_MEM_TYPE(MOD_APU_FW, SECURE_MEM) |
				  IOMMU_RESV_MEM_TYPE_WITH_PGTBL,
		.iova		= 0x0,
		.max_size	= 0x20000000,
		.pgt_msk	= F_BIT(IOMMU_PGT_APU_SECURE),
	},
};
struct iommu_reserved_mem *g_mmu_resv_mem = &iommu_resv_mem[0];
const unsigned int g_mmu_resv_mem_num = ARRAY_SIZE(iommu_pgt);

#endif
