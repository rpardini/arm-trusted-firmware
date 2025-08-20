/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once
#include <platform_def.h>
#include <mt_spm_reg.h>

#define CFG_APU_ARDCM_ENABLE    (1)
#define CFG_CTL_RPC_BY_CE       (1)
#define CFG_DBG_DUMP_ARE_ENTRY  (1)
#define CFG_SECURE_CTL          (0)
#define CFG_PLL_DIRECT_INIT     (0)

enum t_acx_id {
	CLUSTER_NUM,
};

enum apupw_reg {
	sys_vlp = 0,
	sys_spm,
	sys_apmixedsys,
	apu_rcx,
	apu_rcx_dla,
	apu_are,
	apu_are_reg, /* 5 */
	apu_vcore,
	apu_md32_mbox,
	apu_rpc,
	apu_pcu,
	apu_ao_ctl, /* 10 */
	apu_acc,
	apu_pll,
	apu_rpctop_mdla,
	apu_toppmu,
	APUPW_MAX_REGS,
};

enum apu_clksrc_id {
	PLL_CONN = 0, // MNOC
	PLL_DLA,
	PLL_NUM,
};

struct apu_power {
	unsigned long regs[APUPW_MAX_REGS];
	unsigned int phy_addr[APUPW_MAX_REGS];
};

#if CFG_PLL_DIRECT_INIT
enum rcx_ao_range {
RCX_AO_BEGIN = 0,
	PLL_ENTRY_BEGIN = 0, //2(pll)*6(steps) = 12
	PLL_ENTRY_END = 0,
	ACC_ENTRY_BEGIN = 0, //13 ARE entries, mnoc(6) + mdla (7)
	ACC_ENTRY_END = 12,
RCX_AO_END = 12,
};

#else
enum rcx_ao_range {
RCX_AO_BEGIN = 0,
	PLL_ENTRY_BEGIN = 0,
	PLL_ENTRY_END = 0,
	ACC_ENTRY_BEGIN = 0, //11 ARE entries, ARDCM(5) + ACC (6)
	ACC_ENTRY_END = 11,
RCX_AO_END = 11,
};

#endif

#define SYS_VLP                   0x000000 // sys_vlp
#define SYS_SPM                   0x000000 // sys_spm
#define SYS_APMIXEDSYS            0x00c000 // sys_apmixedsys
#define APU_RCX                   0x020000 // apu_rcx
#define APU_RCX_DLA               0x040000 // dummy acx for rcx_dal
#define APU_ARE                   0x0a0000 // apu_are_sram
#define APU_ARE_REG               0x0b0000 // apu_are_reg
#define APU_VCORE                 0x0e0000 // apu_vcore
#define APU_MD32_MBOX             0x0e1000 // apu_md32_mbox 0
#define APU_RPC                   0x0f0000 // apu_rpc
#define APU_PCU                   0x0f1000 // apu_pcu
#define APU_AO_CTL                0x0f2000 // apu_ao_ctl
#define APU_ACC                   0x0f3000 // apu_acc
#define APU_PLL                   0x0f6000 // apu_pll
#define APU_RPCTOP_MDLA           0x0F7400 // rpc for rcx_dla
#define APU_ACX0                  0x100000 // apu_acx0
#define APU_ACX0_RPC_LITE         0x140000 // apu_acx0_rpc_lite
#define APU_TOP_PMU               0x0f7c00 // apu_toppmu

/* RPC offset define */
#define APU_RPC_TOP_CON           0x0000
#define APU_RPC_TOP_SEL           0x0004
#define APU_RPC_SW_FIFO_WE        0x0008
#define APU_RPC_IO_DEBUG          0x000C
#define APU_RPC_STATUS            0x0014
#define APU_RPC_TOP_SEL_1         0x0018
#define APU_RPC_HW_CON            0x001C
#define APU_RPC_LITE_CON          0x0020
#define APU_RPC_HW_CON1           0x0030
#define APU_RPC_INTF_PWR_RDY_REG  0x0040
#define APU_RPC_INTF_PWR_RDY      0x0044

/* APU GRP offset define */
#define APU_GRP_0_BASE            0x000  // mdla:0x190F3000, 0x190F6000
#define APU_GRP_2_BASE            0x800  // mnoc:0x190F3800, 0x190F6800

