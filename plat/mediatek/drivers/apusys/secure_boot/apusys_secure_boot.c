/*
 * Copyright (c) 2021, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* TF-A system header */
#include <common/debug.h>
#include <lib/utils_def.h>
#include <string.h>

/* Vendor header */
#include "apusys_secure_boot.h"
#include "rsa.h"
#include "sha256.h"

/**************************************************************************
 *  INTERNAL VARIABLES
 **************************************************************************/
static const u8 apusys_img_vfy_pubk_m[APUSYS_IMG_VFY_PUBK_SZ] = {APUSYS_IMG_VFY_PUBK_M};
static const u8 apusys_img_vfy_pubk_rr[APUSYS_IMG_VFY_PUBK_SZ] = {APUSYS_IMG_VFY_PUBK_RR};

int apusys_image_verify(uint64_t* addr, uint32_t size, char* sig)
{
    int ret = 0;
    int i = 0;
    u8 hash_sha256[32] = {0};
    u8 buf[256] = {0};
    u8 __attribute__((aligned(8))) key[RSA_LEN], sign[RSA_LEN],
            plain[RSA_LEN] ,rr[RSA_LEN];
    unsigned int image_size = size - 256;

    ret = sha256_hash(addr, image_size, hash_sha256);
    if (0 != ret)
         return ret;

    for (i = 0; i < RSA_LEN; i++) {
        key[RSA_LEN -i-1] = apusys_img_vfy_pubk_m[i];
        sign[RSA_LEN - i -1] = sig[i];
        rr[RSA_LEN - i -1] = apusys_img_vfy_pubk_rr[i];
    }
    memset(plain, 0, RSA_LEN);
    mod_exp_65537_mont((uintptr_t *)plain, (const uintptr_t *)sign, (const uintptr_t *)key, (const uintptr_t *)rr);

    for (i = 0; i < RSA_LEN; i++)
        buf[RSA_LEN - i -1] = plain[i];

    ret = pss_padding_verify(buf, 256, hash_sha256, 32);

    return ret;
}
