/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 MediaTek Inc.
 */

#ifndef IOMMU_PLAT_H
#define IOMMU_PLAT_H
#define IOMMU_PLAT_H

/* mm iommu */
#define ATF_MTK_SMI_LARB_CFG_SUPPORT

/* mm iommu, sec bank dump */
#define ATF_MTK_IOMMU_CFG_SUPPORT

/* infra iommu */
#define ATF_MTK_INFRA_MASTER_CFG_SUPPORT

/* iommu secure pagetable mapping */
#define ATF_MTK_IOMMU_PTBL_MAPPING_SUPPORT

/* iommu sr info support */
#define ATF_MTK_IOMMU_SR_INFO_SUPPORT

#ifdef ATF_MTK_IOMMU_SR_INFO_SUPPORT

#define REG_MMU_TEE_SRINFO              (0xf10)
#define F_MMU_SRINFO_VAL                0x1

/*
 * IOMMU will get SR in descriptor
 * for page: sr[4:2] ---> dsc[8:6]  sr[1:0] ---> dsc[3:2]
 * for section: sr[4:2] ---> dsc[14:12]  sr[1:0] ---> dsc[3:2]
 * mt8189 only support SR_Info: 1 for smpu region based protect
 * so we only set descriptor's bit[2]
 */
#define MT8189_SR_INFO 0x1
#define SR_INFO_LOW_SHIFT 2
#define SEC_SR_INFO (MT8189_SR_INFO << SR_INFO_LOW_SHIFT)

#endif /* ATF_MTK_IOMMU_SR_INFO_SUPPORT */

#endif /* IOMMU_PLAT_H */
