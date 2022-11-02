/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>

#include <common/bl_common.h>
#include <common/debug.h>
#include <drivers/console.h>
#include <drivers/generic_delay_timer.h>
#include <plat/common/common_def.h>
#include <lib/mmio.h>
#include <drivers/io/io_driver.h>
#include <drivers/io/io_block.h>
#include <assert.h>
#include <drivers/mmc.h>
#include <tools_share/firmware_image_package.h>
#include <common/tbbr/tbbr_img_def.h>
#include <drivers/io/io_fip.h>
#include <drivers/partition/partition.h>
#include <common/bl_common.h>
#include <common/desc_image_load.h>
#include <plat/common/platform.h>
#include <platform_def.h>
#include <uart.h>
#include <mmc/mtk-sd.h>


void pwrap_init(void);
void mt_mem_init(void);

static struct msdc_compatible mt8188_compat = {
	.clk_div_bits = 12,
	.pad_tune0 = true,
	.async_fifo = true,
	.data_tune = true,
	.busy_check = true,
	.stop_clk_fix = true,
};

static bl_mem_params_node_t bl2_mem_params_descs[] = {
	/* Fill BL31 related information */
	{
		.image_id = BL31_IMAGE_ID,

		SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
			VERSION_2, entry_point_info_t,
			SECURE | EXECUTABLE | EP_FIRST_EXE),
		.ep_info.pc = BL31_BASE,
		.ep_info.spsr = SPSR_64(MODE_EL3, MODE_SP_ELX,
			DISABLE_ALL_EXCEPTIONS),

		SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
			VERSION_2, image_info_t, IMAGE_ATTRIB_PLAT_SETUP),
		.image_info.image_base = BL31_BASE,
		.image_info.image_max_size = BL31_LIMIT - BL31_BASE,

	#ifdef NEED_BL32
		.next_handoff_image_id = BL32_IMAGE_ID,
	#else
		.next_handoff_image_id = BL33_IMAGE_ID,
	#endif
	},
#ifdef NEED_BL32
	/* Fill BL32 related information */
	{
		.image_id = BL32_IMAGE_ID,

		SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
			VERSION_2, entry_point_info_t, SECURE | EXECUTABLE),
		.ep_info.pc = BL32_BASE,

		SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
			VERSION_2, image_info_t, 0),
		.image_info.image_base = BL32_BASE - BL32_HEADER_SIZE,
		.image_info.image_max_size = BL32_LIMIT,

		.next_handoff_image_id = BL33_IMAGE_ID,
	},
#endif
	/* Fill BL33 related information */
	{
		.image_id = BL33_IMAGE_ID,
		SET_STATIC_PARAM_HEAD(ep_info, PARAM_EP,
			VERSION_2, entry_point_info_t, NON_SECURE | EXECUTABLE),
		.ep_info.pc = BL33_BASE,
		.ep_info.spsr = SPSR_64(MODE_EL2, MODE_SP_ELX,
			DISABLE_ALL_EXCEPTIONS),

		SET_STATIC_PARAM_HEAD(image_info, PARAM_EP,
			VERSION_2, image_info_t, 0),
		.image_info.image_base = BL33_BASE,
		.image_info.image_max_size = 0x200000 /* 2MB */,

		.next_handoff_image_id = INVALID_IMAGE_ID,
	}
};
REGISTER_BL_IMAGE_DESCS(bl2_mem_params_descs)

struct plat_io_policy {
	uintptr_t *dev_handle;
	uintptr_t image_spec;
	int (*check)(const uintptr_t spec);
};

static int check_storage(const uintptr_t spec);
static int check_fip(const uintptr_t spec);


uint32_t g_ddr_reserve_enable = 0;
uint32_t g_ddr_reserve_success = 0;

static uintptr_t storage_dev_handle;
static const io_dev_connector_t *storage_dev_con;
static const io_dev_connector_t *fip_dev_con;
static uintptr_t fip_dev_handle;

