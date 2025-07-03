// SPDX-License-Identifier: GPL-2.0-only OR MIT

/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#include <common/debug.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <api_hre_mminfra.h>

void mminfra_hre_bkrs(enum mminfra_hre_cfg select)
{
	uintptr_t base = MMINFRA_BUS_HRE_BASE;

	switch (select) {
	case INIT_HRE_BKRS_CONFIG:
		/* wdt timer ctrl */
		mmio_write_32(base + 0xEC, 0x0000008BU);
		/* wdt src en */
		mmio_write_32(base + 0xE8, 0xFFFFFFFFU);
		/* Ignore 0x20 for reg update during bkrs */
		mmio_write_32(base + 0x110, 0x0FFFFFDFU);

		/* hre ctrl */
		mmio_write_32(base + 0x0, 0x0000000BU);
		/* hre ip addr base */
		mmio_write_32(base + 0x4, 0x1E800000U);
		mmio_write_32(base + 0x8, 0x00000000U);
		mmio_write_32(base + 0xC, 0x30000);
		/* hre seq en */
		mmio_write_32(base + 0xC4, 0x7FF);

		/* hre count0 */
		mmio_write_32(base + 0xB0, 0x0C020340U);
		mmio_write_32(base + 0xB4, 0x02087780U);
		mmio_write_32(base + 0xB8, 0x01013909U);
		mmio_write_32(base + 0xBC, 0x0B020101U);
		mmio_write_32(base + 0xC0, 0x00000404U);
		mmio_write_32(base + 0xC8, 0x00000000U);
		mmio_write_32(base + 0xCC, 0x00000000U);

		/* hre bank0 vmid0~23+uid2aid, 64 registers(count0) */
		mmio_write_32(base + 0x10, 0x1E800800U);
		/* dom2aid_en/aidheader/uidheader, 3 registers */
		mmio_write_32(base + 0x14, 0x1E800900U);
		/* mminfra dapc apccon, 2 registers */
		mmio_write_32(base + 0x18, 0x1E820F00U);
		/* mminfra dapc dom remap, 12 registers */
		mmio_write_32(base + 0x1C, 0x1E820800U);
		/* mminfra dapc sys0 dom0~7 apc, 128 registers(count1) */
		mmio_write_32(base + 0x20, 0x1E820000U);
		/* mminfra dapc sys1 dom0~7 apc, 119 registers, 1D8 is the last one */
		mmio_write_32(base + 0x24, 0x1E821000U);
		/* mminfra dapc sys0 apc lock0~7, 8 registers */
		mmio_write_32(base + 0x28, 0x1E820C00U);
		/* mminfra dapc sys1 apc lock0~1, 2 registers */
		mmio_write_32(base + 0x2C, 0x1E821C00U);
		/* smi common bw setting, 9 registers(count2) */
		mmio_write_32(base + 0x30, 0x1E801100U);
		/* smi common 0x220~0x300, 57 registers */
		mmio_write_32(base + 0x34, 0x1E801220U);
		/* smi mdp subcommon0, 1 register */
		mmio_write_32(base + 0x38, 0x1E809100U);
		/* smi mdp subcommon1, 1 register */
		mmio_write_32(base + 0x3C, 0x1E80A100U);
		/* ssc0 smi subcommon, 1 register(count3) */
		mmio_write_32(base + 0x40, 0x1E807100U);
		/* ssc1 smi subcommon, 1 register */
		mmio_write_32(base + 0x44, 0x1E808100U);
		/* dapc pdn apccon, 2 registers */
		mmio_write_32(base + 0x48, 0x1E826F00U);
		/* dapc VIO MASK, 11 registers */
		mmio_write_32(base + 0x4C, 0x1E826000U);
		/* gce_d ctrl value, 4 registers(count4) */
		mmio_write_32(base + 0x50, 0x1E980048U);
		/* gce_m ctrl value, 4 registers */
		mmio_write_32(base + 0x54, 0x1E990048U);

		/* hre bank0 sram addr setting */
		mmio_write_32(base + 0x60, 0x5B8U);
		mmio_write_32(base + 0x64, 0x6B8U);
		mmio_write_32(base + 0x68, 0x6C4U);
		mmio_write_32(base + 0x6C, 0x6CCU);
		mmio_write_32(base + 0x70, 0x6fcU);
		mmio_write_32(base + 0x74, 0x8FCU);
		mmio_write_32(base + 0x78, 0xAD8U);
		mmio_write_32(base + 0x7C, 0xAF8U);
		mmio_write_32(base + 0x80, 0xB00U);
		mmio_write_32(base + 0x84, 0xB24U);
		mmio_write_32(base + 0x88, 0xC08U);
		mmio_write_32(base + 0x8C, 0xC0CU);
		mmio_write_32(base + 0x90, 0xC10U);
		mmio_write_32(base + 0x94, 0xC14U);
		mmio_write_32(base + 0x98, 0xC18U);
		mmio_write_32(base + 0x9C, 0xC20U);
		mmio_write_32(base + 0xA0, 0xC4CU);
		mmio_write_32(base + 0xA4, 0xC5CU);
		break;
	case SAVE_HRE_BK_CONFIG:
		/* Save backup settings */
		mmio_write_32(base + 0xd0, 0x00000001U);
		/* TINFO = "Wait for HRE Save Idle" */
		while (mmio_read_32(base + 0x1EC) != 0x0)
			;
		mmio_write_32(base + 0xD0, 0x00000000U);
		break;
	case SAVE_HRE_RS_CONFIG:
		/* Save restore settings */
		mmio_write_32(base + 0xD4, 0x00000001U);
		/* Wait for HRE Save Idle */
		while (mmio_read_32(base + 0x1EC) != 0x0)
			;
		mmio_write_32(base + 0xD4, 0x00000000U);
		break;
	default:
		NOTICE("%s: undefined type %u\n", __func__, select);
		break;
	}
}
