/*
 * Copyright (c) 2019, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <drivers/delay_timer.h>
#include <rtc.h>

/* BBPU = 0 when xreset_rstb goes low */
#define RTC_RELPWR_WHEN_XRST	1

/* Enable this option when pmic initial setting is verified */
#define INIT_SETTING_VERIFIED	1

struct pmic_setting {
	unsigned short addr;
	unsigned short val;
	unsigned short mask;
	unsigned char shift;
};

static struct pmic_setting init_setting[] = {
	{0x1A, 0xA, 0xA, 0},
	{0x1E, 0x1F00, 0x1F00, 0},
	{0x2A, 0x1, 0x1, 0},
	{0x2C, 0x1, 0x1, 0},
	{0x32, 0x8888, 0xFFFF, 0},
	{0x36, 0x8888, 0xFFFF, 0},
	{0x38, 0x8888, 0xFFFF, 0},
	{0x8E, 0x0, 0xFFFF, 0},
	{0x10C, 0x18, 0x18, 0},
	{0x112, 0x4, 0x4, 0},
	{0x118, 0x8, 0x8, 0},
	{0x134, 0x80, 0x2880, 0},
	{0x14C, 0x20, 0x20, 0},
	{0x198, 0x0, 0x1FF, 0},
	{0x408, 0x1, 0x1, 0},
	{0x790, 0x280, 0x780, 0},
	{0x794, 0x88, 0xFF, 0},
	{0x79E, 0x80, 0x1E0, 0},
	{0x7AC, 0x0, 0x2000, 0},
	{0x98A, 0x40, 0x40, 0},
	{0x98C, 0x1, 0x1, 0},
	{0xA08, 0x1, 0x1, 0},
	{0xA20, 0x8, 0x8, 0},
	{0xA2C, 0x81E0, 0x81E0, 0},
	{0xA30, 0xFFF0, 0xFFF0, 0},
	{0xA3C, 0x200, 0x200, 0},
	{0xD08, 0x20, 0x70, 0},
	{0xF8C, 0xAA, 0x3FF, 0},
	{0xF8E, 0x0, 0x1, 0},
	{0x1008, 0x1, 0xF, 0},
	{0x1120, 0x0, 0x8000, 0},
	{0x1126, 0x340, 0xFFC0, 0},
	{0x1128, 0x8083, 0x8FFF, 0},
	{0x112A, 0x30, 0x3FF, 0},
	{0x112E, 0x4000, 0x4000, 0},
	{0x1130, 0xB, 0x3FF, 0},
	{0x1132, 0x30, 0x3FF, 0},
	{0x1134, 0xB, 0x3FF, 0},
	{0x1136, 0x6000, 0x7000, 0},
	{0x1138, 0x60, 0x70, 0},
	{0x113A, 0x95D0, 0xFFF0, 0},
	{0x113C, 0x55, 0xFF, 0},
	{0x1148, 0x4000, 0x4000, 0},
	{0x114C, 0x100, 0x103, 0},
	{0x1208, 0x8040, 0x83FF, 0},
	{0x120C, 0x4, 0x4, 0},
	{0x1210, 0x1, 0x1, 0},
	{0x122E, 0x0, 0x1, 0},
	{0x1412, 0x8, 0x8, 0},
	{0x1418, 0xF, 0xF, 0},
	{0x1444, 0x10, 0x10, 0},
	{0x148A, 0x0, 0x7F, 0},
	{0x148C, 0xF0F, 0x7F7F, 0},
	{0x148E, 0x1030, 0x3030, 0},
	{0x150A, 0x15, 0x7F, 0},
	{0x150C, 0x1429, 0x7F7F, 0},
	{0x150E, 0x1030, 0x3030, 0},
	{0x1588, 0x0, 0x1, 0},
	{0x158A, 0x8, 0x7F, 0},
	{0x158C, 0x70F, 0x7F7F, 0},
	{0x158E, 0x1010, 0x7070, 0},
	{0x160A, 0x35, 0x7F, 0},
	{0x160C, 0x1968, 0x7F7F, 0},
	{0x160E, 0x2020, 0x3030, 0},
	{0x162E, 0x40, 0x7F, 0},
	{0x1632, 0x48, 0x7F, 0},
	{0x168C, 0x202, 0x7F7F, 0},
	{0x168E, 0x70, 0x73, 0},
	{0x1690, 0xE, 0xCE, 0},
	{0x1698, 0x2810, 0x3F3F, 0},
	{0x169A, 0x800, 0x3F00, 0},
	{0x169E, 0x1, 0x1, 0},
	{0x1708, 0x2000, 0x3000, 0},
	{0x170A, 0x120, 0x7E0, 0},
	{0x170C, 0x190, 0x1FF, 0},
	{0x170E, 0x60, 0x60, 0},
	{0x1710, 0x209, 0x60F, 0},
	{0x1712, 0x208, 0x60F, 0},
	{0x1716, 0xFF0, 0x7FF0, 0},
	{0x1718, 0x0, 0x7, 0},
	{0x171C, 0x8000, 0xE000, 0},
	{0x171E, 0x2AD, 0x7FF, 0},
	{0x1720, 0x327A, 0xFFFF, 0},
	{0x1722, 0x323A, 0xFFFF, 0},
	{0x1726, 0x304A, 0x7E7E, 0},
	{0x1728, 0x800, 0x3C00, 0},
	{0x172A, 0x3F, 0x3F, 0},
	{0x172C, 0x5258, 0xFFFF, 0},
	{0x1734, 0x3, 0x7, 0},
	{0x173A, 0x8, 0xF, 0},
	{0x1740, 0x50, 0x4F0, 0},
	{0x1742, 0x3C, 0xFC, 0},
	{0x1744, 0x0, 0x300, 0},
	{0x1746, 0x8886, 0xFFFF, 0},
	{0x174C, 0x0, 0x6000, 0},
	{0x1758, 0x17, 0x3F, 0},
	{0x175C, 0x1700, 0x3F00, 0},
	{0x176C, 0x700, 0xF00, 0},
	{0x188C, 0x1, 0x1, 0},
	{0x188E, 0x0, 0x1, 0},
	{0x1890, 0x0, 0x1, 0},
	{0x1892, 0x0, 0x1, 0},
	{0x1894, 0x0, 0x1, 0},
	{0x1896, 0x0, 0x1, 0},
	{0x1898, 0x0, 0x1, 0},
	{0x189A, 0x0, 0x1, 0},
	{0x189C, 0x0, 0x1, 0},
	{0x189E, 0x0, 0x1, 0},
	{0x18A0, 0x0, 0x1, 0},
	{0x18A2, 0x0, 0x1, 0},
	{0x18A4, 0x0, 0x1, 0},
	{0x18A6, 0x0, 0x1, 0},
	{0x18A8, 0x0, 0x1, 0},
	{0x18AA, 0x0, 0x1, 0},
	{0x18AC, 0x0, 0x1, 0},
	{0x18AE, 0x0, 0x1, 0},
	{0x18B0, 0x0, 0x1, 0},
	{0x18B2, 0x0, 0x1, 0},
	{0x18B4, 0x0, 0x1, 0},
	{0x18B6, 0x0, 0x1, 0},
	{0x18B8, 0x0, 0x1, 0},
	{0x18BA, 0x0, 0x1, 0},
	{0x18BC, 0x0, 0x1, 0},
	{0x18BE, 0x0, 0x1, 0},
	{0x18C0, 0x0, 0x1, 0},
	{0x18C2, 0x0, 0x1, 0},
	{0x18E6, 0x1, 0x1, 0},
	{0x19B4, 0x35, 0x7F, 0},
	{0x19B6, 0x70F, 0x7F7F, 0},
	{0x19D2, 0xD, 0x7F, 0},
	{0x19D4, 0xF0F, 0x7F7F, 0},
	{0x1B9E, 0x1, 0x1, 0},
};

