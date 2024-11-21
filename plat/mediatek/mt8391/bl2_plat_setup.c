// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#include <stdint.h>
#include <lib/mmio.h>
#include <common/debug.h>
#include "drivers/generic_delay_timer.h"
#include "drivers/io/io_block.h"
#include "drivers/mmc.h"
#include "drivers/partition/partition.h"
#include "plat/common/platform.h"
#include "platform_def.h"
#include "mtk_bl2_common.h"
#include "mmc/mtk-sd.h"
#include "wdt.h"
#include "pll.h"
#include "libdram.h"

uint32_t g_ddr_reserve_enable;
uint32_t g_ddr_reserve_success;

static uint32_t buf_in_sram[PLAT_PARTITION_BLOCK_SIZE / sizeof(uint32_t)];
static io_block_dev_spec_t *boot_dev_spec;

static struct msdc_compatible msdc_compat = {
	.clk_div_bits = 12,
	.pad_tune0 = true,
	.async_fifo = true,
	.data_tune = true,
	.busy_check = true,
	.stop_clk_fix = true,
	.enhance_rx = true,
	.use_dma_mode = true,
	.top_base = MSDC0_TOP_BASE,
};

static io_block_dev_spec_t emmc_dev_spec = {
	.buffer = {
		.offset = (size_t)buf_in_sram,
		.length = PLAT_PARTITION_BLOCK_SIZE,
	},
	.ops = {
		.read = mmc_read_blocks,
		.write = mmc_write_blocks,
	},
	.block_size = MMC_BLOCK_SIZE,
};

uint32_t mt_get_storage_type(void)
{
	uint32_t storage_trap_type = STORAGE_UNKNOWN;
	uint32_t gpio_trap_value;

	gpio_trap_value = (mmio_read_32(GPIO_TRAPPING_REG) >> TRAP_BOOTDEV_BIT) & 0x3;
	NOTICE("%s : 0x%x\n", __func__, gpio_trap_value);
	switch (gpio_trap_value) {
	case 0:
		storage_trap_type = STORAGE_UFS;
		NOTICE("storage_tpye: UFS\n");
		break;
	case 1:
		storage_trap_type = STORAGE_SPINOR;
		NOTICE("storage_tpye: SPINOR\n");
		break;
	case 2:
		storage_trap_type = STORAGE_EMMC;
		NOTICE("storage_tpye: EMMC\n");
		break;
	default:
		NOTICE("Wrong Storage Type\n");
		break;
	}

	return storage_trap_type;
}

void bl2_platform_setup(void)
{
	uint32_t storage_type;

	generic_delay_timer_init();
	mtk_wdt_init();

	mt_pll_init();

	pmifclkmgr_init();
	pmif_spmi_init(SPMI_MASTER_P_1);
	pwrap_init_preloader();

	i2c_hw_init();
	pmic_init();

	storage_type = mt_get_storage_type();
	if (storage_type == STORAGE_EMMC) {
		boot_dev_spec = &emmc_dev_spec;
		mtk_mmc_init(MSDC0_BASE, &msdc_compat, 400000000);
	}

	mtk_io_setup((uintptr_t)boot_dev_spec);
	load_partition_table(GPT_IMAGE_ID);

	mt_mem_init();

	/* change storage read buffer to DRAM */
	boot_dev_spec->buffer.offset = 0x41000000;
	boot_dev_spec->buffer.length = 0x1000000;
}
