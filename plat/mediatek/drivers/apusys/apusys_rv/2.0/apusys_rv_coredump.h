/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APUSYS_RV_COREDUMP_H
#define APUSYS_RV_COREDUMP_H

#include <lib/mmio.h>
#include <platform_def.h>

#define RETRY_CNT_MAX	(1000)

/******************************************************************************
 * REGISTER ADDRESS DEFINITION
 ******************************************************************************/
#define MD32_DBG_APB_EN		(APU_MD32_DEBUG_APB + 0x0)
#define MD32_DBG_APB_MODE	(APU_MD32_DEBUG_APB + 0x4)
#define MD32_DBG_APB_INSTR	(APU_MD32_DEBUG_APB + 0x10)
#define MD32_DBG_APB_INSTR_WR	(APU_MD32_DEBUG_APB + 0x14)
#define MD32_DBG_APB_WDATA	(APU_MD32_DEBUG_APB + 0x18)
#define MD32_DBG_APB_WDATA_WR	(APU_MD32_DEBUG_APB + 0x1c)
#define MD32_DBG_APB_RDATA	(APU_MD32_DEBUG_APB + 0x20)
#define MD32_DBG_APB_READY	(APU_MD32_DEBUG_APB + 0x24)

/******************************************************************************
 * DEBUG APB COMMAND DEFINITION
 ******************************************************************************/
#define DBG_DATA_REG_INSTR	(0x800)
#define DBG_ADDR_REG_INSTR	(0x801)
#define DBG_INSTR_REG_INSTR	(0x802)
#define DBG_STATUS_REG_INSTR	(0x803)

#define DBG_REQUEST_INSTR	(0x811)
#define DBG_RESUME_INSTR	(0x812)
#define DBG_RESET_INSTR		(0x813)
#define DBG_STEP_INSTR		(0x814)
#define DBG_EXECUTE_INSTR	(0x815)
#define DBG_READ_PM		(0x840)
#define DBG_READ_DM		(0x842)
#define DBG_ATTACH_INSTR	(0x900)
#define DBG_DEATTACH_INSTR	(0x901)

/******************************************************************************
 * COREDUMP SIZE DEFINITION
 ******************************************************************************/
#define REG_SIZE		(4UL * 151UL)
#define TBUF_SIZE		(4UL * 32UL)
#define NR_TBUF_DBG_DATA	(4)
#define CACHE_DUMP_SIZE		(37UL * 1024UL)

#if defined(KASAN_SUPPORT) && defined(APU_KASAN_UP_CODE_SIZE)
#define CODE_BUF_SIZE	APU_KASAN_UP_CODE_SIZE
#define SHADOW_MEM_SIZE (0x40000)
#define DRAM_DUMP_SIZE	(CODE_BUF_SIZE - APU_MD32_TCM_SZ - SHADOW_MEM_SIZE)
#else
#define CODE_BUF_SIZE	(1024UL * 1024UL)
#define DRAM_DUMP_SIZE	(CODE_BUF_SIZE - APU_MD32_TCM_SZ)
#endif

struct apu_coredump {
	char tcmdump[APU_MD32_TCM_SZ];
	char ramdump[DRAM_DUMP_SIZE];
	char regdump[REG_SIZE];
	char tbufdump[TBUF_SIZE];
	uint32_t cachedump[CACHE_DUMP_SIZE/sizeof(uint32_t)];
} __attribute__ ((__packed__));

struct apusys_secure_info_t {
	unsigned int total_sz;
	unsigned int up_code_buf_ofs;
	unsigned int up_code_buf_sz;

	unsigned int up_fw_ofs;
	unsigned int up_fw_sz;
	unsigned int up_xfile_ofs;
	unsigned int up_xfile_sz;
	unsigned int mdla_fw_boot_ofs;
	unsigned int mdla_fw_boot_sz;
	unsigned int mdla_fw_main_ofs;
	unsigned int mdla_fw_main_sz;
	unsigned int mdla_xfile_ofs;
	unsigned int mdla_xfile_sz;
	unsigned int mvpu_fw_ofs;
	unsigned int mvpu_fw_sz;
	unsigned int mvpu_xfile_ofs;
	unsigned int mvpu_xfile_sz;
	unsigned int mvpu_sec_fw_ofs;
	unsigned int mvpu_sec_fw_sz;
	unsigned int mvpu_sec_xfile_ofs;
	unsigned int mvpu_sec_xfile_sz;

	unsigned int up_coredump_ofs;
	unsigned int up_coredump_sz;
	unsigned int mdla_coredump_ofs;
	unsigned int mdla_coredump_sz;
	unsigned int mvpu_coredump_ofs;
	unsigned int mvpu_coredump_sz;
	unsigned int mvpu_sec_coredump_ofs;
	unsigned int mvpu_sec_coredump_sz;

	unsigned int ce_bin_ofs;
	unsigned int ce_bin_sz;
};

#define CE_MAIN_MAGIC	(0xCEC0DE88)
#define CE_SUB_MAGIC	(0xCEC0DE00)

struct ce_main_hdr_t {
	unsigned int magic;         /* magic number*/
	unsigned int hdr_size;      /* header size */
	unsigned int img_size;      /* img size */
	unsigned int bin_count;     /* bin count */
};

struct ce_sub_hdr_t {
	unsigned int magic;         /* magic number */
	unsigned int bin_offset;    /* binary offset */
	unsigned int bin_size;      /* binary size */
	unsigned int ce_enum;       /* ce enum */
	char job_name[8];           /* job name */
	unsigned int mem_st;        /* ce enum */
	unsigned int hw_entry;      /* hw entry */
	unsigned int hw_entry_bit;  /* hw entry bit */
	unsigned int hw_entry_mask; /* hw entry mask */
	unsigned int hw_entry_val;  /* hw entry val*/
	unsigned int user_info;     /* user_info */
};

struct apusys_aee_coredump_info_t {
	unsigned int up_coredump_ofs;
	unsigned int up_coredump_sz;
	unsigned int regdump_ofs;
	unsigned int regdump_sz;
	unsigned int mdla_coredump_ofs;
	unsigned int mdla_coredump_sz;
	unsigned int mvpu_coredump_ofs;
	unsigned int mvpu_coredump_sz;
	unsigned int mvpu_sec_coredump_ofs;
	unsigned int mvpu_sec_coredump_sz;

	unsigned int up_xfile_ofs;
	unsigned int up_xfile_sz;
	unsigned int mdla_xfile_ofs;
	unsigned int mdla_xfile_sz;
	unsigned int mvpu_xfile_ofs;
	unsigned int mvpu_xfile_sz;
	unsigned int mvpu_sec_xfile_ofs;
	unsigned int mvpu_sec_xfile_sz;

	unsigned int ce_bin_ofs;
	unsigned int ce_bin_sz;

	unsigned int are_sram_ofs;
	unsigned int are_sram_sz;
};


#endif /* APUSYS_RV_COREDUMP_H */

