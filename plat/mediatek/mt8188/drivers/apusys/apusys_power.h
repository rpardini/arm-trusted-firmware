/*
 * Copyright (c) 2021, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef APUSYS_POWER_H
#define APUSYS_POWER_H

#include <platform_def.h>

enum smc_rcx_pwr_op {
	SMC_RCX_PWR_AFC_EN = 0,
	SMC_RCX_PWR_WAKEUP_RPC,
	SMC_RCX_PWR_CG_EN,
};

enum smc_pwr_dump {
	SMC_PWR_DUMP_RPC = 0,
	SMC_PWR_DUMP_PCU,
	SMC_PWR_DUMP_ARE,
	SMC_PWR_DUMP_ALL,
};

enum t_acx_id {
	ACX0 = 0,
	CLUSTER_NUM,
	RCX,
};

enum apu_clksrc_id {
	PLL_CONN = 0, /* MNOC */
	PLL_UP,
	PLL_VPU,
	PLL_DLA,
	PLL_NUM,
};

enum apupw_reg {
	sys_spm,
	bcrm_fmem_pdn,
	apu_rcx,
	apu_vcore,
	apu_md32_mbox,
	apu_rpc,
	apu_pcu,
	apu_ao_ctl,
	apu_pll,
	apu_acc,
	apu_are0,
	apu_are1,
	apu_are2,
	apu_acx0,
	apu_acx0_rpc_lite,
	APUPW_MAX_REGS,
};

struct apu_power {
	uintptr_t regs[APUPW_MAX_REGS];
	unsigned int phy_addr[APUPW_MAX_REGS];
};


#define BCRM_FMEM_PDN				(0x10276000)

/* APU related reg */
#define APU_VCORE_BASE				(APU_RCX_VCORE_CONFIG)
#define APU_RCX_BASE				(APU_RCX_CONFIG)
#define APU_RPC_BASE				(APU_RPCTOP)
#define APU_PCU_BASE				(APU_PCUTOP)
#define APU_ARE0_BASE				(APU_ARETOP_ARE0)
#define APU_ARE1_BASE				(APU_ARETOP_ARE1)
#define APU_ARE2_BASE				(APU_ARETOP_ARE2)
#define APU_MBOX0_BASE				(APU_MBOX0)
#define APU_AO_CTL_BASE				(APU_AO_CTRL)
#define APU_PLL_BASE				(APU_PLL)
#define APU_ACC_BASE				(APU_ACC)
#define APU_ACX0_BASE				(APU_ACX0)
#define APU_ACX0_RPC_LITE_BASE			(APU_ACX0_RPC_LITE)

/* RPC offset define */
#define APU_RPC_TOP_CON				0x0000
#define APU_RPC_TOP_SEL				0x0004
#define APU_RPC_SW_FIFO_WE			0x0008
#define APU_RPC_IO_DEBUG			0x000C
#define APU_RPC_STATUS				0x0014
#define APU_RPC_TOP_SEL_1			0x0018
#define APU_RPC_HW_CON				0x001C
#define APU_RPC_LITE_CON			0x0020
#define APU_RPC_INTF_PWR_RDY_REG		0x0040
#define APU_RPC_INTF_PWR_RDY			0x0044

/* APU PLL1U offset define */
#define MDLA_PLL_BASE				0x000 // 0x190F3000
#define MVPU_PLL_BASE				0x400 // 0x190F3400
#define MNOC_PLL_BASE				0x800 // 0x190F3800

/* PLL offset define */
#define PLL4H_PLL1_CON1				0x00C
#define PLL4H_PLL2_CON1				0x01C
#define PLL4H_PLL3_CON1				0x02C
#define PLL4H_PLL4_CON1				0x03C
#define PLL4HPLL_FHCTL_HP_EN			0xE00
#define PLL4HPLL_FHCTL_CLK_CON			0xE08
#define PLL4HPLL_FHCTL_RST_CON			0xE0C
#define PLL4HPLL_FHCTL0_CFG			0xE3C
#define PLL4HPLL_FHCTL0_DDS			0xE44
#define PLL4HPLL_FHCTL1_CFG			0xE50
#define PLL4HPLL_FHCTL1_DDS			0xE58
#define PLL4HPLL_FHCTL2_CFG			0xE64
#define PLL4HPLL_FHCTL2_DDS			0xE6C
#define PLL4HPLL_FHCTL3_CFG			0xE78
#define PLL4HPLL_FHCTL3_DDS			0xE80

