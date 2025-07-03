/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#ifndef __MMINFRA_COMM_H__
#define __MMINFRA_COMM_H__

#include <stddef.h>

#define SMI_MAX_REG     7

/* Lookup table */
struct mm_lut {
	uint8_t in_start;
	uint8_t in_end;
	uint8_t out_id;
};

struct smi_comm_item {
	uint32_t base;
	struct smi_reg_pair {
		uint32_t offset;
		uint32_t value;
	} regs[SMI_MAX_REG];
};

#endif /* __MMINFRA_COMM_H__ */
