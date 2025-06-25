/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

	.section .rodata
	.global sspm_bin
sspm_bin:
	.balign 8, 0
	.incbin "sspm.bin"
	.balign 8, 0
	.global sspm_bin_end
sspm_bin_end:
