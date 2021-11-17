/*
 * Copyright (c) 2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLAT_SIP_CALLS_H
#define PLAT_SIP_CALLS_H

/*******************************************************************************
 * Plat SiP function constants
 ******************************************************************************/
#define MTK_PLAT_SIP_NUM_CALLS	4

/* SPM get pwr_ctrl args */
#define MTK_SIP_KERNEL_SPM_GET_PWR_CTRL_ARGS_AARCH32 0x8200022B
#define MTK_SIP_KERNEL_SPM_GET_PWR_CTRL_ARGS_AARCH64 0xC200022B

/* SPM related SMC call */
#define MTK_SIP_KERNEL_SPM_PWR_CTRL_ARGS_AARCH32   0x82000224
#define MTK_SIP_KERNEL_SPM_PWR_CTRL_ARGS_AARCH64   0xC2000224

#endif /* PLAT_SIP_CALLS_H */
