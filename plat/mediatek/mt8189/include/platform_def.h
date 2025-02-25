/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#include <arch_def.h>

#define PLAT_PRIMARY_CPU        (0x0)

#define MT_GIC_BASE             (0x0C000000)
#define MCUCFG_BASE             (0x0C530000)
#define MCUCFG_REG_SIZE         (0x10000)
#define IO_PHYS                 (0x10000000)

/* Aggregate of all devices for MMU mapping */
#define MTK_DEV_RNG0_BASE       (MT_GIC_BASE)
#define MTK_DEV_RNG0_SIZE       (0x600000)
#define MTK_DEV_RNG1_BASE       (IO_PHYS)
#define MTK_DEV_RNG1_SIZE       (0x10000000)

#define TOPCKGEN_BASE           (IO_PHYS)

/*******************************************************************************
 * MMinfra related constants
 ******************************************************************************/
#define MMINFRA_BUS_DBG_BASE	(IO_PHYS + 0x0E825000)
#define MMINFRA_BUS_HRE_BASE	(IO_PHYS + 0x0E900000)

/*******************************************************************************
 * Infra IOMMU related constants
 ******************************************************************************/
#define PERICFG_AO_BASE		(IO_PHYS + 0x01036000)
#define PERICFG_AO_REG_SIZE	(0x1000)

/*******************************************************************************
 * MM IOMMU & SMI related constants
 ******************************************************************************/
#define SMI_LARB_0_BASE		(IO_PHYS + 0x0401c000)
#define SMI_LARB_1_BASE		(IO_PHYS + 0x0401d000)
#define SMI_LARB_2_BASE		(IO_PHYS + 0x0f002000)
#define SMI_LARB_4_BASE		(IO_PHYS + 0x0602e000)
#define SMI_LARB_7_BASE		(IO_PHYS + 0x07010000)
#define SMI_LARB_9_BASE		(IO_PHYS + 0x0502e000)
#define SMI_LARB_11_BASE	(IO_PHYS + 0x0582e000)
#define SMI_LARB_13_BASE	(IO_PHYS + 0x0a001000)
#define SMI_LARB_14_BASE	(IO_PHYS + 0x0a002000)
#define SMI_LARB_16_BASE	(IO_PHYS + 0x0a00f000)
#define SMI_LARB_17_BASE	(IO_PHYS + 0x0a010000)
#define SMI_LARB_19_BASE	(IO_PHYS + 0x0b10f000)
#define SMI_LARB_20_BASE	(IO_PHYS + 0x0b00f000)
#define SMI_LARB_REG_RNG_SIZE	(0x1000)

#define MM_IOMMU_BASE		(IO_PHYS + 0x0e802000 + 0x4000)
#define APU_IOMMU_BASE		(IO_PHYS + 0x09010000)

#define IOMMU_REG_RNG_SIZE	(0x5000)

/*******************************************************************************
 * System counter frequency related constants
 ******************************************************************************/
#define SYS_COUNTER_FREQ_IN_HZ  (13000000)
#define SYS_COUNTER_FREQ_IN_MHZ (13)

/*******************************************************************************
 * Platform binary types for linking
 ******************************************************************************/
#define PLATFORM_LINKER_FORMAT          "elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH            aarch64

/*******************************************************************************
 * Generic platform constants
 ******************************************************************************/
#define PLATFORM_STACK_SIZE             (0x800)
#define FIRMWARE_WELCOME_STR            "Booting Trusted Firmware\n"
#define SOC_CHIP_ID                     U(0x8189)

/*******************************************************************************
 * Platform memory map related constants
 ******************************************************************************/
#define TZRAM_BASE                      (0x54600000)
#define TZRAM_SIZE                      (0x00200000)

/*******************************************************************************
 * BL31 specific defines.
 ******************************************************************************/
/*
 * Put BL3-1 at the top of the Trusted SRAM (just below the shared memory, if
 * present). BL31_BASE is calculated using the current BL3-1 debug size plus a
 * little space for growth.
 */
#define BL31_BASE                       (TZRAM_BASE + 0x1000)
#define BL31_LIMIT                      (TZRAM_BASE + TZRAM_SIZE)

/*******************************************************************************
 * BL2 specific defines.
******************************************************************************/
#define BL2_BASE                (0x2001000)
#define BL2_LIMIT               (0x2101000)

#define BOOT_ARGUMENT_LOCATION  (0x40000100)

/*******************************************************************************
 * BL32 specific defines.
******************************************************************************/
#define BL32_BASE               (0x43200000)
#define BL32_LIMIT              (0x00a00000)
#define BL32_HEADER_SIZE        (0x1c)

