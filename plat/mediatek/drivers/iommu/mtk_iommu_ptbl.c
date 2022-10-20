/* SPDX-License-Identifier: MediaTekProprietary AND BSD-3-Clause */
/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mtk_iommu_plat.h>

#ifdef IMU_PTBL_MAPPING_SUPPORT

#include <arch_helpers.h>
#include <lib/libc/errno.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <string.h>

/* iommu register */
#define MMU_PT_BASE_ADDR		(0x000)
#define F_PT_BASE_ADDR(pa)		\
	((((uint32_t)(pa)) & 0xffff0000) | (GET_HIGH(pa) & (0x7)))
#define MMU_TFRP_PADDR			(0x114)
#define F_TFRP_PADDR			(0x7)
#define MMU_INT_CONTROL0		(0x120)
#define F_INT_CLR_BIT			(0x1 << 12)
#define F_INT_CTRL0_MSK			(0x6f)
#define MMU_INT_CONTROL1		(0x124)
#define F_REG_MMU_INT_MASK		(0x3fff)

/* pagetable defination */
#define MMU_PGD_SHIFT			20
#define MMU_PGD_ENTRY_SIZE		0x100000
#define MMU_PGD_ALIGN_DOWN(_val)	\
	(((_val) >> MMU_PGD_SHIFT) << MMU_PGD_SHIFT)
#define MMU_PGD_ALIGN_UP(val)		\
	MMU_PGD_ALIGN_DOWN((val) + (MMU_PGD_ENTRY_SIZE - 1))

#define mmu_pgd_index(iova)		((uint32_t)((iova) >> MMU_PGD_SHIFT))

/* PGD */
#define F_PGD_TYPE_SECTION		(0x2)
#define F_PGD_NS_BIT_SECTION(ns)	((!!(ns)) << 19)
#define F_PGD_PA_SECTION_MSK		F_MSK(31, 20)
#define F_PGD_TBL_ADDR_MSK		(0xffffULL)

/* section */
#define F_PTBL_BIT_34_32_SET(pa_high)	\
	(uint32_t)((F_MSK_SHIFT(pa_high, 2, 1) << 4) | (((pa_high) & 0x1) << 9))
#define F_PGD_PA_SECTION_SET(pa)				\
	((uint32_t)((GET_LOW(pa) & F_PGD_PA_SECTION_MSK) |	\
	F_PTBL_BIT_34_32_SET(GET_HIGH(pa))))

static int mtk_iommu_initial_banks_hw(struct iommu_pgtable *pgt)
{
	uint32_t bank_msk, bank_base;
	unsigned int i, nr, mmu_id;

	if (!pgt->mmu_bank_msk) {
		ERROR("[iommu_ptbl] %s: fail (pgd 0x%llx|0x%x)\n",
		      __func__, pgt->pgd_pa, pgt->pgd_size);
		return -EFAULT;
	}

	nr = pgt->mmu_bank_nr;
	for (i = 0; i < nr; i++) {
		bank_msk = pgt->mmu_bank_msk[i];
		mmu_id = IOMMU_PGT_BANK_INFO_GET_MMU_ID(bank_msk);
		if (mmu_id >= APU_IOMMU_NUM) {
			ERROR("[iommu_ptbl] %s: invalid iommu_id at %u/%u\n",
			      __func__, i, nr);
			return -EFAULT;
		}
		bank_base = IOMMU_BANK_BASE(&g_mmu_data[mmu_id],
				IOMMU_PGT_BANK_INFO_GET_BANK_ID(bank_msk));

		mmio_write_32(bank_base + MMU_PT_BASE_ADDR,
			     F_PT_BASE_ADDR(pgt->pgd_pa));
		mmio_write_32(bank_base + MMU_TFRP_PADDR, F_TFRP_PADDR);
		mmio_write_32(bank_base + MMU_INT_CONTROL0,
			      (F_INT_CLR_BIT | F_INT_CTRL0_MSK));
		mmio_write_32(bank_base + MMU_INT_CONTROL1, F_REG_MMU_INT_MASK);

		DEBUG_LOG("[iommu_ptbl] iommu.%x is inited (pgt 0x%x)\n",
			  bank_base, mmio_read_32(bank_base + MMU_PT_BASE_ADDR));
	}

	return 0;
}

static int mtk_iommu_initial_pagetable(struct iommu_pgtable *pgt,
				       uint64_t pgt_mem_pa)
{
	int ret = 0;

	if (pgt->inited)
		return 0;

	if (!pgt->pgd_pa) {
		if (pgt_mem_pa)
			pgt->pgd_pa = pgt_mem_pa;
		else
			return -EINVAL;

	} else {
		ret = mmap_add_dynamic_region((unsigned long long)pgt->pgd_pa,
				(uintptr_t)pgt->pgd_pa,
				(size_t)pgt->pgd_size,
				(unsigned int)MT_MEMORY | MT_RW | MT_SECURE);
		if (ret) {
			ERROR("[iommu_ptbl] %s: mmap fail %d\n", __func__, ret);
			return ret;
		}
	}

	pgt->pgd_base = (uint32_t *)pgt->pgd_pa;
	memset((void *)pgt->pgd_base, 0, pgt->pgd_size);

	ret = mtk_iommu_initial_banks_hw(pgt);
	if (ret)
		return ret;

	pgt->inited = 1;

	DEBUG_LOG("[iommu_ptbl] pgd(0x%llx|0x%x) is inited\n",
		  pgt->pgd_pa, pgt->pgd_size);

	return 0;
}

