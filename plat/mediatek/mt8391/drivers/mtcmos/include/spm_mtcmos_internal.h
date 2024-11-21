/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 MediaTek Inc.
 */

#pragma once

#include <platform_def.h>
#include <lib/mmio.h>
#include "spm_mtcmos.h"

#define SPM_REGWR_CFG_KEY	(SPM_PROJECT_CODE << 16)
#define SPM_REGWR_EN		0x1

#define spm_read(addr)                  mmio_read_32(addr)
#define spm_write(addr, val)            mmio_write_32(addr, val)
