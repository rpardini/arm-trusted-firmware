/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IOMMU_PRIV_H
#define IOMMU_PRIV_H

#include <common/debug.h>
#include <lib/mmio.h>
#include <mtk_sip_svc.h>

#define F_BIT(bit)			(1UL << (bit))
#define F_MSK(hi, lo)			((F_BIT((hi) - (lo) + 1) - 1) << (lo))
#define F_MSK_SHIFT(regval, msb, lsb)	(((regval) & F_MSK(msb, lsb)) >> lsb)

#define GET_HIGH(val)			\
	(uint32_t)(((((uint64_t)(val)) & 0xffffffff00000000ULL)) >> 32)
#define GET_LOW(val)			\
	(uint32_t)((((uint64_t)(val)) & 0x00000000ffffffffULL))

#define LARB_CFG_ENTRY(bs, p_nr, dom)			\
	{ .base = (bs), .port_nr = (p_nr),		\
	  .dom_id = (dom), .to_sram = 0, }

#define LARB_CFG_ENTRY_WITH_PATH(bs, p_nr, dom, sram)	\
	{ .base = (bs), .port_nr = (p_nr),		\
	  .dom_id = (dom), .to_sram = (sram), }

#define IFR_MST_CFG_ENTRY(idx, bit)	\
	{ .cfg_addr_idx = (idx), .r_mmu_en_bit = (bit), }

#define IOMMU_DATA_ENTRY(bs, bk)	\
	{ .base = (bs), .bank_num = (bk), }

#define IOMMU_BANK_BASE(pdata, bank_id)	\
	((pdata)->base + 0x1000 * (bank_id))

#define SEC_IOMMU_CFG_ENTRY(s_bs)	\
	{ .base = (s_bs), }

enum IOMMU_ATF_CMD {
	IOMMU_ATF_CMD_SECURE_IOMMU_STATUS,	/* For secure iommu translation fault report */
	IOMMU_ATF_CMD_SECURE_IOMMU_SUSPEND,	/* For secure iommu registers back up */
	IOMMU_ATF_CMD_SECURE_IOMMU_RESUME,	/* For secure iommu registers restore */
	IOMMU_ATF_CMD_CONFIG_SMI_LARB,		/* For mm master to enable iommu */
	IOMMU_ATF_CMD_CONFIG_INFRA_IOMMU,	/* For infra master to enable iommu */
	IOMMU_ATF_CMD_COUNT,
};

struct mtk_smi_larb_config {
	uint32_t base;
	uint32_t port_nr;
	uint32_t dom_id;
	uint32_t to_sram;
	uint32_t sec_en_msk;
};

struct mtk_ifr_mst_config {
	uint8_t cfg_addr_idx;
	uint8_t r_mmu_en_bit;
};

struct mtk_iommu_data {
	uint32_t	base;		/* bank 0 base */
	uint32_t	bank_num;
};

struct mtk_secure_iommu_config {
	uint32_t base;
	uint32_t pt_base;
	uint32_t trap_paddr;
};

/**** IOMMU PAGETABLE MAPPING ****/

struct iommu_pgtable {
	uint64_t	pgd_pa;	/* = 0: as secmem_query_m4u()
				 * > 0: stable for reserved memory
				 *      (need to sync with master owners for
				 *      protection)
				 */
	uint32_t	*pgd_base;
	uint32_t	pgd_size; /* fixed */
	uint32_t	*mmu_bank_msk;	/* fixed: used by multi-iommu banks
					 * IOMMU_BANK_PGT_SET(mmu_id, bk_id)
					 */
	uint32_t	mmu_bank_nr;	/* fixed */
	int		inited;
};

#define IOMMU_PGT_BANK_INFO_SET(mmu_id, bk_msk)	(((mmu_id) << 8) | (bk_msk))
#define IOMMU_PGT_BANK_INFO_GET_MMU_ID(msk)	((msk) >> 8)
#define IOMMU_PGT_BANK_INFO_GET_BANK_ID(msk)	((msk) & 0xff)

struct iommu_reserved_mem {
	uint32_t	type;	/* fixed: IOMMU_RESV_MEM_TYPE* */
	uint32_t	pgt_msk;/* fixed: do mapping for multi-pagetable */
	uint64_t	iova;	/* fixed: need to be reserved at kernel */
	uint64_t	pa;	/* configured by mtk_iommu_linear_secure_map() */
	uint64_t	size;	/* configured by mtk_iommu_linear_secure_map() */
	uint64_t	max_size; /* fixed: need to be reserved at kernel */
};

/* WITH_PGTBL: pagetable address is determined by master's secmem region
 * (aligned mem_pa+mem_sz) with the direct-remaped cpu va
 */
#define IOMMU_RESV_MEM_TYPE_WITH_PGTBL		F_BIT(31)
#define IOMMU_RESV_MEM_TYPE_MASK		F_MSK(15, 0)
#define IOMMU_RESV_MEM_TYPE(mod, mem)		(((mod) << 8) | (mem))
#define IOMMU_GET_MEM_TYPE(type)		((type) & 0xff)
#define IOMMU_GET_MOD_ID(type)			(((type) >> 8) & 0xff)

/**** LOG DUMP ****/

#if IMU_LOG_DBG_EN
#define DEBUG_LOG(...)			INFO(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif


#include <mtk_iommu_plat.h>

#ifdef ATF_MTK_SMI_LARB_CFG_SUPPORT
/* mm smi larb security feature is used */
extern struct mtk_smi_larb_config *g_larb_cfg;
extern const unsigned int g_larb_num;
#endif

#ifdef ATF_MTK_INFRA_MASTER_CFG_SUPPORT
/* infra iommu is used */
extern struct mtk_ifr_mst_config *g_ifr_mst_cfg;
extern const unsigned int g_ifr_mst_num;
extern uint32_t *g_ifr_mst_cfg_base;
extern uint32_t *g_ifr_mst_cfg_offs;
extern void mtk_infra_iommu_enable_protect(void);
#endif

#if defined(ATF_MTK_IOMMU_FAULT_REPORT_SUPPORT) ||	\
	defined(ATF_MTK_IOMMU_RUNTIME_RS_SUPPORT)
/* secure iommu is used */
extern struct mtk_secure_iommu_config *g_sec_iommu_cfg;
extern const unsigned int g_sec_iommu_num;
#endif

#ifdef ATF_MTK_IOMMU_PTBL_MAPPING_SUPPORT
/* linear mapping is used */
extern struct mtk_iommu_data *g_mmu_data;
extern const unsigned int g_mmu_data_num;
extern struct iommu_pgtable *g_mmu_pgt;
extern const unsigned int g_mmu_pgt_num;
extern struct iommu_reserved_mem *g_mmu_resv_mem;
extern const unsigned int g_mmu_resv_mem_num;
#endif

#endif	/* IOMMU_PRIV_H */
