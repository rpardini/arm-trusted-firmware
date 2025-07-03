/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#ifndef __API_HRE_MMINFRA_H__
#define __API_HRE_MMINFRA_H__

enum mminfra_hre_cfg {
	INIT_HRE_BKRS_CONFIG = 1,
	SAVE_HRE_BK_CONFIG = 2,
	SAVE_HRE_RS_CONFIG = 3,
};

void mminfra_hre_bkrs(enum mminfra_hre_cfg select);

#endif /* __API_HRE_MMINFRA_H__ */
