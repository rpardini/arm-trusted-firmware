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
void mt_mem_init(void);
unsigned long long platform_memory_size(void);
