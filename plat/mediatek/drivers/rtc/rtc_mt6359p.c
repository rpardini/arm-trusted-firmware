/*
 * Copyright (c) 2020-2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <lib/mtk_init/mtk_init.h>
#include <common/debug.h>
#include <drivers/delay_timer.h>
#include <rtc.h>

static bool s_recovery_flag = false;
static bool g_rtc_2sec_stat;

static void RTC_Config_Interface(uint32_t addr, uint16_t data,
			    uint16_t mask, uint16_t shift)
{
	uint16_t pmic_reg;

	pmic_reg = RTC_Read(addr);

	pmic_reg &= ~(mask << shift);
	pmic_reg |= (data << shift);

	RTC_Write(addr, pmic_reg);
}

static int32_t rtc_disable_2sec_reboot(void)
{
	uint16_t reboot;

	reboot = (RTC_Read(RTC_AL_SEC) & ~RTC_BBPU_2SEC_EN) &
		 ~RTC_BBPU_AUTO_PDN_SEL;
	RTC_Write(RTC_AL_SEC, reboot);

	return RTC_Write_Trigger();
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

static int32_t rtc_enable_k_eosc(void)
{
	uint16_t alm_dow, alm_sec;
	int16_t ret;

	/* Turning on eosc cali mode clock */
	RTC_Config_Interface(PMIC_RG_SCK_TOP_CKPDN_CON0_CLR, 1,
			PMIC_RG_RTC_EOSC32_CK_PDN_MASK,
			PMIC_RG_RTC_EOSC32_CK_PDN_SHIFT);

	alm_sec = RTC_Read(RTC_AL_SEC) & (~RTC_LPD_OPT_MASK);
	RTC_Write(RTC_AL_SEC, alm_sec);
	ret = RTC_Write_Trigger();
	if (ret == 0) {
		return 0;
	}

	RTC_Write(RTC_CON, RTC_LPD_EN);
	ret = RTC_Write_Trigger();
	if (ret == 0) {
		return 0;
	}

	RTC_Write(RTC_CON, RTC_LPD_RST);
	ret = RTC_Write_Trigger();
	if (ret == 0) {
		return 0;
	}

	RTC_Write(RTC_CON, RTC_LPD_EN);
	ret = RTC_Write_Trigger();
	if (ret == 0) {
		return 0;
	}

	RTC_Write(RTC_POWERKEY1, RTC_POWERKEY1_KEY);
	RTC_Write(RTC_POWERKEY2, RTC_POWERKEY2_KEY);
	ret = RTC_Write_Trigger();
	if (ret == 0) {
		return 0;
	}

	/* set RTC EOSC calibration period = 8sec */
	alm_dow = (RTC_Read(RTC_AL_DOW) & (~RTC_RG_EOSC_CALI_TD_MASK)) |
		  RTC_RG_EOSC_CALI_TD_8SEC;
	RTC_Write(RTC_AL_DOW, alm_dow);
	ret = RTC_Write_Trigger();
	if (ret == 0) {
		return 0;
	}

	RTC_Write(RTC_BBPU,
		  RTC_Read(RTC_BBPU) | RTC_BBPU_KEY | RTC_BBPU_RELOAD);
	ret = RTC_Write_Trigger();
	if (ret == 0) {
		return 0;
	}

	/* Enable K EOSC mode :use solution1 of eosc cali to fix mt6359p 32K*/
	RTC_Write(RTC_AL_YEA, (((RTC_Read(RTC_AL_YEA) | RTC_K_EOSC_RSV_0)
				& (~RTC_K_EOSC_RSV_1)) | (RTC_K_EOSC_RSV_2)));
	ret = RTC_Write_Trigger();
	if (ret == 0) {
		return 0;
	}

	INFO("[RTC] RTC_enable_k_eosc\n");

	return 1;
}

void rtc_power_off_sequence(void)
{
	uint16_t bbpu;
	int16_t ret;

	ret = rtc_disable_2sec_reboot();
	if (ret == 0) {
		return;
	}

	ret = rtc_enable_k_eosc();
	if (ret == 0) {
		return;
	}

	bbpu = RTC_BBPU_KEY | RTC_BBPU_PWREN;

	if (Writeif_unlock() != 0) {
		RTC_Write(RTC_BBPU,
			  bbpu | RTC_BBPU_RESET_ALARM | RTC_BBPU_RESET_SPAR);
		RTC_Write(RTC_AL_MASK, RTC_AL_MASK_DOW);
		ret = RTC_Write_Trigger();
		if (ret == 0) {
			return;
		}
		mdelay(1);

		bbpu = RTC_Read(RTC_BBPU);

		if (((bbpu & RTC_BBPU_RESET_ALARM) > 0) ||
		    ((bbpu & RTC_BBPU_RESET_SPAR) > 0)) {
			INFO("[RTC] timeout\n");
		}

		bbpu = RTC_Read(RTC_BBPU) | RTC_BBPU_KEY | RTC_BBPU_RELOAD;
		RTC_Write(RTC_BBPU, bbpu);
		ret = RTC_Write_Trigger();
		if (ret == 0) {
			return;
		}
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

	/*
	 * RTC_FQMTR_RST = 1, reset FQMTR
	 * RTC_FQMTR_RST = 0, release FQMTR
	 */
	RTC_Write(PMIC_SCK_TOP_RST_CON0, RTC_Read(PMIC_SCK_TOP_RST_CON0) | RTC_FQMTR_RST);
	udelay(20);
	RTC_Write(PMIC_SCK_TOP_RST_CON0, RTC_Read(PMIC_SCK_TOP_RST_CON0) & ~RTC_FQMTR_RST);

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
	RTC_Config_Interface(PMIC_RG_DRV_CON0, 0x8,
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
}

int rtc_power_on()
{
	rtc_power_on_sequence();
	return 0;
}

MTK_PLAT_SETUP_0_INIT(rtc_power_on);