static const io_block_dev_spec_t emmc_dev_spec = {
	.buffer = {
		.offset = 0x41000000,
		.length = 0x1000000,
	},
	.ops = {
		.read = mmc_read_blocks,
	},
	.block_size = MMC_BLOCK_SIZE,
};

static const io_block_spec_t emmc_gpt_spec = {
	.offset	= 0,
	.length	= PLAT_PARTITION_BLOCK_SIZE *
		  (PLAT_PARTITION_MAX_ENTRIES / 4 + 2),
};

static io_block_spec_t storage_fip_spec;

static const io_uuid_spec_t bl31_uuid_spec = {
	.uuid = UUID_EL3_RUNTIME_FIRMWARE_BL31,
};

static const io_uuid_spec_t bl32_uuid_spec = {
	.uuid = UUID_SECURE_PAYLOAD_BL32,
};

static const io_uuid_spec_t bl33_uuid_spec = {
	.uuid = UUID_NON_TRUSTED_FIRMWARE_BL33,
};

#if TRUSTED_BOARD_BOOT
static const io_uuid_spec_t trusted_key_cert_uuid_spec = {
	.uuid = UUID_TRUSTED_KEY_CERT,
};

static const io_uuid_spec_t scp_fw_key_cert_uuid_spec = {
	.uuid = UUID_SCP_FW_KEY_CERT,
};

static const io_uuid_spec_t soc_fw_key_cert_uuid_spec = {
	.uuid = UUID_SOC_FW_KEY_CERT,
};

static const io_uuid_spec_t tos_fw_key_cert_uuid_spec = {
	.uuid = UUID_TRUSTED_OS_FW_KEY_CERT,
};

static const io_uuid_spec_t nt_fw_key_cert_uuid_spec = {
	.uuid = UUID_NON_TRUSTED_FW_KEY_CERT,
};

static const io_uuid_spec_t scp_fw_cert_uuid_spec = {
	.uuid = UUID_SCP_FW_CONTENT_CERT,
};

static const io_uuid_spec_t soc_fw_cert_uuid_spec = {
	.uuid = UUID_SOC_FW_CONTENT_CERT,
};

static const io_uuid_spec_t tos_fw_cert_uuid_spec = {
	.uuid = UUID_TRUSTED_OS_FW_CONTENT_CERT,
};

static const io_uuid_spec_t nt_fw_cert_uuid_spec = {
	.uuid = UUID_NON_TRUSTED_FW_CONTENT_CERT,
};
#endif /* TRUSTED_BOARD_BOOT */

static const struct plat_io_policy policies[] = {
	[FIP_IMAGE_ID] = {
		&storage_dev_handle,
		(uintptr_t) &storage_fip_spec,
		check_storage
	},
	[BL31_IMAGE_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bl31_uuid_spec,
		check_fip
	},
	[BL32_IMAGE_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bl32_uuid_spec,
		check_fip
	},
	[BL33_IMAGE_ID] = {
		&fip_dev_handle,
		(uintptr_t)&bl33_uuid_spec,
		check_fip
	},
	[GPT_IMAGE_ID] = {
		&storage_dev_handle,
		(uintptr_t)&emmc_gpt_spec,
		check_storage
	},
#if TRUSTED_BOARD_BOOT
	[TRUSTED_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&trusted_key_cert_uuid_spec,
		check_fip
	},
	[SCP_FW_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&scp_fw_key_cert_uuid_spec,
		check_fip
	},
	[SOC_FW_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&soc_fw_key_cert_uuid_spec,
		check_fip
	},
	[TRUSTED_OS_FW_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&tos_fw_key_cert_uuid_spec,
		check_fip
	},
	[NON_TRUSTED_FW_KEY_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&nt_fw_key_cert_uuid_spec,
		check_fip
	},
	[SCP_FW_CONTENT_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&scp_fw_cert_uuid_spec,
		check_fip
	},
	[SOC_FW_CONTENT_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&soc_fw_cert_uuid_spec,
		check_fip
	},
	[TRUSTED_OS_FW_CONTENT_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&tos_fw_cert_uuid_spec,
		check_fip
	},
	[NON_TRUSTED_FW_CONTENT_CERT_ID] = {
		&fip_dev_handle,
		(uintptr_t)&nt_fw_cert_uuid_spec,
		check_fip
	},
#endif /* TRUSTED_BOARD_BOOT */
};

