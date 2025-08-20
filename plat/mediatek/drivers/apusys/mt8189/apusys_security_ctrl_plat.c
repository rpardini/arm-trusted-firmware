/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <lib/mmio.h>
#include <common/debug.h>
#include "../security_ctrl/1.0/apusys_security_ctrl.h"
#include "apusys_security_ctrl_plat.h"

#define bits_clr(x, m, o)       (x & (~(m << o)))
#define bits_get(x, m, o)       ((x & (m << o)) >> o)
#define bits_set(x, v, m, o)    ((bits_clr(x, m, o)) | ((v & m) << o))

static void sec_sideband_init(void)
{
	uint32_t value = mmio_read_32(SEC_CTRL_SIDE_BAND);

	INFO("%s\n", __func__);

	value = bits_set(value, SEC_CTRL_NARE_DOMAIN,
		SEC_CTRL_DOMAIN_MASK, SEC_CTRL_NARE_DOMAIN_SHF);
	value = bits_set(value, SEC_CTRL_NARE_NS,
		SEC_CTRL_NS_MASK, SEC_CTRL_NARE_NS_SHF);
	value = bits_set(value, SEC_CTRL_SARE0_DOMAIN,
		SEC_CTRL_DOMAIN_MASK, SEC_CTRL_SARE0_DOMAIN_SHF);
	value = bits_set(value, SEC_CTRL_SARE0_NS,
		SEC_CTRL_NS_MASK, SEC_CTRL_SARE0_NS_SHF);
	value = bits_set(value, SEC_CTRL_SARE1_DOMAIN,
		SEC_CTRL_DOMAIN_MASK, SEC_CTRL_SARE1_DOMAIN_SHF);
	value = bits_set(value, SEC_CTRL_SARE1_NS,
		SEC_CTRL_NS_MASK, SEC_CTRL_SARE1_NS_SHF);

	mmio_write_32(SEC_CTRL_SIDE_BAND, value);
}

static void vmid_init(void)
{
	const uint32_t vmids[] = DOMAIN_VMID_MAP(
		0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	uint32_t vmid = 0;
	int i;

	INFO("%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(vmids); i++)
		vmid = vmid | (vmids[i] << (i * 2));

	mmio_write_32(SEC_CTRL_VMID_MAP, vmid);
}

static void domain_remap_init(void)
{
	const uint32_t remap_domains[] = DOMAIN_REMAP(
		0, 1, 2, 3, 4, 6, 6, 6, 8, 9, 10, 11, 12, 13, 6, 15);
	uint32_t lower_domain = 0;
	uint32_t higher_domain = 0;
	int i;

	INFO("%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(remap_domains); i++) {
		if (i < SEC_CTRL_REG_DOMAIN_NUM)
			lower_domain = lower_domain | (remap_domains[i] << (i * 4));
		else
			higher_domain = higher_domain |
				(remap_domains[i] << ((i - SEC_CTRL_REG_DOMAIN_NUM) * 4));
	}

	mmio_write_32(SEC_CTRL_SOC2APU_SET1_0, lower_domain);
	mmio_write_32(SEC_CTRL_SOC2APU_SET1_1, higher_domain);
	mmio_write_32(APU_SEC_CON, mmio_read_32(APU_SEC_CON) |
		(0x1U << SEC_CTRL_DOMAIN_REMAP_SEL));
}

int32_t apusys_security_ctrl_init(void)
{
	domain_remap_init();
	vmid_init();
	sec_sideband_init();

	return 0;
}