#define MDLA_PLL_BASE             APU_GRP_0_BASE
#define MNOC_PLL_BASE             APU_GRP_2_BASE

#define MDLA_ACC_BASE             APU_GRP_0_BASE
#define MNOC_ACC_BASE             APU_GRP_2_BASE

// ACC offset
#define APU_ACC_CONFG_SET0        0x000
#define APU_ACC_CONFG_CLR0        0x010
#define APU_ACC_FM_CONFG_SET      0x020
#define APU_ACC_FM_CONFG_CLR      0x024
#define APU_ACC_FM_SEL            0x028
#define APU_ACC_FM_CNT            0x02C
#define APU_ACC_AUTO_CONFG0       0x080
#define APU_ACC_AUTO_CTRL_SET0    0x084
#define APU_ACC_AUTO_CTRL_CLR0    0x088
#define APU_ACC_AUTO_STATUS0      0x08C
#define APU_ARDCM_CTRL0           0x100
#define APU_ARDCM_CTRL1           0x104
// APU PLL1C offset
#define PLL1C_PLL1_CON1           0x20C
#define PLL1CPLL_FHCTL_HP_EN      0x300
#define PLL1CPLL_FHCTL_CLK_CON    0x308
#define PLL1CPLL_FHCTL_RST_CON    0x30C
#define PLL1CPLL_FHCTL0_CFG       0x314
#define PLL1CPLL_FHCTL0_DDS       0x31C

/* ARE offset define */
//#define APU_ACE_HW_FLAG_DIS     0x05D4

/* vcore offset define */
#define APUSYS_VCORE_CG_CON       0x0000
#define APUSYS_VCORE_CG_SET       0x0004
#define APUSYS_VCORE_CG_CLR       0x0008
#define APUSYS_VCORE_SW_RST       0x000C

/* rcx offset define */
#define APU_RCX_CG_CON            0x0000
#define APU_RCX_CG_SET            0x0004
#define APU_RCX_CG_CLR            0x0008
#define APU_RCX_SW_RST            0x000C

// vlp offset define
#define APUSYS_AO_CTRL_ADDR       (0x200)
#define APUSYS_AO_SRAM_CONFIG     (0x70)
#define APUSYS_AO_SRAM_SET        (0x74)
#define APUSYS_AO_SRAM_CLR        (0x78)

// spm offset define
#define APUSYS_BUCK_ISOLATION     (0x39C)

// mbox offset define (for data exchange with remote)
#define SPARE0_MBOX_DUMMY_0_ADDR  0x640   // mbox6_dummy0
#define SPARE0_MBOX_DUMMY_1_ADDR  0x644   // mbox6_dummy1
#define SPARE0_MBOX_DUMMY_2_ADDR  0x648   // mbox6_dummy2
#define SPARE0_MBOX_DUMMY_3_ADDR  0x64C   // mbox6_dummy3
#define SPARE0_MBOX_DUMMY_4_ADDR  0x740   // mbox7_dummy0

#define BUCK_VAPU_PMIC_REG_EN_ADDR      (0x1508)
#define BUCK_VAPU_PMIC_REG_EN_SET_ADDR  (0x1508)
#define BUCK_VAPU_PMIC_REG_EN_CLR_ADDR  (0x1508)
#define BUCK_VAPU_PMIC_REG_EN_SHIFT     (0)
#define BUCK_VAPU_PMIC_REG_VOSEL_ADDR   (0x152c)

#define LDO_VSRAM_PMIC_REG_EN_ADDR      (0x1f08)
#define LDO_VSRAM_PMIC_REG_EN_SHIFT     (0)
#define LDO_VSRAM_PMIC_REG_LP_ADDR      (0x1f08)
#define LDO_VSRAM_PMIC_REG_LP_SHIFT     (1)

