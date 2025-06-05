// SPDX-License-Identifier: GPL-2.0-only OR MIT

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 9.13
 *
 * Copyright (C) 2024 MediaTek Inc.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <drivers/delay_timer.h>
#include <string.h>
#include <stdbool.h>

#include <mt_thermal_internal.h>
/*=============================================================
 * Global variable definition
 *=============================================================
 */
int tscpu_ts_lvts_temp[L_TS_LVTS_NUM];
int tscpu_ts_lvts_temp_r[L_TS_LVTS_NUM];
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

struct lvts_thermal_controller lvts_tscpu_g_tc[LVTS_CONTROLLER_NUM] = {
	[LVTS_MCU_CONTROLLER0] = {/* MCU_BIG core0/1 */
		.ts = {L_TS_LVTS1_0, L_TS_LVTS1_1, L_TS_LVTS1_2, L_TS_LVTS1_3},
		.sensor_on_off = {SEN_ON, SEN_ON, SEN_ON, SEN_ON},
		.ctrl_on_off = CTRL_ON,
		.ts_number = 4,
		.reboot_temperature = 119000,
		.dominator_ts_idx = 0,
		.tc_speed = {
			.group_interval_delay = 0x0f6,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001
		},
		.tc_regs = mtk_lvts_mcu_controller0,
	},
	[LVTS_MCU_CONTROLLER1] = {/* MCU_BIG core2/3 */
		.ts = {L_TS_LVTS2_0, L_TS_LVTS2_1, L_TS_LVTS2_2, L_TS_LVTS2_3},
		.sensor_on_off = {SEN_ON, SEN_ON, SEN_ON, SEN_ON},
		.ctrl_on_off = CTRL_ON,
		.ts_number = 4,
		.reboot_temperature = 119000,
		.dominator_ts_idx = 0,
		.tc_speed = {
			.group_interval_delay = 0x0f6,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001
		},
		.tc_regs = mtk_lvts_mcu_controller1,
	},
	[LVTS_MCU_CONTROLLER2] = {/* MCU_LITTLE */
		.ts = {L_TS_LVTS3_0, L_TS_LVTS3_1, L_TS_LVTS3_2, L_TS_LVTS3_3},
		.sensor_on_off = {SEN_ON, SEN_ON, SEN_ON, SEN_ON},
		.ctrl_on_off = CTRL_ON,
		.ts_number = 4,
		.reboot_temperature = 119000,
		.dominator_ts_idx = 0,
		.tc_speed = {
			.group_interval_delay = 0x1c1,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001
		},
		.tc_regs = mtk_lvts_mcu_controller2,
	},
	[LVTS_AP_CONTROLLER0] = {/* SOC-TOP,APU */
		.ts = {L_TS_LVTS4_0, L_TS_LVTS4_1, L_TS_LVTS4_2, L_TS_LVTS4_3},
		.sensor_on_off = {SEN_ON, SEN_ON, SEN_ON, SEN_ON},
		.ctrl_on_off = CTRL_ON,
		.ts_number = 4,
		.reboot_temperature = 119000,
		.dominator_ts_idx = 0,
		.tc_speed = {
			.group_interval_delay = 0x0d2,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001
		},
		.tc_regs = mtk_lvts_ap_controller0,
	},
	[LVTS_MFG_CONTROLLER0] = {/*(MFG)*/
		.ts = {L_TS_LVTS5_0, L_TS_LVTS5_1},
		.sensor_on_off = {SEN_ON, SEN_ON},
		.ctrl_on_off = CTRL_ON,
		.ts_number = 2,
		.reboot_temperature = 119000,
		.dominator_ts_idx = 0,
		.tc_speed = {
			.group_interval_delay = 0x08a,
			.period_unit = 0x001,
			.filter_interval_delay = 0x001,
			.sensor_interval_delay = 0x001
		},
		.tc_regs = mtk_lvts_mfg_controller0,
	},
};

/*=============================================================
 * Local variable definition
 *=============================================================
 */