/*******************************************************************************
 * BL33 specific defines.
******************************************************************************/
#define BL33_BASE               (0x4c000000)
#define BL33_LIMIT              (0x00300000)

#define MAX_IO_DEVICES          U(3)
#define MAX_IO_HANDLES          U(4)
#define MAX_IO_BLOCK_DEVICES    1

/*******************************************************************************
 * Platform specific page table and MMU setup constants
 ******************************************************************************/
#define PLAT_PHY_ADDR_SPACE_SIZE        (1ULL << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE       (1ULL << 32)
#define MAX_XLAT_TABLES                 (16)
#define MAX_MMAP_REGIONS                (16)

/*=======================================================================*/
/* Register Bases                                                        */
/*=======================================================================*/
#define VLPCFG_AO_REG_BASE              (IO_PHYS + 0x0C000000)
#define VLPCFG_REG_BASE                 (IO_PHYS + 0x0C00C000)
#define VLP_CK_BASE                     (IO_PHYS + 0x0C012000)
#define INFRACFG_AO_BASE                (IO_PHYS + 0x00001000)
#define GPIO_BASE                       (IO_PHYS + 0x00005000)
#define IFRBUS_AO_REG_BUS_BASE          (IO_PHYS + 0x0002C000)
#define INFRACFG_AO_MEM_BASE            (IO_PHYS + 0x00270000)
#define TRNG_BASE                       (IO_PHYS + 0x00221000)
#define SUB_INFRACFG_AO_MEM_BASE        (IO_PHYS + 0x0030E000)
#define SSR_TOP_BASE                    (IO_PHYS + 0x00400000)
#define PERICFG_AO_BASE                 (IO_PHYS + 0x01036000)
#define INFRACFG_BASE                   (IO_PHYS + 0x0020E000)
#define APMIXED_BASE                    (IO_PHYS + 0x0000C000)
#define UFSCFG_AO_BUS_BASE              (IO_PHYS + 0x012B8000)
#define UFS0_AO_CFG_BASE                (IO_PHYS + 0x012B8000)
#define UFS0_PDN_CFG_BASE               (IO_PHYS + 0x012BB000)
#define PEXTPCFG_AO_BASE                (IO_PHYS + 0x012E0000)
#define IMP_IIC_WRAP_WS_BASE            (IO_PHYS + 0x01B21000)
#define IMP_IIC_WRAP_E_BASE             (IO_PHYS + 0x01C22000)
#define IMP_IIC_WRAP_S_BASE             (IO_PHYS + 0x01D74000)
#define IMP_IIC_WRAP_EN_BASE            (IO_PHYS + 0x01F32000)
#define MMSYS_CONFIG_BASE               (IO_PHYS + 0x04000000)
#define MMSYS1_CONFIG_BASE              (IO_PHYS + 0x04200000)
#define OVLSYS_CONFIG_BASE              (IO_PHYS + 0x04400000)
#define OVLSYS1_CONFIG_BASE             (IO_PHYS + 0x04600000)
#define MDPSYS_CONFIG_BASE              (IO_PHYS + 0x0F000000)
#define MDPSYS1_CONFIG_BASE             (IO_PHYS + 0x0F800000)
#define GPU_EB_RPC_BASE	                (IO_PHYS + 0x03F91000)
#define MFGPLL_PLL_CTRL_BASE	        (IO_PHYS + 0x03FA0000)
#define GPUEBPLL_PLL_CTRL_BASE	        (IO_PHYS + 0x03FA0800)
#define MFGSCPLL_PLL_CTRL_BASE	        (IO_PHYS + 0x03FA0C00)
#define MFGCFG_BASE                     (IO_PHYS + 0x03FBF000)
#define MMSYS_CONFIG_BASE               (IO_PHYS + 0x04000000)
#define IMGSYS1_BASE                    (IO_PHYS + 0x05020000)
#define IMGSYS2_BASE                    (IO_PHYS + 0x05820000)
#define VDEC_CORE_BASE                  (IO_PHYS + 0x0602F000)
#define VENC_GCON_BASE                  (IO_PHYS + 0x07000000)
#define SCP_IIC_BASE                    (IO_PHYS + 0x0C80A000)
#define SCP_BASE                        (IO_PHYS + 0x0CB21000)
#define VAD_BASE                        (IO_PHYS + 0x0E010000)
#define CAMSYS_MAIN_BASE                (IO_PHYS + 0x0A000000)
#define CAMSYS_RAWA_BASE                (IO_PHYS + 0x0A04F000)
#define CAMSYS_RAWB_BASE                (IO_PHYS + 0x0A06F000)
#define IPE_BASE                        (IO_PHYS + 0x0B000000)
#define DVFSRC_TOP_BASE                 (IO_PHYS + 0x0C00F000)
#define SSPM_SRAM_BASE                  (IO_PHYS + 0x0C300000)
#define SSPM_CFG_BASE                   (IO_PHYS + 0x0C340000)
#define GCE_BASE                        (IO_PHYS + 0x0E980000)
#define MDP_GCE_BASE                    (IO_PHYS + 0x0E990000)
#define MDPSYS_CONFIG_BASE              (IO_PHYS + 0x0F000000)

