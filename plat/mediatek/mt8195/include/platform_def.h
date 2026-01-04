/*
 * Copyright (c) 2021-2022, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#define PLAT_PRIMARY_CPU	0x0

#define MT_GIC_BASE		(0x0C000000)
#define MCUCFG_BASE		(0x0C530000)
#define IO_PHYS			(0x10000000)

/* Aggregate of all devices for MMU mapping */
#define MTK_DEV_RNG0_BASE	IO_PHYS
#define MTK_DEV_RNG0_SIZE	0x10000000
#define MTK_DEV_RNG2_BASE	MT_GIC_BASE
#define MTK_DEV_RNG2_SIZE	0x600000
#define MTK_MCDI_SRAM_BASE	0x11B000
#define MTK_MCDI_SRAM_MAP_SIZE	0x1000

#define SYSTIMER_BASE        (IO_PHYS + 0x00017000)

#define TOPCKGEN_BASE           (IO_PHYS + 0x00000000)
#define INFRACFG_AO_BASE        (IO_PHYS + 0x00001000)
#define SPM_BASE		(IO_PHYS + 0x00006000)
#define RGU_BASE		(IO_PHYS + 0x00007000)
#define APMIXEDSYS              (IO_PHYS + 0x0000C000)
#define DRM_BASE		(IO_PHYS + 0x0000D000)
#define SSPM_MBOX_BASE          (IO_PHYS + 0x00480000)
#define PERICFG_AO_BASE         (IO_PHYS + 0x01003000)
#define VPPSYS0_BASE            (IO_PHYS + 0x04000000)
#define VPPSYS1_BASE            (IO_PHYS + 0x04f00000)
#define VDOSYS0_BASE            (IO_PHYS + 0x0C01A000)
#define VDOSYS1_BASE            (IO_PHYS + 0x0C100000)
#define DVFSRC_BASE             (IO_PHYS + 0x00012000)

 /*******************************************************************************
  * IOMMU & SMI related constants
  ******************************************************************************/
 #define APU_IOMMU_0_BASE		(IO_PHYS + 0x09010000)
 #define SEC_APU_IOMMU_0_BASE		(IO_PHYS + 0x09014000)
 #define APU_IOMMU_1_BASE		(IO_PHYS + 0x09015000)
 #define SEC_APU_IOMMU_1_BASE		(IO_PHYS + 0x09019000)

/*******************************************************************************
 * DP/eDP related constants
 ******************************************************************************/
#define EDP_SEC_BASE		(IO_PHYS + 0x0C504000)
#define DP_SEC_BASE		(IO_PHYS + 0x0C604000)
#define EDP_SEC_SIZE		0x1000
#define DP_SEC_SIZE		0x1000
/*******************************************************************************
 * APUSYS related constants
 ******************************************************************************/
#define APU_MD32_DEBUG_APB		(0x0d19c000)
#define APU_MD32_SYSCTRL		(IO_PHYS + 0x09001000)
#define APU_MD32_WDT			(IO_PHYS + 0x09002000)
#define APU_RCX_CONFIG			(IO_PHYS + 0x09020000)
#define APU_SCTRL_REVISER		(IO_PHYS + 0x09021000)
#define APU_MBOX0			(IO_PHYS + 0x09000000)
#define APU_MBOX1			(IO_PHYS + 0x09000100)
#define APU_RPCTOP			(IO_PHYS + 0x090F0000)
#define APU_PCUTOP			(IO_PHYS + 0x090F1000)
#define APU_AO_CTRL			(IO_PHYS + 0x090f2000)
#define APU_PLL				(IO_PHYS + 0x090F3000)
#define APU_ACC				(IO_PHYS + 0x090F4000)
#define APU_SEC_CON			(IO_PHYS + 0x090F5000)
#define APU_ARETOP_ARE0			(IO_PHYS + 0x090F6000)
#define APU_ARETOP_ARE1			(IO_PHYS + 0x090F7000)
#define APU_ARETOP_ARE2			(IO_PHYS + 0x090F8000)
#define APU_RCX_VCORE_CONFIG	(IO_PHYS + 0x09029000)
#define APU_MD32_TCM			(IO_PHYS + 0x0df00000)

