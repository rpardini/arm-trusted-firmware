/*
 * Copyright (c) 2022, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#include <arch_def.h>

#define PLAT_PRIMARY_CPU	(0x0)

#define MT_GIC_BASE		(0x0C000000)
#define MCUCFG_BASE		(0x0C530000)
#define MCUCFG_REG_SIZE		(0x10000)
#define IO_PHYS			(0x10000000)

/* Aggregate of all devices for MMU mapping */
#define MTK_DEV_RNG0_BASE	(MT_GIC_BASE)
#define MTK_DEV_RNG0_SIZE	(0x600000)
#define MTK_DEV_RNG1_BASE	(IO_PHYS)
#define MTK_DEV_RNG1_SIZE	(0x10000000)

/*=======================================================================*/
/* Register Bases                                                        */
/*=======================================================================*/
#define MCUCFG_BASE          (0x0C530000)
#define TOPCKGEN_BASE        (IO_PHYS)
#define INFRACFG_AO_BASE     (IO_PHYS + 0x00001000)
#define EINT_BASE            (IO_PHYS + 0x0000B000)
#define APMIXED_BASE         (IO_PHYS + 0x0000C000)
#define INFRA_AO_BCRM_BASE   (IO_PHYS + 0x00022000)
#define TRNG_BASE            (IO_PHYS + 0x0020f000)
#define AUDIO_BASE           (IO_PHYS + 0xb10000)
#define AUDIO_SRC_BASE       (IO_PHYS + 0xb00000)
#define CAMSYS_MAIN_BASE     (IO_PHYS + 0x6000000)
#define CAMSYS_RAWA_BASE     (IO_PHYS + 0x604f000)
#define CAMSYS_RAWB_BASE     (IO_PHYS + 0x608f000)
#define CAMSYS_YUVA_BASE     (IO_PHYS + 0x606f000)
#define CAMSYS_YUVB_BASE     (IO_PHYS + 0x60af000)
#define IMGSYS1_DIP_NR_BASE  (IO_PHYS + 0x5130000)
#define IMGSYS1_DIP_TOP_BASE (IO_PHYS + 0x5110000)
#define IMGSYS_MAIN_BASE     (IO_PHYS + 0x5000000)
#define IMGSYS_WPE1_BASE     (IO_PHYS + 0x5220000)
#define IMGSYS_WPE2_BASE     (IO_PHYS + 0x5520000)
#define IMGSYS_WPE3_BASE     (IO_PHYS + 0x5620000)
#define IMP_IIC_WRAP_C_BASE  (IO_PHYS + 0x1283000)
#define IMP_IIC_WRAP_EN_BASE (IO_PHYS + 0x1ec2000)
#define IMP_IIC_WRAP_W_BASE  (IO_PHYS + 0x1e02000)
#define IPESYS_BASE          (IO_PHYS + 0x5330000)
#define MD32DR_CH0_BASE      (IO_PHYS + 0x900000)
#define MD32DR_CH1_BASE      (IO_PHYS + 0xa00000)
#define MSDC0_TOP_BASE       (IO_PHYS + 0x1f50000)
#define MSDC2_TOP_BASE       (IO_PHYS + 0x1e60000)
#define PMIF_SPI_BASE        (IO_PHYS + 0x00024000)
#define PERICFG_BASE         (IO_PHYS + 0x3000)
#define SUB_EMI_BASE         (IO_PHYS + 0x21d000)
#define SYSTIMER_BASE        (IO_PHYS + 0x00017000)
#define VDEC_GCON_BASE       (IO_PHYS + 0x802f000)
#define VDEC_SOC_GCON_BASE   (IO_PHYS + 0x800f000)
#define VDO0_DISP_DSC_WRAP_BASE  (IO_PHYS + 0xc009000)
#define VDOSYS0_CONFIG_BASE  (IO_PHYS + 0xc01d000)
#define VDOSYS1_CONFIG_BASE  (IO_PHYS + 0xc100000)
#define VENC_GCON_BASE       (IO_PHYS + 0xa000000)
#define VPP0_REG_BASE        (IO_PHYS + 0x4000000)
#define VPPSYS1_CONFIG_BASE  (IO_PHYS + 0x4f00000)
#define WPESYS_TOP_REG_BASE  (IO_PHYS + 0x4e00000)
#define WPE_VPP0_BASE        (IO_PHYS + 0x4e02000)
#define CCU_MAIN_BASE        (IO_PHYS + 0x7200000)
#define MFGCFG_BASE          (IO_PHYS + 0x3fbf000)
#define APMIXEDSYS           (IO_PHYS + 0x0000C000)
#define SSPM_MBOX_BASE       (IO_PHYS + 0x00480000)
#define VPPSYS0_BASE         (IO_PHYS + 0x04000000)
#define VPPSYS1_BASE         (IO_PHYS + 0x04f00000)
#define VDOSYS0_BASE         (IO_PHYS + 0x0C01A000)
#define VDOSYS1_BASE         (IO_PHYS + 0x0C100000)
#define DVFSRC_BASE          (IO_PHYS + 0x00012000)
#define APU_IOMMU_0_BASE     (IO_PHYS + 0x09010000)
#define APU_IOMMU_1_BASE     (IO_PHYS + 0x09015000)

