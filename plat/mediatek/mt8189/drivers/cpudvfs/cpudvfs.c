// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#include <drivers/delay_timer.h>
#include <lib/mmio.h>
#include <common/debug.h>
#include <platform_def.h>
#include <lib/mtk_init/mtk_init.h>

#define CPUDVFS_BASE		0x00108D68
#define CPUDVFS_MAGICNUMBER	0x55AA55AA

__attribute__((weak)) uint32_t get_cpu_segment_id(void)
{
	uint32_t id = mmio_read_32(EFUSEC_BASE + 0x7E0);

	NOTICE("CPU Segment ID: 0x%x\n", id);

	return id;
}

int dvfs_init(void)
{
	uint32_t id, val;

	id = get_cpu_segment_id();
	val = (id << 8) | BIT(0);

	mmio_write_32(CPUDVFS_BASE + 0x8, CPUDVFS_MAGICNUMBER);
	mmio_write_32(CPUDVFS_BASE + 0xC, val);

	return 0;
}

MTK_EARLY_PLAT_INIT(dvfs_init);
