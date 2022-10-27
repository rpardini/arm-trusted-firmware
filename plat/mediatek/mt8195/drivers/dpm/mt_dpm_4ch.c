///* SPDX-License-Identifier: GPL-2.0-only */

#include <mt_dpm.h>
#include <mt_spm_internal.h>

static struct dpm_regs *const mtk_dpm2 = (void *)DPM_CFG_BASE2;
extern const uint8_t dpm_data_binary[];
extern const uint8_t dpm_program_binary[];

static struct mtk_mcu dpm_mcu_4ch[] = {
	{
	 .firmware_name = CONFIG_DPM_DM_FIRMWARE,
	 .load_buffer = dpm_data_binary,
	 .run_size = 66,
	 .run_address = (void *)DPM_DM_SRAM_BASE2,
	 },
	{
	 .firmware_name = CONFIG_DPM_PM_FIRMWARE,
	 .load_buffer = dpm_program_binary,
	 .run_size = 13702,
	 .run_address = (void *)DPM_PM_SRAM_BASE2,
	 .priv = mtk_dpm2,
	 .reset = dpm_reset,
	 },
};

static int wake_dpm_sram_up(void)
{
	int loop = 100;

	/* TODO: convert to new APIs (SET32_BITFIELDS/READ32_BITFIELD) */
	mmio_setbits_32(DRAMC_MCU_SRAM_CON, DRAMC_MCU_SRAM_SLEEP_B_LSB);
	mmio_setbits_32(DRAMC_MCU2_SRAM_CON, DRAMC_MCU2_SRAM_SLEEP_B_LSB);

	while (loop > 0 &&
	       ((mmio_read_32(DRAMC_MCU_SRAM_CON) &
		 DRAMC_MCU_SRAM_SLEEP_B_LSB) == 0 ||
		(mmio_read_32(DRAMC_MCU2_SRAM_CON) &
		 DRAMC_MCU2_SRAM_SLEEP_B_LSB) == 0)) {
		mdelay(1);
		--loop;
	}

	if (loop == 0) {
		NOTICE("failed to wake DPM up.\n");
		return -1;
	}

	mmio_setbits_32(DRAMC_MCU_SRAM_CON, DRAMC_MCU_SRAM_ISOINT_B_LSB);
	mmio_setbits_32(DRAMC_MCU2_SRAM_CON, DRAMC_MCU2_SRAM_ISOINT_B_LSB);

	return 0;
}

static void dpm_mtcoms_sleep_on(void)
{
	/* DPM MTCMOS sleep on */
	mmio_write_32(DPM0_PWR_CON, 0x0000204d);
	mmio_write_32(DPM1_PWR_CON, 0x0000204d);
	mdelay(1);
	mmio_write_32(DPM0_PWR_CON, 0x0000224d);
	mmio_write_32(DPM1_PWR_CON, 0x0000224d);
	mdelay(1);
	mmio_clrbits_32(&mtk_dpm->sw_rstn, DPM_SW_RSTN_RESET);
	mmio_clrbits_32(&mtk_dpm2->sw_rstn, DPM_SW_RSTN_RESET);
}

int dpm_4ch_init(void)
{
	dpm_mtcoms_sleep_on();
	if (wake_dpm_sram_up())
		return -1;
	return 0;
}

int dpm_4ch_para_setting(void)
{
	int i;
	struct mtk_mcu *dpm;

	for (i = 0; i < ARRAY_SIZE(dpm_mcu_4ch); i++) {
		dpm = &dpm_mcu_4ch[i];
		if (mtk_init_mcu(dpm))
			return -1;
	}

	return 0;
}