/*******************************************************************************
 * APUSYS related constants
 ******************************************************************************/
#define DEBUGSYS_APB			(0x0d000000)
#define APU_MD32_DEBUG_APB		(DEBUGSYS_APB + 0x00298000)
#define APU_MD32_SYSCTRL		(IO_PHYS + 0x09001000)
#define APU_MD32_WDT			(IO_PHYS + 0x09002000)
#define APU_RCX_CONFIG			(IO_PHYS + 0x09020000)
#define APU_LOGTOP			(IO_PHYS + 0x09024000)
#define APU_NOC_MNI_RCX			(IO_PHYS + 0x0903e000)
#define APU_MGALS			(IO_PHYS + 0x0903f000)
#define APUSYS_CTRL_DAPC_RCX_BASE	(IO_PHYS + 0x09034000)
#define APUSYS_NOC_DAPC_RCX_BASE	(IO_PHYS + 0x09038000)
#define APU_SCTRL_REVISER		(IO_PHYS + 0x0903c000)
#define APU_ACS_RCX			(IO_PHYS + 0x09044000)
#define APU_CACHE_DUMP			(IO_PHYS + 0x09050000)
#define APU_MBOX0			(IO_PHYS + 0x090e1000)
#define APU_MBOX1			(IO_PHYS + 0x090e2000)
#define APU_RPCTOP			(IO_PHYS + 0x090F0000)
#define APU_PCUTOP			(IO_PHYS + 0x090F1000)
#define APU_AO_CTRL			(IO_PHYS + 0x090f2000)
#define APU_PLL				(IO_PHYS + 0x090F3000)
#define APU_ACC				(IO_PHYS + 0x090F4000)
#define APU_SEC_CON			(IO_PHYS + 0x090F5000)
#define APU_ARETOP_ARE0			(IO_PHYS + 0x090F6000)
#define APU_ARETOP_ARE1			(IO_PHYS + 0x090F7000)
#define APU_ARETOP_ARE2			(IO_PHYS + 0x090F8000)
#define APUSYS_CTRL_DAPC_AO_BASE	(IO_PHYS + 0x090FC000)
#define APU_RCX_VCORE_CONFIG		(IO_PHYS + 0x090E0000)
#define APU_ACX0			(IO_PHYS + 0x09100000)
#define APU_ACX0_RPC_LITE		(IO_PHYS + 0x09140000)
#define APU_ACX0_CONFIG			(IO_PHYS + 0x0913C000)
#define APU_ACS_ACX0			(IO_PHYS + 0x09112000)
#define APU_MD32_TCM			(IO_PHYS + 0x0d000000)

#define APU_ACS_RCX_SZ			0x1000
#define APU_RPCTOP_LITE_ACX0_SZ		0x1000
#define APU_ACX0_CONFIG_SZ		0x1000
#define APU_ACS_ACX0_SZ			0x1000
#define APU_SEC_CON_SZ			0x1000
#define APU_RPCTOP_SZ			0x1000
#define APU_PCUTOP_SZ			0x1000
#define APU_ARETOP_ARE0_SZ		0x1000
#define APU_ARETOP_ARE1_SZ		0x1000
#define APU_ARETOP_ARE2_SZ		0x1000
#define APU_PLL_COMMON_SZ		0x1000
#define APU_LOGTOP_SZ			0x1000
#define APU_NOC_MNI_RCX_SZ		0x1000
#define APU_MGALS_SZ			0x1000

