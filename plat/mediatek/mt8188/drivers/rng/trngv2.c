/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include <common/debug.h>
#include <lib/mmio.h>
#include <drivers/delay_timer.h>
#include <mtk_mmap_pool.h>
#include <platform_def.h>

#include <lib/smccc.h>
#include <services/trng_svc.h>
#include <smccc_helpers.h>

#include <plat/common/platform.h>

#include "trngv2.h"

static const mmap_region_t rng_mmap[] MTK_MMAP_SECTION = {
	MAP_REGION_FLAT(TRNG_BASE, MTK_TRNG_REG_SIZE,
MT_DEVICE | MT_RW | MT_SECURE),
	{0}
};
DECLARE_MTK_MMAP_REGIONS(rng_mmap);

DEFINE_SVC_UUID2(_plat_trng_uuid,
	0xf6b2c8d9, 0x1abb, 0x4d83, 0xb2, 0x3f,
	0x5c, 0x51, 0xb6, 0xef, 0xfc, 0xaf
);
uuid_t plat_trng_uuid;

static uint32_t plat_trng_external_swrst(void)
{
	/* external swrst to reset whole rng module */
	mmio_setbits_32(TRNG_SWRST_SET_REG, RNG_SWRST_B);
	mmio_setbits_32(TRNG_SWRST_CLR_REG, RNG_SWRST_B);

	/* disable irq */
	mmio_clrbits_32(RNG_IRQ_CFG, IRQ_EN);
	/* set default cutoff value */
	mmio_write_32(RNG_HTEST, RNG_DEFAULT_CUTOFF);
	/* enable rng */
	mmio_setbits_32(RNG_EN, DRBG_EN | NRBG_EN);

	return 0;
}

/* Get random number frorm hwrng to return 8 bytes of entropy,returns 'true'
 * when random value generated successfully, otherwise 'false'.
 */
bool plat_get_entropy(uint64_t *out)
{
	uint64_t time = timeout_init_us(MTK_TIMEOUT_POLL);
	uint32_t retry_times = 0;
	uint32_t seed[2] = { 0 };
	int i = 0;

	assert(out);
	assert(!check_uptr_overflow((uintptr_t)out, sizeof(*out)));

	/* Disable interrupt mode */
	mmio_clrbits_32(RNG_IRQ_CFG, IRQ_EN);
	/* Set rng health test cutoff value */
	mmio_write_32(RNG_HTEST, RNG_DEFAULT_CUTOFF);
	/* enable rng module */
	mmio_setbits_32(RNG_EN, (DRBG_EN | NRBG_EN));

	for (i = 0; i < 2; i++) {
		while (!(mmio_read_32(RNG_STATUS) & DRBG_VALID)) {
			if (mmio_read_32(RNG_STATUS) & (RNG_ERROR | APB_ERROR)) {
				mmio_clrbits_32(RNG_EN, (DRBG_EN | NRBG_EN));

				mmio_clrbits_32(RNG_SWRST, SWRST_B);
				mmio_setbits_32(RNG_SWRST, SWRST_B);

				mmio_setbits_32(RNG_EN, (DRBG_EN | NRBG_EN));
			}

			if (timeout_elapsed(time)) {
				plat_trng_external_swrst();
				time = timeout_init_us(MTK_TIMEOUT_POLL);
				retry_times++;
			}

			if (retry_times > MTK_RETRY_CNT) {
				ERROR("%s: trng NOT ready\n", __func__);
				return false;
			}
		}
		seed[i] = mmio_read_32(RNG_OUT);
	}
	/* Output 8 bytes entropy combine with 2 32-bit random number. */
	*out = ((uint64_t)seed[0] << 32) | seed[1];

	return true;
}

void plat_entropy_setup(void)
{
	uint64_t dummy;

	plat_trng_uuid = _plat_trng_uuid;

	/* Initialise the entropy source and trigger RNG generation */
	plat_get_entropy(&dummy);
}