/*******************************************************************************
 * Devapc related constants
 ******************************************************************************/
#define DEVAPC_INFRA_AO_BASE            (IO_PHYS + 0x00030000)
#define DEVAPC_INFRA_AO1_BASE           (IO_PHYS + 0x00034000)
#define DEVAPC_INFRA_SECU_AO_BASE       (IO_PHYS + 0x0001C000)
#define DEVAPC_PERI_PAR_AO_BASE         (IO_PHYS + 0x0103C000)
#define DEVAPC_VLP_AO_BASE              (IO_PHYS + 0x0C018000)
#define DEVAPC_ADSP_AO_BASE             (IO_PHYS + 0x0E340000)
#define DEVAPC_ADSP_PD_BASE             (IO_PHYS + 0x0E019000)
#define DEVAPC_MMINFRA_AO_BASE          (IO_PHYS + 0x0E820000)
#define DEVAPC_MMUP_AO_BASE             (IO_PHYS + 0x0ECA0000)
#define DEVAPC_GPU_AO_BASE              (IO_PHYS + 0x03FA1000)
#define DEVAPC_GPU_PD_BASE              (IO_PHYS + 0x03FA2000)
#define SRAMROM_BASE                    (IO_PHYS + 0x00214000)
#define DEVAPC_REG_SIZE                 (0x4000)
#define DEVAPC_ADSP_PD_SIZE             (0x1000)

/*******************************************************************************
 * APMIXEDSYS related constants
 ******************************************************************************/
#define APMIXEDSYS                      (IO_PHYS + 0x0000C000)
#define APMIXEDSYS_REG_SIZE             0x1000

/*******************************************************************************
 * GIC & interrupt handling related constants
 * Base MTK_platform compatible GIC memory map
 ******************************************************************************/
#define BASE_GICD_BASE          (MT_GIC_BASE)
#define MT_GIC_RDIST_BASE       (MT_GIC_BASE + 0x40000)
#define BASE_GICR_BASE          (MT_GIC_RDIST_BASE)
#define MTK_GIC_REG_SIZE        0x400000
#define MTK_GIC_MAX_NUM         726

/*******************************************************************************
 * INTID definitions
 ******************************************************************************/
#define SYSTIMER_IRQ_BIT_ID     281 /* 249 + 32 = 281 */
#define WDT_IRQ_BIT_ID          519
#define FIQ_SMP_CALL_SGI        13
#define MD_WDT_IRQ_BIT_ID       523

/*******************************************************************************
 * syscirq definitions
 ******************************************************************************/
#define SYS_CIRQ_BASE           (IO_PHYS + 0x204000)
#define CIRQ_REG_NUM            19
#define CIRQ_SPI_START          96
#define CIRQ_IRQ_NUM            598

#define DBGSYS_BASE             (0x0D000000)
#define DBGAO_BASE              (0x0D01A000)
#define DEM_BASE                (0x0D0A0000)

/*******************************************************************************
 * Systracker related constants
 *******************************************************************************/
#define AP_TRACKER_BASE                 (IO_PHYS + 0x00208000)
#define INFRA_TRACKER_BASE              (IO_PHYS + 0x00314000)
#define BUS_TRACER_MONITOR_BASE         (0x0d040000)
#define BUS_DBG_CON_AO_OFFSET           (0x8fc)
#define BUS_DBG_CON_AO_1_OFFSET         (0x9fc)
#define BUS_DBG_CON_AO                  (BUS_TRACER_MONITOR_BASE + BUS_DBG_CON_AO_OFFSET)
#define BUS_DBG_CON_AO_1                (BUS_TRACER_MONITOR_BASE + BUS_DBG_CON_AO_1_OFFSET)
#define SYSTRACKER_WP_EN                ((1U << 3) | (1U << 8))