/* ACC offset define */
#define APU_ACC_CONFG_SET0			0x0000
#define APU_ACC_CONFG_SET1			0x0004
#define APU_ACC_CONFG_SET2			0x0008
#define APU_ACC_CONFG_SET3			0x000C
#define APU_ACC_CONFG_CLR0			0x0040
#define APU_ACC_CONFG_CLR1			0x0044
#define APU_ACC_CONFG_CLR2			0x0048
#define APU_ACC_CONFG_CLR3			0x004C
#define APU_ACC_FM_CONFG_SET			0x00C0
#define APU_ACC_FM_CONFG_CLR			0x00C4
#define APU_ACC_FM_SEL				0x00C8
#define APU_ACC_FM_CNT				0x00CC
#define APU_ACC_CLK_EN_SET			0x00E0
#define APU_ACC_CLK_EN_CLR			0x00E4
#define APU_ACC_CLK_INV_EN_SET			0x00E8
#define APU_ACC_CLK_INV_EN_CLR			0x00EC
#define APU_ACC_AUTO_CONFG0			0x0100
#define APU_ACC_AUTO_CONFG1			0x0104
#define APU_ACC_AUTO_CONFG2			0x0108
#define APU_ACC_AUTO_CONFG3			0x010C
#define APU_ACC_AUTO_CTRL_SET0			0x0120
#define APU_ACC_AUTO_CTRL_SET1			0x0124
#define APU_ACC_AUTO_CTRL_SET2			0x0128
#define APU_ACC_AUTO_CTRL_SET3			0x012C
#define APU_ACC_AUTO_CTRL_CLR0			0x0140
#define APU_ACC_AUTO_CTRL_CLR1			0x0144
#define APU_ACC_AUTO_CTRL_CLR2			0x0148
#define APU_ACC_AUTO_CTRL_CLR3			0x014C
#define APU_ACC_AUTO_STATUS0			0x0160
#define APU_ACC_AUTO_STATUS1			0x0164
#define APU_ACC_AUTO_STATUS2			0x0168

#define APU_ACC_AUTO_STATUS3			0x016C

/* ARE offset define */
#define APU_ARE_INI_CTRL			0x0000
#define APU_ARE_SRAM_CON			0x0004
#define APU_ARE_CONFG0				0x0040
#define APU_ARE_CONFG1				0x0044
#define APU_ARE_GLO_FSM				0x0048
#define APU_ARE_APB_FSM				0x004C
#define APU_ARE_ETRY0_SRAM_H			0x0C00
#define APU_ARE_ETRY0_SRAM_L			0x0800
#define APU_ARE_ETRY1_SRAM_H			0x0C04
#define APU_ARE_ETRY1_SRAM_L			0x0804
#define APU_ARE_ETRY2_SRAM_H			0x0C08
#define APU_ARE_ETRY2_SRAM_L			0x0808

/* vcore offset define */
#define APUSYS_VCORE_CG_CON			0x0000
#define APUSYS_VCORE_CG_SET			0x0004
#define APUSYS_VCORE_CG_CLR			0x0008
#define APUSYS_VCORE_SW_RST			0x000C

/* rcx offset define */
#define APU_RCX_CG_CON				0x0000
#define APU_RCX_CG_SET				0x0004
#define APU_RCX_CG_CLR				0x0008
#define APU_RCX_SW_RST				0x000C

/* acx 0/1 offset define */
#define APU_ACX_CONN_CG_CON			0x3C000
#define APU_ACX_CONN_CG_CLR			0x3C008
#define APU_ACX_MVPU_CG_CON			0x2B000
#define APU_ACX_MVPU_CG_CLR			0x2B008
#define APU_ACX_MVPU_SW_RST			0x2B00C
#define APU_ACX_MVPU_RV55_CTRL0			0x2B018
#define APU_ACX_MDLA0_CG_CON			0x30000
#define APU_ACX_MDLA0_CG_CLR			0x30008