static uint32_t g_cali_mode;
static uint32_t g_golden_temp;
static uint32_t g_golden_temp_ht;
static uint32_t g_count_r[L_TS_LVTS_NUM];
static uint32_t g_count_rc[LVTS_CONTROLLER_NUM];
static uint32_t g_count_rc_now[L_TS_LVTS_NUM];

static int lvts_write_device(uint32_t config, uint32_t dev_reg_idx, uint32_t data, uint32_t tc_num)
{
	uint32_t result;

	dev_reg_idx &= 0xFF;
	data &= 0xFF;

	config = config | (dev_reg_idx << 8) | data;

	write32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvts_config_0, config);

	/*
	 * LVTS Device Register Setting take 1us(by 26MHz clock source)
	 * interface latency to access.
	 * So we set 2~3us delay could guarantee access complete.
	 */
	udelay(3);

	/* Check ASIF bus status for transaction finished
	 * Wait until DEVICE_ACCESS_START = 0
	 */
	result = retry(100, !(read32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvts_config_0)
		       & DEVICE_ACCESS_START), udelay(2));

	if (!result)
		printf("write err: DEVICE_ACCESS_START didn't ready, reg0x%x\n", dev_reg_idx);

	return 1;
}

static uint32_t lvts_read_device(uint32_t config, uint32_t dev_reg_idx, uint32_t tc_num)
{
	uint32_t result;
	uint32_t data;

	dev_reg_idx &= 0xFF;

	config = config | (dev_reg_idx << 8);

	write32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvts_config_0, config);

	/* wait 2us + 3us buffer*/
	udelay(5);
	/* Check ASIF bus status for transaction finished
	 * Wait until DEVICE_ACCESS_START = 0
	 */
	result = retry(100, !(read32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvts_config_0) &
		       DEVICE_ACCESS_START), udelay(2));

	if (!result)
		printf("read err: DEVICE_ACCESS_START didn't ready, reg0x%x\n", dev_reg_idx);

	data = read32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsrdata0_0);

	return data;
}

#define LVTS_COEFF_A_X_1000		-250460
#define LVTS_COEFF_B_X_1000		250460

static int lvts_raw_to_temp(uint32_t msr_raw, enum lvts_sensor_enum ts_name)
{
	/* This function returns degree mC
	 * temp[i] = a * MSR_RAW/16384 + GOLDEN_TEMP/2 + b
	 * a = -250.46
	 * b =  250.46
	 */
	int temp_mC = (LVTS_COEFF_A_X_1000 * ((uint64_t)msr_raw)) >> 14;

	temp_mC += g_golden_temp * 500 + LVTS_COEFF_B_X_1000;

	return temp_mC;
}

static void lvts_efuse_setting(void)
{
	uint32_t i;
	uint32_t j, s_index;
	uint32_t efuse_data;

	printf("%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		for (j = 0; j < lvts_tscpu_g_tc[i].ts_number; j++) {
			if (lvts_tscpu_g_tc[i].sensor_on_off[j] == SEN_OFF)
				continue;

			s_index = lvts_tscpu_g_tc[i].ts[j];

			efuse_data = ((uint64_t)g_count_rc_now[s_index] * g_count_r[s_index]) >> 14;

			switch (j) {
			case 0:
				write32(&lvts_tscpu_g_tc[i].tc_regs->lvtsedata00_0, efuse_data);
				printf("efuse LVTSEDATA00_%d 0x%x\n", i,
				       read32(&lvts_tscpu_g_tc[i].tc_regs->lvtsedata00_0));
				break;
			case 1:
				write32(&lvts_tscpu_g_tc[i].tc_regs->lvtsedata01_0, efuse_data);
				printf("efuse LVTSEDATA01_%d 0x%x\n", i,
				       read32(&lvts_tscpu_g_tc[i].tc_regs->lvtsedata01_0));
				break;
			case 2:
				write32(&lvts_tscpu_g_tc[i].tc_regs->lvtsedata02_0, efuse_data);
				printf("efuse LVTSEDATA02_%d 0x%x\n", i,
				       read32(&lvts_tscpu_g_tc[i].tc_regs->lvtsedata02_0));
				break;
			case 3:
				write32(&lvts_tscpu_g_tc[i].tc_regs->lvtsedata03_0, efuse_data);
				printf("efuse LVTSEDATA03_%d 0x%x\n", i,
				       read32(&lvts_tscpu_g_tc[i].tc_regs->lvtsedata03_0));
				break;
			default:
				printf("%s, illegal ts order : %d!!\n", __func__, j);
				break;
			}
		}
	}
}