/* Platform cacheline size */
#define PLATFORM_CACHE_LINE_SIZE CACHE_WRITEBACK_GRANULE

/*******************************************************************************
 * CCU address
 ******************************************************************************/
#define CCU_BASE                (IO_PHYS + 0x0B000000)
#define CCU_REG_BASE            (CCU_BASE + 0x80000)
#define CCU_BASE_DIFF           (0x100000)
#define CCU_REG_SIZE            (0x1000)
#define CCU_EXTREG_BASE         (CCU_BASE + 0x88000)
#define CCU_EXTREG_SIZE         (0x1000)
#define CCU_CG_MUX_BASE         (IO_PHYS)
#define CCU_CG_MUX_SIZE         (0x1000)

/*******************************************************************************
 * UART related constants
 ******************************************************************************/
#define UART0_BASE              (IO_PHYS + 0x01001000)
#define UART1_BASE              (IO_PHYS + 0x01002000)
#define UART2_BASE              (IO_PHYS + 0x01003000)
#define UART3_BASE              (IO_PHYS + 0x01004000)
/* The UART port numbers of this SoC */
#define HW_SUPPORT_UART_PORTS   (4)

/*UART CG*/
#define UART0_CG_BIT            (1 << 0)

/*******************************************************************************
 * GPIO related
 ******************************************************************************/
#define GPIO_TRAPPING_REG       (IO_PHYS + 0x000056f0)
#define TRAP_BOOTDEV_BIT        10
#define TRAP_USBDL_BIT          12
#define TRAP_DDR_BIT            13

/*******************************************************************************
 * Storage related
 ******************************************************************************/
#define STORAGE_EMMC    0
#define STORAGE_UFS     1
#define STORAGE_SPINOR  2
#define STORAGE_UNKNOWN 3

/*******************************************************************************
 * MSDC related
 ******************************************************************************/
#define MSDC0_BASE              (IO_PHYS + 0x01230000)
#define MSDC0_BASE_SIZE         0x1000
#define MSDC0_TOP_BASE          (IO_PHYS + 0x01e70000)
#define MSDC0_TOP_BASE_SIZE     0x1000

/*******************************************************************************
 * RGU related constants
 ******************************************************************************/
#define RGU_BASE                (IO_PHYS + 0x0C00A000)

/*******************************************************************************
 * SECURE_AO related constants
 ******************************************************************************/
#define SECURE_AO_BASE          (IO_PHYS + 0x40D000)
#define SECURE_AO_REG_SIZE      0x1000

/*******************************************************************************
 * CPU_EB related constants
 ******************************************************************************/
#define CPU_EB_TCM_BASE         (0x0C56F000)
#define CPU_EB_TCM_SIZE         (0x1000)

/*******************************************************************************
 * CPU MPMM register specific definitions.
 ******************************************************************************/
#define CPU_TYPE1_MPMMEN        (MCUCFG_BASE + 0xCD10)

#define MPMMPINCTL              UL(0x1)
#define MXP_EN                  UL(0x1)
#define MXP_EN_SHIFT            1

#define _MXP_TP_p90             UL(0x0)
#define _MXP_TP_p80             UL(0x1)
#define _MXP_TP_p70             UL(0x2)
#define _MXP_TP_p60             UL(0x3)
#define _MXP_TP_p50             UL(0x4)
#define _MXP_TP_p40             UL(0x5)
#define _MXP_TP_p30             UL(0x6)
#define _MXP_TP_p20             UL(0x7)
#define MXP_TP_SHIFT            7

#define _MXP_ATHR_p20           UL(0x0)
#define _MXP_ATHR_p30           UL(0x1)
#define _MXP_ATHR_p40           UL(0x2)
#define _MXP_ATHR_p50           UL(0x3)
#define _MXP_ATHR_p60           UL(0x4)
#define _MXP_ATHR_p70           UL(0x5)
#define _MXP_ATHR_p80           UL(0x6)
#define _MXP_ATHR_p90           UL(0x7)
#define MXP_ATHR_SHIFT          4

#define MPMM_ENABLE             (MXP_EN << MXP_EN_SHIFT)
#define MPMM_DEFAULT_TP         _MXP_TP_p60
#define MPMM_DEFAULT_ATHR       _MXP_ATHR_p70
#define MPMM_SETTING_STRING     "60-70"
#define PLATFORM_LITTLE_CORE_COUNT      (4)

/*******************************************************************************
 * PMIC related constants
 ******************************************************************************/
#define PMIC_WRAP_BASE		(IO_PHYS + 0x0CC04000)