static int check_storage(const uintptr_t spec)
{
	int result;
	uintptr_t local_handle;

	result = io_dev_init(storage_dev_handle, (uintptr_t)NULL);
	if (result == 0) {
		result = io_open(storage_dev_handle, spec, &local_handle);
		if (result == 0)
			io_close(local_handle);
	}
	return result;
}

static int check_fip(const uintptr_t spec)
{
	int result;
	uintptr_t local_image_handle;

	/* See if a Firmware Image Package is available */
	result = io_dev_init(fip_dev_handle, (uintptr_t)FIP_IMAGE_ID);
	if (result == 0) {
		result = io_open(fip_dev_handle, spec, &local_image_handle);
		if (result == 0) {
			VERBOSE("Using FIP\n");
			io_close(local_image_handle);
		}
	}
	return result;
}

void mtk_io_setup(void)
{
	int result;

	result = register_io_dev_block(&storage_dev_con);
	assert(result == 0);

	result = register_io_dev_fip(&fip_dev_con);
	assert(result == 0);

	result = io_dev_open(storage_dev_con, (uintptr_t)&emmc_dev_spec,
			     &storage_dev_handle);
	assert(result == 0);

	result = io_dev_open(fip_dev_con, (uintptr_t)NULL, &fip_dev_handle);
	assert(result == 0);

	/* Ignore improbable errors in release builds */
	(void)result;
}

void bl2_platform_setup(void)
{
	generic_delay_timer_init();
	mtk_wdt_init();
	mt_pll_init();

	mt_gpio_init();
	pmifclkmgr_init();
	pmif_spmi_init(0);

	pwrap_init_preloader();

	i2c_hw_init();
	pmic_init();
	pmic_initial_setting();

	mt_mem_init();
	mtk_mmc_init(0x11230000, &mt8188_compat, 400000000);
	mtk_io_setup();  
}

struct bl_load_info *plat_get_bl_image_load_info(void)
{
	return get_bl_load_info_from_mem_params_desc();
}

struct bl_params *plat_get_next_bl_params(void)
{
	return get_next_bl_params_from_mem_params_desc();
}

void plat_flush_next_bl_params(void)
{
	flush_bl_params_desc();
}

int plat_get_image_source(unsigned int image_id,
			  uintptr_t *dev_handle,
			  uintptr_t *image_spec)
{
	const struct plat_io_policy *policy;

	assert(image_id < ARRAY_SIZE(policies));

	policy = &policies[image_id];
	policy->check(policy->image_spec);

	*image_spec = policy->image_spec;
	*dev_handle = *(policy->dev_handle);

	return 0;
}

void bl2_el3_early_platform_setup(u_register_t arg0, u_register_t arg1,
				  u_register_t arg2, u_register_t arg3)
{
	static console_t console;

	console_8250_register(UART0_BASE, UART_CLOCK, UART_BAUDRATE, &console);
}

void bl2_el3_plat_arch_setup(void)
{
}

void platform_mem_init(void)
{
}

int bl2_plat_handle_pre_image_load(unsigned int image_id)
{
	const partition_entry_t *entry;
	const char *name = get_boot_partition_name();

	if (storage_fip_spec.length == 0) {
		partition_init(GPT_IMAGE_ID);
		entry = get_partition_entry(name);
		if (entry == NULL) {
			ERROR("Could NOT find the %s partition!\n", name);
			return -ENOENT;
		}
		storage_fip_spec.offset = entry->start;
		storage_fip_spec.length = entry->length;
	}
	return 0;
}

unsigned int plat_get_syscnt_freq2(void)
{
	return SYS_COUNTER_FREQ_IN_HZ;
}
