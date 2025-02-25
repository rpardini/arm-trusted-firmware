/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 MediaTek Inc.
 */

#ifndef PMIC_WRAP_INIT_H
#define PMIC_WRAP_INIT_H

#include <stdint.h>

#include "platform_def.h"
#include <pmic_wrap_init_common.h>

#define PWRAP_DEBUG				0
#define PMIF_SPI_SWINF_NO			2

static struct mt8189_pmic_wrap_regs *const mtk_pwrap = (void *)PMIC_WRAP_BASE;

/* PMIC_WRAP registers */
struct mt8189_pmic_wrap_regs {
	uint32_t init_done;
	uint32_t reserved[511];
	struct {
		uint32_t cmd;
		uint32_t wdata;
		uint32_t reserved1[3];
		uint32_t rdata;
		uint32_t reserved2[3];
		uint32_t vldclr;
		uint32_t sta;
		uint32_t reserved3[5];
	} wacs[4];
};

#endif /* PMIC_WRAP_INIT_H */
