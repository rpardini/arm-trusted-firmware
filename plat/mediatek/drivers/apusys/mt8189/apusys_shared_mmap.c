/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <mtk_mmap_pool.h>
#include <platform_def.h>

#define APUSYS_RESERVED_MEM_START	(0x55000000)
#define APUSYS_RESERVED_MEM_SZ		(0x1400000)

static const mmap_region_t apu_resv_mmap[] MTK_MMAP_SECTION = {
	MAP_REGION_FLAT(APUSYS_RESERVED_MEM_START, APUSYS_RESERVED_MEM_SZ,
			MT_DEVICE | MT_RW | MT_SECURE),
};
DECLARE_MTK_MMAP_REGIONS(apu_resv_mmap);

