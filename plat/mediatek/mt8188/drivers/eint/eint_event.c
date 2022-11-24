/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <lib/mtk_init/mtk_init.h>
#include <eint_event.h>
#include <lib/utils_def.h>

void unmask_eint_event_mask(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(mtk_eint_event->eint_event_mask_clr); i++)
		mmio_write_32(&mtk_eint_event->eint_event_mask_clr[i], 0xffffffff);
}

int unmask_eint(void)
{
	unmask_eint_event_mask();
	return 0;
}

MTK_PLAT_SETUP_0_INIT(unmask_eint);
