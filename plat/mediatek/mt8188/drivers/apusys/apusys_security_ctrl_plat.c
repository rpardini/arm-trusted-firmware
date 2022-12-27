/*
 * Copyright (c) 2021, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <lib/mmio.h>
#include <common/debug.h>
#include "security_ctrl/1.0/apusys_security_ctrl.h"
#include "apusys_security_ctrl_plat.h"

static void apusys_domain_remap_init(void)
{
	const uint32_t remap_domains[] = DOMAIN_REMAP(
		0, 1, 2, 3, 4, 5, 6, 14, 8, 9, 10, 11, 12, 13, 14, 15);
	uint32_t lower_domain = 0;
	uint32_t higher_domain = 0;
	int i;

	INFO("%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(remap_domains); i++) {
		if (i < REG_DOMAIN_NUM)
			lower_domain = lower_domain | (remap_domains[i] << (i * 4));
		else
			higher_domain = higher_domain |
				(remap_domains[i] << ((i - REG_DOMAIN_NUM) * 4));
	}

	mmio_write_32(SOC2APU_SET1_0, lower_domain);
	mmio_write_32(SOC2APU_SET1_1, higher_domain);
	mmio_write_32(APU_SEC_CON, mmio_read_32(APU_SEC_CON) | (0x1 << DOMAIN_REMAP_SEL));
}

int32_t apusys_security_ctrl_init(void)
{
	apusys_domain_remap_init();

	return 0;
}