static void lvts_device_identification(void)
{
	uint32_t dev_id, i, data, result;

	printf("===== %s begin ======\n", __func__);
	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		/* Enable LVTS_CTRL Clock */
		write32(&lvts_tscpu_g_tc[i].tc_regs->lvtsclken_0, 0x00000001);

		/* Reset All Devices */
		lvts_write_device(0xC1030000, 0xFF, 0xFF, i);

		/*  Read back Dev_ID with Update */
		lvts_write_device(0xC5020000, 0xFC, 0x55, i);

		dev_id = 0x81 + i;

		result = retry(5, (data = read32(&lvts_tscpu_g_tc[i].tc_regs->lvts_id_0) &
			       GENMASK(7, 0)) == dev_id, udelay(3));

		if (!result)
			printf("LVTS_TC_%d, addr:0x%lx, Device ID should be 0x%x, but 0x%x\n",
			       i, (uintptr_t)&lvts_tscpu_g_tc[i].tc_regs->lvts_id_0, dev_id,
			       data);
	}
}

static void lvts_device_enable_init_all_devices(void)
{
	uint32_t i;

	printf("%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		/*  Stop Counting (RG_TSFM_ST=0) */
		lvts_write_device(0xC1030000, 0x03, 0x00, i);
		/* RG_TSFM_LPDLY[1:0]=2' 10 */
		lvts_write_device(0xC1030000, 0x07, 0xA6, i);
		/* Set LVTS device counting window 20us */
		/* Counting Window */
		lvts_write_device(0xC1030000, 0x05, 0x00, i);
		lvts_write_device(0xC1030000, 0x04, 0x20, i);
		/* TSV2F_CHOP_CKSEL & TSV2F_EN */
		lvts_write_device(0xC1030000, 0x0A, 0x8C, i);
		/* TSBG_DEM_CKSEL * TSBG_CHOP_EN */
		lvts_write_device(0xC1030000, 0x0C, 0xFC, i);
		/* Set TS_RSV */
		lvts_write_device(0xC1030000, 0x09, 0x8D, i);
		/* Set TS_CHOP control */
		lvts_write_device(0xC1030000, 0x08, 0xF1, i);
	}
}

