/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __APUSYS_AMMU_H__
#define __APUSYS_AMMU_H__
#include <stdint.h>

enum apummu_page_size {
	APUMMU_PAGE_LEN_128KB = 0,
	APUMMU_PAGE_LEN_256KB,
	APUMMU_PAGE_LEN_512KB,
	APUMMU_PAGE_LEN_1MB,
	APUMMU_PAGE_LEN_128MB,
	APUMMU_PAGE_LEN_256MB,
	APUMMU_PAGE_LEN_512MB,
	APUMMU_PAGE_LEN_4GB
};

int rv_boot(uint32_t uP_seg_output, uint32_t uP_hw_thread,
	uint32_t logger_seg_output, enum apummu_page_size logger_page_size,
	uint32_t XPU_seg_output, enum apummu_page_size XPU_page_size);

#endif /* __APUSYS_AMMU_H__ */
