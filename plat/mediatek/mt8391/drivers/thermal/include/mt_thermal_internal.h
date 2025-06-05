/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#ifndef __SOC_MEDIATEK_MT8189_INCLUDE_SOC_MT_THERMAL_INTERNAL_H__
#define __SOC_MEDIATEK_MT8189_INCLUDE_SOC_MT_THERMAL_INTERNAL_H__

#define check_member(structure, member, offset) _Static_assert( \
	offsetof(struct structure, member) == offset, \
	"`struct " #structure "` offset for `" #member "` is not " #offset)

#define retry(attempts, condition, ...) \
	_retry_impl(attempts, condition, __VA_ARGS__)

#define _retry_impl(attempts, condition, expr, ...)	\
({							\
	__typeof__(condition) _retry_ret =		\
		(__typeof__(condition))0;		\
	int _retry_attempts = (attempts);		\
	do {						\
		_retry_ret = (condition);		\
		if (_retry_ret)				\
			break;				\
		if (--_retry_attempts > 0) {		\
			expr;				\
		} else {				\
			break;				\
		}					\
	} while (1);					\
	_retry_ret;					\
})

#define __clrsetbits_impl(bits, addr, clear, set) write##bits(addr, \
	(read##bits(addr) & ~((uint##bits##_t)(clear))) | (set))

#define clrsetbits32(addr, clear, set)	__clrsetbits_impl(32, addr, clear, set)

#define setbits32(addr, set)		clrsetbits32(addr, 0, set)
#define clrbits32(addr, clear)		clrsetbits32(addr, clear, 0)

#define REG32(addr) ((uint32_t *)(uintptr_t)(addr))

#define read32(addr)       (*REG32(addr))
#define write32(addr, val) (*REG32(addr) = (val))

#include <stddef.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <mt_thermal.h>

#define BIOS_INFO       6
#define BIOS_ERR        3

#define IO_PHYS	0x10000000
#define THERM_CTRL_BASE	(IO_PHYS + 0x00315000)

#define INIT_LVTS_MAGIC		0x0000555

#define LVTS_COF_T_SLP_GLD		219960
#define LVTS_COF_COUNT_R_GLD		14437
#define LVTS_COF_T_CONST_OFS		280000
#define LVTS_ADDRESS_INDEX_NUM		(L_TS_LVTS_NUM + LVTS_CONTROLLER_NUM)

#define DEFAULT_EFUSE_GOLDEN_TEMP	50
#define DEFAULT_EFUSE_GOLDEN_TEMP_HT	170
#define DEFAULT_EFUSE_COUNT		35000
#define DEFAULT_EFUSE_COUNT_RC		2750

/* (INFRACFG_AO_BASE + 0x0084) */
#define THERM_CG			BIT(10)

/* (THERM_CTRL_BASE + lvts_tc_offset_enum + 0x0050) */
#define DEVICE_SENSING_STATUS		BIT(25)
#define DEVICE_ACCESS_START		BIT(24)
#define WRITE_ACCESS			BIT(16)

/* (THERM_CTRL_BASE + lvts_tc_offset_enum + 0x000C) */
#define STAGE3_INT_EN			BIT(31)

/*
 * module            LVTS Plan
 *=====================================================
 * MCU_BIG core0/1 LVTS1-0, LVTS1-1, LVTS1-2, LVTS1-3
 * MCU_BIG core2/3 LVTS2-0, LVTS2-1, LVTS2-2, LVTS2-3
 * MCU_LITTLE      LVTS3-0, LVTS3-1, LVTS3-2, LVTS3-3
 * SOC-TOP,APU     LVTS4-0, LVTS4-1, LVTS4-2, LVTS4-3
 * GPU             LVTS5-0, LVTS5-1
 * ptp_therm_ctrl_AP  Base address: (+0x1031_5000), 0x1031_5100,
 *                                 0x1031_5200
 * ptp_therm_ctrl_MCU Base address: (+0x1031_6000), 0x1031_6100,
 *                                 0x1031_6200, 0x1031_6300
 */

/* private thermal sensor enum */
enum lvts_sensor_enum {
	L_TS_LVTS1_0 = 0,	/* LVTS1-0 Big-core0 */
	L_TS_LVTS1_1,		/* LVTS1-1 Big-core0 */
	L_TS_LVTS1_2,		/* LVTS1-2 Big-core1 */
	L_TS_LVTS1_3,		/* LVTS1-3 Big-core1 */
	L_TS_LVTS2_0,		/* LVTS2-0 Big-core2 */
	L_TS_LVTS2_1,		/* LVTS2-1 Big-core2 */
	L_TS_LVTS2_2,		/* LVTS2-2 Big-core3 */
	L_TS_LVTS2_3,		/* LVTS2-3 Big-core3 */
	L_TS_LVTS3_0,		/* LVTS3-0 Little-core0 */
	L_TS_LVTS3_1,		/* LVTS3-1 Little-core1 */
	L_TS_LVTS3_2,		/* LVTS3-2 Little-core2 */
	L_TS_LVTS3_3,		/* LVTS3-3 Little-core3 */
	L_TS_LVTS4_0,		/* LVTS4-0 SCO-TOP */
	L_TS_LVTS4_1,		/* LVTS4-1 SCO-TOP */
	L_TS_LVTS4_2,		/* LVTS4-0 SCO-TOP */
	L_TS_LVTS4_3,		/* LVTS4-1 APU */
	L_TS_LVTS5_0,		/* LVTS5-0 GPU */
	L_TS_LVTS5_1,		/* LVTS5-1 GPU */
	L_TS_LVTS_NUM
};

enum lvts_tc_enum {
	LVTS_MCU_CONTROLLER0 = 0,
	LVTS_MCU_CONTROLLER1,
	LVTS_MCU_CONTROLLER2,
	LVTS_AP_CONTROLLER0,
	LVTS_MFG_CONTROLLER0,
	LVTS_CONTROLLER_NUM
};

enum lvts_tc_offset_enum {
	TC_OFFSET_AP_CONTROLLER0 = 0,
	TC_OFFSET_MFG_CONTROLLER0 = 0x100,
	TC_OFFSET_MCU_CONTROLLER0 = 0x1000,
	TC_OFFSET_MCU_CONTROLLER1 = 0x1100,
	TC_OFFSET_MCU_CONTROLLER2 = 0x1200,
};

enum sensor_switch_status {
	SEN_OFF,
	SEN_ON
};

struct lvts_thermal_controller_speed {
	uint32_t group_interval_delay;
	uint32_t period_unit;
	uint32_t filter_interval_delay;
	uint32_t sensor_interval_delay;
};

enum controller_switch_status {
	CTRL_OFF,
	CTRL_ON
};

struct lvts_thermal_controller {
	enum lvts_sensor_enum ts[4]; /* sensor point 0 ~ 3 */
	enum sensor_switch_status sensor_on_off[4];
	enum controller_switch_status ctrl_on_off;
	uint32_t ts_number;
	uint32_t reboot_temperature;
	uint32_t dominator_ts_idx;
	struct lvts_thermal_controller_speed tc_speed;
	struct mtk_thermal_controller_regs *tc_regs;
};

/*=============================================================
 * LVTS Thermal Controller Register Definition
 *=============================================================
 */

static struct mtk_thermal_controller_regs *const
	mtk_lvts_mcu_controller0  = (void *)(THERM_CTRL_BASE + TC_OFFSET_MCU_CONTROLLER0);
static struct mtk_thermal_controller_regs *const
	mtk_lvts_mcu_controller1  = (void *)(THERM_CTRL_BASE + TC_OFFSET_MCU_CONTROLLER1);
static struct mtk_thermal_controller_regs *const
	mtk_lvts_mcu_controller2  = (void *)(THERM_CTRL_BASE + TC_OFFSET_MCU_CONTROLLER2);
static struct mtk_thermal_controller_regs *const
	mtk_lvts_ap_controller0  = (void *)(THERM_CTRL_BASE + TC_OFFSET_AP_CONTROLLER0);
static struct mtk_thermal_controller_regs *const
	mtk_lvts_mfg_controller0  = (void *)(THERM_CTRL_BASE + TC_OFFSET_MFG_CONTROLLER0);

static struct mtk_infracfg_regs *const
	mtk_infracfg  = (void *)INFRACFG_BASE;

static struct mtk_infracfg_ao_regs *const
	mtk_infracfg_ao  = (void *)INFRACFG_AO_BASE;

static struct mtk_efusec_regs *const
	mtk_efusec = (void *)EFUSEC_BASE;

struct mtk_thermal_controller_regs {
	uint32_t lvtsmonctl0_0;
	uint32_t lvtsmonctl1_0;
	uint32_t lvtsmonctl2_0;
	uint32_t lvtsmonint_0;
	uint32_t reserved1[5];
	uint32_t lvtsh2nthre_0;
	uint32_t lvtshthre_0;
	uint32_t lvtscthre_0;
	uint32_t lvtsoffseth_0;
	uint32_t lvtsoffsetl_0;
	uint32_t lvtsmsrctl0_0;
	uint32_t lvtsmsrctl1_0;
	uint32_t lvtstssel_0;
	uint32_t reserved2[1];
	uint32_t lvtscalscale_0;
	uint32_t lvts_id_0;
	uint32_t lvts_config_0;
	uint32_t lvtsedata00_0;
	uint32_t lvtsedata01_0;
	uint32_t lvtsedata02_0;
	uint32_t lvtsedata03_0;
	uint32_t reserved3[11];
	uint32_t lvtsmsr0_0;
	uint32_t lvtsmsr1_0;
	uint32_t lvtsmsr2_0;
	uint32_t lvtsmsr3_0;
	uint32_t reserved4[4];
	uint32_t lvtsrdata0_0;
	uint32_t reserved5[3];
	uint32_t lvtsprotctl_0;
	uint32_t reserved6[2];
	uint32_t lvtsprottc_0;
	uint32_t reserved7[5];
	uint32_t lvtsclken_0;
	uint32_t reserved8[2];
	uint32_t lvtsspare0_0;
	uint32_t lvtsspare1_0;
	uint32_t reserved9[2];
};
check_member(mtk_thermal_controller_regs, lvtsmonint_0, 0x000C);
check_member(mtk_thermal_controller_regs, lvtsh2nthre_0, 0x0024);
check_member(mtk_thermal_controller_regs, lvtstssel_0, 0x0040);
check_member(mtk_thermal_controller_regs, lvtscalscale_0, 0x0048);
check_member(mtk_thermal_controller_regs, lvtsedata03_0, 0x0060);
check_member(mtk_thermal_controller_regs, lvtsmsr0_0, 0x0090);
check_member(mtk_thermal_controller_regs, lvtsmsr3_0, 0x009C);
check_member(mtk_thermal_controller_regs, lvtsrdata0_0, 0x00B0);
check_member(mtk_thermal_controller_regs, lvtsprotctl_0, 0x00C0);
check_member(mtk_thermal_controller_regs, lvtsprottc_0, 0x00CC);
check_member(mtk_thermal_controller_regs, lvtsclken_0, 0x00E4);
check_member(mtk_thermal_controller_regs, lvtsspare0_0, 0x00F0);
check_member(mtk_thermal_controller_regs, lvtsspare1_0, 0x00F4);

struct mtk_infracfg_regs {
	uint32_t reserved1[968];
	uint32_t mcu_rst_set;
	uint32_t mcu_rst_clr;
	uint32_t reserved2[10];
	uint32_t ap_rst_set;
	uint32_t ap_rst_clr;
};
check_member(mtk_infracfg_regs, mcu_rst_set, 0x0F20);
check_member(mtk_infracfg_regs, mcu_rst_clr, 0x0F24);
check_member(mtk_infracfg_regs, ap_rst_set, 0x0F50);
check_member(mtk_infracfg_regs, ap_rst_clr, 0x0F54);

struct mtk_infracfg_ao_regs {
	uint32_t reserved1[33];
	uint32_t therm_module_sw_cg_0_clr;
};

check_member(mtk_infracfg_ao_regs, therm_module_sw_cg_0_clr, 0x0084);

struct mtk_efusec_regs {
	uint32_t reserved1[105];
	uint32_t lvts_address_index_addr[LVTS_ADDRESS_INDEX_NUM];
};
check_member(mtk_efusec_regs, lvts_address_index_addr, 0x01A4);
#endif /* __SOC_MEDIATEK_MT8189_INCLUDE_SOC_MT_THERMAL_INTERNAL_H__ */
