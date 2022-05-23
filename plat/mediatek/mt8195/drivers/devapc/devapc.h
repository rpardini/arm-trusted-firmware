/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SOC_MEDIATEK_MT8195_DEVAPC_H
#define SOC_MEDIATEK_MT8195_DEVAPC_H

#include <lib/mmio.h>
#include <devapc_common.h>
#include <mt8195.h>

void dapc_init(void);

enum devapc_ao_offset {
	SYS0_D0_APC_0 = 0x0,
	SYS1_D0_APC_0 = 0x1000,
	SYS2_D0_APC_0 = 0x2000,
	DOM_REMAP_0_0 = 0x800,
	DOM_REMAP_1_0 = 0x810,
	DOM_REMAP_1_1 = 0x814,
	DOM_REMAP_2_0 = 0x820,
	MAS_DOM_0 = 0x0900,
	MAS_DOM_4 = 0x0910,
	MAS_SEC_0 = 0x0A00,
	AO_APC_CON = 0x0F00,
};

enum scp_offset {
	SCP_DOM = 0xA5080,
	ADSP_DOM = 0xA5088,
	ONETIME_LOCK = 0xA5104,
};

/******************************************************************************
 * STRUCTURE DEFINITION
 ******************************************************************************/
/* Common */
enum trans_type {
	NON_SECURE_TRANS = 0,
	SECURE_TRANS,
};

enum devapc_perm_type {
	NO_PROTECTION = 0,
	SEC_RW_ONLY,
	SEC_RW_NS_R,
	FORBIDDEN,
	PERM_NUM,
};

enum domain_id {
	DOMAIN_0 = 0,
	DOMAIN_1,
	DOMAIN_2,
	DOMAIN_3,
	DOMAIN_4,
	DOMAIN_5,
	DOMAIN_6,
	DOMAIN_7,
	DOMAIN_8,
	DOMAIN_9,
	DOMAIN_10,
	DOMAIN_11,
	DOMAIN_12,
	DOMAIN_13,
	DOMAIN_14,
	DOMAIN_15,
};

struct apc_infra_peri_dom_16 {
	unsigned char d_permission[16];
};

struct apc_infra_peri_dom_8 {
	unsigned char d_permission[8];
};

struct apc_infra_peri_dom_4 {
	unsigned char d_permission[4];
};

enum devapc_sys_dom_num {
	DOM_NUM_INFRA_AO_SYS0 = 16,
	DOM_NUM_INFRA_AO_SYS1 = 4,
	DOM_NUM_INFRA_AO_SYS2 = 4,
	DOM_NUM_PERI_AO_SYS0 = 16,
	DOM_NUM_PERI_AO_SYS1 = 8,
	DOM_NUM_PERI2_AO_SYS0 = 16,
	DOM_NUM_PERI_PAR_AO_SYS0 = 16,
};

enum devapc_cfg_index {
	DEVAPC_DEBUGSYS_INDEX = 17,
};

/* PERM_ATTR MACRO */
#define DAPC_INFRA_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_16(__VA_ARGS__) } }
#define DAPC_INFRA_AO_SYS1_ATTR(...)	{ { DAPC_PERM_ATTR_4(__VA_ARGS__) } }
#define DAPC_INFRA_AO_SYS2_ATTR(...)	{ { DAPC_PERM_ATTR_4(__VA_ARGS__) } }
#define DAPC_PERI_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_16(__VA_ARGS__) } }
#define DAPC_PERI_AO_SYS1_ATTR(...)	{ { DAPC_PERM_ATTR_8(__VA_ARGS__) } }
#define DAPC_PERI2_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_16(__VA_ARGS__) } }
#define DAPC_PERI_PAR_AO_SYS0_ATTR(...)	{ { DAPC_PERM_ATTR_16(__VA_ARGS__) } }

/******************************************************************************
 * Variable DEFINITION
 ******************************************************************************/
#define MOD_NO_IN_1_DEVAPC		16

/******************************************************************************
 * Bit Field DEFINITION
 ******************************************************************************/
/* INFRA */
#define CPU_EB_SEC BIT(1)

#define CPU_EB_DOM (BIT(11)|BIT(10)|BIT(9)|BIT(8))
#define SCP_SSPM_DOM (BIT(19)|BIT(18)|BIT(17)|BIT(16))

/* PERI */
#define SPM_DOM (BIT(11)|BIT(10)|BIT(9)|BIT(8))

/* PERI_PAR */
#define SSUSB_SEC BIT(21)
#define SSUSB2_SEC BIT(0)
#define SSUSB_P1_0_SEC BIT(1)
#define SSUSB_P1_1_SEC BIT(2)
#define SSUSB_P2_SEC BIT(3)
#define SSUSB_P3_SEC BIT(4)

#define PCIE0_DOM (BIT(11)|BIT(10)|BIT(9)|BIT(8))
#define PCIE1_DOM (BIT(19)|BIT(18)|BIT(17)|BIT(16))

/* Domain Remap */
#define FOUR_BIT_DOM_REMAP_0 (BIT(3)|BIT(2)|BIT(1)|BIT(0))
#define FOUR_BIT_DOM_REMAP_1 (BIT(7)|BIT(6)|BIT(5)|BIT(4))
#define FOUR_BIT_DOM_REMAP_2 (BIT(11)|BIT(10)|BIT(9)|BIT(8))
#define FOUR_BIT_DOM_REMAP_3 (BIT(15)|BIT(14)|BIT(13)|BIT(12))
#define FOUR_BIT_DOM_REMAP_4 (BIT(19)|BIT(18)|BIT(17)|BIT(16))
#define FOUR_BIT_DOM_REMAP_5 (BIT(23)|BIT(22)|BIT(21)|BIT(20))
#define FOUR_BIT_DOM_REMAP_6 (BIT(27)|BIT(26)|BIT(25)|BIT(24))
#define FOUR_BIT_DOM_REMAP_7 (BIT(31)|BIT(30)|BIT(29)|BIT(28))

#define THREE_BIT_DOM_REMAP_0 (BIT(2)|BIT(1)|BIT(0))
#define THREE_BIT_DOM_REMAP_1 (BIT(5)|BIT(4)|BIT(3))
#define THREE_BIT_DOM_REMAP_2 (BIT(8)|BIT(7)|BIT(6))
#define THREE_BIT_DOM_REMAP_3 (BIT(11)|BIT(10)|BIT(9))
#define THREE_BIT_DOM_REMAP_4 (BIT(14)|BIT(13)|BIT(12))
#define THREE_BIT_DOM_REMAP_5 (BIT(17)|BIT(16)|BIT(15))

#define TWO_BIT_DOM_REMAP_0 (BIT(1)|BIT(0))
#define TWO_BIT_DOM_REMAP_1 (BIT(3)|BIT(2))
#define TWO_BIT_DOM_REMAP_2 (BIT(5)|BIT(4))
#define TWO_BIT_DOM_REMAP_3 (BIT(7)|BIT(6))
#define TWO_BIT_DOM_REMAP_4 (BIT(9)|BIT(8))
#define TWO_BIT_DOM_REMAP_5 (BIT(11)|BIT(10))

#endif /* SOC_MEDIATEK_MT8195_DEVAPC_H */
