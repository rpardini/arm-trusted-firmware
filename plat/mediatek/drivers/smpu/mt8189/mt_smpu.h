/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#ifndef MT_SMPU_H
#define MT_SMPU_H

#define NEMI_SMPU_BASE                          (0x10351000)
#define NTH_EMICFG_AO_MEM_BASE                  (0x10270000)

#define SECURE_REGION_SA_BASE                   (NEMI_SMPU_BASE + 0x0)
#define SECURE_REGION_SA(region)                (SECURE_REGION_SA_BASE + (region - 1) * 0x8)
#define SECURE_REGION_EA_BASE                   (NEMI_SMPU_BASE + 0x4)
#define SECURE_REGION_EA(region)                (SECURE_REGION_EA_BASE + (region - 1) * 0x8)
#define SECURE_REGION_ENABLE0                   (NEMI_SMPU_BASE + 0x2a4)
#define SECURE_REGION_ENABLE(region_set)        (SECURE_REGION_ENABLE0 + region_set * 0x4)

#define NEMI_AID_PERM_TABEL_IDX                 (NEMI_SMPU_BASE + 0x9bc)
#define NEMI_AID_PERM_TABEL_NON_SEC             (NEMI_SMPU_BASE + 0x9c0)
#define NEMI_AID_PERM_TABEL_SEC0                (NEMI_SMPU_BASE + 0x9c4)
#define NEMI_AID_PERM_TABEL_SEC(num)            (NEMI_AID_PERM_TABEL_SEC0 + (num * 4))
#define NEMI_AID_PERM_TABEL_TRIG_WRITE          (NEMI_SMPU_BASE + 0x800)
#define NEMI_READ_AID_PERM_TABEL_NON_SEC0       (NEMI_SMPU_BASE + 0x9e0)
#define NEMI_READ_AID_PERM_TABEL_SEC0           (NEMI_SMPU_BASE + 0x9e4)
#define NEMI_READ_AID_PERM_TABEL_SEC(num)       (NEMI_READ_AID_PERM_TABEL_SEC0 + ((num) * 4))
#define NEMI_MD_IRQ_MONITOR                     (NEMI_SMPU_BASE + 0xf40)
#define NEMI_MD_IRQ_ENABLE                      (NEMI_SMPU_BASE + 0x2a0)
#define NEMI_ERROR_CASE_EN                      (NEMI_SMPU_BASE + 0xf70)
#define NEMI_MASK_IRQ_WRITE                     (NEMI_SMPU_BASE + 0xe40)
#define NEMI_MASK_IRQ_READ                      (NEMI_SMPU_BASE + 0xec0)
#define NEMI_SEC_LOG_W                          (NEMI_SMPU_BASE + 0xe00)
#define NEMI_SEC_LOG_R                          (NEMI_SMPU_BASE + 0xe80)
#define NEMI_IOMMU_MPU_ENABLE                   (NEMI_SMPU_BASE + 0x6a4)
#define NEMI_SMPU_HRE_W1S_XRST                  (NTH_EMICFG_AO_MEM_BASE + 0x818)
#define NEMI_SMPU_HRE_W1C_XRST                  (NTH_EMICFG_AO_MEM_BASE + 0x81c)
#define INFRA_SECU_AO_RG_CPU                    (DEVAPC_INFRA_SECU_AO_BASE + 0x640)
#define INFRA_SECU_AO_RG_GCPU                   (DEVAPC_INFRA_SECU_AO_BASE + 0x648)

#define ALIGN_BIT                               12
#define ADDR_BIT                                32
#define ADDR_MASK                               (0x7fffff)
#define DRAM_OFFSET                             (0x40000000 >> ALIGN_BIT)
#define REGION_ENABLE                           (1UL << 31)
#define MPU_REGION_NUMBER_PER_SET               ADDR_BIT
#define MPU_REGION_BIT                          16
#define REGION_MAX                              63
#define NSR                                     1
#define AID_NUM_MAX                             256
#define SMPU_REGION_NUM                         (REGION_MAX + NSR)
#define FORBIDDEN                               0
#define WRITE                                   1
#define READ                                    2
#define RW                                      3

#endif
