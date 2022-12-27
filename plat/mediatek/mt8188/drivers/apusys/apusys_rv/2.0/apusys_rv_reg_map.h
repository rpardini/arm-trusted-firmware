/*
 * Copyright (c) 2021, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APUSYS_RV_REG_MAP_H
#define APUSYS_RV_REG_MAP_H

#define IDX_REG_DUMP_SIZE (0)
#define IDX_REG_DUMP_GPR_LR (1)
#define IDX_REG_DUMP_GPR_SP (2)
#define IDX_REG_DUMP_PC (32)
#define IDX_REG_DUMP_SP (33)
#define IDX_CSR_DUMP_START (34)

#define NUM_GPR_REG (32)
#define GPR_ID_SHIFT (15)
#define CSR_ID_SHIFT (20)

#define GPR_READ_CMD (0x7DF01073)
#define CSR_ID_CMD (0x2473)
#define CSR_READ_CMD (0x7DF41073)

static const uint32_t rv_reg_map[] = {
	0x300, /* CSR */
	0x341, /* CSR */
	0x301, /* CSR */
	0x315, /* CSR */
	0x7c0, /* CSR */
	0xb00, /* CSR */
	0xc01, /* CSR */
	0xb02, /* CSR */
	0xb80, /* CSR */
	0xc81, /* CSR */
	0xb82, /* CSR */
	0xbc0, /* CSR */
	0xbd0, /* CSR */
	0xf11, /* CSR */
	0xf12, /* CSR */
	0xf13, /* CSR */
	0xf14, /* CSR */
	0xfc1, /* CSR */
	0xfc2, /* CSR */
	0xfc3, /* CSR */
	0xfc4, /* CSR */
	0x5c0, /* CSR */
	0x5d0, /* CSR */
	0x5d1, /* CSR */
	0x5d2, /* CSR */
	0x5d3, /* CSR */
	0x5d4, /* CSR */
	0x5d5, /* CSR */
	0x5d6, /* CSR */
	0x5d7, /* CSR */
	0x5d8, /* CSR */
	0x5d9, /* CSR */
	0x5da, /* CSR */
	0x5db, /* CSR */
	0x5dc, /* CSR */
	0x5dd, /* CSR */
	0x5de, /* CSR */
	0x5df, /* CSR */
	0x5e0, /* CSR */
	0x5e1, /* CSR */
	0x5e2, /* CSR */
	0x5e3, /* CSR */
	0x5e4, /* CSR */
	0x5e5, /* CSR */
	0x5e6, /* CSR */
	0x5e7, /* CSR */
	0x5e8, /* CSR */
	0x5e9, /* CSR */
	0x5ea, /* CSR */
	0x5eb, /* CSR */
	0x5ec, /* CSR */
	0x5ed, /* CSR */
	0x5ee, /* CSR */
	0x5ef, /* CSR */
	0x5f0, /* CSR */
	0x5f1, /* CSR */
	0x5f2, /* CSR */
	0x5f3, /* CSR */
	0x5f4, /* CSR */
	0x5f5, /* CSR */
	0x5f6, /* CSR */
	0x5f7, /* CSR */
	0x302, /* CSR */
	0x303, /* CSR */
	0x304, /* CSR */
	0x344, /* CSR */
	0x9c0, /* CSR */
	0x9dc, /* CSR */
	0x9dd, /* CSR */
	0x9de, /* CSR */
	0x9df, /* CSR */
	0x9e0, /* CSR */
	0x9e1, /* CSR */
	0x9e2, /* CSR */
	0x9e3, /* CSR */
	0x9e4, /* CSR */
	0x9e5, /* CSR */
	0x9e6, /* CSR */
	0x9e7, /* CSR */
	0x9e8, /* CSR */
	0x9e9, /* CSR */
	0x9ea, /* CSR */
	0x9eb, /* CSR */
	0x9ec, /* CSR */
	0x9ed, /* CSR */
	0x9ee, /* CSR */
	0x9ef, /* CSR */
	0x9f0, /* CSR */
	0x9f1, /* CSR */
	0x9f2, /* CSR */
	0x9f3, /* CSR */
	0x9f4, /* CSR */
	0x9f5, /* CSR */
	0x9f6, /* CSR */
	0x9f7, /* CSR */
	0x9f8, /* CSR */
	0x9f9, /* CSR */
	0x9fa, /* CSR */
	0x9fb, /* CSR */
	0x9fc, /* CSR */
	0x9fd, /* CSR */
	0x9fe, /* CSR */
	0x9ff, /* CSR */
	0x7d0, /* CSR */
	0x7d1, /* CSR */
	0x7d2, /* CSR */
	0x7d3, /* CSR */
	0x7d4, /* CSR */
	0x7d8, /* CSR */
	0x7d9, /* CSR */
	0x7da, /* CSR */
	0x7db, /* CSR */
	0x340, /* CSR */
	0x342, /* CSR */
	0x343, /* CSR */
	0x7c5, /* CSR */
	0x7c6, /* CSR */
};


#endif /* APUSYS_RV_REG_MAP_H */
