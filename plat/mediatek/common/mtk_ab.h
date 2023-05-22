/*
 * Copyright (c) 2023, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MTK_AB_H
#define MTK_AB_H

#include <stdint.h>
#include <errno.h>
#include <drivers/io/io_storage.h>
#include <common/debug.h>
#include <common/bl_common.h>

#define BOOTCTRL_PART			"misc"
#define BOOTCTRL_MAGIC			0x544F494D
#define BOOTCTRL_VERSION		1
#define BOOTCTRL_BOOT_SUCCESS	0
#define BOOTCTRL_SRC			1
#define BOOTCTRL_MAX_RETRY		9
#define BOOTCTRL_GET			0
#define BOOTCTRL_SET			1

#define PART_BOOT_A				0
#define PART_BOOT_B				1
#define PART_SUFFIX_A			"bootloaders"
#define PART_SUFFIX_B			"bootloaders_b"

struct mtk_bl_ctrl {
	uint32_t slot_suffix;
	uint32_t magic;
	uint8_t version;
	uint8_t tries[2];
	uint32_t reserved[3];
	uint32_t crc32_le;
};
#endif /* MTK_AB_H */
