/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MCUPM__H__
#define __MCUPM__H__

#include <stddef.h>
#include <string.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <mcu.h>

#define CONFIG_MCUPM_FIRMWARE "mcupm.bin"

#define MCUPM_SW_RSTN		(MCUPM_CFG_BASE + 0x000)

int mcupm_init(void);

#endif