// PCU initial data
#define APU_PCUTOP_CTRL_SET         0x0
#define APU_PCU_BUCK_STEP_SEL       0x0030
#define APU_PCU_BUCK_ON_DAT0_L      0x0080
#define APU_PCU_BUCK_ON_DAT0_H      0x0084
#define APU_PCU_BUCK_ON_DAT1_L      0x0088
#define APU_PCU_BUCK_ON_DAT1_H      0x008C
#define APU_PCU_BUCK_ON_DAT2_L      0x0090
#define APU_PCU_BUCK_ON_DAT2_H      0x0094
#define APU_PCU_BUCK_OFF_DAT0_L     0x00A0
#define APU_PCU_BUCK_OFF_DAT0_H     0x00A4
#define APU_PCU_BUCK_OFF_DAT1_L     0x00A8
#define APU_PCU_BUCK_OFF_DAT1_H     0x00AC
#define APU_PCU_BUCK_ON_SLE0        0x00C0
#define APU_PCU_BUCK_ON_SLE1        0x00C4
#define APU_PCU_BUCK_ON_SLE2        0x00C8
#define APU_PCU_BUCK_OFF_SLE0       0x00D0
#define APU_PCU_BUCK_OFF_SLE1       0x00D4
#define APU_PCU_BUCK_OFF_SLE2       0x00D8
#define VAPU_BUCK_ON_SETTLE_TIME    0x12C
#define VAPU_BUCK_OFF_SETTLE_TIME   0x12C
#define APU_PCU_PMIC_TAR_BUF1       0x0190
#define APU_PCU_PMIC_TAR_BUF2       0x0194
#define APU_PCU_PMIC_CMD            0x0184
#define APU_PCU_PMIC_IRQ            0x0180
#define APU_PCU_PMIC_RDATA          0x0188

//#define udelay(x)   spin(x)
#define ARE_ENTRIES(x, y) ((((y) - (x)) + 1) * 2)
#define ARE_ENTRY(x) (((x) * 2) + 36)
#define ARE_RCX_AO_CONFIG           0x0014

#define MDLA_DEFAULT_FREQ   900
#define APUTOP_DEFAULT_FREQ 630

#define BIT(nr) (1 << (nr))

/* APU_ACC_CONFG_SET0 */
#define APU_ACC_CGEN_SOC    BIT(2)
#define APU_ACC_CGEN_APU    BIT(3)
#define APU_ACC_SEL_APU     BIT(11)
#define APU_ACC_HW_CTRL_EN  BIT(15)
/* APU_ACC_AUTO_CTRL_SET0 */
#define APU_ACC_CLK_REQ_SW_EN   BIT(8)

// ACC offset
#define APU_ACC_CONFG_SET0        0x000
#define APU_ACC_CONFG_CLR0        0x010
#define APU_ACC_FM_CONFG_SET      0x020
#define APU_ACC_FM_CONFG_CLR      0x024
#define APU_ACC_FM_SEL            0x028
#define APU_ACC_FM_CNT            0x02C
#define APU_ACC_AUTO_CONFG0       0x080
#define APU_ACC_AUTO_CTRL_SET0    0x084
#define APU_ACC_AUTO_CTRL_CLR0    0x088
#define APU_ACC_AUTO_STATUS0      0x08C
#define APU_ARDCM_CTRL0           0x100
#define APU_ARDCM_CTRL1           0x104

/* APUPLL in APMIXEDSYS */
#define APUPLL_CON0                 0x604
#define APUPLL_CON1                 0x608
#define APUPLL2_CON0                0x614
#define APUPLL2_CON1                0x618
/* FHCTL in APMIXEDSYS */
#define APMIXEDSYS_FHCTL_HP_EN      0xE00
#define APMIXEDSYS_FHCTL_CLK_CON    0xE08
#define APMIXEDSYS_FHCTL_RST_CON    0xE0C
#define APMIXEDSYS_FHCTL11_CFG      0xF18
#define APMIXEDSYS_FHCTL11_DDS      0xF20
#define APMIXEDSYS_FHCTL12_CFG      0xF2C
#define APMIXEDSYS_FHCTL12_DDS      0xF34

// apu toppmu
#define APU_ARDCM_CFG               0x80

#define APUSYS_BASE                (0x19000000)
#define APUSYS_CE_BASE             (0x190B0000)
#define APU_ACE_HW_FLAG_DIS        (APUSYS_CE_BASE + 0x05D4)

static inline void apu_writel(const unsigned int val, unsigned long regs)
{
	mmio_write_32(regs, val);
	/* make sure all the write instructions are done */
	// smp_mb();
}

static inline uint32_t apu_readl(unsigned long regs)
{
	return mmio_read_32(regs);
}

static inline void apu_setl(const unsigned int val, unsigned long regs)
{
	mmio_setbits_32(regs, val);
}

static inline void apu_clearl(const unsigned int val, unsigned long regs)
{
	mmio_clrbits_32(regs, val);
}

int apusys_power_init(void);
