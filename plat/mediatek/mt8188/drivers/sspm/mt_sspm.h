/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __SSPM__H__
#define __SSPM__H__

#include <stddef.h>
#include <string.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <mcu.h>

#define CONFIG_SSPM_FIRMWARE "sspm.bin"

#define SSPM_SW_RSTN		(SSPM_CFG_BASE + 0x000)

#endif