static void lvts_thermal_cal_prepare(void)
{
	uint32_t temp[LVTS_ADDRESS_INDEX_NUM];
	uint32_t i, cnt;
	int lvts_coef_a;

	for (i = 0, cnt = 0; i < ARRAY_SIZE(temp); i++) {
		temp[i] = read32(&mtk_efusec->lvts_address_index_addr[i]);
		printf("[lvts_cali] %d: 0x%x\n", i, temp[i]);

		if (temp[i] == 0)
			cnt++;
	}

	/*0x11F1_01A4,LVTS1*/
	g_cali_mode = ((temp[0] & GENMASK(31, 31)) >> 31);
	g_golden_temp_ht = ((temp[0] & GENMASK(15, 8)) >> 8);
	g_golden_temp = (temp[0] & GENMASK(7, 0));

	g_count_r[L_TS_LVTS1_0] = (temp[1] & GENMASK(23, 0)); /*0x11F1_01A8,LVTS1_0*/
	g_count_r[L_TS_LVTS1_1] = (temp[2] & GENMASK(23, 0)); /*0x11F1_01AC,LVTS1_1*/
	g_count_r[L_TS_LVTS1_2] = (temp[3] & GENMASK(23, 0)); /*0x11F1_01B0,LVTS1_2*/
	g_count_r[L_TS_LVTS1_3] = (temp[4] & GENMASK(23, 0)); /*0x11F1_01B4,LVTS1_3*/
	g_count_r[L_TS_LVTS2_0] = (temp[6] & GENMASK(23, 0)); /*0x11F1_01BC,LVTS2_0*/
	g_count_r[L_TS_LVTS2_1] = (temp[7] & GENMASK(23, 0)); /*0x11F1_01C0,LVTS2_1*/
	g_count_r[L_TS_LVTS2_2] = (temp[8] & GENMASK(23, 0)); /*0x11F1_01C4,LVTS2_2*/
	g_count_r[L_TS_LVTS2_3] = (temp[9] & GENMASK(23, 0)); /*0x11F1_01C8,LVTS2_3*/
	g_count_r[L_TS_LVTS3_0] = (temp[11] & GENMASK(23, 0)); /*0x11F1_01D0,LVTS3_0*/
	g_count_r[L_TS_LVTS3_1] = (temp[12] & GENMASK(23, 0)); /*0x11F1_01D4,LVTS3_1*/
	g_count_r[L_TS_LVTS3_2] = (temp[13] & GENMASK(23, 0)); /*0x11F1_01D8,LVTS3_2*/
	g_count_r[L_TS_LVTS3_3] = (temp[14] & GENMASK(23, 0)); /*0x11F1_01DC,LVTS3_3*/
	g_count_r[L_TS_LVTS4_0] = (temp[16] & GENMASK(23, 0)); /*0x11F1_01E4,LVTS4_0*/
	g_count_r[L_TS_LVTS4_1] = (temp[17] & GENMASK(23, 0)); /*0x11F1_01E8,LVTS4_1*/
	g_count_r[L_TS_LVTS4_2] = (temp[18] & GENMASK(23, 0)); /*0x11F1_01EC,LVTS4_2*/
	g_count_r[L_TS_LVTS4_3] = (temp[19] & GENMASK(23, 0)); /*0x11F1_01F0,LVTS4_3*/
	g_count_r[L_TS_LVTS5_0] = (temp[21] & GENMASK(23, 0)); /*0x11F1_01EC,LVTS5_0*/
	g_count_r[L_TS_LVTS5_1] = (temp[22] & GENMASK(23, 0)); /*0x11F1_01F0,LVTS5_1*/

	/*0x11F1_01A4,LVTS1*/
	g_count_rc[LVTS_MCU_CONTROLLER0] = ((temp[0] & GENMASK(31, 8)) >> 8);
	/*0x11F1_01B8,LVTS2*/
	g_count_rc[LVTS_MCU_CONTROLLER1] = (temp[5] & GENMASK(23, 0));
	/*0x11F1_01CC,LVTS3*/
	g_count_rc[LVTS_MCU_CONTROLLER2] = (temp[10] & GENMASK(23, 0));

	/*0x11F1_01E0,LVTS4*/
	g_count_rc[LVTS_AP_CONTROLLER0] = (temp[15] & GENMASK(23, 0));

	/*0x11F1_01F4,LVTS5*/
	g_count_rc[LVTS_MFG_CONTROLLER0] = (temp[20] & GENMASK(23, 0));

	if (cnt == ARRAY_SIZE(temp)) {
		/* It means all efuse data are equal to 0 */
		printf("[lvts_cal] This sample is not calibrated, fake !!\n");
		g_golden_temp = DEFAULT_EFUSE_GOLDEN_TEMP;
		g_golden_temp_ht = DEFAULT_EFUSE_GOLDEN_TEMP_HT;
		for (i = 0; i < L_TS_LVTS_NUM; i++)
			g_count_r[i] = DEFAULT_EFUSE_COUNT;

		for (i = 0; i < LVTS_CONTROLLER_NUM; i++)
			g_count_rc[i] = DEFAULT_EFUSE_COUNT_RC;
	}

	printf(BIOS_INFO,
	       "[lvts_cal] g_golden_temp = %d, g_golden_temp_ht = %d\n",
	       g_golden_temp, g_golden_temp_ht);

	printf("[lvts_cal] num:g_count_r:g_count_rc ");
	for (i = 0; i < ARRAY_SIZE(g_count_r); i++) {
		if (i < ARRAY_SIZE(g_count_rc))
			printf("%d:%d:%d ", i, g_count_r[i], g_count_rc[i]);
		else
			printf("%d:%d ", i, g_count_r[i]);
	}
	printf("\n");

	printf("[lvts_coef_a] ");
	for (i = 0; i < ARRAY_SIZE(g_count_r); i++) {
		lvts_coef_a = LVTS_COF_T_SLP_GLD + ((long long)g_count_r[i] *
			      LVTS_COF_T_CONST_OFS / LVTS_COF_COUNT_R_GLD -
			      LVTS_COF_T_CONST_OFS);

		printf("%d:%d ", i, lvts_coef_a);
	}
	printf("\n");

	/* write gold temp to spare reg LVTSSPARE1_0 */
	write32(&lvts_tscpu_g_tc[0].tc_regs->lvtsspare1_0, g_golden_temp);
}

