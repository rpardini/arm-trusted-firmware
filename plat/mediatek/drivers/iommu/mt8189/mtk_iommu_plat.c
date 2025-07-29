// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2025 MediaTek Inc.
 *
 */
#include <mtk_iommu_priv.h>
#include <mtk_mmap_pool.h>
#include <platform_def.h>

/* mm iommu */
#define SMI_L0_ID		(0)
#define SMI_L1_ID		(1)
#define SMI_L2_ID		(2)
#define SMI_L4_ID		(3)
#define SMI_L7_ID		(4)
#define SMI_L9_ID		(5)
#define SMI_L11_ID		(6)
#define SMI_L13_ID		(7)
#define SMI_L14_ID		(8)
#define SMI_L16_ID		(9)
#define SMI_L17_ID		(10)
#define SMI_L19_ID		(11)
#define SMI_L20_ID		(12)

/* infra iommu */
#define PERICFG_AO_IOMMU_0	(0x90)
#define PERICFG_AO_IOMMU_1	(0x94)
#define MMU_DEV_PCIE_0		(0)
#define IFR_CFG_GROUP_NUM	(1)

static struct mtk_smi_larb_config mt8189_larb_cfg[] = {
	[SMI_L0_ID] = LARB_CFG_ENTRY(SMI_LARB_0_BASE, 8, 0),
	[SMI_L1_ID] = LARB_CFG_ENTRY(SMI_LARB_1_BASE, 8, 0),
	[SMI_L2_ID] = LARB_CFG_ENTRY(SMI_LARB_2_BASE, 11, 0),
	[SMI_L4_ID] = LARB_CFG_ENTRY(SMI_LARB_4_BASE, 12, 0),
	[SMI_L7_ID] = LARB_CFG_ENTRY(SMI_LARB_7_BASE, 18, 0),
	[SMI_L9_ID] = LARB_CFG_ENTRY(SMI_LARB_9_BASE, 29, 0),
	[SMI_L11_ID] = LARB_CFG_ENTRY(SMI_LARB_11_BASE, 29, 0),
	[SMI_L13_ID] = LARB_CFG_ENTRY(SMI_LARB_13_BASE, 15, 0),
	[SMI_L14_ID] = LARB_CFG_ENTRY(SMI_LARB_14_BASE, 10, 0),
	[SMI_L16_ID] = LARB_CFG_ENTRY(SMI_LARB_16_BASE, 17, 0),
	[SMI_L17_ID] = LARB_CFG_ENTRY(SMI_LARB_17_BASE, 17, 0),
	[SMI_L19_ID] = LARB_CFG_ENTRY(SMI_LARB_19_BASE, 4, 0),
	[SMI_L20_ID] = LARB_CFG_ENTRY(SMI_LARB_20_BASE, 6, 0),
};

static uint32_t mt8189_ifr_mst_cfg_base[IFR_CFG_GROUP_NUM] = {
	PERICFG_AO_BASE,
};
static uint32_t mt8189_ifr_mst_cfg_offs[IFR_CFG_GROUP_NUM] = {
	PERICFG_AO_IOMMU_1,
};
static struct mtk_ifr_mst_config mt8189_ifr_mst_cfg[] = {
	[MMU_DEV_PCIE_0] = IFR_MST_CFG_ENTRY(0, 0),
};

struct mtk_smi_larb_config *g_larb_cfg = &mt8189_larb_cfg[0];
const unsigned int g_larb_num = ARRAY_SIZE(mt8189_larb_cfg);

static struct mtk_secure_iommu_config mt8189_secure_iommu_config[] = {
	SEC_IOMMU_CFG_ENTRY(MM_IOMMU_BASE),
};

struct mtk_secure_iommu_config *g_sec_iommu_cfg = &mt8189_secure_iommu_config[0];
const unsigned int g_sec_iommu_num = ARRAY_SIZE(mt8189_secure_iommu_config);

struct mtk_ifr_mst_config *g_ifr_mst_cfg = &mt8189_ifr_mst_cfg[0];
const unsigned int g_ifr_mst_num = ARRAY_SIZE(mt8189_ifr_mst_cfg);

uint32_t *g_ifr_mst_cfg_base = &mt8189_ifr_mst_cfg_base[0];
uint32_t *g_ifr_mst_cfg_offs = &mt8189_ifr_mst_cfg_offs[0];

/**
 * Protect infra iommu enable setting registers as secure access.
 * This is removed in MT8189, just return here.
 */
void mtk_infra_iommu_enable_protect(void)
{
}

#ifdef ATF_MTK_IOMMU_PTBL_MAPPING_SUPPORT

#include <mtk_iommu_public.h>

#define APU_IOMMU_ID		0

enum iommu_pgt_type {
	IOMMU_PGT_APU_SECURE		= 0,
	IOMMU_PGT_TYPE_NR,
};

static struct mtk_iommu_data mt8189_mmu_data[] = {
	[APU_IOMMU_ID] = IOMMU_DATA_ENTRY(APU_IOMMU_BASE, 5),
};

struct mtk_iommu_data *g_mmu_data = &mt8189_mmu_data[0];
const unsigned int g_mmu_data_num = ARRAY_SIZE(mt8189_mmu_data);

static uint32_t iommu_pgt_bank_info[] = {
	/* use IOMMU_PGT_APU_SECURE */
	IOMMU_PGT_BANK_INFO_SET(APU_IOMMU_ID, 4),
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
		.iova		= 0x200000,
		.max_size	= 0x20000000,
		.pgt_msk	= F_BIT(IOMMU_PGT_APU_SECURE),
	},
};
struct iommu_reserved_mem *g_mmu_resv_mem = &iommu_resv_mem[0];
const unsigned int g_mmu_resv_mem_num = ARRAY_SIZE(iommu_pgt);

#endif
