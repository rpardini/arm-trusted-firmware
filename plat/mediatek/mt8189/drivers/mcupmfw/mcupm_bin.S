/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

	.section .rodata
	.global mcupm_bin
mcupm_bin:
	.balign 8, 0
	.incbin "mcupm.bin"
	.balign 8, 0
	.global mcupm_bin_end
mcupm_bin_end:
