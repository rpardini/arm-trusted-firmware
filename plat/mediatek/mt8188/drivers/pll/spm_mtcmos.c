/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <platform_def.h>
#include <lib/mmio.h>
#include "spm_mtcmos.h"
#include "spm_mtcmos_internal.h"
#include "pll.h"

#ifndef BIT
#define BIT(x)		(1U << x)
#endif

#ifndef GENMASK
#define GENMASK(h, l)	(((1U << ((h) - (l) + 1)) - 1) << (l))
#endif

#define POWERON_CONFIG_EN	(SPM_BASE + 0x0000)

/* Define MTCMOS power control */
#define PWR_RST_B		BIT(0)
#define PWR_ISO			BIT(1)
#define PWR_ON			BIT(2)
#define PWR_ON_2ND		BIT(3)
#define PWR_CLK_DIS		BIT(4)
#define PWR_SRAM_CLKISO_BIT	BIT(5)
#define PWR_SRAM_ISOINT_B_BIT	BIT(6)

#define MTK_SCPD_SRAM_ISO	BIT(0)
#define MTK_SCPD_EXT_BUCK_ISO	BIT(1)

struct power_domain_data {
	char *name;
	unsigned int sta_mask;
	unsigned int ctl_offs;
	unsigned int pwr_sta_offs;
	unsigned int pwr_sta2nd_offs;
	unsigned int sram_pdn_bits;
	unsigned int sram_pdn_ack_bits;
	unsigned int ext_buck_iso_bits;
	unsigned int caps;
};