/*******************************************************************************
 * DFD_MCU related constants
 ******************************************************************************/
#define DFD_V30_BASE            (IO_PHYS + 0x00022000)
#define DFD_REG_SIZE            (0x1000)

/*******************************************************************************
 * SPM related constants
 ******************************************************************************/
#define SPM_BASE                (IO_PHYS + 0x0C001000)
#define SPM_REG_SIZE            (0x1000)

#define SPM_VLP_MCUSYS_PWR_CON	           (SPM_BASE + 0x260)
#define SPM_VLP_MP0_CPUTOP_PWR_CON         (SPM_BASE + 0x264)
#define SPM_VLP_CPU0_PWR_CON               (SPM_BASE + 0x268)
#define SPM_VLP_CPU1_PWR_CON               (SPM_BASE + 0x26C)
#define SPM_VLP_CPU2_PWR_CON               (SPM_BASE + 0x270)
#define SPM_VLP_CPU3_PWR_CON               (SPM_BASE + 0x274)
#define SPM_VLP_CPU4_PWR_CON               (SPM_BASE + 0x278)
#define SPM_VLP_CPU5_PWR_CON               (SPM_BASE + 0x27C)
#define SPM_VLP_CPU6_PWR_CON               (SPM_BASE + 0x280)
#define SPM_VLP_CPU7_PWR_CON               (SPM_BASE + 0x284)

/*******************************************************************************
 * CPU PM definitions
 ******************************************************************************/
#define PLAT_CPU_PM_B_BUCK_ISO_ID       (6)
#define PLAT_CPU_PM_ILDO_ID             (6)
#define CPU_IDLE_SRAM_BASE		(0x11B000)
#define CPU_IDLE_SRAM_SIZE		(0x1000)


/*******************************************************************************
 * SYSTIMER related constants
 ******************************************************************************/
#define SYSTIMER_BASE            (IO_PHYS + 0x0CC10000)

/*******************************************************************************
 * DBGTOP_DRM related constants
 ******************************************************************************/
#define MTK_DRM_BASE             (IO_PHYS + 0x000d000)

/*******************************************************************************
 * EINT registers
 ******************************************************************************/
#define EINT_E_BASE             (IO_PHYS + 0x01ce0000)
#define EINT_S_BASE             (IO_PHYS + 0x01de0000)
#define EINT_W_BASE             (IO_PHYS + 0x01e60000)
#define EINT_C_BASE             (IO_PHYS + 0x0c01e000)

/*******************************************************************************
 * dvfsrc related constants
 ******************************************************************************/
#define DVFSRC_BASE             (IO_PHYS + 0x0C00F000)

/*******************************************************************************
 * infra related constants
 ******************************************************************************/
#define INFRACFG_AO_BASE                (IO_PHYS + 0x00001000)
#define INFRACFG_AO_REG_SIZE            (0x1000)
#define IFRBUS_AO_REG_BUS_BASE          (IO_PHYS + 0x0002C000)

/*******************************************************************************
 * EMI related
 ******************************************************************************/
#define EMI_APB_BASE                   (IO_PHYS + 0x00219000)
#define INFRA_EMI_DEBUG_CFG_BASE       (IO_PHYS + 0x0021C000)
#define EMI_MPU_BASE                   (IO_PHYS + 0x00226000)
#define CHN0_EMI_APB_BASE              (IO_PHYS + 0x00235000)
#define CHN1_EMI_APB_BASE              (IO_PHYS + 0x00245000)
#define INFRACFG_AO_MEM_BASE           (IO_PHYS + 0x00270000)
#define NEMI_SMPU_BASE                 (IO_PHYS + 0x00351000)

/*******************************************************************************
 * CRYPTO related constants
 ******************************************************************************/
#define SSR_TOP_BASE       (IO_PHYS + 0x00400000)
#define SSR_TOP_SIZE       (0x1000)
#define CRYPTO_BASE        (IO_PHYS + 0x0040C000)

/*******************************************************************************
 * HACC related constants
 ******************************************************************************/
#define HACC_BASE           (IO_PHYS + 0x0040E000)

/*******************************************************************************
 * TRNG related constants
 ******************************************************************************/
#define TRNG_BASE           (IO_PHYS + 0x00221000)

/*******************************************************************************
 * CKSYS related constants
 ******************************************************************************/
#define CKSYS_BASE              (IO_PHYS)

/*******************************************************************************
 * PERI related constants
 ******************************************************************************/