#define APUSYS_BASE			0x19000000
#define APUSYS_APC_AO_WRAPPER_OFFSET	0xf8000
#define APUSYS_APC_AO_WRAPPER_BASE	(APUSYS_BASE +	\
					 APUSYS_APC_AO_WRAPPER_OFFSET)
#define APUSYS_APC_AO_WRAPPER_SIZE	0x1000
#define APUSYS_SCTRL_REVISER_OFFSET		0x21000
#define APUSYS_SCTRL_REVISER_BASE	(APUSYS_BASE + \
					APUSYS_SCTRL_REVISER_OFFSET)
#define APUSYS_SCTRL_REVISER_SIZE	0x1000
#define APUSYS_APU_S_S_4_OFFSET		0xF2000
#define APUSYS_APU_S_S_4_BASE		(APUSYS_BASE + \
					APUSYS_APU_S_S_4_OFFSET)
#define APUSYS_APU_S_S_4_SIZE		0x1000

#define APUSYS_NOC_DAPC_AO_OFFSET	0xfc000
#define APUSYS_NOC_DAPC_AO_BASE		(APUSYS_BASE +	\
					APUSYS_NOC_DAPC_AO_OFFSET)
#define APUSYS_NOC_DAPC_AO_SIZE		0x1000

/*******************************************************************************
 * GPIO related constants
 ******************************************************************************/
#define GPIO_BASE		(IO_PHYS + 0x00005000)
#define IOCFG_BM_BASE		(IO_PHYS + 0x01D10000)
#define IOCFG_BL_BASE		(IO_PHYS + 0x01D30000)
#define IOCFG_BR_BASE		(IO_PHYS + 0x01D40000)
#define IOCFG_LM_BASE		(IO_PHYS + 0x01E20000)
#define IOCFG_RB_BASE		(IO_PHYS + 0x01EB0000)
#define IOCFG_TL_BASE		(IO_PHYS + 0x01F40000)

/*******************************************************************************
 * UART related constants
 ******************************************************************************/
#define UART0_BASE			(IO_PHYS + 0x01001100)
#define UART1_BASE			(IO_PHYS + 0x01001200)

#define UART_CLOCK    26000000
#define UART_BAUDRATE			921600

/*******************************************************************************
 * PMIC related constants
 ******************************************************************************/
#define PMIC_WRAP_BASE			(IO_PHYS + 0x00024000)

/*******************************************************************************
 * EMI MPU related constants
 ******************************************************************************/
#define EMI_MPU_BASE		(IO_PHYS + 0x00226000)
#define SUB_EMI_MPU_BASE	(IO_PHYS + 0x00225000)

/*******************************************************************************
 * SNFC related constants
 ******************************************************************************/
#define SNFC_REG_BASE			0x1132C000
#define SNFC_MEM_BASE			0x28000000
#define EXTRA_DUMMY_BIT			0

#define SNFC_GPIO_GPOUP1		(GPIO_BASE + 0x400)
#define SNFC_GPIO_GPOUP2		(GPIO_BASE + 0x410)

#define SNFC_PU0_SET_GROUP		(IOCFG_BM_BASE + 0x0B0)
#define SNFC_PU1_SET_GROUP		(IOCFG_BR_BASE + 0x070)

#define SNFC_PD0_SET_GROUP		(IOCFG_BM_BASE + 0x090)
#define SNFC_PD1_SET_GROUP		(IOCFG_BR_BASE + 0x050)

#define SNFC_DRIVING0_SET		(IOCFG_BM_BASE + 0x020)
#define SNFC_DRIVING1_SET		(IOCFG_BR_BASE + 0x010)
#define SNFC_DRIVING2_SET		(IOCFG_BR_BASE + 0x020)

