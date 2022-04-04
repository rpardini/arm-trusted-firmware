/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MCUPM__H__
#define __MCUPM__H__

#include <stddef.h>
#include <string.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <plat_pm.h>
#include <platform_def.h>
#include <sleep_def.h>
#include <assert.h>
#include <mt8195.h>
#include <mcu.h>

#define CONFIG_MCUPM_FIRMWARE "mcupm.bin"

#define MCUPM_SW_RSTN		(MCUPM_CFG_BASE + 0x000)

void mcupm_init(void);

#endif