static const struct power_domain_data power_domain_data_mt8188[MT8188_POWER_DOMAIN_NR] = {
	[MT8188_POWER_DOMAIN_MFG0] = {
		.name = "MFG0",
		.sta_mask = BIT(1),
		.ctl_offs = 0x300,
		.pwr_sta_offs = 0x174,
		.pwr_sta2nd_offs = 0x178,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_MFG1] = {
		.name = "MFG1",
		.sta_mask = BIT(2),
		.ctl_offs = 0x304,
		.pwr_sta_offs = 0x174,
		.pwr_sta2nd_offs = 0x178,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_MFG2] = {
		.name = "MFG2",
		.sta_mask = BIT(3),
		.ctl_offs = 0x308,
		.pwr_sta_offs = 0x174,
		.pwr_sta2nd_offs = 0x178,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_MFG3] = {
		.name = "MFG3",
		.sta_mask = BIT(4),
		.ctl_offs = 0x30C,
		.pwr_sta_offs = 0x174,
		.pwr_sta2nd_offs = 0x178,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_MFG4] = {
		.name = "MFG4",
		.sta_mask = BIT(5),
		.ctl_offs = 0x310,
		.pwr_sta_offs = 0x174,
		.pwr_sta2nd_offs = 0x178,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_PEXTP_MAC_P0] = {
		.name = "PEXTP_MAC_P0",
		.sta_mask = BIT(10),
		.ctl_offs = 0x324,
		.pwr_sta_offs = 0x174,
		.pwr_sta2nd_offs = 0x178,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_PEXTP_PHY_TOP] = {
		.name = "PEXTP_PHY_TOP",
		.sta_mask = BIT(12),
		.ctl_offs = 0x328,
		.pwr_sta_offs = 0x174,
		.pwr_sta2nd_offs = 0x178,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_CSIRX_TOP] = {
		.name = "CSIRX_TOP",
		.sta_mask = BIT(17),
		.ctl_offs = 0x3C4,
		.pwr_sta_offs = 0x174,
		.pwr_sta2nd_offs = 0x178,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_ETHER] = {
		.name = "ETHER",
		.sta_mask = BIT(1),
		.ctl_offs = 0x338,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_HDMI_TX] = {
		.name = "HDMI_TX",
		.sta_mask = BIT(18),
		.ctl_offs = 0x37C,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_ADSP_AO] = {
		.name = "ADSP_AO",
		.sta_mask = BIT(10),
		.ctl_offs = 0x35C,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.ext_buck_iso_bits = BIT(10),
		.caps = MTK_SCPD_EXT_BUCK_ISO,
	},
	[MT8188_POWER_DOMAIN_ADSP_INFRA] = {
		.name = "ADSP_INFRA",
		.sta_mask = BIT(9),
		.ctl_offs = 0x358,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
		.caps = MTK_SCPD_SRAM_ISO,
	},
	[MT8188_POWER_DOMAIN_ADSP] = {
		.name = "ADSP",
		.sta_mask = BIT(8),
		.ctl_offs = 0x354,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
		.caps = MTK_SCPD_SRAM_ISO,
	},
	[MT8188_POWER_DOMAIN_AUDIO] = {
		.name = "AUDIO",
		.sta_mask = BIT(6),
		.ctl_offs = 0x34C,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_AUDIO_ASRC] = {
		.name = "AUDIO_ASRC",
		.sta_mask = BIT(7),
		.ctl_offs = 0x350,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_VPPSYS0] = {
		.name = "VPPSYS0",
		.sta_mask = BIT(11),
		.ctl_offs = 0x360,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_VDOSYS0] = {
		.name = "VDOSYS0",
		.sta_mask = BIT(13),
		.ctl_offs = 0x368,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_VDOSYS1] = {
		.name = "VDOSYS1",
		.sta_mask = BIT(14),
		.ctl_offs = 0x36C,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_DP_TX] = {
		.name = "DP_TX",
		.sta_mask = BIT(16),
		.ctl_offs = 0x374,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_EDP_TX] = {
		.name = "EDP_TX",
		.sta_mask = BIT(17),
		.ctl_offs = 0x378,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_VPPSYS1] = {
		.name = "VPPSYS1",
		.sta_mask = BIT(12),
		.ctl_offs = 0x364,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_WPE] = {
		.name = "WPE",
		.sta_mask = BIT(15),
		.ctl_offs = 0x370,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_VDEC0] = {
		.name = "VDEC0",
		.sta_mask = BIT(19),
		.ctl_offs = 0x380,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_VDEC1] = {
		.name = "VDEC1",
		.sta_mask = BIT(20),
		.ctl_offs = 0x384,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_VENC] = {
		.name = "VENC",
		.sta_mask = BIT(22),
		.ctl_offs = 0x38C,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_IMG_VCORE] = {
		.name = "IMG_VCORE",
		.sta_mask = BIT(28),
		.ctl_offs = 0x3A4,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.ext_buck_iso_bits = BIT(12),
		.caps = MTK_SCPD_EXT_BUCK_ISO,
	},
	[MT8188_POWER_DOMAIN_IMG_MAIN] = {
		.name = "IMG_MAIN",
		.sta_mask = BIT(29),
		.ctl_offs = 0x3A8,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_DIP] = {
		.name = "DIP",
		.sta_mask = BIT(30),
		.ctl_offs = 0x3AC,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_IPE] = {
		.name = "IPE",
		.sta_mask = BIT(31),
		.ctl_offs = 0x3B0,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_CAM_VCORE] = {
		.name = "CAM_VCORE",
		.sta_mask = BIT(27),
		.ctl_offs = 0x3A0,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.ext_buck_iso_bits = BIT(11),
		.caps = MTK_SCPD_EXT_BUCK_ISO,
	},
	[MT8188_POWER_DOMAIN_CAM_MAIN] = {
		.name = "CAM_MAIN",
		.sta_mask = BIT(24),
		.ctl_offs = 0x394,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_CAM_SUBA] = {
		.name = "CAM_SUBA",
		.sta_mask = BIT(25),
		.ctl_offs = 0x398,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
	[MT8188_POWER_DOMAIN_CAM_SUBB] = {
		.name = "CAM_SUBB",
		.sta_mask = BIT(26),
		.ctl_offs = 0x39C,
		.pwr_sta_offs = 0x16C,
		.pwr_sta2nd_offs = 0x170,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
};

/**************************************
 * for non-CPU MTCMOS
 **************************************/
void spm_mtcmos_on(enum power_domain_id ID)
{
	const struct power_domain_data *pd = &power_domain_data_mt8188[ID];
	void *ctl_reg = SPM_BASE + pd->ctl_offs;
	void *pwr_sta_reg = SPM_BASE + pd->pwr_sta_offs;
	void *pwr_sta2nd_reg = SPM_BASE + pd->pwr_sta2nd_offs;
	int i;

	/* enable SPM register control */
	spm_write(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | (0x1 << 0));

	if (pd->caps & MTK_SCPD_EXT_BUCK_ISO) {
		 spm_write(0x100063EC, spm_read(0x100063EC) & ~pd->ext_buck_iso_bits);
	}

	/* Set PWR_ON = 1 */
	spm_write(ctl_reg, spm_read(ctl_reg) | PWR_ON);
	/* Set PWR_ON_2ND = 1 */
	spm_write(ctl_reg, spm_read(ctl_reg) | PWR_ON_2ND);
	/* Wait until PWR_STATUS = 1 and PWR_STATUS_2ND = 1 */
	i = 0;
	while (((spm_read(pwr_sta_reg) & pd->sta_mask) != pd->sta_mask)
		|| ((spm_read(pwr_sta2nd_reg) & pd->sta_mask) != pd->sta_mask)) {
		udelay(50);
		i++;
		if(i > 20) {
			printf("error: mtcmos %s sta_mask: 0x%08x != 1(pwr_sta: 0x%08x, pwr_sta2nd: 0x%08x)\n",
				pd->name, pd->sta_mask, spm_read(pwr_sta_reg), spm_read(pwr_sta2nd_reg));
			break;
		}
	}

	/* Set PWR_CLK_DIS = 0 */
	spm_write(ctl_reg, spm_read(ctl_reg) & ~PWR_CLK_DIS);
	/* Set PWR_ISO = 0 */
	spm_write(ctl_reg, spm_read(ctl_reg) & ~PWR_ISO);
	/* Set PWR_RST_B = 1 */
	spm_write(ctl_reg, spm_read(ctl_reg) | PWR_RST_B);
	/* Set SRAM_PDN = 0 */
	spm_write(ctl_reg, spm_read(ctl_reg) & ~pd->sram_pdn_bits);
	/* Wait until SRAM_PDN_ACK = 0 */
	i = 0;
	while (spm_read(ctl_reg) & pd->sram_pdn_ack_bits) {
		udelay(50);
		i++;
		if(i > 20) {
			printf("error: mtcmos %s sram_pdn_ack: 0x%08x != 0(ctl_reg: 0x%08x)\n",
				pd->name, pd->sram_pdn_ack_bits, spm_read(ctl_reg));
			break;
		}
	}

	if (pd->caps & MTK_SCPD_SRAM_ISO) {
		spm_write(ctl_reg, spm_read(ctl_reg) | PWR_SRAM_ISOINT_B_BIT);
		udelay(1);
		spm_write(ctl_reg, spm_read(ctl_reg) & ~PWR_SRAM_CLKISO_BIT);
	}
}

void spm_mtcmos_off(enum power_domain_id ID)
{
	const struct power_domain_data *pd = &power_domain_data_mt8188[ID];
	void *ctl_reg = SPM_BASE + pd->ctl_offs;
	void *pwr_sta_reg = SPM_BASE + pd->pwr_sta_offs;
	void *pwr_sta2nd_reg = SPM_BASE + pd->pwr_sta2nd_offs;
	int i;

	if (pd->caps & MTK_SCPD_SRAM_ISO) {
		spm_write(ctl_reg, spm_read(ctl_reg) | PWR_SRAM_CLKISO_BIT);
		udelay(1);
		spm_write(ctl_reg, spm_read(ctl_reg) & ~ PWR_SRAM_ISOINT_B_BIT);
	}

	/* Set SRAM_PDN = 1 */
	spm_write(ctl_reg, spm_read(ctl_reg) | pd->sram_pdn_bits);
	/* Wait until SRAM_PDN_ACK = 1 */
	i = 0;
	while (!(spm_read(ctl_reg) & pd->sram_pdn_ack_bits)) {
		udelay(50);
		i++;
		if(i > 20) {
			printf("error: mtcmos %s sram_pdn_ack: 0x%08x != 0(ctl_reg: 0x%08x)\n",
				pd->name, pd->sram_pdn_ack_bits, spm_read(ctl_reg));
			break;
		}
	}

	/* Set PWR_RST_B = 0 */
	spm_write(ctl_reg, spm_read(ctl_reg) & ~PWR_RST_B);
	/* Set PWR_ISO = 1 */
	spm_write(ctl_reg, spm_read(ctl_reg) | PWR_ISO);
	/* Set PWR_CLK_DIS = 1 */
	spm_write(ctl_reg, spm_read(ctl_reg) | PWR_CLK_DIS);

	/* Set PWR_ON_2ND = 0 */
	spm_write(ctl_reg, spm_read(ctl_reg) & ~PWR_ON_2ND);
	/* Set PWR_ON = 0 */
	spm_write(ctl_reg, spm_read(ctl_reg) & ~PWR_ON);
	/* Wait until PWR_STATUS = 0 and PWR_STATUS_2ND = 0 */
	i = 0;
	while ((spm_read(pwr_sta_reg) & pd->sta_mask)
		|| (spm_read(pwr_sta2nd_reg) & pd->sta_mask)) {
		udelay(50);
		i++;
		if(i > 20) {
			printf("error: mtcmos %s sta_mask: 0x%08x != 1(pwr_sta: 0x%08x, pwr_sta2nd: 0x%08x)\n",
				pd->name, pd->sta_mask, spm_read(pwr_sta_reg), spm_read(pwr_sta2nd_reg));
			break;
		}
	}
}

void spm_mtcmos_on_all(void)
{
	int i;

	for (i = 0; i < MT8188_POWER_DOMAIN_NR; i++)
		spm_mtcmos_on(i);
}