/*******************************************************************************
 * System counter frequency related constants
 ******************************************************************************/
#define SYS_COUNTER_FREQ_IN_TICKS	13000000
#define SYS_COUNTER_FREQ_IN_MHZ		13

/*******************************************************************************
 * GIC-600 & interrupt handling related constants
 ******************************************************************************/
/* Base MTK_platform compatible GIC memory map */
#define BASE_GICD_BASE			MT_GIC_BASE
#define MT_GIC_RDIST_BASE		(MT_GIC_BASE + 0x40000)

#define SYS_CIRQ_BASE			(IO_PHYS + 0x204000)
#define CIRQ_REG_NUM			23
#define CIRQ_IRQ_NUM			730
#define CIRQ_SPI_START			96
#define MD_WDT_IRQ_BIT_ID		141
/*******************************************************************************
 * Platform binary types for linking
 ******************************************************************************/
#define PLATFORM_LINKER_FORMAT		"elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH		aarch64

/*******************************************************************************
 * Generic platform constants
 ******************************************************************************/
#define PLATFORM_STACK_SIZE		0x1000

#define FIRMWARE_WELCOME_STR		"Booting Trusted Firmware\n"

#define PLAT_MAX_PWR_LVL		U(3)
#define PLAT_MAX_RET_STATE		U(1)
#define PLAT_MAX_OFF_STATE		U(9)

#define PLATFORM_SYSTEM_COUNT		U(1)
#define PLATFORM_MCUSYS_COUNT		U(1)
#define PLATFORM_CLUSTER_COUNT		U(1)
#define PLATFORM_CLUSTER0_CORE_COUNT	U(8)
#define PLATFORM_CLUSTER1_CORE_COUNT	U(0)

#define PLATFORM_CORE_COUNT		(PLATFORM_CLUSTER0_CORE_COUNT)
#define PLATFORM_MAX_CPUS_PER_CLUSTER	U(8)

#define SOC_CHIP_ID			U(0x8195)

/*******************************************************************************
 * Platform memory map related constants
 ******************************************************************************/
#define TZRAM_BASE          0x54600000
#define TZRAM_SIZE          0x00200000

/*******************************************************************************
 * BL2 specific defines.
******************************************************************************/
#define BL2_BASE		(0x201000)
#define BL2_LIMIT		(0x400000)

#define BOOT_ARGUMENT_LOCATION	(0x40000100) // Gotta match u-boot's arch/arm/include/asm/arch-mediatek/misc.h

#define MAX_IO_DEVICES			U(3)
#define MAX_IO_HANDLES			U(4)
#define MAX_IO_BLOCK_DEVICES			1
#define MAX_IO_MTD_DEVICES              U(1)

#define BL31_BASE		(TZRAM_BASE + 0x1000)
#define BL31_LIMIT		(TZRAM_BASE + TZRAM_SIZE)
#define TZRAM2_LIMIT		(TZRAM2_BASE + TZRAM2_SIZE)

#define BL32_BASE              (0x43200000)
#define BL32_LIMIT             (0x00a00000)
#define BL32_HEADER_SIZE	(0x1c)

#define BL33_BASE		(0x4c000000)
#define BL33_LIMIT		(0x00300000)

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
#define MAX_XLAT_TABLES			16
#define MAX_MMAP_REGIONS		16

/*******************************************************************************
 * Declarations and constants to access the mailboxes safely. Each mailbox is
 * aligned on the biggest cache line size in the platform. This is known only
 * to the platform as it might have a combination of integrated and external
 * caches. Such alignment ensures that two maiboxes do not sit on the same cache
 * line at any cache level. They could belong to different cpus/clusters &
 * get written while being protected by different locks causing corruption of
 * a valid mailbox address.
 ******************************************************************************/
#define CACHE_WRITEBACK_SHIFT		6
#define CACHE_WRITEBACK_GRANULE		(1 << CACHE_WRITEBACK_SHIFT)
#endif /* PLATFORM_DEF_H */
