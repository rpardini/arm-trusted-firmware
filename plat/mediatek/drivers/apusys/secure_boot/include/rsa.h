/*
 * Copyright (c) 2021, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RSA_H
#define _RSA_H

#include <errno.h>

#define RSA2048_BYTES   (2048 / 8)
#define RSA4096_BYTES   (4096 / 8)

/* This is the minimum/maximum key size we support, in bits */
#define RSA_MIN_KEY_BITS    2048
#define RSA_MAX_KEY_BITS    4096

/* This is the maximum signature length that we support, in bits */
#define RSA_MAX_SIG_BITS    4096

void mod_exp_65537_mont(uintptr_t *r, const uintptr_t *a, const uintptr_t *m, const uintptr_t *rr);
int pss_padding_verify(unsigned char *msg, int msg_len, const unsigned char *hash, int hash_len);

#endif