static int lvts_read_tc_raw_and_temp(uint32_t *addr, enum lvts_sensor_enum ts_name)
{
	int temp = 0, raw = 0, raw1 = 0, raw2 = 0;

	if (addr == 0)
		return 0;

	raw = read32(addr);
	raw1 = (raw & BIT(16)) >> 16; /*bit 16 : valid bit*/
	raw2 = raw & GENMASK(15, 0);

	if (raw2 == 0) {
		/* 26111 is magic num
		 * this is to keep system alive for a while
		 * to wait HW init done,
		 * because 0 msr raw will translates to 28x'C
		 * and then 28x'C will trigger a SW reset.
		 *
		 * if HW init finish, this msr raw will not be 0,
		 * system can report normal temperature.
		 * if wait over 60 times zero, this means something
		 * wrong with HW, must trigger BUG on and dump useful
		 * register for debug.
		 */

		temp = 26111;
	} else
		temp = lvts_raw_to_temp(raw2, ts_name);

	printf(BIOS_INFO,
	       "[LVTS_MSR] ts%d msr_all=%x, valid=%d, msr_temp=%d, temp=%d\n",
	       ts_name, raw, raw1, raw2, temp);

	tscpu_ts_lvts_temp_r[ts_name] = raw2;

	return temp;
}

static void lvts_tscpu_thermal_read_tc_temp(uint32_t tc_num, enum lvts_sensor_enum type, int order)
{
	if (lvts_tscpu_g_tc[tc_num].ctrl_on_off == CTRL_OFF)
		return;

	switch (order) {
	case 0:
		tscpu_ts_lvts_temp[type] =
			lvts_read_tc_raw_and_temp(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmsr0_0,
						  type);
		break;
	case 1:
		tscpu_ts_lvts_temp[type] =
			lvts_read_tc_raw_and_temp(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmsr1_0,
						  type);
		break;
	case 2:
		tscpu_ts_lvts_temp[type] =
			lvts_read_tc_raw_and_temp(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmsr2_0,
						  type);
		break;
	case 3:
		tscpu_ts_lvts_temp[type] =
			lvts_read_tc_raw_and_temp(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmsr3_0,
						  type);
		break;
	default:
		tscpu_ts_lvts_temp[type] =
			lvts_read_tc_raw_and_temp(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmsr0_0,
						  type);

		printf(BIOS_INFO,
		       "%s order %d tc_num %d type %d temp %d\n",
		       __func__, order, tc_num, type, tscpu_ts_lvts_temp[type]);
		break;
	}
}

static void read_all_tc_lvts_temperature(void)
{
	uint32_t i, j;

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		for (j = 0; j < lvts_tscpu_g_tc[i].ts_number; j++)
			lvts_tscpu_thermal_read_tc_temp(i, lvts_tscpu_g_tc[i].ts[j], j);
	}
}

static void lvts_enable_sensing_points(uint32_t tc_num)
{
	uint32_t i;
	uint32_t value = 0x200;

	printf("===== %s begin ======\n", __func__);

	for (i = 0; i < lvts_tscpu_g_tc[tc_num].ts_number; i++) {
		if (lvts_tscpu_g_tc[tc_num].sensor_on_off[i] == SEN_ON)
			value |= (0x1 << i);
	}

	write32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmonctl0_0, value);

	printf(BIOS_INFO,
	       "%s tc_num=%d,value in LVTSMONCTL0_0 = %d\n", __func__, tc_num, value);
}

/*
 * disable ALL periodoc temperature sensing point
 */
