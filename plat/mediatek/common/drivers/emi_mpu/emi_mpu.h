/*
 * Copyright (c) 2022, BayLibre SAS
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef COMMON_EMI_MPU_H
#define COMMON_EMI_MPU_H

#include <platform_def.h>

#define NO_PROTECTION                   0
#define SEC_RW                          1
#define SEC_RW_NSEC_R                   2
#define SEC_RW_NSEC_W                   3
#define SEC_R_NSEC_R                    4
#define FORBIDDEN                       5
#define SEC_R_NSEC_RW                   6

#define RO_LOCK                         (1 << 31)

#define APC(d0, d1, d2, d3, d4, d5, d6, d7)                     \
        ((d7) << 21 | (d6) << 18 | (d5) << 15 | (d4) << 12 |    \
        (d3) <<  9 | (d2) <<  6 | (d1) <<  3 | (d0)) | RO_LOCK  \

#define REGION_APC(_start, _size, _region, _apc)                \
        {                                                       \
                .start = (_start),                              \
                .end = (_start) + (_size),                      \
                .region = (_region),                            \
                .apc = (_apc),                                  \
        }

#define REGION_INIT(_region, _start, _size, _region_id, _apc)   \
        {                                                       \
                (_region).start = (_start);                     \
                (_region).end = (_start) + (_size);             \
                (_region).region = (_region_id);                \
                (_region).apc = (_apc);                         \
        }

struct emi_region_info_t {
        unsigned long long start;
        unsigned long long end;
        unsigned int region;
        unsigned int apc;
};

void emi_mpu_init_common(struct emi_region_info_t regions[], int count);
int emi_mpu_set_protection(struct emi_region_info_t *region_info);

#include <plat_emi_mpu.h>

#endif /* COMMON_EMI_MPU_H */
