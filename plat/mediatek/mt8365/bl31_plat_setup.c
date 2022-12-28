/*
 * Copyright (c) 2021, MediaTek Inc. All rights reserved.
 * Copyright (c) 2021, BayLibre, SAS. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* System Includes */
#include <assert.h>

/* Project Includes */
#include <arch_helpers.h>
#include <common/bl_common.h>
#include <common/debug.h>
#include <common/desc_image_load.h>
#include <drivers/generic_delay_timer.h>
#include <drivers/ti/uart/uart_16550.h>
#include <lib/mmio.h>

/* Platform Includes */
#include <plat_params.h>
#include <plat_private.h>
#include <mtspmc.h>
#include <mt_spm.h>
#include <mtk_mcdi.h>
#include <mt_timer.h>
#include "plat/mediatek/mt8365/include/scu.h"
#include <mtk_gic_v3_main.h>

#include <emi_mpu.h>
#include <devapc.h>

static entry_point_info_t bl32_ep_info;
static entry_point_info_t bl33_ep_info;

/*******************************************************************************
 * Return a pointer to the 'entry_point_info' structure of the next image for
 * the security state specified. BL33 corresponds to the non-secure image type
 * while BL32 corresponds to the secure image type. A NULL pointer is returned
 * if the image does not exist.
 ******************************************************************************/
entry_point_info_t *bl31_plat_get_next_image_ep_info(uint32_t type)
{
	entry_point_info_t *next_image_info;

	next_image_info = (type == NON_SECURE) ? &bl33_ep_info : &bl32_ep_info;
	assert(next_image_info->h.type == PARAM_EP);

	/* None of the images on this platform can have 0x0 as the entrypoint */
	if (next_image_info->pc) {
		return next_image_info;
	} else {
		return NULL;
	}
}

#define L2C_CFG_MP0			(MCUCFG_BASE + 0x7f0)
#define L2C_SIZE_CFG_OFF	8
#define L2C_SHARE_ENABLE	0
/*******************************************************************************
 * Perform any BL31 early platform setup. Here is an opportunity to copy
 * parameters passed by the calling EL (S-EL1 in BL2 & S-EL3 in BL1) before they
 * are lost (potentially). This needs to be done before the MMU is initialized
 * so that the memory layout can be used while creating page tables.
 * BL2 has flushed this information to memory, so we are guaranteed to pick up
 * good data.
 ******************************************************************************/
void bl31_early_platform_setup2(u_register_t arg0, u_register_t arg1,
				u_register_t arg2, u_register_t arg3)
{
	static console_t console;

	params_early_setup(arg1);

	console_16550_register(UART0_BASE, UART_CLOCK, UART_BAUDRATE, &console);
	mt_serial_setbrg(UART0_BASE, UART_CLOCK, UART_BAUDRATE);

	bl31_params_parse_helper(arg0, &bl32_ep_info, &bl33_ep_info);
}

#include <mcucfg.h>

static void platform_setup_cpu(void)
{
	INFO("%s()\n", __func__);

	mmio_write_32(MP0_RW_RSVD0, 0x00000001);
	INFO("addr of sync_dcm_config: 0x%x\n",
	mmio_read_32(SYNC_DCM_CONFIG));

	INFO("mp0_spmc: 0x%x\n", mmio_read_32(MP0_SPMC));
	INFO("mp1_spmc: 0x%x\n", mmio_read_32(MP1_SPMC));
}

/*******************************************************************************
 * Perform any BL31 platform setup code
 ******************************************************************************/
void bl31_platform_setup(void)
{
	platform_setup_cpu();

	generic_delay_timer_init();

	/* Initialize the GIC driver, CPU and distributor interfaces */
	gic_setup();

	spm_boot_init();

	/* Initialize mcupm at boot time */
	mcdi_mcupm_boot_init();

	emi_mpu_init();
	devapc_init();
}

/*******************************************************************************
 * Perform the very early platform specific architectural setup here. At the
 * moment this is only intializes the mmu in a quick and dirty way.
 ******************************************************************************/
void bl31_plat_arch_setup(void)
{
	enable_scu(read_mpidr());

	plat_configure_mmu_el3(BL31_START,
			       BL31_END - BL31_START,
			       BL_CODE_BASE,
			       BL_CODE_END);

	spmc_init();
	mmio_write_32(MCUCFG_BASE + 0xB58, 1);
}