static void mtk_iommu_map_reserved_mem(struct iommu_reserved_mem *resv_mem,
				       struct iommu_pgtable *pgt)
{
	uint32_t pgd_dsc_msk, *pgd_entry;
	uint64_t iova = resv_mem->iova, pa = resv_mem->pa, mapped_size = 0ULL;
	uint32_t is_ns = (IOMMU_GET_MEM_TYPE(resv_mem->type) == SECURE_MEM) ?
			 0 : 1;

	pgd_dsc_msk = F_PGD_NS_BIT_SECTION(is_ns) | F_PGD_TYPE_SECTION;
	pgd_entry = pgt->pgd_base + mmu_pgd_index(iova);

	while (mapped_size < resv_mem->size) {
		*pgd_entry = (pgd_dsc_msk | F_PGD_PA_SECTION_SET(pa));

		mapped_size += MMU_PGD_ENTRY_SIZE;
		iova += MMU_PGD_ENTRY_SIZE;
		pa += MMU_PGD_ENTRY_SIZE;
		pgd_entry++;
	}

	DEBUG_LOG("[iommu_ptbl] map 0x%x|0x%llx:0x%llx|0x%llx for pgd 0x%llx\n",
		  resv_mem->type, resv_mem->iova,
		  resv_mem->pa, mapped_size, pgt->pgd_pa);

	flush_dcache_range((unsigned long)pgt->pgd_base, pgt->pgd_size);
}

uint64_t mtk_iommu_linear_secure_map(uint64_t mem_pa, uint64_t mem_size,
				     uint32_t mem_type, uint32_t mod_id)
{
	uint32_t type = IOMMU_RESV_MEM_TYPE(mod_id, mem_type);
	struct iommu_reserved_mem *resv_mem = NULL;
	uint64_t mem_sta_pa =  MMU_PGD_ALIGN_DOWN(mem_pa);
	uint64_t mem_end_pa =  MMU_PGD_ALIGN_UP(mem_pa + mem_size);
	uint32_t idx = 0;
	struct iommu_pgtable *pgt = NULL;

	if ((mem_end_pa - mem_sta_pa) != mem_size)
		goto map_fail;

	for (idx = 0; idx < IOMMU_RESV_MEM_NR; idx++) {
		if (type != (g_mmu_resv_mem[idx].type & IOMMU_RESV_MEM_TYPE_MASK))
			continue;

		if ((g_mmu_resv_mem[idx].pa != 0) ||
		    (g_mmu_resv_mem[idx].max_size < mem_size)) {
			ERROR("[iommu_ptbl] %s: conflict 0x%llx|0x%llx:0x%llx\n",
			      __func__, g_mmu_resv_mem[idx].pa,
			      g_mmu_resv_mem[idx].size,
			      g_mmu_resv_mem[idx].max_size);
			goto map_fail;
		}

		resv_mem = &g_mmu_resv_mem[idx];
		resv_mem->pa = mem_sta_pa;
		resv_mem->size = mem_size;
		break;
	}

	if (!resv_mem) {
		ERROR("[iommu_ptbl] %s: no type 0x%x\n",
		      __func__, type);
		goto map_fail;
	}

	if (!(resv_mem->type & IOMMU_RESV_MEM_TYPE_WITH_PGTBL))
		mem_end_pa = 0ULL;

	for (idx = 0; idx < IOMMU_PGT_TYPE_NR; idx++) {
		if (!(F_BIT(idx) & resv_mem->pgt_msk))
			continue;

		pgt = &g_mmu_pgt[idx];

		if (mtk_iommu_initial_pagetable(pgt, mem_end_pa))
			goto map_fail;

		mtk_iommu_map_reserved_mem(resv_mem, pgt);
	}

	return resv_mem->iova;

map_fail:
	ERROR("[iommu_ptbl] %s: map %u(0x%llx|0x%llx-%u:%u) fail\n",
	      __func__, idx, mem_pa, mem_size, mem_type, mod_id);

	return 0ULL;
}

#else	/* !defined(IMU_PTBL_MAPPING_SUPPORT) */

uint64_t mtk_iommu_linear_secure_map(uint64_t mem_pa, uint64_t mem_size,
				     uint32_t mem_type, uint32_t mod_id)
{
	ERROR("[iommu_ptbl] %s: not supported\n", __func__);

	return 0ULL;
}

#endif	/* IMU_PTBL_MAPPING_SUPPORT */

