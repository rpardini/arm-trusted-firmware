/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

	.section .rodata
	.global dpm_dm_bin
dpm_dm_bin:
	.balign 8, 0
	.incbin "dpm.dm"
	.balign 8, 0
	.global dpm_dm_bin_end
dpm_dm_bin_end:

	.global dpm_pm_bin
dpm_pm_bin:
	.balign 8, 0
	.incbin "dpm.pm"
	.balign 8, 0
	.global dpm_pm_bin_end
dpm_pm_bin_end:
