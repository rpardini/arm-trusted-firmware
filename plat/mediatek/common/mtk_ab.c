/*
 * Copyright (c) 2023, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mtk_ab.h>

static void plat_ab_handle_init(struct mtk_bl_ctrl *bctrl)
{
	memset(bctrl, 0, sizeof(struct mtk_bl_ctrl));
	bctrl->magic = BOOTCTRL_MAGIC;
	bctrl->version = BOOTCTRL_VERSION;
	bctrl->slot_suffix = PART_BOOT_A;
	bctrl->tries[PART_BOOT_A] = BOOTCTRL_SRC;
	bctrl->tries[PART_BOOT_B] = BOOTCTRL_SRC;
	bctrl->crc32_le = tf_crc32(0, (const unsigned char *)bctrl,
								sizeof(struct mtk_bl_ctrl) - sizeof(uint32_t));
}

static int32_t plat_ab_handle_blctrl(struct mtk_bl_ctrl *bctrl, uintptr_t dev, int mode)
{
	size_t rs = 0;
	int ret = 0;
	switch (mode) {
		case BOOTCTRL_GET:
			if (io_read(dev, bctrl, sizeof(struct mtk_bl_ctrl), &rs)) {
				return -EPERM;
			}
			break;
		case BOOTCTRL_SET:
			bctrl->crc32_le = tf_crc32(0, (const unsigned char *)bctrl,
										sizeof(struct mtk_bl_ctrl) - sizeof(uint32_t));
			if (io_seek(dev, IO_SEEK_SET, BOOTCTRL_GET)) {
				return -EPERM;
			}
			if (io_write(dev, bctrl, sizeof(struct mtk_bl_ctrl), &rs)) {
				return -EPERM;
			}
			break;
		default:
			ret = -1;
			break;
	}

	if (rs <= 0 || ret < 0) {
		return -EPERM;
	}
	return 0;
}

char *plat_ab_handle_boot(void)
{
	struct mtk_bl_ctrl bl_ctrl = {0};
	uintptr_t dev, image;
	uintptr_t dev_io = NULL;
	int32_t ret = -1;
	int32_t crc = 0;

	if (plat_get_image_source(MISC_IMAGE_ID, &dev, &image)) {
		goto exit;
	}

	if (io_open(dev, image, &dev_io)) {
		goto exit;
	}

	if (plat_ab_handle_blctrl(&bl_ctrl, dev_io, BOOTCTRL_GET) < 0) {
		goto exit;
	}

	crc = tf_crc32(0, (const unsigned char *)&bl_ctrl,
					sizeof(struct mtk_bl_ctrl) - sizeof(uint32_t));

	if (bl_ctrl.magic != BOOTCTRL_MAGIC || bl_ctrl.crc32_le != crc) {
		INFO("Incorrect Boot Control content, establishing.\n");
		plat_ab_handle_init(&bl_ctrl);
		if (plat_ab_handle_blctrl(&bl_ctrl, dev_io, BOOTCTRL_SET) < 0) {
			goto exit;
		}
		return PART_SUFFIX_A;
	}

	ret = bl_ctrl.slot_suffix;
	if (bl_ctrl.tries[ret] == BOOTCTRL_BOOT_SUCCESS) {
		INFO("Boot control is normally.\n");
	} else if (bl_ctrl.tries[ret] < BOOTCTRL_MAX_RETRY) {
		bl_ctrl.tries[ret] ++;
		if (plat_ab_handle_blctrl(&bl_ctrl, dev_io, BOOTCTRL_SET) < 0) {
			goto exit;
		}
	} else {
		bl_ctrl.slot_suffix = (ret == PART_BOOT_A) ? PART_BOOT_B : PART_BOOT_A;
		ret = bl_ctrl.slot_suffix;
		if (bl_ctrl.tries[ret] == BOOTCTRL_BOOT_SUCCESS) {
			INFO("Boot control is rollback.\n");
		} else if (bl_ctrl.tries[ret] < BOOTCTRL_MAX_RETRY) {
			bl_ctrl.tries[ret] ++;
		} else {
			ERROR("BL2: Boot control is in trouble and rollback failure.\n");
		}

		if (plat_ab_handle_blctrl(&bl_ctrl, dev_io, BOOTCTRL_SET) < 0) {
			goto exit;
		}
	}

exit:
	if (dev_io)
		io_close(dev_io);

	if (ret == PART_BOOT_B)
		return PART_SUFFIX_B;
	else
		return PART_SUFFIX_A;
}
