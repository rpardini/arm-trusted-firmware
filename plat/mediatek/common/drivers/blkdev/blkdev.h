/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 MediaTek Inc.
 */

#ifndef BLKDEV_H
#define BLKDEV_H

#include <stdint.h>

#define BOOTDEV_NAND		(0)
#define BOOTDEV_SDMMC		(1)
#define BOOTDEV_UFS		(2)
#define BOOTDEV_NOR		(3)

struct blkdev {
	uint32_t type;       /* block device type */
	uint32_t blksz;      /* block size. (read/write unit) */
	uint32_t erasesz;    /* erase size */
	uint32_t blks;       /* number of blocks in the device */
	uint32_t offset;     /* user area offset in blksz unit */
	uint8_t *blkbuf;     /* block size buffer */
	void *priv;          /* device private data */
	struct blkdev *next;      /* next block device */
	int (*bread)(struct blkdev *bdev, uint32_t blknr, uint32_t blks, uint8_t *buf,
		     uint32_t part_id);
	int (*bwrite)(struct blkdev *bdev, uint32_t blknr, uint32_t blks, uint8_t *buf,
		      uint32_t part_id);
	int (*erase)(struct blkdev *bdev, uint64_t src, uint64_t size, uint32_t part_id);
	uint64_t (*get_part_size)(struct blkdev *bdev, uint32_t part_id);
};

extern void blkdev_set_dramk_data_offset(uint64_t dramk_addr);
extern int blkdev_register(struct blkdev *bdev);
extern int blkdev_read(struct blkdev *bdev, uint64_t src, uint32_t size, uint8_t *dst,
			uint32_t part_id);
extern int blkdev_write(struct blkdev *bdev, uint64_t dst, uint32_t size, uint8_t *src,
			uint32_t part_id);
extern int blkdev_bread(struct blkdev *bdev, uint32_t blknr, uint32_t blks, uint8_t *buf,
			uint32_t part_id);
extern int blkdev_bwrite(struct blkdev *bdev, uint32_t blknr, uint32_t blks, uint8_t *buf,
			 uint32_t part_id);
extern int blkdev_read64(struct blkdev *bdev, uint64_t src, uint64_t size, uint8_t *dst,
			 uint32_t part_id);
extern int blkdev_write64(struct blkdev *bdev, uint64_t dst, uint64_t size, uint8_t *src,
			  uint32_t part_id);
extern int blkdev_erase(struct blkdev *bdev, uint64_t src, uint64_t size, uint32_t part_id);
extern int blkdev_boot_read(struct blkdev *bdev, uint64_t src, uint32_t size, uint8_t *dst);
extern struct blkdev *blkdev_get(uint32_t type);

#endif /* BLKDEV_H */
