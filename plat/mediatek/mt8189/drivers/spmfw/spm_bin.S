/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

	.section .rodata
	.global spm_bin
spm_bin:
	.balign 8, 0
	.incbin "spm_firmware.bin"
	.byte 0, 0
	.balign 8, 0
	.global spm_bin_end
spm_bin_end:
