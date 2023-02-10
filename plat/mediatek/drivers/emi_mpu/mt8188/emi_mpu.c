/*
 * Copyright (c) 2022, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <emi_mpu.h>

void set_emi_mpu_regions(void)
{
    struct emi_region_info_t region_info;

    region_info.start = BL31_BASE;
    region_info.end = BL31_LIMIT;
    region_info.region = 0;
    SET_ACCESS_PERMISSION(region_info.apc, 1,
                            FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                            FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                            FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                            FORBIDDEN, FORBIDDEN, FORBIDDEN, SEC_RW);
    emi_mpu_set_protection(&region_info);

    region_info.start = BL32_BASE;
    region_info.end = (BL32_BASE + BL32_LIMIT);
    region_info.region = 1;
    SET_ACCESS_PERMISSION(region_info.apc, 1,
                            FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                            FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                            FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                            FORBIDDEN, FORBIDDEN, FORBIDDEN, SEC_RW);
    emi_mpu_set_protection(&region_info);
}
