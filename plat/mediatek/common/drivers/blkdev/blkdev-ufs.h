/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 MediaTek Inc.
 */

#ifndef BLKDEV_UFS_H
#define BLKDEV_UFS_H

#include "blkdev.h"

size_t mtk_ufs_read(int lba, uintptr_t buf, size_t size);
size_t mtk_ufs_write(int lba, uintptr_t buf, size_t size);
unsigned int ufs_register_blkdev(void);

#endif
