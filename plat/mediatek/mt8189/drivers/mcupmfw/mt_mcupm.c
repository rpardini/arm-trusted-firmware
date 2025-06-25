// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#include <stddef.h>
#include <string.h>
#include <drivers/delay_timer.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <mcu.h>
#include <lib/mtk_init/mtk_init.h>
#include "mt_spm_reg.h"
#include "mt_mcupm.h"

#define ABNORMALBOOT_REG			MCUPM_ABNORMAL_BOOT
#define WARMBOOT_REG MCUPM_WARM_BOOT
#define DEVAPC_INFRA_SECU_AO_SEC_REPLACE_0	(DEVAPC_INFRA_SECU_AO_BASE + 0x0300)
#define REPLACE_15				BIT(15)

#define MCUPM_SW_RSTN				(MCUPM_CFG_BASE + 0x000)
#define POWERON_CONFIG_EN			(SPM_BASE + 0x0000)
#define CPUEB_PWR_CON				(SPM_BASE + 0xEB0)

#define CPUEB_PWR_RST_B_LSB			BIT(0)
#define CPUEB_PWR_ISO_LSB			BIT(1)
#define CPUEB_PWR_ON_LSB			BIT(2)
#define CPUEB_PWR_ON_2ND_LSB			BIT(3)
#define CPUEB_PWR_CLK_DIS_LSB			BIT(4)
#define CPUEB_SRAM_CKISO_LSB			BIT(5)
#define CPUEB_SRAM_ISOINT_B_LSB			BIT(6)
#define CPUEB_SRAM_PDN_LSB			BIT(8)
#define CPUEB_SRAM_SLEEP_B_LSB			BIT(9)
#define SC_CPUEB_SRAM_PDN_ACK_LSB		BIT(12)
#define SC_CPUEB_SRAM_SLEEP_B_ACK_LSB		BIT(13)
#define SC_CPUEB_PWR_ACK_LSB			BIT(30)
#define SC_CPUEB_PWR_ACK_2ND_LSB		BIT(31)

/* Clear abnormal boot register */
#define ABNORMALBOOT_REG_STATUS			0x0
#define WARMBOOT_REG_STATUS			0x0
#define MCUPM_RSTN_RESET			0x1F

#define MCUPM_RSTN_RSTN_INIT			0x0
#define MCUPM_CFGREG_SW_RSTN_SW_RSTN		BIT(0)
#define MCUPM_CFGREG_SW_RSTN_DMA_BUSY_MASK	BIT(1)

static void reset_mcupm(struct mtk_mcu *mcu)
{
	mmio_write_32(ABNORMALBOOT_REG, ABNORMALBOOT_REG_STATUS);
	mmio_write_32(WARMBOOT_REG, WARMBOOT_REG_STATUS);
	mmio_write_32(MCUPM_SW_RSTN, MCUPM_RSTN_RESET);
}

static struct mtk_mcu mcupm = {
	.firmware_name = "mcupm.bin",
	.load_buffer = mcupm_bin,
	.run_address = (void *)MCUPM_SRAM_BASE,
	.reset = reset_mcupm,
};

int mcupm_init(void)
{
	mcupm.run_size = mcupm_bin_len;

	/* Set CPUEB as secure master */
	mmio_setbits_32(POWERON_CONFIG_EN, REPLACE_15);

	/* Unlock SPM POWERON_CONFIG_EN */
	mmio_setbits_32(POWERON_CONFIG_EN, SPM_REGWR_CFG_KEY | BCLK_CG_EN_LSB);

	/* Reinit CPUEB_PWR_CON */
	/* (1) CPUEB MTCMOS */
	mmio_setbits_32(CPUEB_PWR_CON, CPUEB_PWR_ON_LSB);
	mmio_setbits_32(CPUEB_PWR_CON, CPUEB_PWR_ON_2ND_LSB);
	while ((mmio_read_32(CPUEB_PWR_CON) &
	       (SC_CPUEB_PWR_ACK_LSB | SC_CPUEB_PWR_ACK_2ND_LSB)) !=
	       (SC_CPUEB_PWR_ACK_LSB | SC_CPUEB_PWR_ACK_2ND_LSB))
		;

	mmio_clrbits_32(CPUEB_PWR_CON, CPUEB_PWR_CLK_DIS_LSB);
	mmio_clrbits_32(CPUEB_PWR_CON, CPUEB_PWR_ISO_LSB);
	mmio_setbits_32(CPUEB_PWR_CON, CPUEB_PWR_RST_B_LSB);
	INFO("%s MCUPM MTCMOS Power on success\n", __func__);

	/* (2) SRAM leave DORMANT mode */
	mmio_setbits_32(CPUEB_PWR_CON, CPUEB_SRAM_SLEEP_B_LSB);
	while (!(mmio_read_32(CPUEB_PWR_CON) & SC_CPUEB_SRAM_SLEEP_B_ACK_LSB))
		;

	udelay(1);
	mmio_setbits_32(CPUEB_PWR_CON, CPUEB_SRAM_ISOINT_B_LSB);
	mmio_clrbits_32(CPUEB_PWR_CON, CPUEB_SRAM_CKISO_LSB);

	/* Power on MCUPM sram */
	mmio_clrbits_32(CPUEB_PWR_CON, CPUEB_SRAM_PDN_LSB);
	do {
	} while ((mmio_read_32(CPUEB_PWR_CON) & 0x1000) == 0x1000);

	mmio_write_32(MCUPM_SW_RSTN, MCUPM_RSTN_RSTN_INIT);
	if (mtk_init_mcu(&mcupm)) {
		ERROR("%s() failed\n", __func__);
		return -1;
	}

	mmio_setbits_32(MCUPM_SW_RSTN,
		  MCUPM_CFGREG_SW_RSTN_SW_RSTN | MCUPM_CFGREG_SW_RSTN_DMA_BUSY_MASK);

	INFO("mcupm: MCUPM_SRAM_GPR0: 0x%x, 0x%x\n",
	       MCUPM_INFO, mmio_read_32(MCUPM_INFO));
	INFO("mcupm: MCUPM_SRAM_GPR1: 0x%x, 0x%x\n",
	       MCUPM_ABNORMAL_BOOT, mmio_read_32(MCUPM_ABNORMAL_BOOT));
	INFO("mcupm: MCUPM_SRAM_GPR23: 0x%x, 0x%x\n",
	       MCUPM_WARM_BOOT, mmio_read_32(MCUPM_WARM_BOOT));
	INFO("mcupm: MCUPM_SW_RSTN: 0x%x, 0x%x\n",
	       MCUPM_SW_RSTN, mmio_read_32(MCUPM_SW_RSTN));
	INFO("mcupm: MCUPM_SRAM_BASE: 0x%x, 0x%x\n",
	       MCUPM_SRAM_BASE, mmio_read_32(MCUPM_SRAM_BASE));
	INFO("%s MCUPM part. load & reset finished\n", __func__);

	return 0;
}

MTK_ARCH_INIT(mcupm_init);
