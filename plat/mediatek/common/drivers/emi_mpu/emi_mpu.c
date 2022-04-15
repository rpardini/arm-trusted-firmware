/*
 * Copyright (c) 2022, BayLibre SAS
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <debug.h>
#include <lib/mmio.h>

#include <emi_mpu.h>

static void _emi_mpu_set_protection(unsigned int start, unsigned int end,
                                   unsigned int region, unsigned int apc)
{
        mmio_write_32(EMI_MPU_APC(region), 0);
        mmio_write_32(EMI_MPU_SA(region), start);
        mmio_write_32(EMI_MPU_EA(region), end);
        mmio_write_32(EMI_MPU_APC(region), apc);

        return 0;
}

int emi_mpu_set_protection(struct emi_region_info_t *region_info)
{
        unsigned int start, end;

        if (region_info->region >= EMI_MPU_REGION_NUM) {
                WARN("invalid region\n");
                return -1;
        }

        start = (region_info->start - EMI_PHY_OFFSET) >> EMI_MPU_ALIGN_BITS;
        end = (region_info->end - EMI_PHY_OFFSET) >> EMI_MPU_ALIGN_BITS;

        _emi_mpu_set_protection(start, end, region_info->region,
                                region_info->apc);

        return 0;
}

void emi_mpu_init_common(struct emi_region_info_t regions[], int count)
{
        int i;

        for (i = 0; i < count; i++) {
                emi_mpu_set_protection(&regions[i]);
        }
}