/*******************************************************************************
 * GPIO related constants
 ******************************************************************************/
#define GPIO_BASE		(IO_PHYS + 0x00005000)
#define RGU_BASE		(IO_PHYS + 0x00007000)
#define DRM_BASE		(IO_PHYS + 0x0000D000)
#define IOCFG_RM_BASE		(IO_PHYS + 0x01C00000)
#define IOCFG_LT_BASE		(IO_PHYS + 0x01E10000)
#define IOCFG_LM_BASE		(IO_PHYS + 0x01E20000)
#define IOCFG_RT_BASE		(IO_PHYS + 0x01EA0000)

/*******************************************************************************
 * UART related constants
 ******************************************************************************/
#define UART0_BASE	(IO_PHYS + 0x01001100)
#define UART1_BASE	(IO_PHYS + 0x01001200)

/*******************************************************************************
 * PMIC related constants
 ******************************************************************************/
#define PMIC_WRAP_BASE		(IO_PHYS + 0x00024000)

#define SPM_BASE		(IO_PHYS + 0x00006000)
#define MCUSYS_CFGREG_BASE	(0x0C530000)

/*******************************************************************************
 * MCUPM related constants
 ******************************************************************************/
#define	MCUPM_SRAM_BASE		(0x0C540000)
#define	MCUPM_CFG_BASE		(0x0C560000)

/*******************************************************************************
 * SSPM related constants
 ******************************************************************************/
#define SSPM_SRAM_BASE		(IO_PHYS + 0x00400000)
#define SSPM_CFG_BASE		(IO_PHYS + 0x00440000)

/*******************************************************************************
 * Infra IOMMU related constants
 ******************************************************************************/
#define PERICFG_AO_BASE		(IO_PHYS + 0x01003000)
#define PERICFG_AO_REG_SIZE	(0x1000)

/*******************************************************************************
 * GIC-600 & interrupt handling related constants
 ******************************************************************************/
/* Base MTK_platform compatible GIC memory map */
#define BASE_GICD_BASE		(MT_GIC_BASE)
#define MT_GIC_RDIST_BASE	(MT_GIC_BASE + 0x40000)

/*******************************************************************************
 * CIRQ related constants
 ******************************************************************************/
#define SYS_CIRQ_BASE		(IO_PHYS + 0x204000)
#define MD_WDT_IRQ_BIT_ID	(141)
#define CIRQ_IRQ_NUM		(730)
#define CIRQ_REG_NUM		(23)
#define CIRQ_SPI_START		(96)

/*******************************************************************************
 * MM IOMMU & SMI related constants
 ******************************************************************************/
#define SMI_LARB_0_BASE		(IO_PHYS + 0x0c022000)
#define SMI_LARB_1_BASE		(IO_PHYS + 0x0c023000)
#define SMI_LARB_2_BASE		(IO_PHYS + 0x0c102000)
#define SMI_LARB_3_BASE		(IO_PHYS + 0x0c103000)
#define SMI_LARB_4_BASE		(IO_PHYS + 0x04013000)
#define SMI_LARB_5_BASE		(IO_PHYS + 0x04f02000)
#define SMI_LARB_6_BASE		(IO_PHYS + 0x04f03000)
#define SMI_LARB_7_BASE		(IO_PHYS + 0x04e04000)
#define SMI_LARB_9_BASE		(IO_PHYS + 0x05001000)
#define SMI_LARB_10_BASE	(IO_PHYS + 0x05120000)
#define SMI_LARB_11A_BASE	(IO_PHYS + 0x05230000)
#define SMI_LARB_11B_BASE	(IO_PHYS + 0x05530000)
#define SMI_LARB_11C_BASE	(IO_PHYS + 0x05630000)
#define SMI_LARB_12_BASE	(IO_PHYS + 0x05340000)
#define SMI_LARB_13_BASE	(IO_PHYS + 0x06001000)
#define SMI_LARB_14_BASE	(IO_PHYS + 0x06002000)
#define SMI_LARB_15_BASE	(IO_PHYS + 0x05140000)
#define SMI_LARB_16A_BASE	(IO_PHYS + 0x06008000)
#define SMI_LARB_16B_BASE	(IO_PHYS + 0x0600a000)
#define SMI_LARB_17A_BASE	(IO_PHYS + 0x06009000)
#define SMI_LARB_17B_BASE	(IO_PHYS + 0x0600b000)
#define SMI_LARB_19_BASE	(IO_PHYS + 0x0a010000)
#define SMI_LARB_21_BASE	(IO_PHYS + 0x0802e000)
#define SMI_LARB_23_BASE	(IO_PHYS + 0x0800d000)
#define SMI_LARB_27_BASE	(IO_PHYS + 0x07201000)
#define SMI_LARB_28_BASE	(IO_PHYS + 0x00000000)
#define SMI_LARB_REG_RNG_SIZE	(0x1000)

