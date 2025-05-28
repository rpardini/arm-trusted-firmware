// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#include <smpu.h>

static void _emi_mpu_set_protection(u_register_t start, u_register_t end, u_register_t region)
{
	if (region == 0)
		return;

	unsigned int region_set, region_bit, val;

	region_set = ((unsigned int)region - 1) / MPU_REGION_NUMBER_PER_SET;
	region_bit = ((unsigned int)region - 1) % MPU_REGION_NUMBER_PER_SET;
	smpu_write(SECURE_REGION_SA(region), start & ADDR_MASK);
	smpu_write(SECURE_REGION_EA(region), ((unsigned int)end & (unsigned int)ADDR_MASK)
			| (unsigned int)REGION_ENABLE);

	val = smpu_read(SECURE_REGION_ENABLE(region_set));
	smpu_write(SECURE_REGION_ENABLE(region_set), val | 0x1U << region_bit);
}

void emi_mpu_set_protection(u_register_t start, u_register_t end, u_register_t region)
{
	u_register_t st, ed;

	st = ((start >> ALIGN_BIT) & 0x00FFFFFF);
	ed = ((end >> ALIGN_BIT) & 0x00FFFFFF);

	if ((st >= DRAM_OFFSET) && (ed >= st)) {
		st -= DRAM_OFFSET;
		ed -= DRAM_OFFSET;
	}

	_emi_mpu_set_protection(st, ed, region);
}

void set_smpu_regions(void)
{
	u_register_t start, end;
	unsigned int val;

	start = BL31_BASE;
	end = BL31_LIMIT;
	emi_mpu_set_protection(start, end, 1);

	start = BL32_BASE;
	end = (BL32_BASE + BL32_LIMIT);
	emi_mpu_set_protection(start, end, 2);

	val = smpu_read(NEMI_SMPU_BASE + 0x2a0);
	smpu_write(NEMI_SMPU_BASE + 0x2a0, val | 0x1);
}
