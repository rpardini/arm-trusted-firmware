/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SSPM__H__
#define __SSPM__H__

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

#define CONFIG_SSPM_FIRMWARE "sspm.bin"

#define SSPM_SW_RSTN		(SSPM_CFG_BASE + 0x000)

void sspm_init(void);

#endif
