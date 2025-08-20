/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APUSYS_SECURITY_CTRL_PLAT_H
#define APUSYS_SECURITY_CTRL_PLAT_H

#include <platform_def.h>

#define SEC_CTRL_SOC2APU_SET1_0 (APU_SEC_CON + 0xC)
#define SEC_CTRL_SOC2APU_SET1_1 (APU_SEC_CON + 0x10)
#define SEC_CTRL_VMID_MAP       (APU_SEC_CON + 0x20)
#define SEC_CTRL_SIDE_BAND      (APU_SEC_CON + 0x24)

#define SEC_CTRL_REG_DOMAIN_NUM   8
#define SEC_CTRL_DOMAIN_REMAP_SEL 6
#define SEC_CTRL_DOMAIN_MASK      0xF
#define SEC_CTRL_NS_MASK          0x1

#define SEC_CTRL_NARE_DOMAIN      5
#define SEC_CTRL_NARE_NS          0
#define SEC_CTRL_NARE_DOMAIN_SHF  0
#define SEC_CTRL_NARE_NS_SHF      4

#define SEC_CTRL_SARE0_DOMAIN     5
#define SEC_CTRL_SARE0_NS         0
#define SEC_CTRL_SARE0_DOMAIN_SHF 5
#define SEC_CTRL_SARE0_NS_SHF     9

#define SEC_CTRL_SARE1_DOMAIN     5
#define SEC_CTRL_SARE1_NS         0
#define SEC_CTRL_SARE1_DOMAIN_SHF 10
#define SEC_CTRL_SARE1_NS_SHF     14

#endif
