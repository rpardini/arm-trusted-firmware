// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#include <lib/mtk_init/mtk_init.h>
#include <smpu.h>

static void smpu_write_or(unsigned int addr, unsigned int or_value)
{
	unsigned int value = smpu_read(addr);

	smpu_write(addr, value | or_value);
}

static void smpu_write_and(unsigned int addr, unsigned int and_value)
{
	unsigned int value = smpu_read(addr);

	smpu_write(addr, value & and_value);
}

static void read_aid_table(void)
{
	unsigned char i;

	smpu_write(NEMI_AID_PERM_TABEL_NON_SEC, smpu_read(NEMI_READ_AID_PERM_TABEL_NON_SEC0));
	for (i = 0; i < (REGION_MAX / MPU_REGION_BIT) + 1; i++)
		smpu_write(NEMI_AID_PERM_TABEL_SEC(i), smpu_read(NEMI_READ_AID_PERM_TABEL_SEC(i)));
}

static void smpu_setting(void)
{
	smpu_write_or(INFRA_SECU_AO_RG_CPU, 0x1 << 16);
	smpu_write_and(INFRA_SECU_AO_RG_CPU, ~(0x1 << 17));

	smpu_write_or(INFRA_SECU_AO_RG_GCPU, (0x1 << 16));
	smpu_write_and(INFRA_SECU_AO_RG_GCPU, ~(0x1 << 17));
	smpu_write_or(INFRA_SECU_AO_RG_GCPU, (0x1 << 18));
	smpu_write_and(INFRA_SECU_AO_RG_GCPU, ~(0x1 << 19));
	smpu_write_or(INFRA_SECU_AO_RG_GCPU, (0x1 << 20));

	smpu_write_or(NEMI_SMPU_BASE + 0x2a0, 0x100);
	smpu_write(NEMI_IOMMU_MPU_ENABLE, 0x0);
}

static void clear_aid_table(void)
{
	unsigned char i;

	smpu_write(NEMI_AID_PERM_TABEL_NON_SEC, 0);
	for (i = 0; i < (REGION_MAX / MPU_REGION_BIT) + 1; i++)
		smpu_write(NEMI_AID_PERM_TABEL_SEC(i), 0);
}

static int set_aid_perm(unsigned int aid, unsigned int region, unsigned char perm)
{
	unsigned int reg_num, reg_bit;

	smpu_write(NEMI_AID_PERM_TABEL_IDX, aid & 0xff);
	read_aid_table();

	if (!region) {
		smpu_write_or(NEMI_AID_PERM_TABEL_NON_SEC, perm);
	} else {
		reg_num = (region - 1) / MPU_REGION_BIT;
		reg_bit = (region - 1) % MPU_REGION_BIT;
		smpu_write_or(NEMI_AID_PERM_TABEL_SEC(reg_num),
				((unsigned int)perm << (reg_bit * 2)));
	}

	smpu_write(NEMI_AID_PERM_TABEL_TRIG_WRITE, 1);

	clear_aid_table();

	return 0;
}

static void set_aid_table(void)
{
	int total = ARRAY_SIZE(smpu_region_info_table);
	int i, j;

	for (i = 0; i < total; i++) {
		for (j = 0; j < AID_NUM_MAX; j++) {
			if ((i != 0 && smpu_region_info_table[i].region_num == 0) ||
					smpu_region_info_table[i].region_perm[j].aid == 0 ||
					smpu_region_info_table[i].region_perm[j].perm > 3)
				break;
			set_aid_perm(smpu_region_info_table[i].region_perm[j].aid,
					smpu_region_info_table[i].region_num,
					smpu_region_info_table[i].region_perm[j].perm);
		}
	}
}

static void clear_violation(void)
{
	smpu_write(NEMI_SEC_LOG_W, 0x1);
	smpu_write(NEMI_SEC_LOG_W, 0x0);
	smpu_write(NEMI_SEC_LOG_R, 0x1);
	smpu_write(NEMI_SEC_LOG_R, 0x0);
}

static void smpu_reset(void)
{
	smpu_write_or(NEMI_SMPU_HRE_W1S_XRST, 0x1 << 1);
	smpu_write_or(NEMI_SMPU_HRE_W1C_XRST, 0x1 << 1);
}

int smpu_init(void)
{
	clear_violation();
	smpu_reset();

	set_aid_table();
	smpu_setting();
	set_smpu_regions();
	return 0;
}
MTK_PLAT_SETUP_0_INIT(smpu_init);
