// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <common/debug.h>
#include <drivers/spi_nor.h>
#include "blkdev.h"

#define NOR_BLOCK_SIZE		4096

static struct blkdev g_nor_bdev;

static int nor_bread(struct blkdev *bdev, uint32_t blknr, uint32_t blks, uint8_t *buf,
			uint32_t part_id)
{
	size_t length;

	spi_nor_read(blknr*bdev->blksz, (uintptr_t)buf, blks*bdev->blksz, &length);

	return 0;
}

static int nor_bwrite(struct blkdev *bdev, uint32_t blknr, uint32_t blks, uint8_t *buf,
			uint32_t part_id)
{
	size_t length;

	spi_nor_write(blknr*bdev->blksz, (uintptr_t)buf, blks*bdev->blksz, &length);

	return 0;
}

static int nor_erase(struct blkdev *bdev, uint64_t src, uint64_t size, uint32_t part_id)
{
	size_t length;

	spi_nor_erase(src, size, &length);

	return 0;
}

uint32_t nor_register_blkdev(void)
{
	if (!blkdev_get(BOOTDEV_NOR)) {
		memset(&g_nor_bdev, 0, sizeof(struct blkdev));

		g_nor_bdev.blksz   = 1;
		g_nor_bdev.erasesz = NOR_BLOCK_SIZE;
		g_nor_bdev.bread   = nor_bread;
		g_nor_bdev.bwrite  = nor_bwrite;
		g_nor_bdev.erase  = nor_erase;
		g_nor_bdev.type    = BOOTDEV_NOR;
		g_nor_bdev.blkbuf  = NULL;
		g_nor_bdev.priv    = NULL;

		blkdev_register(&g_nor_bdev);
	}
	return 0;
}
