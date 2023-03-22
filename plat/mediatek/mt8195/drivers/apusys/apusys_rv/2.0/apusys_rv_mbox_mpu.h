/*
 * Copyright (c) 2021, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APUSYS_RV_MBOX_MPU_H
#define APUSYS_RV_MBOX_MPU_H

struct mbox_mpu_setting {
	uint32_t no_mpu;
	uint32_t rx_ns;
	uint32_t rx_domain;
	uint32_t tx_ns;
	uint32_t tx_domain;
};

static const struct mbox_mpu_setting mbox_mpu_setting_tab[] = {
	/* no_mpu, rx_ns, rx_domain, tx_ns, tx_domain */
	{0, 0, 5, 1, 0},
	{0, 0, 5, 0, 0},
	{0, 0, 5, 0, 0},
	{0, 0, 5, 0, 0},
	{1, 0, 0, 0, 0},
	{0, 0, 5, 0, 3},
	{0, 0, 5, 0, 0},
	{0, 0, 5, 0, 0},
};

#define APU_MBOX_NUM ARRAY_SIZE(mbox_mpu_setting_tab)

#endif /* APUSYS_RV_MBOX_MPU_H */
