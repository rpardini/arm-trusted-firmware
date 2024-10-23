/* SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2022 MediaTek Inc. All rights reserved.
 */

#ifndef __SNFC__H__
#define __SNFC__H__

#define IO_PHYS			(0x10000000)
/*******************************************************************************
 * GPIO related constants
 ******************************************************************************/
#define GPIO_BASE		(IO_PHYS + 0x00005000)

/*******************************************************************************
 * SNFC related constants
 ******************************************************************************/
#define SNFC_GPIO_GPOUP1	0x100053F0
#define SNFC_GPIO_GPOUP2	0x10005400
#define SNFC_PU_SET_GROUP	0x11E100D0
#define SNFC_PD_SET_GROUP	0x11E100A0
#define SNFC_DRIVING_SET	0x11E10020

#endif