static bool s_recovery_flag = false;
static bool g_rtc_2sec_stat;

static void RTC_Config_Interface(uint32_t addr, uint16_t data,
			    uint16_t MASK, uint16_t SHIFT)
{
	uint16_t pmic_reg = 0;

	pmic_reg = RTC_Read(addr);

	pmic_reg &= ~(MASK << SHIFT);
	pmic_reg |= (data << SHIFT);

	RTC_Write(addr, pmic_reg);
}

static void rtc_disable_2sec_reboot(void)
{
	uint16_t reboot;

	reboot = (RTC_Read(RTC_AL_SEC) & ~RTC_BBPU_2SEC_EN) &
		 ~RTC_BBPU_AUTO_PDN_SEL;
	RTC_Write(RTC_AL_SEC, reboot);
	RTC_Write_Trigger();
}

static void rtc_xosc_write(uint16_t val, bool reload)
{
	uint16_t bbpu;

	RTC_Write(RTC_OSC32CON, RTC_OSC32CON_UNLOCK1);
	rtc_busy_wait();
	RTC_Write(RTC_OSC32CON, RTC_OSC32CON_UNLOCK2);
	rtc_busy_wait();

	RTC_Write(RTC_OSC32CON, val);
	rtc_busy_wait();

	if (reload) {
		bbpu = RTC_Read(RTC_BBPU) | RTC_BBPU_KEY | RTC_BBPU_RELOAD;
		RTC_Write(RTC_BBPU, bbpu);
		RTC_Write_Trigger();
	}
}