static void lvts_disable_all_sensing_points(void)
{
	uint32_t i;

	printf("%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		write32(&lvts_tscpu_g_tc[i].tc_regs->lvtsmonctl0_0, 0x00000200);
	}
}

static void lvts_wait_all_sensing_points_idle(void)
{
	uint32_t mask, error_code, is_error, i, cnt, result;

	mask = BIT(10) | BIT(7) | BIT(0);

	for (cnt = 0; cnt < 2; cnt++) {
		is_error = 0;
		for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
			if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
				continue;

			error_code = read32(&lvts_tscpu_g_tc[i].tc_regs->lvtsmsrctl1_0);
			result = retry(100, !(error_code & mask), udelay(2); error_code =
				       read32(&lvts_tscpu_g_tc[i].tc_regs->lvtsmsrctl1_0));

			if (!result) {
				printf("Error LVTS %d sensing points aren't idle,", i);
				printf(" error_code %d\n", error_code);
			}

			error_code = ((error_code & BIT(10)) >> 8) +
				     ((error_code & BIT(7)) >> 6) +
				     (error_code & BIT(0));

			if (error_code != 0) {
				is_error = 1;
				break;
			}
		}
		if (is_error == 0)
			break;

	}
}

static void lvts_enable_all_sensing_points(void)
{
	uint32_t i = 0;

	printf("%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		lvts_enable_sensing_points(i);
	}
}

static void lvts_set_init_flag(void)
{
	uint32_t i;

	printf("%s\n", __func__);

	/*write init done flag to inform kernel */
	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		write32(&lvts_tscpu_g_tc[i].tc_regs->lvtsspare0_0, INIT_LVTS_MAGIC);
	}
}


static void lvts_configure_polling_speed_and_filter(uint32_t tc_num)
{
	uint32_t lvtsMonCtl1, lvtsMonCtl2;

	printf("%s\n", __func__);

	lvtsMonCtl1 = ((lvts_tscpu_g_tc[tc_num].tc_speed.group_interval_delay << 20) &
			GENMASK(29, 20)) |
			(lvts_tscpu_g_tc[tc_num].tc_speed.period_unit &
			GENMASK(9, 0));

	lvtsMonCtl2 = ((lvts_tscpu_g_tc[tc_num].tc_speed.filter_interval_delay << 16) &
			GENMASK(25, 16)) |
			(lvts_tscpu_g_tc[tc_num].tc_speed.sensor_interval_delay &
			GENMASK(9, 0));

	/*
	 * Calculating period unit in Module clock x 256, and the Module clock
	 * will be changed to 26M when Infrasys enters Sleep mode.
	 */

	/*
	 * bus clock 66M counting unit is
	 *           12 * 1/66M * 256 = 12 * 3.879us = 46.545 us
	 */
	write32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmonctl1_0, lvtsMonCtl1);
	/*
	 * filt interval is 1 * 46.545us = 46.545us,
	 * sen interval is 429 * 46.545us = 19.968ms
	 */
	write32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmonctl2_0, lvtsMonCtl2);

	/* temperature sampling control, 1 sample */
	write32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmsrctl0_0, 0);

	udelay(1);
	printf(BIOS_INFO,
	       "%s %d,LVTSMONCTL1_0= 0x%x,LVTSMONCTL2_0= 0x%x,LVTSMSRCTL0_0= 0x%x\n",
	       __func__, tc_num,
	       read32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmonctl1_0),
	       read32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmonctl2_0),
	       read32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmsrctl0_0));
}

static void lvts_tscpu_thermal_initial_all_tc(void)
{
	uint32_t i = 0;

	printf("%s\n", __func__);

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		/* Set sensor index of LVTS */
		write32(&lvts_tscpu_g_tc[i].tc_regs->lvtstssel_0, 0x13121110);
		/* Set calculation scale rules */
		write32(&lvts_tscpu_g_tc[i].tc_regs->lvtscalscale_0, 0x00000300);
		/* Set Device Single mode */
		lvts_write_device(0xC1030000, 0x06, 0xF8, i);

		lvts_configure_polling_speed_and_filter(i);
	}

}

