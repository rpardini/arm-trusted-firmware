/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#ifndef SMPU_H
#define SMPU_H

#include <arch_helpers.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <mt_smpu.h>

#define MAX_REGION_NUM  SMPU_REGION_NUM

struct per_region {
	unsigned char aid;
	unsigned char perm;
};

struct smpu_region_info {
	unsigned char region_num;
	struct per_region region_perm[AID_NUM_MAX];
	struct per_region region_perm_drm[AID_NUM_MAX];
};

extern const struct smpu_region_info smpu_region_info_table[MAX_REGION_NUM];

static inline void smpu_write(unsigned int addr, unsigned int value)
{
	mmio_write_32(addr, value);
	dsb();
}

static inline unsigned int smpu_read(unsigned int addr)
{
	dsb();
	return mmio_read_32(addr);
}

#endif
