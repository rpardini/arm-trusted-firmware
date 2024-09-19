// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#include <stdint.h>
#include <stddef.h>
#include <common/debug.h>
#include <drivers/mmc.h>
#include "blkdev.h"


static struct blkdev g_mmc_bdev;

static int mmc_bread(struct blkdev *bdev, uint32_t blknr, uint32_t blks, uint8_t *buf,
			uint32_t part_id)
{
	if (mmc_read_blocks(blknr, buf, blks*MMC_BLOCK_SIZE) != blks*MMC_BLOCK_SIZE) {
		NOTICE("%s error\n", __func__);
		return -1;
	}

	return 0;
}

static int mmc_bwrite(struct blkdev *bdev, uint32_t blknr, uint32_t blks, uint8_t *buf,
			uint32_t part_id)
{
	if (mmc_write_blocks(blknr, buf, blks*MMC_BLOCK_SIZE) != blks*MMC_BLOCK_SIZE) {
		NOTICE("%s error\n", __func__);
		return -1;
	} else
		return 0;
}

static int mmc_rase(struct blkdev *bdev, uint64_t src, uint64_t size, uint32_t part_id)
{
	if (mmc_erase_blocks(src*MMC_BLOCK_SIZE, size*MMC_BLOCK_SIZE) != size*MMC_BLOCK_SIZE) {
		NOTICE("%s error\n", __func__);
		return -1;
	} else
		return 0;
}

unsigned int mmc_register_blkdev(void)
{
	if (!blkdev_get(BOOTDEV_SDMMC)) {
		memset(&g_mmc_bdev, 0, sizeof(struct blkdev));

		g_mmc_bdev.blksz   = MMC_BLOCK_SIZE;
		g_mmc_bdev.erasesz = MMC_BLOCK_SIZE;
		g_mmc_bdev.bread   = mmc_bread;
		g_mmc_bdev.bwrite  = mmc_bwrite;
		g_mmc_bdev.erase  = mmc_rase;
		g_mmc_bdev.type    = BOOTDEV_SDMMC;
		g_mmc_bdev.blkbuf  = NULL;
		g_mmc_bdev.priv    = NULL;

		blkdev_register(&g_mmc_bdev);
	}
	return 0;
}
