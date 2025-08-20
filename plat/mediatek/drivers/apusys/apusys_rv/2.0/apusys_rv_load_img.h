/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APUSYS_RV_LOAD_IMG_H
#define APUSYS_RV_LOAD_IMG_H

#include <stdint.h>

#define ROUNDUP(a, b)	(((a) + ((b)-1)) & ~((b)-1))

#define APUSYS_MEM_ALIGN		(0x10000)	/* 64K (minimal size for EMI MPU) */
#define APUSYS_MEM_IOVA_ALIGN		(0x100000)	/* 1M (for secure iova mapping) */
#define APUSYS_AEE_COREDUMP_MEM_ALIGN	(0x10000)	/* 64K (minimal size for EMI MPU) */

#define REGDUMP_BUF_SZ			(0x10000)	/* Need same with DTS:mediatek,mtxxxx-apusys_rv */
#define CE_COREDUMP_BUF_SZ		(0x010000)	/* Need same with DTS:mediatek,mtxxxx-apusys_rv */
#define APUSYS_IOVA_PAGE		(0x4000)	/* secure iova page table size */
#define APUSYS_FW_ALIGN			(16)		/* for mdla dma alignment limitation */

#define ENABLE_APUSYS_EMI_PROTECTION	(1)

#define APUSYS_NS_FW_EMI_REGION		(25)
#define APUSYS_SEC_FW_EMI_REGION	(23)
#define APUSYS_CTRLMEM_EMI_REGION	(24)

#define APUSYS_RESERVED_MEM_PA		(0x55000000)	/* Need same with DTS:apusys-reserve-memory */
#define APUSYS_RESERVED_MEM_SZ		(0x1400000)	/* Need same with DTS:apusys-reserve-memory */

#define PT_MAGIC			(0x58901690)

enum PT_ID_APUSYS {
	PT_ID_APUSYS_FW,
	PT_ID_APUSYS_XFILE,
	PT_ID_MDLA_FW_BOOT,
	PT_ID_MDLA_FW_MAIN,
	PT_ID_MDLA_XFILE,
	PT_ID_MVPU_FW,
	PT_ID_MVPU_XFILE,
	PT_ID_MVPU_SEC_FW,
	PT_ID_MVPU_SEC_XFILE,
	PT_ID_CE_BIN,
	PT_ID_CE_BIN_DUMMY
};

struct ptimg_hdr_t {
	unsigned int magic;     /* magic number*/
	unsigned int hdr_size;  /* header size */
	unsigned int img_size;  /* img size */
	unsigned int align;     /* alignment */
	unsigned int id;        /* image id */
	unsigned int addr;      /* memory addr */
};

int32_t apusys_kernel_apusys_rv_load_image(uint64_t unused, uint64_t res_mem_size, uint64_t apusys_img_size);

#endif /* APUSYS_RV_LOAD_IMG_H */
