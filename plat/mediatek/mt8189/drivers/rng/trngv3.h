/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#ifndef RNG_H
#define RNG_H

#include <lib/utils_def.h>

#define MTK_TIMEOUT_POLL     2000

#define MTK_RETRY_CNT        50

#define MTK_TRNG_REG_SIZE    0x1000
#define RNG_DEFAULT_CUTOFF   0x09C8AE1A
#define RNG_SEED_NUM         2

/*******************************************************************************
 * TRNG related constants
 ******************************************************************************/
#define RNG_STATUS          (TRNG_BASE + 0x0004)
#define RNG_SWRST           (TRNG_BASE + 0x0010)
#define RNG_IRQ_CFG         (TRNG_BASE + 0x0014)
#define RNG_EN              (TRNG_BASE + 0x0020)
#define RNG_HTEST           (TRNG_BASE + 0x0028)
#define RNG_OUT             (TRNG_BASE + 0x0030)
#define RNG_RAW             (TRNG_BASE + 0x0038)
#define RNG_SRC             (TRNG_BASE + 0x0050)

#define RAW_VALID           BIT(12)
#define DRBG_VALID          BIT(4)
#define RAW_EN              BIT(8)
#define NRBG_EN             BIT(4)
#define DRBG_EN             BIT(0)
#define IRQ_EN              BIT(0)
#define SWRST_B             BIT(0)
/* Error conditions */
#define RNG_ERROR           GENMASK_32(31, 24)
#define APB_ERROR           BIT(16)

/* External swrst */
#define TRNG_SWRST_SET_REG	(INFRACFG_BASE + 0xF60)
#define TRNG_SWRST_CLR_REG	(INFRACFG_BASE + 0xF64)
#define TRNG_SWRST_BIT		(0x1 << 22)

#endif /* RNG_H */