#define PERICFG_AO_BASE         (IO_PHYS + 0x01036000)
#define PERICFG_AO_SIZE         (0x1000)
#define PERI_CG0_STA            (PERICFG_AO_BASE + 0x10)

/*******************************************************************************
 * SSPM CFGREG related constants
 ******************************************************************************/
#define SSPM_REG_OFFSET         (0x40000)
#define SSPM_CFGREG_BASE        (IO_PHYS + 0x0C300000 + SSPM_REG_OFFSET)
#define SSPM_CFGREG_SIZE        (0x1000)

/*******************************************************************************
 * SSPM_MBOX_3 related constants
 ******************************************************************************/
#define SSPM_MBOX_3_BASE        (IO_PHYS + 0x0C380000)
#define SSPM_MBOX_3_SIZE        (0x1000)

/*******************************************************************************
 * LPM syssram related constants
 ******************************************************************************/
#define MTK_LPM_SRAM_BASE       (0x11B000)
#define MTK_LPM_SRAM_MAP_SIZE   (0x1000)

/*******************************************************************************
 * MFGSYS related constants
 ******************************************************************************/
#define MFGSYS_BASE             (IO_PHYS + 0x03000000)
#define MFGPLL_BASE             (MFGSYS_BASE + 0x00FA0000)
#define MFGPLL_SIZE             (0x1000)
#define GPU_EB_RPC_BASE         (IO_PHYS + 0x03F91000)
#define GPU_EB_RPC_SIZE         (0x1000)

/*******************************************************************************
 * SCP registers
 ******************************************************************************/
#define MTK_SCP_L2TCM_BASE          (IO_PHYS + 0x0C400000)
#define MTK_SCP_L2TCM_SIZE          (0x100000)
#define MTK_SCP_REG_BASE            (IO_PHYS + 0x0CB00000)
#define MTK_SCP_REG_BANK_SIZE       (0x1000)
#define SCP_CLK_CTRL_BASE           (IO_PHYS + 0x0CB21000)
#define SCP_CLK_CTRL_SIZE           (0x1000)

/*******************************************************************************
 * PMIF address
 ******************************************************************************/
#define PMIF_SPMI_BASE                  (IO_PHYS + 0x0CC04000)
#define PMIF_SPMI_SIZE                  (0x1000)
#define PMIF_SPMI_P_BASE                (IO_PHYS + 0x0CC06000)

/*******************************************************************************
 * SPMI address
 ******************************************************************************/
#define PMICSPI_MST_BASE                (IO_PHYS + 0x0c013000)
#define PMICSPI_MST_SIZE                (0x1000)
#define SPMI_MST_P_BASE                 (IO_PHYS + 0x0CC00000)
#define SPMI_MST_P_SIZE                 (0x1000)

/* CLKMGR */
#define INFRA_AO_BCRM_BASE              (IO_PHYS + 0x00022000)
#define PERI_AO_BCRM_BASE               (IO_PHYS + 0x01035000)
#define VLPCFG_REG_BUS_BASE             (IO_PHYS + 0x0C00C000)
#define EMICFG_AO_MEM_BASE              (IO_PHYS + 0x00270000)

/*******************************************************************************
 * MMSYS related constants
 ******************************************************************************/
#define MMSYS_BASE              (IO_PHYS + 0x04000000)
#define MMSYS_REG_SIZE          0x1000
#define MMSYS1_BASE             (IO_PHYS + 0x04200000)
#define MMSYS1_REG_SIZE         0x1000

#define MMINFRA_CONFIG_BASE             (IO_PHYS + 0x0e800000)
#define MMINFRA_CONFIG_REG_SIZE         0x1000

/*******************************************************************************
 * MDPSYS related constants
 ******************************************************************************/
#define MDPSYS_BASE                     (IO_PHYS + 0x0F000000)
#define MDPSYS_BASE_SIZE                (0x1000)

#define MDPSYS1_BASE                    (IO_PHYS + 0x0F800000)
#define MDPSYS1_BASE_SIZE               (0x1000)

/*******************************************************************************
 * IOMMU related constants
 ******************************************************************************/
#define DISP_IOMMU_BK0_BASE             (IO_PHYS + 0x0E802000)
#define DISP_IOMMU_BK1_BASE             (IO_PHYS + 0x0E803000)
#define DISP_IOMMU_BK2_BASE             (IO_PHYS + 0x0E804000)
#define DISP_IOMMU_BK3_BASE             (IO_PHYS + 0x0E805000)
#define DISP_IOMMU_BK4_BASE             (IO_PHYS + 0x0E806000)

