// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#include <stdint.h>
#include <stddef.h>
#include <common/debug.h>
#include "blkdev.h"

#define UFS_BLOCK_SHIFT		12		/* 4KB */
#define UFS_BLOCK_SIZE		(1 << UFS_BLOCK_SHIFT)

static struct blkdev g_ufs_bdev;

static int ufs_bread(struct blkdev *bdev, uint32_t blknr, uint32_t blks, uint8_t *buf,
			uint32_t part_id)
{
	if (mtk_ufs_read(blknr, buf, blks*UFS_BLOCK_SIZE) != blks*UFS_BLOCK_SIZE) {
		NOTICE("%s error\n", __func__);
		return -1;
	}

	return 0;
}

static int ufs_bwrite(struct blkdev *bdev, uint32_t blknr, uint32_t blks, uint8_t *buf,
			uint32_t part_id)
{
	if (mtk_ufs_write(blknr, buf, blks*UFS_BLOCK_SIZE) != blks*UFS_BLOCK_SIZE) {
		NOTICE("%s error\n", __func__);
		return -1;
	} else
		return 0;
}

static int ufs_erase(struct blkdev *bdev, uint64_t src, uint64_t size, uint32_t part_id)
{
	return -1;
}

uint32_t ufs_register_blkdev(void)
{
	if (!blkdev_get(BOOTDEV_UFS)) {
		memset(&g_ufs_bdev, 0, sizeof(struct blkdev));

		g_ufs_bdev.blksz   = UFS_BLOCK_SIZE;
		g_ufs_bdev.erasesz = UFS_BLOCK_SIZE;
		g_ufs_bdev.bread   = ufs_bread;
		g_ufs_bdev.bwrite  = ufs_bwrite;
		g_ufs_bdev.erase  = ufs_erase;
		g_ufs_bdev.type    = BOOTDEV_UFS;
		g_ufs_bdev.blkbuf  = NULL;
		g_ufs_bdev.priv    = NULL;

		blkdev_register(&g_ufs_bdev);
	}
	return 0;
}
