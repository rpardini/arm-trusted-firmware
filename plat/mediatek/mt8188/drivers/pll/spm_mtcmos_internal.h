/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SPM_MTCMOS_INTERNAL_
#define _SPM_MTCMOS_INTERNAL_


#define SPM_PROJECT_CODE	0xb16
#define SPM_REGWR_CFG_KEY	(SPM_PROJECT_CODE << 16)
#define SPM_REGWR_EN		0x1


#define spm_read(addr)                  mmio_read_32(addr)
#define spm_write(addr, val)            mmio_write_32(addr, val)

#endif //#ifndef _SPM_MTCMOS_INTERNAL_