#define APU_IOMMU0_BK0_BASE             (IO_PHYS + 0x09010000)
#define APU_IOMMU0_BK1_BASE             (IO_PHYS + 0x09011000)
#define APU_IOMMU0_BK2_BASE             (IO_PHYS + 0x09012000)
#define APU_IOMMU0_BK3_BASE             (IO_PHYS + 0x09013000)
#define APU_IOMMU0_BK4_BASE             (IO_PHYS + 0x09014000)

#define IOMMU_BANK_SIZE			(0x5000)

/*******************************************************************************
 * INFRA_SECURITY_AO related constants
 ******************************************************************************/
#define INFRA_SECURITY_AO_BASE          (IO_PHYS + 0x0C045000)
#define INFRA_SECURITY_AO_REG_SIZE      (0x1000)

/*******************************************************************************
 * Connsys related
 ******************************************************************************/

#define CONN_INFRA_BUS_CR_DECLARE_BASE  (0x1804b000)
#define CONN_INFRA_BUS_CR_DECLARE_SIZE  (0x00001000)

#define CONN_HOST_CSR_TOP_DECLARE_BASE  (0x18060000)
#define CONN_HOST_CSR_TOP_DECLARE_SIZE  (0x00001000)

/*******************************************************************************
 * GPS related
 ******************************************************************************/
#define CONN_INFRA_RGU_ON_DECLARE_BASE  (0x18000000)
#define CONN_INFRA_RGU_ON_DECLARE_SIZE  (0x00001000)

#define CONN_INFRA_CFG_ON_DECLARE_BASE  (0x18001000)
#define CONN_INFRA_CFG_ON_DECLARE_SIZE  (0x00001000)

#define CONN_WT_SLP_CTL_REG_DECLARE_BASE        (0x18003000)
#define CONN_WT_SLP_CTL_REG_DECLARE_SIZE        (0x00001000)

#define CONN_INFRA_CFG_DECLARE_BASE     (0x18011000)
#define CONN_INFRA_CFG_DECLARE_SIZE     (0x00001000)

#define CONN_SEMAPHORE_DECLARE_BASE     (0x18070000)
#define CONN_SEMAPHORE_DECLARE_SIZE     (0x00009000)

/*******************************************************************************
 * VLP CFG related constants
 ******************************************************************************/
#define VLPCFG_REG_BASE         (IO_PHYS + 0x0C00C000)
#define VLPCFG_REG_SIZE         (0x1000)

/*******************************************************************************
 * APUSYS related constants
 ******************************************************************************/
#define DEBUGSYS_APB                (0x0d000000)
#define APU_MD32_DEBUG_APB          (DEBUGSYS_APB + 0x002A0000)
#define APUSYS_CTRL_DAPC_AO_BASE    (IO_PHYS + 0x090FC000)
#define APUSYS_CTRL_DAPC_RCX_BASE   (IO_PHYS + 0x09030000)
#define APU_MD32_SYSCTRL            (IO_PHYS + 0x09001000)
#define APU_CACHE_DUMP              (IO_PHYS + 0x09050000)
#define APU_MD32_TCM                (IO_PHYS + 0x0d000000)
#define APU_MD32_WDT                (IO_PHYS + 0x09002000)
#define APU_SCTRL_REVISER           (IO_PHYS + 0x0903c000)
#define APU_AO_CTRL                 (IO_PHYS + 0x090f2000)
#define APU_MBOX0                   (IO_PHYS + 0x090e1000)
#define APU_MBOX1                   (IO_PHYS + 0x090e2000)
#define APU_RPCTOP                  (IO_PHYS + 0x090F0000)
#define APU_PCUTOP                  (IO_PHYS + 0x090F1000)
#define APU_ARETOP_ARE              (IO_PHYS + 0x090a0000)
#define APU_ARE_AO                  (IO_PHYS + 0x090b0000)
#define APU_RCX_CONFIG              (IO_PHYS + 0x09020000)
#define APU_RCX_VCORE_CONFIG        (IO_PHYS + 0x090E0000)
#define APU_ACS_RCX                 (IO_PHYS + 0x0903C000)
#define APU_RPCTOP_LITE_ACX0        (IO_PHYS + 0x09140000)
#define APU_ACX0_CONFIG             (IO_PHYS + 0x0913C000)
#define APU_ACS_ACX0                (IO_PHYS + 0x09106000)
#define APU_SEC_CON                 (IO_PHYS + 0x090F5000)
#define APU_ACC_COMMON              (IO_PHYS + 0x090F3000)
#define APU_PLL_COMMON              (IO_PHYS + 0x090F6000)
#define APU_LOGTOP                  (IO_PHYS + 0x09024000)
#define APU_CMU_TOP                 (IO_PHYS + 0x09067000)
#define APU_RCX_UPRV_TCU            (IO_PHYS + 0x09060000)
#define APU_RCX_EXTM_TCU            (IO_PHYS + 0x09061000)

