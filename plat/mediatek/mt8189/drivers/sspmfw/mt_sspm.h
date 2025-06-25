/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

_Alignas(8) extern const unsigned char sspm_bin[];
extern const unsigned char sspm_bin_end[];
#define sspm_bin_len (sspm_bin_end - sspm_bin)
