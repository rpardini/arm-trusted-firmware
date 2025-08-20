/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* TF-A system header */
#include <common/debug.h>
#include <lib/mmio.h>
#include <lib/utils_def.h>

#include <mtk_sip_svc.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <mtk_mmap_pool.h>
#include <mtk_mmap_pool.h>
#include <platform_def.h>
#include "apusys_rv_coredump.h"
#include "apusys_ce.h"

#define MODULE_TAG "[APUSYS_CE]"
#define MAX_SMC_OP_NUM  0x3

enum CE_SMC_OP_ID {
#define ENTRY(ID, ADDR) ID,
	CE_SMC_OP_TABLE
#undef ENTRY
	CE_SMC_OP_ID_MAX
};

static const uint32_t CE_SMC_OP_ADDR[CE_SMC_OP_ID_MAX] = {
#define ENTRY(ID, ADDR) ADDR,
	CE_SMC_OP_TABLE
#undef ENTRY
};

static const uint32_t CE_DUMP_ADDR[] = {
#define ENTRY(ADDR, SIZE) ADDR,
	CE_DUMP_TABLE
#undef ENTRY
};

static const uint32_t CE_DUMP_SIZE[] = {
#define ENTRY(ADDR, SIZE) SIZE,
	CE_DUMP_TABLE
#undef ENTRY
};

int apusys_ce_debug_regdump(uint64_t aee_coredump_buf_pa,
	struct apusys_aee_coredump_info_t *aee_coredump_info)
{
	unsigned int addr, cnt, buf_offset = 0;
	unsigned int reg_dump_sz = aee_coredump_info->ce_bin_sz;
	unsigned int *reg_dump_buf = (unsigned int *)(
		aee_coredump_buf_pa + aee_coredump_info->ce_bin_ofs);

	INFO("%s: enter\n", __func__);

	memset(reg_dump_buf, 0, reg_dump_sz);

	for (cnt = 0; cnt < ARRAY_SIZE(CE_DUMP_ADDR); cnt++) {
		if (buf_offset + 12 + CE_DUMP_SIZE[cnt] < reg_dump_sz) {
			reg_dump_buf[buf_offset++] = CE_REG_DUMP_MAGIC_NUM;
			reg_dump_buf[buf_offset++] = CE_DUMP_ADDR[cnt];
			reg_dump_buf[buf_offset++] = CE_DUMP_ADDR[cnt] + CE_DUMP_SIZE[cnt];

			for (addr = CE_DUMP_ADDR[cnt];
				 addr < CE_DUMP_ADDR[cnt] + CE_DUMP_SIZE[cnt];
				 addr += 4)
				reg_dump_buf[buf_offset++] = mmio_read_32(addr);
		}
	}

	return 0;
}

int apusys_ce_sram_dump(uint64_t aee_coredump_buf_pa,
	struct apusys_aee_coredump_info_t *aee_coredump_info)
{
	unsigned int addr_offset, buf_offset = 0;
	unsigned int *dump_buf = (unsigned int *)(
		aee_coredump_buf_pa + aee_coredump_info->are_sram_ofs);

	INFO("%s: enter\n", __func__);

	/* dump fw from debug apb */
	for (addr_offset = 0; addr_offset < APU_ARETOP_ARE_SZ; addr_offset += 4) {
		mmio_write_32(APU_ACE_SRAM_ENTRY_SEL, addr_offset >> 2);
		dump_buf[buf_offset++] = mmio_read_32(APU_ACE_SRAM_ENTRY_RD);
	}
	return 0;
}

int apusys_kernel_apusys_ce_regdump(uint32_t op, struct smccc_res *smccc_ret)
{
	uint8_t smc_op;
	uint32_t reg_addr[MAX_SMC_OP_NUM];
	uint32_t i;

	if (op == 0) {
		ERROR("%s empty op = 0x%08x\n", MODULE_TAG, op);
		return -EINVAL;
	}

	for (i = 0; i < MAX_SMC_OP_NUM; i++) {
		smc_op = (0xFF) & (op >> (8 * i));
		if (smc_op < CE_SMC_OP_ID_MAX)
			reg_addr[i] = CE_SMC_OP_ADDR[smc_op];
		else {
			ERROR("%s unknown op = 0x%08x\n", MODULE_TAG, smc_op);
			return -EINVAL;
		}
	}

	for (i = 0; i < MAX_SMC_OP_NUM; i++) {
		if (reg_addr[i] != 0) {
			switch (i) {
			case 0:
				smccc_ret->a1 = mmio_read_32(reg_addr[i]);
				break;
			case 1:
				smccc_ret->a2 = mmio_read_32(reg_addr[i]);
				break;
			case 2:
				smccc_ret->a3 = mmio_read_32(reg_addr[i]);
				break;
			}
		}
	}

	return 0;
}

int apusys_kernel_apusys_ce_mask_init(void)
{
    /*
     * 1.Initial exception setting
     * Initial ACE exception setting, default all mask off
     * Initial CE exception setting, default all mask off
     * Initial ACE abnormal IRQ flag (customized), default all mask off
     */
	mmio_write_32(APU_ACE_ABN_IRQ_MASK_CE, 0x1111);/* mask on polling_timeout_mask_CE */
	mmio_write_32(APU_ACE_ABN_IRQ_MASK_ACE_SW, 0x400);/* mask on HW Timer 2 */

    /*
     * 2.Initial job_timeout_value
     * Initial job_timeout_value, unit: 61us, range: 61us ~ 4s
     * default set 4s timeout
     * CE will stall
     */
	mmio_write_32(APU_ACE_JOB_TIEMOUT_CE0, 0xFFFF0001);
	mmio_write_32(APU_ACE_JOB_TIEMOUT_CE1, 0xFFFF0001);
	mmio_write_32(APU_ACE_JOB_TIEMOUT_CE2, 0xFFFF0001);
	mmio_write_32(APU_ACE_JOB_TIEMOUT_CE3, 0xFFFF0001);

#ifdef APU_EXCEPTION_MASK_ON
	mmio_write_32(APU_RCX_CONFIG + 0x380, 0x200000);
	mmio_write_32(APU_RCX_CONFIG + 0x384, 0x0);
#endif
	return 0;
}

int apusys_kernel_apusys_ce_reg_write(uint32_t op, uint32_t write_val)
{
	uint32_t reg_addr = 0;

	if (op > 0 && op < CE_SMC_OP_ID_MAX)
		reg_addr = CE_SMC_OP_ADDR[op];
	else {
		ERROR("%s unknown or not support op = %x\n", MODULE_TAG, op);
		return -EINVAL;
	}

	mmio_write_32(reg_addr, write_val);
	return 0;
}