static void rtc_enable_k_eosc(void)
{
	uint16_t osc32;
	uint16_t rtc_eosc_cali_td = 8; /* eosc cali period time */

	/* Truning on eosc cali mode clock */
	RTC_Config_Interface(PMIC_RG_TOP_CON, 1,
			PMIC_RG_SRCLKEN_IN0_HW_MODE_MASK,
			PMIC_RG_SRCLKEN_IN0_HW_MODE_SHIFT);
	RTC_Config_Interface(PMIC_RG_TOP_CON, 1,
			PMIC_RG_SRCLKEN_IN1_HW_MODE_MASK,
			PMIC_RG_SRCLKEN_IN1_HW_MODE_SHIFT);
	RTC_Config_Interface(PMIC_RG_SCK_TOP_CKPDN_CON0, 0,
			PMIC_RG_RTC_EOSC32_CK_PDN_MASK,
			PMIC_RG_RTC_EOSC32_CK_PDN_SHIFT);

	switch (rtc_eosc_cali_td) {
	case 1:
		RTC_Config_Interface(PMIC_RG_EOSC_CALI_CON0, 0x3,
			PMIC_RG_EOSC_CALI_TD_MASK, PMIC_RG_EOSC_CALI_TD_SHIFT);
		break;
	case 2:
		RTC_Config_Interface(PMIC_RG_EOSC_CALI_CON0, 0x4,
			PMIC_RG_EOSC_CALI_TD_MASK, PMIC_RG_EOSC_CALI_TD_SHIFT);
		break;
	case 4:
		RTC_Config_Interface(PMIC_RG_EOSC_CALI_CON0, 0x5,
			PMIC_RG_EOSC_CALI_TD_MASK, PMIC_RG_EOSC_CALI_TD_SHIFT);
		break;
	case 16:
		RTC_Config_Interface(PMIC_RG_EOSC_CALI_CON0, 0x7,
			PMIC_RG_EOSC_CALI_TD_MASK, PMIC_RG_EOSC_CALI_TD_SHIFT);
		break;
	default:
		RTC_Config_Interface(PMIC_RG_EOSC_CALI_CON0, 0x6,
			PMIC_RG_EOSC_CALI_TD_MASK, PMIC_RG_EOSC_CALI_TD_SHIFT);
		break;
	}
	/* Switch the DCXO from 32k-less mode to RTC mode,
	 * otherwise, EOSC cali will fail
	 */
	/* RTC mode will have only OFF mode and FPM */
	RTC_Config_Interface(PMIC_RG_DCXO_CW02, 0, PMIC_RG_XO_EN32K_MAN_MASK,
		PMIC_RG_XO_EN32K_MAN_SHIFT);
	RTC_Write(RTC_BBPU,
		  RTC_Read(RTC_BBPU) | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	RTC_Write_Trigger();
	/* Enable K EOSC mode for normal power off and then plug out battery */
	RTC_Write(RTC_AL_YEA, ((RTC_Read(RTC_AL_YEA) | RTC_K_EOSC_RSV_0)
				& (~RTC_K_EOSC_RSV_1)) | RTC_K_EOSC_RSV_2);
	RTC_Write_Trigger();

	osc32 = RTC_Read(RTC_OSC32CON);
	rtc_xosc_write(osc32 | RTC_EMBCK_SRC_SEL, true);
	INFO("[RTC] RTC_enable_k_eosc\n");
}

void rtc_power_off_sequence(void)
{
	uint16_t bbpu;

	rtc_disable_2sec_reboot();
	rtc_enable_k_eosc();

	/* clear alarm */
	bbpu = RTC_BBPU_KEY | RTC_BBPU_CLR | RTC_BBPU_PWREN;
	if (Writeif_unlock()) {
		RTC_Write(RTC_BBPU, bbpu);

		RTC_Write(RTC_AL_MASK, RTC_AL_MASK_DOW);
		RTC_Write_Trigger();
		mdelay(1);

		bbpu = RTC_Read(RTC_BBPU) | RTC_BBPU_KEY | RTC_BBPU_RELOAD;
		RTC_Write(RTC_BBPU, bbpu);
		RTC_Write_Trigger();
		INFO("[RTC] BBPU=0x%x, IRQ_EN=0x%x, AL_MSK=0x%x, AL_SEC=0x%x\n",
		     RTC_Read(RTC_BBPU), RTC_Read(RTC_IRQ_EN),
		     RTC_Read(RTC_AL_MASK), RTC_Read(RTC_AL_SEC));
	}
}

static void rtc_set_recovery_mode_stat(bool enable)
{
	s_recovery_flag = enable;
}

static bool rtc_get_recovery_mode_stat(void)
{
	return s_recovery_flag;
}

void rtc_set_xomode(bool bIs32kLess)
{
	/* XO_XMODE_M=1 & XO_EN32K_M=1 is 32k-less mode */
	/* XO_XMODE_M=1 & XO_EN32K_M=0 is RTC/32k mode */
	RTC_Config_Interface(PMIC_RG_DCXO_CW02, 1,
		PMIC_RG_XO_XMODE_M_MASK, PMIC_RG_XO_XMODE_M_SHIFT);

	if (bIs32kLess)
		RTC_Config_Interface(PMIC_RG_DCXO_CW02, 1,
			PMIC_RG_XO_EN32K_M_MASK, PMIC_RG_XO_EN32K_M_SHIFT);
	else
		RTC_Config_Interface(PMIC_RG_DCXO_CW02, 0,
			PMIC_RG_XO_EN32K_M_MASK, PMIC_RG_XO_EN32K_M_SHIFT);

	/* Switch to override mode XO_EN32K_MAN=1 and XO_XMODE_MAN=1 */
	RTC_Config_Interface(PMIC_RG_DCXO_CW02, 0,
		PMIC_RG_XO_EN32K_MAN_MASK, PMIC_RG_XO_EN32K_MAN_SHIFT);
	RTC_Config_Interface(PMIC_RG_DCXO_CW02, 1,
		PMIC_RG_XO_XMODE_MAN_MASK, PMIC_RG_XO_XMODE_MAN_SHIFT);
}

static bool rtc_xosc_check_clock(uint16_t *result)
{
	if ((result[0] >= 3  &&result[0] <= 7 ) &&
			(result[1] > 1500 && result[1] < 6000) &&
			(result[2] == 0) &&
			(result[3] == 0))
		return true;
	else
		return false;
}

static void rtc_switch_mode(bool XOSC, bool recovery)
{
	uint16_t osc32con;

	if (XOSC) {
		if (recovery) {
			/* Since HW XTAL_DET is removed, there is a register
			 * for switch the external crystal or the embedded
			 * clock */
			/* 0: with the external xtal */
			RTC_Config_Interface(PMIC_RG_SCK_TOP_CON0, 0,
				PMIC_RG_SCK_TOP_XTAL_SEL_MASK,
				PMIC_RG_SCK_TOP_XTAL_SEL_SHIFT);
		}
		osc32con = RTC_OSC32CON_ANALOG_SETTING;
		rtc_xosc_write(osc32con | 0x7, false);
		if (recovery)
			mdelay(1500);
	} else {
		if (recovery) {
			/* 1: without the external xtal */
			RTC_Config_Interface(PMIC_RG_SCK_TOP_CON0, 1,
				PMIC_RG_SCK_TOP_XTAL_SEL_MASK,
				PMIC_RG_SCK_TOP_XTAL_SEL_SHIFT);
			mdelay(100);
		}
		osc32con = RTC_OSC32CON_ANALOG_SETTING | RTC_REG_XOSC32_ENB;
		rtc_xosc_write(osc32con, false);
		mdelay(10);
	}
}

static void rtc_switch_to_dcxo_mode(void)
{
	rtc_switch_mode(false, false);
}

static void rtc_switch_to_xosc_recv_mode(void)
{
	rtc_switch_mode(true, true);
}

static void rtc_switch_to_dcxo_recv_mode(void)
{
	rtc_switch_mode(false, true);
}

static bool rtc_get_xosc_mode(void)
{
	uint16_t con, xosc_mode;

	con = RTC_Read(RTC_OSC32CON);

	if ((con & RTC_XOSC32_ENB) == 0)
		xosc_mode = 1;
	else
		xosc_mode = 0;

	return xosc_mode;
}

void rtc_enable_dcxo(void)
{
	if (!Writeif_unlock()) {
		INFO("Writeif_unlock() fail\n");
		return;
	}

	RTC_Write(RTC_BBPU,
		RTC_Read(RTC_BBPU) | RTC_BBPU_KEY | RTC_BBPU_RELOAD);

	RTC_Write_Trigger();

	rtc_xosc_write(RTC_Read(RTC_OSC32CON) & ~RTC_EMBCK_SRC_SEL, false);

	RTC_Write(RTC_BBPU,
		RTC_Read(RTC_BBPU) | RTC_BBPU_KEY | RTC_BBPU_RELOAD);

	RTC_Write_Trigger();
}

static uint16_t get_frequency_meter(uint16_t val, uint16_t measureSrc,
					uint16_t window_size)
{
	uint16_t ret;
	unsigned long begin = 0;
	uint16_t osc32con;

	if (val != 0) {
		RTC_Write(RTC_BBPU,
			RTC_Read(RTC_BBPU) | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
		RTC_Write_Trigger();
		osc32con = RTC_Read(RTC_OSC32CON) & 0xFFE0;
		rtc_xosc_write(osc32con | (val & 0x1f), false);
	}

	RTC_Config_Interface(PMIC_RG_TOP_CKPDN_CON0_CLR, 1,
		PMIC_RG_FQMTR_CK_PDN_MASK, PMIC_RG_FQMTR_CK_PDN_SHIFT);

	RTC_Config_Interface(PMIC_RG_TOP_CKPDN_CON0_CLR, 1,
		PMIC_RG_FQMTR_32K_CK_PDN_MASK, PMIC_RG_FQMTR_32K_CK_PDN_SHIFT);

	RTC_Write(PMIC_RG_TOP_RST_CON0,
		RTC_Read(PMIC_RG_TOP_RST_CON0) | RTC_FQMTR_RST);

	while (!(RTC_Read(PMIC_RG_FQMTR_CON2) == 0) &&
		(RTC_FQMTR_BUSY & RTC_Read(PMIC_RG_FQMTR_CON0))
			== RTC_FQMTR_BUSY);

	RTC_Write(PMIC_RG_TOP_RST_CON0,
		RTC_Read(PMIC_RG_TOP_RST_CON0) & ~RTC_FQMTR_RST);

	RTC_Config_Interface(PMIC_RG_FQMTR_CON0, 1,
		PMIC_FQMTR_DCXO26M_EN_MASK, PMIC_FQMTR_DCXO26M_EN_SHIFT);

	RTC_Write(PMIC_RG_FQMTR_CON1, window_size);

	RTC_Write(PMIC_RG_FQMTR_CON0, FQMTR_DCXO26M_EN | measureSrc);

	mdelay(1);

	RTC_Write(PMIC_RG_FQMTR_CON0,
		RTC_Read(PMIC_RG_FQMTR_CON0) | RTC_FQMTR_EN);

	mdelay(1);

	while ((RTC_FQMTR_BUSY & RTC_Read(PMIC_RG_FQMTR_CON0))
		== RTC_FQMTR_BUSY) {
		if (begin > 1000)
			break;
		begin++;
		mdelay(1);
	};

	ret = RTC_Read(PMIC_RG_FQMTR_CON2);

	RTC_Config_Interface(PMIC_RG_FQMTR_CON0, 0,
		PMIC_FQMTR_DCXO26M_EN_MASK, PMIC_FQMTR_DCXO26M_EN_SHIFT);

	RTC_Write(PMIC_RG_FQMTR_CON0,
		RTC_Read(PMIC_RG_FQMTR_CON0) & ~FQMTR_DCXO26M_EN);

	mdelay(1);

	RTC_Write(PMIC_RG_FQMTR_CON0,
		RTC_Read(PMIC_RG_FQMTR_CON0) & ~RTC_FQMTR_EN);

	RTC_Config_Interface(PMIC_RG_TOP_CKPDN_CON0_SET, 1,
		PMIC_RG_FQMTR_CK_PDN_MASK, PMIC_RG_FQMTR_CK_PDN_SHIFT);

	RTC_Config_Interface(PMIC_RG_TOP_CKPDN_CON0_SET, 1,
		PMIC_RG_FQMTR_32K_CK_PDN_MASK, PMIC_RG_FQMTR_32K_CK_PDN_SHIFT);

	return ret;
}

static uint16_t eosc_cali(void)
{
	uint16_t val = 0, diff1, diff2, regval;
	int middle;
	int left = RTC_XOSCCALI_START, right = RTC_XOSCCALI_END;

	regval = RTC_Read(PMIC_RG_TOP_CKSEL_CON0) &
	    (~(PMIC_RG_FQMTR_CK_CKSEL_MASK << PMIC_RG_FQMTR_CK_CKSEL_SHIFT));

	RTC_Write(PMIC_RG_TOP_CKSEL_CON0, regval | FQMTR_FIX_CLK_EOSC_32K);

	while (left <= right) {
		middle = (right + left) / 2;
		if (middle == left)
			break;

		val = get_frequency_meter(middle, FQMTR_FQM26M_CK, 0);

		if ((val >= RTC_FQMTR_LOW_BASE) && (val <= RTC_FQMTR_HIGH_BASE))
			break;

		if (val > RTC_FQMTR_HIGH_BASE)
			right = middle;
		else
			left = middle;
	}

	if ((val >= RTC_FQMTR_LOW_BASE) && (val <= RTC_FQMTR_HIGH_BASE))
		return middle;

	val = get_frequency_meter(left, FQMTR_FQM26M_CK, 0);

	if (val > RTC_FQMTR_LOW_BASE)
		diff1 = val - RTC_FQMTR_LOW_BASE;
	else
		diff1 = RTC_FQMTR_LOW_BASE - val;

	val = get_frequency_meter(right, FQMTR_FQM26M_CK, 0);

	if (val > RTC_FQMTR_LOW_BASE)
		diff2 = val - RTC_FQMTR_LOW_BASE;
	else
		diff2 = RTC_FQMTR_LOW_BASE - val;

	if (diff1 < diff2)
		return left;
	else
		return right;
}

static void rtc_measure_four_clock(uint16_t *result)
{
	uint16_t window_size;
	uint16_t regval;

	regval = RTC_Read(PMIC_RG_TOP_CKSEL_CON0) &
	    (~(PMIC_RG_FQMTR_CK_CKSEL_MASK << PMIC_RG_FQMTR_CK_CKSEL_SHIFT));

	RTC_Write(PMIC_RG_TOP_CKSEL_CON0, regval | FQMTR_FIX_CLK_26M);
	window_size = 4;
	mdelay(1);
	result[0] = get_frequency_meter(0, FQMTR_FQM26M_CK, window_size);

	regval = RTC_Read(PMIC_RG_TOP_CKSEL_CON0) &
	    (~(PMIC_RG_FQMTR_CK_CKSEL_MASK << PMIC_RG_FQMTR_CK_CKSEL_SHIFT));

	RTC_Write(PMIC_RG_TOP_CKSEL_CON0, regval | FQMTR_FIX_CLK_XOSC_32K_DET);
	window_size = 4;
	mdelay(1);
	result[1] = get_frequency_meter(0, FQMTR_FQM26M_CK, window_size);

	regval = RTC_Read(PMIC_RG_TOP_CKSEL_CON0) &
	   (~(PMIC_RG_FQMTR_CK_CKSEL_MASK << PMIC_RG_FQMTR_CK_CKSEL_SHIFT));

	RTC_Write(PMIC_RG_TOP_CKSEL_CON0, regval | FQMTR_FIX_CLK_26M);
	window_size = 3970;
	mdelay(1);
	result[2] = get_frequency_meter(0, FQMTR_XOSC32_CK, window_size);
	result[2] = get_frequency_meter(0, FQMTR_DCXO_F32K_CK, window_size);

	regval = RTC_Read(PMIC_RG_TOP_CKSEL_CON0) &
	    (~(PMIC_RG_FQMTR_CK_CKSEL_MASK << PMIC_RG_FQMTR_CK_CKSEL_SHIFT));

	RTC_Write(PMIC_RG_TOP_CKSEL_CON0, regval | FQMTR_FIX_CLK_EOSC_32K);
	window_size = 4;
	mdelay(1);
	result[3] = get_frequency_meter(0, FQMTR_FQM26M_CK, window_size);
}


static void rtc_lpd_state_clr(void)
{
	uint16_t spar0;

	spar0 = RTC_Read(RTC_SPAR0);
	RTC_Write(RTC_SPAR0, spar0 & (~0x0080));
	RTC_Write_Trigger();
}

static bool rtc_gpio_init(void)
{
	uint16_t con;

	/* GPI mode and pull enable + pull down */
	con = RTC_Read(RTC_CON) &
		(RTC_CON_LPSTA_RAW | RTC_CON_LPRST |
		RTC_XOSC32_LPEN | RTC_EOSC32_LPEN);
	con &= ~RTC_CON_GPU;
	con &= ~RTC_CON_F32KOB;
	con |= RTC_CON_GPEN | RTC_CON_GOE;
	RTC_Write(RTC_CON, con);
	if (RTC_Write_Trigger())
		return true;
	else
		return false;
}

static void rtc_osc_init(void)
{
	uint16_t osc32con;
	uint16_t val;

	/* disable 32K export if there are no RTC_GPIO users */
	if (!(RTC_Read(RTC_PDN1) & RTC_GPIO_USER_MASK))
		rtc_gpio_init();

	if (rtc_get_xosc_mode()) {
		osc32con = RTC_OSC32CON_ANALOG_SETTING;
		rtc_xosc_write(osc32con | 0x7, false);
	} else {
		val = eosc_cali();
		osc32con = RTC_OSC32CON_ANALOG_SETTING | RTC_REG_XOSC32_ENB;
		val = (val & 0x001f) | osc32con;
		/* SW_EOSC_Mode */
		rtc_xosc_write(val, false);
	}
	rtc_lpd_state_clr();
}

static bool rtc_lpd_init(void)
{
	uint16_t con;

	con = RTC_Read(RTC_CON) | RTC_XOSC32_LPEN;
	con &= ~RTC_CON_LPRST;
	RTC_Write(RTC_CON, con);
	if (!RTC_Write_Trigger())
		return false;

	con |= RTC_CON_LPRST;
	RTC_Write(RTC_CON, con);
	if (!RTC_Write_Trigger())
		return false;

	con &= ~RTC_CON_LPRST;
	RTC_Write(RTC_CON, con);
	if (!RTC_Write_Trigger())
		return false;

	con = RTC_Read(RTC_CON) | RTC_EOSC32_LPEN;
	con &= ~RTC_CON_LPRST;
	RTC_Write(RTC_CON, con);
	if (!RTC_Write_Trigger())
		return false;

	con |= RTC_CON_LPRST;
	RTC_Write(RTC_CON, con);
	if (!RTC_Write_Trigger())
		return false;

	con &= ~RTC_CON_LPRST;
	RTC_Write(RTC_CON, con);
	if (!RTC_Write_Trigger())
		return false;

	RTC_Write(RTC_SPAR0, RTC_Read(RTC_SPAR0) | 0x0080);
	if (!RTC_Write_Trigger())
		return false;

	return true;
}

static void rtc_clean_mark(void)
{
	uint16_t pdn1, pdn2;

	pdn1 = RTC_Read(RTC_PDN1) & ~(RTC_PDN1_DEBUG | RTC_PDN1_BYPASS_PWR);
	pdn2 = RTC_Read(RTC_PDN2) & ~RTC_PDN1_FAC_RESET;
	RTC_Write(RTC_PDN1, pdn1);
	RTC_Write(RTC_PDN2, pdn2);
	RTC_Write_Trigger();
}

static bool rtc_2sec_stat_clear(void)
{
	RTC_Write(RTC_AL_SEC, RTC_Read(RTC_AL_SEC) & ~RTC_BBPU_2SEC_STAT_CLEAR);
	if (!RTC_Write_Trigger())
		return false;

	RTC_Write(RTC_AL_SEC, RTC_Read(RTC_AL_SEC) | RTC_BBPU_2SEC_STAT_CLEAR);
	if (!RTC_Write_Trigger())
		return false;

	RTC_Write(RTC_AL_SEC, RTC_Read(RTC_AL_SEC) & ~RTC_BBPU_2SEC_STAT_CLEAR);
	if (!RTC_Write_Trigger())
		return false;

	return true;
}

void rtc_save_2sec_stat(void)
{
	uint16_t reboot, pdn2;
	static bool save_stat = false;

	if (save_stat == true)
		return;
	else
		save_stat = true;

	reboot = RTC_Read(RTC_AL_SEC);
	if (reboot & RTC_BBPU_2SEC_EN) {
		switch((reboot &
			RTC_BBPU_2SEC_MODE_MSK) >> RTC_BBPU_2SEC_MODE_SHIFT) {
			case 0:
			case 1:
			case 2:
				if (reboot & RTC_BBPU_2SEC_STAT_STA) {
					rtc_2sec_stat_clear();
					pdn2 = RTC_Read(RTC_PDN2);
					if (pdn2 & RTC_PDN2_AUTOBOOT)
					{
						g_rtc_2sec_stat = false;
						RTC_Write(RTC_PDN2, pdn2 &
							(~RTC_PDN2_AUTOBOOT));
						RTC_Write_Trigger();
					}
					else {
						g_rtc_2sec_stat = true;
					}
				} else {
					rtc_2sec_stat_clear();
					g_rtc_2sec_stat = false;
				}
				break;
			case 3:
				rtc_2sec_stat_clear();
				g_rtc_2sec_stat = true;
			default:
				break;
		}
	} else {
		INFO("rtc 2sec reboot is not enabled\n");
	}

#if (CFG_PMIC_FULL_RESET)
	if (RTC_Read(RTC_AL_MTH) & 0x100) {
		INFO("Force SPAR power on\n");
		RTC_Write(RTC_AL_MTH, RTC_Read(RTC_AL_MTH) & (~0x100));
		g_rtc_2sec_stat = true;
	}
#endif
}

bool rtc_boot_check(void)
{
	uint16_t irqsta, pdn1, pdn2, spar0, spar1;
	uint16_t irqen, sec, min, hou, dom, mth, yea;
	unsigned long now_time, time = 0;
	/* FIXME: `time` is never set, the code below is broken*/
	unsigned long time_upper, time_lower;

	irqsta = RTC_Read(RTC_IRQ_STA);
	pdn1 = RTC_Read(RTC_PDN1);
	pdn2 = RTC_Read(RTC_PDN2);
	spar0 = RTC_Read(RTC_SPAR0);
	spar1 = RTC_Read(RTC_SPAR1);

	if (irqsta & RTC_IRQ_STA_AL) {
#if RTC_RELPWR_WHEN_XRST
		uint16_t bbpu = RTC_Read(RTC_BBPU) | RTC_BBPU_KEY;
		RTC_Write(RTC_BBPU, bbpu);
		RTC_Write_Trigger();
#endif

		if (pdn1 & RTC_PDN1_PWRON_TIME) {

			sec = ((spar0 & RTC_SPAR0_PWRON_SEC_MASK) >>
				RTC_SPAR0_PWRON_SEC_SHIFT);
			min = ((spar1 & RTC_SPAR1_PWRON_MIN_MASK) >>
				RTC_SPAR1_PWRON_MIN_SHIFT);
			hou = ((spar1 & RTC_SPAR1_PWRON_HOU_MASK) >>
				RTC_SPAR1_PWRON_HOU_SHIFT);
			dom = ((spar1 & RTC_SPAR1_PWRON_DOM_MASK) >>
				RTC_SPAR1_PWRON_DOM_SHIFT);
			mth = ((pdn2  & RTC_PDN2_PWRON_MTH_MASK) >>
				RTC_PDN2_PWRON_MTH_SHIFT);
			yea = ((pdn2  & RTC_PDN2_PWRON_YEA_MASK) >>
				RTC_PDN2_PWRON_YEA_SHIFT) + RTC_MIN_YEAR;

#if defined (MTK_KERNEL_POWER_OFF_CHARGING)
			if (kpoc_flag == true) {
				time_upper = time + 5;
				time_lower = time - 2;
			} else
#endif
			{
				time_upper = time + 4;
				time_lower = time - 1;
			}
			if (now_time >= time_lower && now_time <= time_upper) {
				pdn1 = (pdn1 & ~RTC_PDN1_PWRON_TIME) |
						RTC_PDN1_BYPASS_PWR;
				RTC_Write(RTC_PDN1, pdn1);
				RTC_Write(RTC_PDN2, pdn2 |
						RTC_PDN2_PWRON_ALARM);
				RTC_Write_Trigger();
#if defined (MTK_KERNEL_POWER_OFF_CHARGING)
				if ((pdn1 & RTC_PDN1_KPOC) == RTC_PDN1_KPOC) {
					kpoc_flag = false;
					RTC_Write(RTC_PDN1,
						pdn1 & ~RTC_PDN1_KPOC);
					RTC_Write_Trigger();
				}
#endif
				return true;
			} else if (now_time < time) {
				RTC_Write(RTC_AL_YEA,
					(RTC_Read(RTC_AL_YEA) &
					(~RTC_AL_YEA_MASK)) |
					((yea - RTC_MIN_YEAR) &
					RTC_AL_YEA_MASK));
				RTC_Write(RTC_AL_MTH,
					(RTC_Read(RTC_AL_MTH) &
					RTC_NEW_SPARE3) | mth);
				RTC_Write(RTC_AL_DOM,
					(RTC_Read(RTC_AL_DOM) &
					RTC_NEW_SPARE1) | dom);
				RTC_Write(RTC_AL_HOU,
					(RTC_Read(RTC_AL_HOU) &
					RTC_NEW_SPARE0) | hou);
				RTC_Write(RTC_AL_MIN, min);
				RTC_Write(RTC_AL_SEC,
					(RTC_Read(RTC_AL_SEC) &
					(~RTC_AL_SEC_MASK)) |
					(sec & RTC_AL_SEC_MASK));
				RTC_Write(RTC_AL_MASK, RTC_AL_MASK_DOW);
				RTC_Write_Trigger();
				irqen = RTC_Read(RTC_IRQ_EN) |
					RTC_IRQ_EN_ONESHOT_AL;
				RTC_Write(RTC_IRQ_EN, irqen);
				RTC_Write_Trigger();
			}
		}
	}

	if ((pdn1 & RTC_PDN1_RECOVERY_MASK) == RTC_PDN1_FAC_RESET) {
		/* keep bit 4 set until rtc_boot_check() in U-Boot */
		return true;
	}
#if defined (MTK_KERNEL_POWER_OFF_CHARGING)
	if ((pdn1 & RTC_PDN1_KPOC) == RTC_PDN1_KPOC) {
		kpoc_flag = true;
		return false;
	}
#endif

	return false;
}

static void rtc_lpsd_solution(void)
{
	uint16_t irqen;

	RTC_Write(RTC_AL_MASK, (RTC_AL_MASK_SEC | RTC_AL_MASK_MIN |
				RTC_AL_MASK_HOU | RTC_AL_MASK_DOM |
				RTC_AL_MASK_DOW | RTC_AL_MASK_MTH |
				RTC_AL_MASK_YEA));
	irqen = RTC_Read(RTC_IRQ_EN) | RTC_IRQ_EN_ONESHOT_AL;
	RTC_Write(RTC_IRQ_EN, irqen);
	RTC_Write_Trigger();

	RTC_Write(RTC_BBPU, RTC_Read(RTC_BBPU) |
					RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	RTC_Write_Trigger();
}

static bool rtc_lpd_enable_check(void)
{
	RTC_Write(RTC_BBPU, RTC_Read(RTC_BBPU) |
				RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	RTC_Write_Trigger();

	return ((RTC_Read(RTC_AL_SEC) & RTC_LPD_OPT_F32K_CK_ALIVE) ==
				RTC_LPD_OPT_F32K_CK_ALIVE) ? false : true;
}

void rtc_bbpu_power_on(void)
{
	uint16_t bbpu;

	bbpu = RTC_BBPU_KEY | RTC_BBPU_PWREN;
	RTC_Write(RTC_BBPU, bbpu);
	RTC_Write_Trigger();

	rtc_lpsd_solution();

#if RTC_2SEC_REBOOT_ENABLE
	uint16_t pdn2;
	rtc_enable_2sec_reboot();
	pdn2 = RTC_Read(RTC_PDN2) & ~RTC_PDN2_AUTOBOOT;
	RTC_Write(RTC_PDN2, pdn2);
	RTC_Write_Trigger();
#else
	RTC_Write(RTC_AL_SEC, RTC_Read(RTC_AL_SEC) & ~RTC_BBPU_2SEC_EN);
	RTC_Write_Trigger();
#endif
}

void pmic_init_setting(void)
{
	uint16_t i;

#if INIT_SETTING_VERIFIED
	RTC_Config_Interface(PMIC_TMA_KEY_ADDR, 0x9CA7,
				PMIC_TMA_KEY_MASK, PMIC_TMA_KEY_SHIFT);
	for (i = 0; i < ARRAY_SIZE(init_setting); i++)
		RTC_Config_Interface(
			init_setting[i].addr, init_setting[i].val,
			init_setting[i].mask, init_setting[i].shift);

	RTC_Config_Interface(PMIC_TMA_KEY_ADDR, 0,
				PMIC_TMA_KEY_MASK, PMIC_TMA_KEY_SHIFT);

#endif /* INIT_SETTING_VERIFIED */

	/* MT6358 HW tracking init */
	RTC_Config_Interface(0x1B66, 0x1000, 0x7F7F, 0);
	RTC_Config_Interface(0x1B68, 0x6340, 0x7F7F, 0);
	RTC_Config_Interface(0x1B64, 0x6, 0x6, 0);
	RTC_Config_Interface(0x1B6E, 0x1000, 0x7F7F, 0);
	RTC_Config_Interface(0x1B70, 0x6340, 0x7F7F, 0);
	RTC_Config_Interface(0x1B6C, 0x6, 0x6, 0);
}

static bool rtc_eosc_check_clock(uint16_t *result)
{
	if ((result[0] >= 3  &&result[0] <= 7 )&&
			(result[1] < 500) &&
			(result[2] > 2 && result[2] < 9) &&
			(result[3] > 300 && result[3] < 10400))
		return true;
	else
		return false;
}

static bool rtc_hw_init(void)
{
	int begin = 0;

	RTC_Write(RTC_BBPU, RTC_Read(RTC_BBPU) | RTC_BBPU_KEY | RTC_BBPU_INIT);
	RTC_Write_Trigger();

	do {
		RTC_Write(RTC_BBPU, RTC_Read(RTC_BBPU) |
					RTC_BBPU_KEY | RTC_BBPU_RELOAD);
		RTC_Write_Trigger();
		begin++;
		mdelay(1);
	} while ((RTC_Read(RTC_BBPU) & RTC_BBPU_INIT) && begin < 500);

	if (RTC_Read(RTC_BBPU) & RTC_BBPU_INIT) {
		return false;
	}

	return true;
}

static bool rtc_frequency_meter_check(void)
{
	uint16_t  result[4], osc32con, val = 0;
	int begin;

	return true;

	if (rtc_get_recovery_mode_stat())
		rtc_switch_to_xosc_recv_mode();

	rtc_measure_four_clock(result);

	if (rtc_xosc_check_clock(result)) {
		osc32con = RTC_OSC32CON_ANALOG_SETTING;
		rtc_xosc_write(osc32con | 0x7, false);
		return true;
	} else {
		if (!rtc_get_recovery_mode_stat())
			rtc_switch_to_dcxo_mode();
		else
			rtc_switch_to_dcxo_recv_mode();
	}

	begin = 0;
	do {
		if (begin > 1000) {
			return false;
		}
		rtc_measure_four_clock(result);
		mdelay(1);
	} while (!rtc_eosc_check_clock(result));

	val = eosc_cali();
	/* EMB_HW_Mode */
	osc32con = RTC_OSC32CON_ANALOG_SETTING | RTC_REG_XOSC32_ENB;
	val = (val & 0x001f) | osc32con;
	rtc_xosc_write(val, false);

	return true;
}

static bool rtc_android_init(void)
{
	RTC_Write(RTC_IRQ_EN, 0);
	RTC_Write(RTC_CII_EN, 0);
	RTC_Write(RTC_AL_MASK, 0);

	RTC_Write(RTC_AL_YEA, (RTC_Read(RTC_AL_YEA) & (~RTC_AL_YEA_MASK)) |
				((1970 - RTC_MIN_YEAR) & RTC_AL_YEA_MASK));
	RTC_Write(RTC_AL_MTH, 1);
	RTC_Write(RTC_AL_DOM, 1);
	RTC_Write(RTC_AL_DOW, 1);
	RTC_Write(RTC_AL_HOU, RTC_Read(RTC_AL_HOU) & RTC_NEW_SPARE0);
	RTC_Write(RTC_AL_MIN, 0);
	RTC_Write(RTC_AL_SEC, RTC_Read(RTC_AL_SEC) & (~RTC_AL_SEC_MASK));

	RTC_Write(RTC_PDN1, RTC_PDN1_DEBUG);
	RTC_Write(RTC_PDN2, ((1970 - RTC_MIN_YEAR) <<
					RTC_PDN2_PWRON_YEA_SHIFT) | 1);
	RTC_Write(RTC_SPAR0, 0);
	RTC_Write(RTC_SPAR1, (1 << RTC_SPAR1_PWRON_DOM_SHIFT));

	RTC_Write(RTC_DIFF, 0);
	RTC_Write(RTC_CALI, 0);

	if (!rtc_2sec_stat_clear())
		return false;

	if (!RTC_Write_Trigger())
		return false;

	RTC_Read(RTC_IRQ_STA);

	/* init time counters after resetting RTC_DIFF and RTC_CALI */
	RTC_Write(RTC_TC_YEA, RTC_DEFAULT_YEA - RTC_MIN_YEAR);
	RTC_Write(RTC_TC_MTH, RTC_DEFAULT_MTH);
	RTC_Write(RTC_TC_DOM, RTC_DEFAULT_DOM);
	RTC_Write(RTC_TC_DOW, 1);
	RTC_Write(RTC_TC_HOU, 0);
	RTC_Write(RTC_TC_MIN, 0);
	RTC_Write(RTC_TC_SEC, 0);

	if (!RTC_Write_Trigger())
		return false;

	return true;
}

static bool rtc_first_boot_init(uint16_t *result)
{
	/* turn on DCXO */
	rtc_set_xomode(true);
	if (rtc_xosc_check_clock(result))
		rtc_switch_to_xosc_recv_mode();
	else
		rtc_switch_to_dcxo_recv_mode();

	if (!Writeif_unlock()) {
		return false;
	}

	if (!rtc_gpio_init())
		return false;

	/* write powerkeys */
	RTC_Write(RTC_AL_SEC, RTC_Read(RTC_AL_SEC) &
					(~RTC_K_EOSC32_VTCXO_ON_SEL));
	RTC_Write(RTC_AL_YEA, (RTC_Read(RTC_AL_YEA) &
					RTC_AL_YEA_MASK) |
					RTC_K_EOSC_RSV_7 |
					RTC_K_EOSC_RSV_6);
	RTC_Write(RTC_POWERKEY1, RTC_POWERKEY1_KEY);
	RTC_Write(RTC_POWERKEY2, RTC_POWERKEY2_KEY);

	if (!RTC_Write_Trigger()) {
		return false;
	}

	if (!Writeif_unlock()) {
		return false;
	}

	/* disable both XOSC & EOSC LPD */
	RTC_Write(RTC_AL_SEC, RTC_Read(RTC_AL_SEC) | RTC_LPD_OPT_F32K_CK_ALIVE);
	RTC_Write_Trigger();

	if (!rtc_lpd_init())
		return false;

	/* MT6351 need write POWERKEY again to unlock RTC */
	RTC_Write(RTC_POWERKEY1, RTC_POWERKEY1_KEY);
	RTC_Write(RTC_POWERKEY2, RTC_POWERKEY2_KEY);

	if (!RTC_Write_Trigger()) {
		return false;
	}

	if (!Writeif_unlock()) {
		return false;
	}

	if (!rtc_frequency_meter_check())
		return false;

	if (!rtc_lpd_init())
		return false;

	if (!rtc_hw_init())
		return false;

	if (!rtc_android_init())
		return false;

	return true;
}

static bool rtc_init_after_recovery(void)
{
	/* write powerkeys */
	RTC_Write(RTC_POWERKEY1, RTC_POWERKEY1_KEY);
	RTC_Write(RTC_POWERKEY2, RTC_POWERKEY2_KEY);
	if (!RTC_Write_Trigger())
		return false;
	if (!Writeif_unlock())
		return false;
	if (!rtc_gpio_init())
		return false;
	if (!rtc_hw_init())
		return false;
	if (!rtc_android_init())
		return false;
	if (!rtc_lpd_init())
		return false;

	RTC_Write(RTC_POWERKEY1, RTC_POWERKEY1_KEY);
	RTC_Write(RTC_POWERKEY2, RTC_POWERKEY2_KEY);
	if (!RTC_Write_Trigger())
		return false;
	if (!Writeif_unlock())
		return false;

	return true;
}

static bool rtc_recovery_mode_check(void)
{
	/* fix me add return ret for recovery mode check fail */
	if (!rtc_frequency_meter_check())
		return false;

	return true;
}

static void rtc_recovery_flow(void)
{
	uint8_t count = 0;

	rtc_set_recovery_mode_stat(true);
	while (count < 3) {
		if(rtc_recovery_mode_check()) {
			if (rtc_init_after_recovery())
				break;
		}
		count++;
	}
	rtc_set_recovery_mode_stat(false);
	if (count == 3)
		INFO("Impossible to recover!!\n");
}

void rtc_power_on_sequence(void)
{
	uint16_t spar0;
	uint16_t result[4];
	bool check_mode_flag = false;

	RTC_Config_Interface(PMIC_RG_TOP_CKPDN_CON0_CLR, 1,
		PMIC_RG_FQMTR_CK_PDN_MASK, PMIC_RG_FQMTR_CK_PDN_SHIFT);
	RTC_Config_Interface(PMIC_RG_TOP_CKPDN_CON0_CLR, 1,
		PMIC_RG_FQMTR_32K_CK_PDN_MASK, PMIC_RG_FQMTR_32K_CK_PDN_SHIFT);

	/* If EOSC cali is enabled in last power off.
	 * Needing to switch to DCXO clock source,
	 * Or the FQMTR can't measure DCXO clock source.*/
	if ((RTC_Read(RTC_DIFF) & RTC_POWER_DETECTED) &&
			!(RTC_Read(RTC_CON) & RTC_CON_LPSTA_RAW) &&
			(RTC_Read(RTC_POWERKEY1) == RTC_POWERKEY1_KEY &&
			RTC_Read(RTC_POWERKEY2) == RTC_POWERKEY2_KEY))
		rtc_enable_dcxo();

	rtc_measure_four_clock(result);
	if (!rtc_xosc_check_clock(result) && !rtc_eosc_check_clock(result)) {
		check_mode_flag = true;
	}

	if ((RTC_Read(RTC_CON) & RTC_CON_LPSTA_RAW) || check_mode_flag ||
				!(RTC_Read(RTC_DIFF) & RTC_POWER_DETECTED) ||
				!rtc_lpd_enable_check()) {
		if (!rtc_first_boot_init(result)) {
			rtc_recovery_flow();
		}
		RTC_Write(RTC_BBPU, RTC_Read(RTC_BBPU) |
					RTC_BBPU_KEY | RTC_BBPU_RELOAD);
		RTC_Write_Trigger();
	} else {
		/* normally HW reload is done in BROM but check again here */
		RTC_Write(RTC_BBPU, RTC_Read(RTC_BBPU) |
					RTC_BBPU_KEY | RTC_BBPU_RELOAD);
		if (!RTC_Write_Trigger()) {
			rtc_recovery_flow();
		} else {

			if (!Writeif_unlock()) {
				rtc_recovery_flow();
			} else {
				if (RTC_Read(RTC_POWERKEY1) !=
					RTC_POWERKEY1_KEY ||
					RTC_Read(RTC_POWERKEY2) !=
					RTC_POWERKEY2_KEY) {
					if (!rtc_first_boot_init(result)) {
						rtc_recovery_flow();
					}
				} else {
					rtc_osc_init();
				}
			}
		}
		RTC_Write(RTC_BBPU, RTC_Read(RTC_BBPU) |
					RTC_BBPU_KEY | RTC_BBPU_RELOAD);
		RTC_Write_Trigger();
	}

	/* HW K EOSC mode whatever power off (including plug out battery) */
	RTC_Write(RTC_AL_YEA, (RTC_Read(RTC_AL_YEA) | RTC_K_EOSC_RSV_0) &
				(~RTC_K_EOSC_RSV_1) & (~RTC_K_EOSC_RSV_2));
	/* 75k div 2 change to 32k source */
	RTC_Write(PMIC_RG_TOP_CKSEL_CON0_SET,
			RTC_Read(PMIC_RG_TOP_CKSEL_CON0_SET) | RTC_75K_TO_32K);

	RTC_Config_Interface(PMIC_RG_TOP_CKPDN_CON0_SET, 1,
			PMIC_RG_FQMTR_CK_PDN_MASK, PMIC_RG_FQMTR_CK_PDN_SHIFT);

	/* Truning off eosc cali mode clock */
	RTC_Config_Interface(PMIC_RG_SCK_TOP_CKPDN_CON0_SET, 1,
			PMIC_RG_RTC_EOSC32_CK_PDN_MASK,
			PMIC_RG_RTC_EOSC32_CK_PDN_SHIFT);

	/*Reduce the 1V8 driving current to avoid overshoot current*/
	RTC_Config_Interface(PMIC_RG_OCTL_RTC_32K1V8_0_ADDR, 0x8,
			PMIC_RG_OCTL_RTC_32K1V8_0_MASK,
			PMIC_RG_OCTL_RTC_32K1V8_0_SHIFT);
	rtc_clean_mark();
	spar0 = RTC_Read(RTC_SPAR0);
	if (rtc_get_xosc_mode()) {
		RTC_Write(RTC_SPAR0, (spar0 | RTC_SPAR0_32K_LESS));
		rtc_set_xomode(false);
	} else {
		RTC_Write(RTC_SPAR0, (spar0 & ~RTC_SPAR0_32K_LESS));
		rtc_set_xomode(true);
	}
	RTC_Write_Trigger();
	rtc_save_2sec_stat();
	RTC_Write(RTC_BBPU, RTC_Read(RTC_BBPU) |
				RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	RTC_Write_Trigger();

	/*Before enable EOSC LPD, disable and clear both XOSC & EOSC LPD first*/
	RTC_Write(RTC_AL_SEC, RTC_Read(RTC_AL_SEC) | RTC_LPD_OPT_F32K_CK_ALIVE);
	RTC_Write_Trigger();
	if (!rtc_lpd_init()) {
		return;
	}

	/*Enable EOSC LPD*/
	RTC_Write(RTC_AL_SEC, RTC_Read(RTC_AL_SEC) & (~RTC_LPD_OPT_MASK));
	RTC_Write_Trigger();
	RTC_Write(RTC_AL_SEC, RTC_Read(RTC_AL_SEC) | RTC_LPD_OPT_EOSC_LPD);
	RTC_Write_Trigger();

	rtc_boot_check();
	rtc_bbpu_power_on();
	pmic_init_setting();
}