static void lvts_device_check_counting_status(uint32_t tc_num)
{
	/* Check this when LVTS device is counting for
	 * a temperature or a RC now
	 */

	uint32_t result;

	result = retry(100, !(read32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvts_config_0) &
		       DEVICE_SENSING_STATUS), udelay(2));

	if (!result)
		printf("Error: DEVICE_SENSING_STATUS didn't ready\n");
}

static void lvts_device_read_count_rc_n(void)
{
	/* Resistor-Capacitor Calibration */
	/* count_rc_n: count RC now */
	int j, num_ts, s_index;
	uint32_t data, i;

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		/* Set LVTS MANUAL-RCK operation */
		lvts_write_device(0xC1030000, 0x0E, 0x00, i);
	}

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		num_ts = lvts_tscpu_g_tc[i].ts_number;

		/* Set LVTS Manual-RCK operation */
		lvts_write_device(0xC1030000, 0x0E, 0x00, i);

		for (j = 0; j < num_ts; j++) {
			if (lvts_tscpu_g_tc[i].sensor_on_off[j] == SEN_OFF)
				continue;

			s_index = lvts_tscpu_g_tc[i].ts[j];

			/* Select sensor-N with RCK */
			lvts_write_device(0xC1030000, 0x0D, j, i);
			/* Set Device Low-Power Single mode */
			lvts_write_device(0xC1030000, 0x06, 0x78, i);
			/* Set TS_EN and DEV_EN */
			lvts_write_device(0xC1030000, 0x08, 0xF5, i);
			/* Toggle VCO_RST */
			lvts_write_device(0xC1030000, 0x08, 0xFD, i);
			/* Set TS_EN and DEV_EN */
			lvts_write_device(0xC1030000, 0x08, 0xF5, i);
			/* wait 10us */
			udelay(10);
			lvts_write_device(0xC1030000, 0x03, 0x02, i);
			/* wait 44.3us or polling counting status */
			lvts_device_check_counting_status(i);
			/* disable TS_EN */
			lvts_write_device(0xC1030000, 0x08, 0xF1, i);
			/* wait 40us at sensor0 */
			/* 2us is margin for different IC */
			if (j == 0)
				udelay(42);

			/* Get RCK count data (sensor-N) */
			data = lvts_read_device(0xC1020000, 0x00, i);
			/* wait 2us + 3us buffer*/
			udelay(5);

			/* Get RCK value from LSB[23:0] */
			g_count_rc_now[s_index] = (data & GENMASK(23, 0));

			/* Recover Setting for Normal Access on
			 * temperature fetch
			 */
			/* Select Sensor-N without RCK */
			lvts_write_device(0xC1030000, 0x0D, (0x10 | j), i);
		}
	}

	printf("[COUNT_RC_NOW] ");
	for (i = 0; i < L_TS_LVTS_NUM; i++)
		printf("%d:%d ", i, g_count_rc_now[i]);

	printf("\n");

}

static void lvts_print_mcu_temp(void)
{
	int i;

	for (i = 0; i < L_TS_LVTS_NUM; i++)
		printf("LVTS%d=%d,", i, tscpu_ts_lvts_temp[i]);

	printf("\n");
}

static void lvts_tscpu_reset_thermal(void)
{
	/* chip dependent, Have to confirm with DE */
	printf("%s\n", __func__);

	/* reset AP thremal ctrl */
	/* 1: Enables thermal control software reset */
	write32(&mtk_infracfg->ap_rst_set, BIT(23));
	/* 1: Clear thermal control software reset */
	write32(&mtk_infracfg->ap_rst_clr, BIT(23));


	/* reset MCU thremal ctrl */
	/* 2: Enables thermal control software reset */
	write32(&mtk_infracfg->mcu_rst_set, BIT(12));
	/* 2: Clear thermal control software reset */
	write32(&mtk_infracfg->mcu_rst_clr, BIT(12));
}

