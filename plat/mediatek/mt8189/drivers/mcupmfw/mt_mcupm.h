/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#ifndef MT_MCUPM_H
#define MT_MCUPM_H

#include <stddef.h>
#include <stdint.h>
#include <platform_def.h>
#include <mcu.h>

#define MCUPM_SRAM_BASE		0x0C540000
#define MCUPM_CFG_BASE		0x0C580000

#ifndef MCUPM_GPR_SIZE
#define MCUPM_GPR_SIZE		0x00000064
#endif
#define SRAM_GPR_SIZE_4B	0x4 /*4 Bytes*/
#ifdef IPI_NUMBER
#define IPI_MBOX_TOTAL		IPI_NUMBER
#else
#define IPI_MBOX_TOTAL		8
#endif
#define SRAM_RESERVED_20B	0x00000014 /* 20 Bytes */
#define MBOX_SLOT_SIZE_4B	0x00000004
#define SRAM_SLOT_SIZE_80B	0x00000014 /* 0x14 = 20 slots = 20*4Bytes = 80 Bytes */
#define PIN_S_SIZE		SRAM_SLOT_SIZE_80B
#define PIN_R_SIZE		SRAM_SLOT_SIZE_80B
#define MBOX_TABLE_SIZE		(PIN_S_SIZE + PIN_R_SIZE)

#define GPR_BASE_ADDR_MCU(x)	(MCUPM_SRAM_BASE + \
	MCUPM_SRAM_SIZE - \
	(IPI_MBOX_TOTAL * MBOX_TABLE_SIZE * MBOX_SLOT_SIZE_4B) - \
	MCUPM_GPR_SIZE + \
	(x * SRAM_GPR_SIZE_4B)) /* 0x1000BC7C */

#define RESERVED_SRAM_SIZE_MCU	((IPI_MBOX_TOTAL * MBOX_TABLE_SIZE * MBOX_SLOT_SIZE_4B) + \
	MCUPM_GPR_SIZE + SRAM_RESERVED_20B)

#define MCUPM_INFO		GPR_BASE_ADDR_MCU(0)
#define MCUPM_ABNORMAL_BOOT	GPR_BASE_ADDR_MCU(1)
#define PLT_RD_MAGIC_MCU	GPR_BASE_ADDR_MCU(3)
#define MCUPM_LASTK_SZ		GPR_BASE_ADDR_MCU(7)
#define MCUPM_LASTK_ADDR	GPR_BASE_ADDR_MCU(8)
#define MCUPM_DATA_SZ		GPR_BASE_ADDR_MCU(9)
#define MCUPM_DATA_ADDR		GPR_BASE_ADDR_MCU(10)
#define MCUPM_RM_ADDR		GPR_BASE_ADDR_MCU(12)
#define MCUPM_RM_SZ		GPR_BASE_ADDR_MCU(13)
#define MCUPM_DM_ADDR		GPR_BASE_ADDR_MCU(14)
#define MCUPM_DM_SZ		GPR_BASE_ADDR_MCU(15)
#define MCUPM_LASTK_W_POS	GPR_BASE_ADDR_MCU(19)
#define MCUPM_LASTK_R_POS	GPR_BASE_ADDR_MCU(20)
#define MCUPM_LAST_PCLRS	GPR_BASE_ADDR_MCU(21)
#define MCUPM_LAST_TBUF		GPR_BASE_ADDR_MCU(22)
#define MCUPM_WARM_BOOT		GPR_BASE_ADDR_MCU(23)

#define MCUPM_PC		(MCUPM_SW_RSTN + 0x40)
#define MCUPM_AHB_STATUS	(MCUPM_SW_RSTN + 0x44)
#define MCUPM_AHB_M0_ADDR	(MCUPM_SW_RSTN + 0x48)
#define MCUPM_AHB_M1_ADDR	(MCUPM_SW_RSTN + 0x4C)
#define MCUPM_TBUF_WPTR		(MCUPM_SW_RSTN + 0xD0)
#define MCUPM_TBUF_ADDR		(MCUPM_SW_RSTN + 0xD4)
#define MCUPM_TBUF_DATA31_0	(MCUPM_SW_RSTN + 0xE0)
#define MCUPM_TBUF_DATA63_32	(MCUPM_SW_RSTN + 0xE4)
#define MCUPM_TBUF_DATA95_64	(MCUPM_SW_RSTN + 0xE8)
#define MCUPM_TBUF_DATA127_96	(MCUPM_SW_RSTN + 0xEC)

#define CPUEB_CFGREG_DBG_MON_PC 0x184

#define MCUPM_SRAM_SIZE		0x30000 /* 192K Bytes */
#define MDUMP_L2TCM_SIZE	MCUPM_SRAM_SIZE /* L2_TCM */
#define MDUMP_REGDUMP_SIZE	0x003f00 /* register backup (max size) */
#define MDUMP_TBUF_SIZE		0x000100
#define MDUMP_DRAM_SIZE		0x100000 /* due to ee size limit, max dump 1MB */
#define MCUPM_TBUF_SIZE		0x80 /* 4 bytes PC * 32 PCs */

_Alignas(8) extern const unsigned char mcupm_bin[];
extern const unsigned char mcupm_bin_end[];
#define mcupm_bin_len (mcupm_bin_end - mcupm_bin)

#endif