// vlp offset define
#define APUSYS_AO_CTRL_ADDR			(0x200)

// spm offset define
#define APUSYS_BUCK_ISOLATION			(0x3EC)

// mbox offset define (for data exchange with remote)
#define SPARE0_MBOX_DUMMY_0_ADDR		0x640   // mbox6_dummy0
#define SPARE0_MBOX_DUMMY_1_ADDR		0x644   // mbox6_dummy1
#define SPARE0_MBOX_DUMMY_2_ADDR		0x648   // mbox6_dummy2
#define SPARE0_MBOX_DUMMY_3_ADDR		0x64C   // mbox6_dummy3
#define SPARE0_MBOX_DUMMY_4_ADDR		0x740   // mbox7_dummy0

/* apu_rcx_ao_ctrl  */
#define CSR_DUMMY_0_ADDR			(0x0024)

// PCU initial data
#define APU_PCUTOP_CTRL_SET			0x0

// PCU initial data
#define TOP_VRCTL_VR0_EN			0x240
#define TOP_VRCTL_VR0_EN_SET			0x241
#define TOP_VRCTL_VR0_EN_CLR			0x242
#define MT6368_PMIC_RG_BUCK_VBUCK4_EN_ADDR	TOP_VRCTL_VR0_EN
#define MT6368_PMIC_RG_BUCK_VBUCK4_EN_ADDR_SET	TOP_VRCTL_VR0_EN_SET
#define MT6368_PMIC_RG_BUCK_VBUCK4_EN_ADDR_CLR	TOP_VRCTL_VR0_EN_CLR
#define MT6368_PMIC_RG_BUCK_VBUCK4_EN_SHIFT	(4)

#define MT6359P_RG_BUCK_VMODEM_EN_ADDR		(0x1688)
#define MT6359P_RG_LDO_VSRAM_MD_EN_ADDR		(0x1f2e)
#define BUCK_VAPU_PMIC_REG_EN_ADDR		MT6359P_RG_BUCK_VMODEM_EN_ADDR
#define BUCK_VAPU_SRAM_PMIC_REG_EN_ADDR		MT6359P_RG_LDO_VSRAM_MD_EN_ADDR

#define MT6363_SLAVE_ID				(0x4)
#define MT6368_SLAVE_ID				(0x5)
#define MAIN_PMIC_ID				MT6363_SLAVE_ID
#define SUB_PMIC_ID				MT6368_SLAVE_ID
#define APU_PCU_BUCK_STEP_SEL			0x0030
#define APU_PCU_BUCK_ON_DAT0_L			0x0080
#define APU_PCU_BUCK_ON_DAT0_H			0x0084
#define APU_PCU_BUCK_ON_DAT1_L			0x0088
#define APU_PCU_BUCK_ON_DAT1_H			0x008C
#define APU_PCU_BUCK_OFF_DAT0_L			0x00A0
#define APU_PCU_BUCK_OFF_DAT0_H			0x00A4
#define APU_PCU_BUCK_OFF_DAT1_L			0x00A8
#define APU_PCU_BUCK_OFF_DAT1_H			0x00AC
#define APU_PCU_BUCK_ON_SLE0			0x00C0
#define APU_PCU_BUCK_ON_SLE1			0x00C4
#define APU_PCU_BUCK_ON_SETTLE_TIME		0x12C

#define ENABLE_SW_BUCK_CTL	(1) /* 1: enable regulator in rpm resume */

static inline void apu_writel(const unsigned int val, uintptr_t regs)
{
	mmio_write_32(regs, val);
}

static inline uint32_t apu_readl(uintptr_t regs)
{
	return mmio_read_32(regs);
}

static inline void apu_setl(const unsigned int val, uintptr_t regs)
{
	apu_writel((apu_readl(regs) | val), regs);
}

static inline void apu_clearl(const unsigned int val, uintptr_t regs)
{
	apu_writel((apu_readl(regs) & ~val), regs);
}

int apusys_power_init(void);
int apusys_kernel_apusys_pwr_rcx(uint32_t op);

#endif

