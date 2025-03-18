/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 MediaTek Inc.
 */

#ifndef PMIC_WRAP_INIT_H
#define PMIC_WRAP_INIT_H

#include <stdint.h>

#include "platform_def.h"
#include <pmic_wrap_init_common.h>

static struct mt8189_pmic_wrap_regs *const mtk_pwrap = (void *)PMIC_WRAP_BASE;

/* PMIC_WRAP registers */
struct mt8189_pmic_wrap_regs {
	uint32_t init_done;
	uint32_t reserved[543];
	uint32_t wacs2_cmd;
	uint32_t wacs2_wdata;
	uint32_t reserved1[3];
	uint32_t wacs2_rdata;
	uint32_t reserved2[3];
	uint32_t wacs2_vldclr;
	uint32_t wacs2_sta;
};

#endif /* PMIC_WRAP_INIT_H */
