/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2024 MediaTek Inc.
 */

#pragma once

#include <stdint.h>

enum spmi_master {
	SPMI_MASTER_0 = 0,
	SPMI_MASTER_1,
	SPMI_MASTER_P_1,
	SPMI_MASTER_MAX
};

typedef enum {
	TYPE_DDR1 = 1,
	TYPE_DDR2,
	TYPE_DDR3,
	TYPE_DDR4,
	TYPE_DDR5,
	TYPE_LPDDR2,
	TYPE_LPDDR3,
	TYPE_PCDDR3,
	TYPE_LPDDR4,
	TYPE_LPDDR4X,
	TYPE_LPDDR4P,
	TYPE_LPDDR5,
	TYPE_LPDDR5X,
	TYPE_MAX,
} DRAM_DRAM_TYPE_T;

//==============================================================================
// PMIC Exported Function
//==============================================================================
extern uint32_t pmic_read_interface(uint32_t RegNum, uint32_t *val, uint32_t MASK, uint32_t SHIFT);
extern uint32_t pmic_config_interface(uint32_t RegNum, uint32_t val, uint32_t MASK, uint32_t SHIFT);

int i2c_hw_init(void);
int pmifclkmgr_init(void);
int pmif_spmi_init(int mstid);
int pwrap_init_preloader(void);
unsigned int pmic_init(void);
unsigned int mt_get_dram_type(void);
void mt_mem_init(void);
unsigned long long platform_memory_size(void);
