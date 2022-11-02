/*
 * Copyright (c) 2021, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* TF-A system header */
#include <common/debug.h>
#include <lib/utils_def.h>

#define SHA256_BLOCK_SIZE       64

typedef unsigned char    u8;
typedef unsigned int     u32;
typedef unsigned long    u64;

struct sha256_context {
    u32 state[8];
    u64 count;
    u8 buf[SHA256_BLOCK_SIZE];
};

int sha256_start(struct sha256_context *s_ctx);
int sha256_process(struct sha256_context *s_ctx, const u8 *input, unsigned int len);
int sha256_end(struct sha256_context *s_ctx, u8 *out);
int sha256_hash(const void *input, int len, u8 *output);
