/*
 * Copyright (c) 2022, BayLibre SAS
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MT8168_EMI_MPU_H
#define MT8168_EMI_MPU_H

#include <common/bl_common.h>
#include <lib/utils_def.h>

#define EMI_MPU_BASE			(0x10226000)

#define EMI_MPU_SA0			(EMI_MPU_BASE + 0x100)
#define EMI_MPU_EA0			(EMI_MPU_BASE + 0x200)
#define EMI_MPU_SA(region)		(EMI_MPU_SA0 + (region * 4))
#define EMI_MPU_EA(region)		(EMI_MPU_EA0 + (region * 4))
#define EMI_MPU_APC0			(EMI_MPU_BASE + 0x300)
#define EMI_MPU_APC(region)		(EMI_MPU_APC0 + (region * 4))


#define EMI_MPU_DOMAIN_NUM		(16)
#define EMI_MPU_REGION_NUM		(32)
#define EMI_MPU_ALIGN_BITS		(16)

#define EMI_PHY_OFFSET			(0x40000000UL)

#define BL32_REGION             (0)
#define BL32_APC                                        \
        APC(SEC_RW, FORBIDDEN, FORBIDDEN, FORBIDDEN,    \
            SEC_RW, FORBIDDEN, FORBIDDEN, FORBIDDEN)

#define BL31_REGION             (1)
#define BL31_APC                BL32_APC

static inline void emi_mpu_init(void)
{
        struct emi_region_info_t bl32_region;
        struct emi_region_info_t bl31_region;

        REGION_INIT(bl31_region, BL32_BASE, BL32_LIMIT, BL32_REGION, BL32_APC);
        REGION_INIT(bl32_region, BL31_BASE, BL31_END - BL31_START, BL31_REGION, BL31_APC);

        emi_mpu_set_protection(&bl31_region);
        emi_mpu_set_protection(&bl32_region);
}


#endif /* MT8168_EMI_MPU_H */