/*
 * Copyright (c) 2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __APUSYS_DEVAPC_DEF_H__
#define __APUSYS_DEVAPC_DEF_H__

#include <lib/mmio.h>

/******************************************************************************
 * STRUCTURE DEFINITION
 ******************************************************************************/
enum APUSYS_APC_ERR_STATUS {
	APUSYS_APC_OK = 0x0,

	APUSYS_APC_ERR_GENERIC = 0x1000,
	APUSYS_APC_ERR_INVALID_CMD = 0x1001,
	APUSYS_APC_ERR_SLAVE_TYPE_NOT_SUPPORTED = 0x1002,
	APUSYS_APC_ERR_SLAVE_IDX_NOT_SUPPORTED = 0x1003,
	APUSYS_APC_ERR_DOMAIN_NOT_SUPPORTED = 0x1004,
	APUSYS_APC_ERR_PERMISSION_NOT_SUPPORTED = 0x1005,
	APUSYS_APC_ERR_OUT_OF_BOUNDARY = 0x1006,
	APUSYS_APC_ERR_REQ_TYPE_NOT_SUPPORTED = 0x1007,
};

enum APUSYS_APC_PERM_TYPE {
	NO_PROTECTION = 0,
	SEC_RW_ONLY,
	SEC_RW_NS_R,
	FORBIDDEN,
	PERM_NUM,
};

enum APUSYS_APC_DOMAIN_ID {
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

struct APC_DOM_16 {
	unsigned char d0_permission;
	unsigned char d1_permission;
	unsigned char d2_permission;
	unsigned char d3_permission;
	unsigned char d4_permission;
	unsigned char d5_permission;
	unsigned char d6_permission;
	unsigned char d7_permission;
	unsigned char d8_permission;
	unsigned char d9_permission;
	unsigned char d10_permission;
	unsigned char d11_permission;
	unsigned char d12_permission;
	unsigned char d13_permission;
	unsigned char d14_permission;
	unsigned char d15_permission;
};

#define APUSYS_APC_AO_ATTR(DEV_NAME, PERM_ATTR0, PERM_ATTR1, \
		PERM_ATTR2, PERM_ATTR3, PERM_ATTR4, PERM_ATTR5, \
		PERM_ATTR6, PERM_ATTR7, PERM_ATTR8, PERM_ATTR9, \
		PERM_ATTR10, PERM_ATTR11, PERM_ATTR12, PERM_ATTR13, \
		PERM_ATTR14, PERM_ATTR15) \
	{(unsigned char)PERM_ATTR0, (unsigned char)PERM_ATTR1, \
	(unsigned char)PERM_ATTR2, (unsigned char)PERM_ATTR3, \
	(unsigned char)PERM_ATTR4, (unsigned char)PERM_ATTR5, \
	(unsigned char)PERM_ATTR6, (unsigned char)PERM_ATTR7, \
	(unsigned char)PERM_ATTR8, (unsigned char)PERM_ATTR9, \
	(unsigned char)PERM_ATTR10, (unsigned char)PERM_ATTR11, \
	(unsigned char)PERM_ATTR12, (unsigned char)PERM_ATTR13, \
	(unsigned char)PERM_ATTR14, (unsigned char)PERM_ATTR15}

/******************************************************************************
 * UTILITY DEFINITION
 ******************************************************************************/
#define apuapc_writel(VAL, REG)		mmio_write_32((uintptr_t)REG, VAL)
#define apuapc_readl(REG)		mmio_read_32((uintptr_t)REG)

/******************************************************************************
 * REGISTER ADDRESS DEFINITION
 ******************************************************************************/
#define APUSYS_APC_AO_BASE		0x190F8000

