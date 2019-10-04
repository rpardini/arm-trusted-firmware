/*
 * Copyright (c) 2015, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <common/debug.h>
#include <drivers/delay_timer.h>

#include <mt8516_def.h>
#include <pmic_wrap_init.h>
#include <rtc.h>

/* RTC busy status polling interval and retry count */
enum {
	RTC_WRTGR_POLLING_DELAY_MS	= 10,
	RTC_WRTGR_POLLING_CNT		= 100
};

static uint16_t RTC_Read(uint32_t addr)
{
	uint32_t rdata = 0;

	pwrap_read((uint32_t)addr, &rdata);
	return (uint16_t)rdata;
}

static void RTC_Write(uint32_t addr, uint16_t data)
{
	pwrap_write((uint32_t)addr, (uint32_t)data);
}

static inline int32_t rtc_busy_wait(void)
{
	uint64_t retry = RTC_WRTGR_POLLING_CNT;

	do {
		mdelay(RTC_WRTGR_POLLING_DELAY_MS);
		if (!(RTC_Read(RTC_BBPU) & RTC_BBPU_CBUSY))
			return 1;
		retry--;
	} while (retry);

	ERROR("[RTC] rtc cbusy time out!\n");
	return 0;
}

static int32_t Write_trigger(void)
{
	RTC_Write(RTC_WRTGR, 1);
	return rtc_busy_wait();
}

static int32_t Writeif_unlock(void)
{
	RTC_Write(RTC_PROT, RTC_PROT_UNLOCK1);
	if (!Write_trigger())
		return 0;
	RTC_Write(RTC_PROT, RTC_PROT_UNLOCK2);
	if (!Write_trigger())
		return 0;

	return 1;
}

#define RTC_POWERKEY1		0x8028
#define RTC_POWERKEY2		0x802a
#define RTC_POWERKEY1_KEY	0xa357
#define RTC_POWERKEY2_KEY	0x67d2
#define RTC_CALI_BBPU_2SEC_EN		(1U << 8)
#define RTC_CALI_BBPU_2SEC_MODE_SHIFT	9
#define RTC_CALI_BBPU_2SEC_MODE_MSK	(3U << RTC_CALI_BBPU_2SEC_MODE_SHIFT)
#define RTC_CALI_BBPU_2SEC_STAT		(1U << 11)

void rtc_bbpu_power_up(void)
{
	uint16_t bbpu;
	uint16_t cali;

	/* pull PWRBB high */
	bbpu = RTC_BBPU_KEY | RTC_BBPU_BBPU | RTC_BBPU_AUTO | RTC_BBPU_PWREN;
	if (Writeif_unlock()) {

		RTC_Write(RTC_POWERKEY1, RTC_POWERKEY1_KEY);
		RTC_Write(RTC_POWERKEY2, RTC_POWERKEY2_KEY);
		if (!Write_trigger())
			assert(0);

		RTC_Write(RTC_BBPU, bbpu);
		if (!Write_trigger())
			assert(0);

		cali = RTC_Read(RTC_CALI);
		RTC_Write(RTC_CALI, cali & ~RTC_CALI_BBPU_2SEC_EN);
	} else {
		assert(0);
	}
}

void rtc_bbpu_power_down(void)
{
	uint16_t bbpu;

	/* pull PWRBB low */
	bbpu = RTC_BBPU_KEY | RTC_BBPU_AUTO | RTC_BBPU_PWREN;
	if (Writeif_unlock()) {
		RTC_Write(RTC_BBPU, bbpu);
		if (!Write_trigger())
			assert(0);
	} else {
		assert(0);
	}
}