/*******************************************************************************
 * DP related constants
 ******************************************************************************/
#define EDP_SEC_BASE		(IO_PHYS + 0x0C504000)
#define DP_SEC_BASE		(IO_PHYS + 0x0C604000)
#define EDP_SEC_SIZE		(0x1000)
#define DP_SEC_SIZE		(0x1000)

/*******************************************************************************
 * EMI MPU related constants
 *******************************************************************************/
#define EMI_MPU_BASE		(IO_PHYS + 0x00226000)
#define SUB_EMI_MPU_BASE	(IO_PHYS + 0x00225000)

/*******************************************************************************
 * System counter frequency related constants
 ******************************************************************************/
#define SYS_COUNTER_FREQ_IN_HZ	(13000000)
#define SYS_COUNTER_FREQ_IN_MHZ	(13)

/*******************************************************************************
 * Platform binary types for linking
 ******************************************************************************/
#define PLATFORM_LINKER_FORMAT		"elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH		aarch64

/*******************************************************************************
 * Generic platform constants
 ******************************************************************************/
#define PLATFORM_STACK_SIZE		(0x800)
#define FIRMWARE_WELCOME_STR		"Booting Trusted Firmware\n"
#define SOC_CHIP_ID			U(0x8188)

/*******************************************************************************
 * Platform memory map related constants
 ******************************************************************************/
#define TZRAM_BASE			(0x54600000)
#define TZRAM_SIZE			(0x00200000)

/*******************************************************************************
 * BL2 specific defines.
******************************************************************************/
#define BL2_BASE		(0x201000)
#define BL2_LIMIT		(0x400000)

#define MAX_IO_DEVICES			U(3)
#define MAX_IO_HANDLES			U(4)
#define MAX_IO_BLOCK_DEVICES			1

#define BL31_BASE		(TZRAM_BASE + 0x1000)
#define BL31_LIMIT		(TZRAM_BASE + TZRAM_SIZE)
#define TZRAM2_LIMIT		(TZRAM2_BASE + TZRAM2_SIZE)

#define BL32_BASE		(0x43200000)
#define BL32_LIMIT		(0x00a00000)
#define BL32_HEADER_SIZE	(0x1c)

#define BL33_BASE		(0x4c000000)

/*******************************************************************************
 * BL31 specific defines.
 ******************************************************************************/
/*
 * Put BL3-1 at the top of the Trusted SRAM (just below the shared memory, if
 * present). BL31_BASE is calculated using the current BL3-1 debug size plus a
 * little space for growth.
 */
#define BL31_BASE			(TZRAM_BASE + 0x1000)
#define BL31_LIMIT			(TZRAM_BASE + TZRAM_SIZE)

/*******************************************************************************
 * Platform specific page table and MMU setup constants
 ******************************************************************************/
#define PLAT_PHY_ADDR_SPACE_SIZE	(1ULL << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(1ULL << 32)
#define MAX_XLAT_TABLES			(16)
#define MAX_MMAP_REGIONS		(16)

/*******************************************************************************
 * CPU_EB TCM handling related constants
 ******************************************************************************/
#define CPU_EB_TCM_BASE		(0x0C550000)
#define CPU_EB_TCM_SIZE		(0x10000)
#define CPU_EB_MBOX3_OFFSET	(0xFCE0)

/*******************************************************************************
 * CPU PM definitions
 *******************************************************************************/
#define PLAT_CPU_PM_B_BUCK_ISO_ID	(6)
#define PLAT_CPU_PM_ILDO_ID		(6)
#define CPU_IDLE_SRAM_BASE		(0x11B000)
#define CPU_IDLE_SRAM_SIZE		(0x1000)

#endif /* PLATFORM_DEF_H */