/******************************************************************************/
/* APUSYS APC REGISTER OFFSETS */
#define APUSYS_APC_CON		((uint32_t *)(APUSYS_APC_AO_BASE + 0x00F00))
#define APUSYS_SYS0_D0_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00000))
#define APUSYS_SYS0_D0_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00004))
#define APUSYS_SYS0_D0_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00008))
#define APUSYS_SYS0_D1_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00040))
#define APUSYS_SYS0_D1_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00044))
#define APUSYS_SYS0_D1_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00048))
#define APUSYS_SYS0_D2_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00080))
#define APUSYS_SYS0_D2_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00084))
#define APUSYS_SYS0_D2_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00088))
#define APUSYS_SYS0_D3_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x000C0))
#define APUSYS_SYS0_D3_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x000C4))
#define APUSYS_SYS0_D3_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x000C8))
#define APUSYS_SYS0_D4_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00100))
#define APUSYS_SYS0_D4_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00104))
#define APUSYS_SYS0_D4_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00108))
#define APUSYS_SYS0_D5_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00140))
#define APUSYS_SYS0_D5_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00144))
#define APUSYS_SYS0_D5_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00148))
#define APUSYS_SYS0_D6_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00180))
#define APUSYS_SYS0_D6_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00184))
#define APUSYS_SYS0_D6_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00188))
#define APUSYS_SYS0_D7_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x001C0))
#define APUSYS_SYS0_D7_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x001C4))
#define APUSYS_SYS0_D7_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x001C8))
#define APUSYS_SYS0_D8_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00200))
#define APUSYS_SYS0_D8_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00204))
#define APUSYS_SYS0_D8_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00208))
#define APUSYS_SYS0_D9_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00240))
#define APUSYS_SYS0_D9_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00244))
#define APUSYS_SYS0_D9_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00248))
#define APUSYS_SYS0_D10_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00280))
#define APUSYS_SYS0_D10_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00284))
#define APUSYS_SYS0_D10_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00288))
#define APUSYS_SYS0_D11_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x002C0))
#define APUSYS_SYS0_D11_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x002C4))
#define APUSYS_SYS0_D11_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x002C8))
#define APUSYS_SYS0_D12_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00300))
#define APUSYS_SYS0_D12_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00304))
#define APUSYS_SYS0_D12_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00308))
#define APUSYS_SYS0_D13_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00340))
#define APUSYS_SYS0_D13_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00344))
#define APUSYS_SYS0_D13_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00348))
#define APUSYS_SYS0_D14_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00380))
#define APUSYS_SYS0_D14_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00384))
#define APUSYS_SYS0_D14_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00388))
#define APUSYS_SYS0_D15_APC_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x003C0))
#define APUSYS_SYS0_D15_APC_1	((uint32_t *)(APUSYS_APC_AO_BASE + 0x003C4))
#define APUSYS_SYS0_D15_APC_2	((uint32_t *)(APUSYS_APC_AO_BASE + 0x003C8))
#define APUSYS_SYS0_APC_LOCK_0	((uint32_t *)(APUSYS_APC_AO_BASE + 0x00700))

#define APUSYS_NOC_DAPC_CON	((uint32_t *)(APUSYS_NOC_DAPC_AO_BASE + 0x00F00))

/******************************************************************************
 * SLAVE INDEX
 ******************************************************************************/
#define APUSYS_APC_SLAVE_APU_RPCTOP		0  /* apusys_ao-0 */
#define APUSYS_APC_SLAVE_APU_PCUTOP		1  /* apusys_ao-1 */

#define APUSYS_APC_SLAVE_VPU_CORE0_CONFIG_0	34
#define APUSYS_APC_SLAVE_VPU_CORE0_CONFIG_1	35
#define APUSYS_APC_SLAVE_VPU_CORE1_CONFIG_0	36
#define APUSYS_APC_SLAVE_VPU_CORE1_CONFIG_1	37

#define APUSYS_NOC_DAPC_SLV00_0			0
#define APUSYS_NOC_DAPC_SLV01_2			5

/******************************************************************************
 * Variable DEFINITION
 ******************************************************************************/
#define APUSYS_APC_SYS0_AO_SLAVE_NUM_IN_1_DOM		16
#define APUSYS_APC_SYS0_AO_DOM_NUM			16
#define APUSYS_APC_SYS0_AO_SLAVE_NUM			71
#define APUSYS_APC_SYS0_LOCK_BIT_APU_SCTRL_REVISER	15 /* apu_sctrl_reviser */
#define APUSYS_APC_SYS0_LOCK_BIT_DEVAPC_AO_WRAPPER	8  /* apusys_ao-9 */

#define APUSYS_NOC_DAPC_AO_SLAVE_NUM_IN_1_DOM		16
#define APUSYS_NOC_DAPC_AO_DOM_NUM			16
#define APUSYS_NOC_DAPC_AO_SLAVE_NUM			27
#endif /* __APUSYS_DEVAPC_DEF_H__ */
