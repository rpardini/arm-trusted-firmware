/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SOC_MEDIATEK_MT8195_EINT_EVENT_H
#define SOC_MEDIATEK_MT8195_EINT_EVENT_H

#include <lib/mmio.h>
#include <mt8195.h>

/* eint event mask cler register */
struct eint_event_reg {
	uint32_t eint_event_mask_clr[7];
};

/* eint_base + 0x880 is eint_event_mask_clr register with access type W1C. */
static struct eint_event_reg *const mtk_eint_event = (void *)(EINT_BASE + 0x880);

/* unmask eint event, eint can wakeup by spm */
void unmask_eint_event_mask(void);

#endif
