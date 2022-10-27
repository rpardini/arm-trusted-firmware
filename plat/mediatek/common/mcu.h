/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __MCU__H__
#define __MCU__H__

struct mtk_mcu {
	const char *firmware_name;	/* The firmware file name in CBFS */

	volatile void *run_address;	/* The address for running the firmware */
	size_t run_size;		/* The buffer for loading the firmware */
	void *load_buffer;		/* The buffer size */
	size_t buffer_size;		/* The firmware real size */
	void *priv;			/* The additional data required by the reset callback */
	void (*reset)(struct mtk_mcu *mcu);	/* The reset callback */
};

static int mtk_init_mcu(struct mtk_mcu *mcu)
{
	int i;

	if (!mcu)
		return -1;

	if (mcu->run_address) {
		volatile uint32_t *run_address =
		    (volatile uint32_t *)mcu->run_address;

		/* We cannot use memcpy because it does 1-byte writes but
		 * some addresses allow only 4-byte writes, therefore
		 * the FW copy is open-coded for now to perform 4-byte
		 * writes.
		 */
		for (i = 0; i < (int)mcu->run_size; i += 4) {
			uint32_t tmp =
			    ((uint8_t *) mcu->load_buffer)[i] |
			    ((uint8_t *) mcu->load_buffer)[i + 1] << 8 |
			    ((uint8_t *) mcu->load_buffer)[i + 2] << 16 |
			    ((uint8_t *) mcu->load_buffer)[i + 3] << 24;
			run_address[i / 4] = tmp;
		}
		/* Memory barrier to ensure data is flushed before resetting MCU. */
		__asm__ volatile ("dmb sy":::"memory");
	}

	if (mcu->reset)
		mcu->reset(mcu);

	NOTICE("%s: Loaded (and reset) %s: (%zd bytes)\n",
	       __func__, mcu->firmware_name, mcu->run_size);

	return 0;
}

#endif