#define APU_MD32_TCM_SZ 0x20000
#define APU_CACHE_DUMP_SZ 0x10000
#define APU_MD32_DEBUG_SZ 0x10000
#define APU_ACS_RCX_SZ 0x1000
#define APU_RPCTOP_LITE_ACX0_SZ 0x1000
#define APU_ACX0_CONFIG_SZ 0x1000
#define APU_ACS_ACX0_SZ 0x1000
#define APU_SEC_CON_SZ 0x1000
#define APU_RPCTOP_SZ 0x1000
#define APU_PCUTOP_SZ 0x1000
#define APU_ARETOP_ARE_SZ 0x10000
#define APU_ACC_COMMON_SZ 0x1000
#define APU_PLL_COMMON_SZ 0x1000
#define APU_LOGTOP_SZ 0x1000
#define APU_ARE_AO_SZ 0x1000
#define APU_CMU_TOP_SZ 0x5000
#define APU_RCX_UPRV_TCU_SZ 0x1000
#define APU_RCX_EXTM_TCU_SZ 0x1000

/*******************************************************************************
 * VCP related constants
 ******************************************************************************/
#define MTK_VCP_SRAM_BASE               (IO_PHYS + 0x0EA00000)
#define MTK_VCP_SRAM_SIZE               (0x40000)
#define MTK_VCP_REG_BASE                (IO_PHYS + 0x0EC00000)
#define MTK_VCP_REG_BANK_SIZE           (0x1000)

#define VLP_AO_REG_BASE                 (IO_PHYS + 0x0C000000)
#define VLP_AO_REG_SIZE                 (0x1000)

#define APU_ARE_DEBUG_APB_BASE          (DEBUGSYS_APB + 0x0029B000)
#define APU_ARE_REG_BASE                (IO_PHYS + 0x090B0000)
#define APU_ARE_SRAM_SZ                 (24 * 1024)

/*******************************************************************************
 * AUDIO related constants
 ******************************************************************************/
#define AUDIO_BASE              (IO_PHYS + 0x01050000)
#define AUDIO_SIZE              (0x9000)
#define THERM_CTRL_AP_BASE      (IO_PHYS + 0x00315000)
#define THERM_AP_REG_SIZE       (0x1000)

#define THERM_CTRL_MCU_BASE     (IO_PHYS + 0x00316000)
#define THERM_MCU_REG_SIZE      (0x1000)

/*******************************************************************************
 * SPMI related definitions
 ******************************************************************************/
#define SPMI_MST_P_BASE			(IO_PHYS + 0x0CC00000)
#define PMIF_SPMI_P_BASE		(IO_PHYS + 0x0CC06000)
#define SPMI_MST_P_SIZE			(0x1000)

/*******************************************************************************
 * PWRAP related definitions
 ******************************************************************************/
#define PMICSPI_MST_BASE		(IO_PHYS + 0x0c013000)
#define PMICSPI_MST_SIZE		(0x1000)
#define PMIC_WRAP_BASE			(IO_PHYS + 0x0CC04000)
#define PMIF_SPI_BASE			(0x1CC04000)
#define PWRAP_REG_BASE			(0x1C013000)
#define PWRAP_WRAP_EN			(PWRAP_REG_BASE + 0x14)

/*******************************************************************************
 * PMIC regsister related definitions
 ******************************************************************************/
#define PMIC_REG_BASE			(0x0000)
#define PWRAP_SIZE			(0x1000)
#define DEW_READ_TEST			(PMIC_REG_BASE + 0x040e)
#define DEW_WRITE_TEST			(PMIC_REG_BASE + 0x0410)

/*******************************************************************************
 * Differentiate between 3G and 2.6G-related definitions
 ******************************************************************************/
#define EFUSEC_BASE			(IO_PHYS + 0x01F10000)
#define CHIP_ID_REG			(EFUSEC_BASE + 0x7A0)
#define CPU_SEG_ID_REG			(EFUSEC_BASE + 0x7E0)

#define MTK_CPU_ID_MT8189		0x81890000
#define MTK_CPU_SEG_ID_MT8189G		0x20
#define MTK_CPU_SEG_ID_MT8189H		0x21

#endif /* PLATFORM_DEF_H */