static uint32_t lvts_temp_to_raw(int temp, enum lvts_sensor_enum ts_name)
{
	/* MSR_RAW = ((temp[i] - GOLDEN_TEMP/2 - b) * 16384) / a
	 * a = -250.46
	 * b =  250.46
	 */
	uint32_t msr_raw = 0;

	msr_raw = ((int64_t)(g_golden_temp * 500 + LVTS_COEFF_B_X_1000 - temp) << 14) /
		  (-1 * LVTS_COEFF_A_X_1000);
	msr_raw = msr_raw & 0xFFFF;

	printf("%s msr_raw = %u,temp=%d\n", __func__, msr_raw, temp);
	return msr_raw;
}

static void lvts_set_tc_trigger_hw_protect(uint32_t tc_num)
{
	int temp = 0, raw_high;
	uint32_t d_index, i;
	enum lvts_sensor_enum ts_name;

	if (lvts_tscpu_g_tc[tc_num].dominator_ts_idx < lvts_tscpu_g_tc[tc_num].ts_number)
		d_index = lvts_tscpu_g_tc[tc_num].dominator_ts_idx;
	else {
		d_index = 0;

		printf("Error: LVTS TC%d", tc_num);
		printf("dominator_ts_idx = %d", lvts_tscpu_g_tc[tc_num].dominator_ts_idx);
		printf("should smaller than ts_number = %d\n", lvts_tscpu_g_tc[tc_num].ts_number);
		printf("Use the sensor point %d as the dominated sensor\n", d_index);
	}

	ts_name = lvts_tscpu_g_tc[tc_num].ts[d_index];

	printf(BIOS_INFO,
	       "%s # in tc%d, the dominator ts_name is %d\n", __func__, tc_num, ts_name);

	/* Maximum of 4 sensing points */
	raw_high = 0;
	for (i = 0; i < lvts_tscpu_g_tc[tc_num].ts_number; i++) {
		ts_name = lvts_tscpu_g_tc[tc_num].ts[i];
		temp = lvts_temp_to_raw(lvts_tscpu_g_tc[tc_num].reboot_temperature, ts_name);

		if (raw_high < temp)
			raw_high = temp;
	}

	setbits32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsprotctl_0, GENMASK(13, 0));
	/* disable trigger SPM interrupt */
	write32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmonint_0, 0);

	clrsetbits32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsprotctl_0, GENMASK(19, 16),
		     WRITE_ACCESS);

	write32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsprottc_0, raw_high);

	/* enable trigger Hot SPM interrupt */
	write32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsmonint_0, STAGE3_INT_EN);

	clrbits32(&lvts_tscpu_g_tc[tc_num].tc_regs->lvtsprotctl_0, GENMASK(15, 0));
}

static void lvts_config_all_tc_hw_protect(void)
{
	uint32_t i = 0;

	printf("===== %s begin ======\n", __func__);

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		lvts_set_tc_trigger_hw_protect(i);
	}
}

static bool lvts_init_check(void)
{
	uint32_t i, data;

	for (i = 0; i < ARRAY_SIZE(lvts_tscpu_g_tc); i++) {
		if (lvts_tscpu_g_tc[i].ctrl_on_off == CTRL_OFF)
			continue;

		/* Check LVTS device ID */
		data = (read32(&lvts_tscpu_g_tc[i].tc_regs->lvtsspare0_0) & GENMASK(11, 0));

		if (data != INIT_LVTS_MAGIC)
			return false;
	}

	return true;
}

void lvts_thermal_init(void)
{

	printf("===== %s begin ======\n", __func__);

	if (lvts_init_check() == true) {
		printf("LVTS thermal already init\n");
		return;
	}

	write32(&mtk_infracfg_ao->therm_module_sw_cg_0_clr, THERM_CG);

	lvts_tscpu_reset_thermal();

	lvts_thermal_cal_prepare();
	lvts_device_identification();
	lvts_device_enable_init_all_devices();
	lvts_device_read_count_rc_n();
	lvts_efuse_setting();

	lvts_disable_all_sensing_points();
	lvts_wait_all_sensing_points_idle();
	lvts_tscpu_thermal_initial_all_tc();
	lvts_config_all_tc_hw_protect();
	lvts_enable_all_sensing_points();

	lvts_set_init_flag();

	printf("thermal_init: thermal initialized\n");

	read_all_tc_lvts_temperature();
	lvts_print_mcu_temp();

}

void thermal_init(void)
{
	lvts_thermal_init();
}
