/*
 * Copyright (c) 2013-2015, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <common/debug.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <mtk_rgu.h>
#include <drivers/delay_timer.h>

#define WDT_CLR_IRQ	0
#define RGU_SW_RESET_DELAY_MS   (100)
#define RGU_SW_RESET_DELAY_CNT  (10)

static void plat_rgu_clear_all(void)
{
#if WDT_CLR_IRQ
	mmio_write_32(MTK_WDT_IRQ_CLR, MTK_WDT_IRQ_CLR_MASK |
			 MTK_WDT_IRQ_CLR_KEY);
#else
	return;
#endif
}

static void plat_rgu_set_time_out_value(unsigned int value)
{
	unsigned int timeout;
	/*
	* TimeOut = BitField 15:5
	* Key	  = BitField  4:0 = 0x08
	*/

	/* sec * 32768 / 512 = sec * 64 = sec * 1 << 6 */
	timeout = value * (1 << 6);
	timeout = timeout << 5;
	mmio_write_32(MTK_WDT_LENGTH, (timeout | MTK_WDT_LENGTH_KEY));
}

/* called in aee_wdt_dump_all_core to deassert RGU's IRQ */
void plat_rgu_deassert_irq(void)
{
	unsigned int tmp;

	mmio_write_32(MTK_WDT_NONRST_REG, mmio_read_32(MTK_WDT_STATUS));

	/* Workaround for clear interrupt design change issue
	 * that all interrupt triggered by RGU, including sspm
	 * spm, etc., would not be cleared. This is just fix
	 * AP side irq issue when level_en is set
	 */
	tmp = mmio_read_32(MTK_WDT_MODE);
	if (tmp & MTK_WDT_MODE_IRQ_LEVEL_EN) {
		tmp &= ~MTK_WDT_MODE_IRQ_LEVEL_EN;
		mmio_write_32(MTK_WDT_MODE, tmp | MTK_WDT_MODE_KEY);
		udelay(70);
		tmp = mmio_read_32(MTK_WDT_MODE);
		tmp |= MTK_WDT_MODE_IRQ_LEVEL_EN;
		mmio_write_32(MTK_WDT_MODE, tmp | MTK_WDT_MODE_KEY);
	} else {
		tmp |= MTK_WDT_MODE_IRQ_LEVEL_EN;
		mmio_write_32(MTK_WDT_MODE, tmp | MTK_WDT_MODE_KEY);
		udelay(70);
		tmp = mmio_read_32(MTK_WDT_MODE);
		tmp &= ~MTK_WDT_MODE_IRQ_LEVEL_EN;
		mmio_write_32(MTK_WDT_MODE, tmp | MTK_WDT_MODE_KEY);
	}
#if WDT_CLR_IRQ
	mmio_write_32(MTK_WDT_IRQ_CLR, MTK_WDT_IRQ_CLR_KEY |
			MTK_WDT_IRQ_CLR_DEASSERT);
#else
	return;
#endif
}

void plat_rgu_request_dis(unsigned int mark_bit)
{
	unsigned int reg = mmio_read_32(MTK_WDT_REQ_MODE);

	if (mark_bit == MTK_WDT_REQ_MODE_EINT) {
		mmio_write_32(MTK_WDT_EXT_REQ_CON,
			mmio_read_32(MTK_WDT_EXT_REQ_CON) & ~MTK_WDT_EXT_EINT_EN);
		reg &= ~MTK_WDT_REQ_MODE_EINT;
	}

	if (mark_bit == MTK_WDT_REQ_MODE_SYSRST) {
		mmio_write_32(MTK_WDT_SYSDBG_DEG_EN1, 0);
		mmio_write_32(MTK_WDT_SYSDBG_DEG_EN2, 0);
		reg &= ~MTK_WDT_REQ_MODE_SYSRST;
	}

	mmio_write_32(MTK_WDT_REQ_MODE, reg | MTK_WDT_REQ_MODE_KEY);
}

int plat_rgu_cfg_dvfsrc(int enable)
{
	unsigned int dbg_ctl, latch;

	dbg_ctl = mmio_read_32(MTK_WDT_DEBUG_CTL2);
	latch = mmio_read_32(MTK_WDT_LATCH_CTL);

	if (enable == 1) {
		/* enable dvfsrc_en */
		dbg_ctl |= MTK_WDT_DEBUG_CTL_DVFSRC_EN;

		/* set dvfsrc_latch */
		latch |= MTK_WDT_LATCH_CTL_DVFSRC;
	} else {
		/* disable is not allowed */
		return -1;
	}

	dbg_ctl |= MTK_WDT_DEBUG_CTL2_KEY;
	mmio_write_32(MTK_WDT_DEBUG_CTL2, dbg_ctl);

	latch |= MTK_WDT_LATCH_CTL_KEY;
	mmio_write_32(MTK_WDT_LATCH_CTL, latch);

	INFO("%s: MTK_WDT_DEBUG_CTL2(0x%x)\n",
		__func__, mmio_read_32(MTK_WDT_DEBUG_CTL2));
	INFO("%s: MTK_WDT_LATCH_CTL(0x%x)\n",
		__func__, mmio_read_32(MTK_WDT_LATCH_CTL));

	return 0;
}

/* called in low power enter/exit function to force WDT stop/start */
void plat_rgu_suspend_notify(void)
{
	unsigned int tmp;

	plat_rgu_clear_all();
	tmp = mmio_read_32(MTK_WDT_MODE);

	/* keep record of wdt is disabled in ATF, and wdt should */
	/* be enabled again depends on this flag when system	 */
	/* resume						 */
	if (tmp & MTK_WDT_MODE_ENABLE)
		mmio_write_32(MTK_WDT_NONRST_REG2,
			mmio_read_32(MTK_WDT_NONRST_REG2) |
			MTK_WDT_NONRST_REG2_SUSPEND_DIS);
	else
		mmio_write_32(MTK_WDT_NONRST_REG2,
			mmio_read_32(MTK_WDT_NONRST_REG2) &
			~MTK_WDT_NONRST_REG2_SUSPEND_DIS);

	tmp &= ~MTK_WDT_MODE_ENABLE;
	mmio_write_32(MTK_WDT_MODE, tmp | MTK_WDT_MODE_KEY);
	mmio_write_32(MTK_WDT_RESTART, MTK_WDT_RESTART_KEY);
}

void plat_rgu_resume_notify(void)
{
	if (mmio_read_32(MTK_WDT_NONRST_REG2) &
	    MTK_WDT_NONRST_REG2_SUSPEND_DIS) {
		unsigned int tmp;

		mmio_write_32(MTK_WDT_NONRST_REG2,
			mmio_read_32(MTK_WDT_NONRST_REG2) &
			~MTK_WDT_NONRST_REG2_SUSPEND_DIS);

		plat_rgu_clear_all();
		tmp = mmio_read_32(MTK_WDT_MODE);
		mmio_write_32(MTK_WDT_MODE, tmp | MTK_WDT_MODE_KEY |
				MTK_WDT_MODE_ENABLE);
		mmio_write_32(MTK_WDT_RESTART, MTK_WDT_RESTART_KEY);
	}
}

void plat_rgu_change_tmo(unsigned int tmo)
{
	unsigned int wdt_status = plat_rgu_read_status();
	/* change watchdog timeout to tmo to avoid */
	/* pmic reset when sysrst/eintrst		*/

	if (wdt_status & (MTK_WDT_STATUS_EINT_RST |
	    MTK_WDT_STATUS_SYSRST_RST)) {
		plat_rgu_set_time_out_value(tmo);
		mmio_write_32(MTK_WDT_RESTART, MTK_WDT_RESTART_KEY);
	}
}

void plat_rgu_sw_reset(unsigned int ddr_rsvd)
{
	unsigned int tmp, reboot_mode, i = 0;

	plat_rgu_dump_reg();

	mmio_write_32(MTK_WDT_RESTART, MTK_WDT_RESTART_KEY);

	plat_rgu_clear_all();
	tmp = mmio_read_32(MTK_WDT_BASE);
	reboot_mode = mmio_read_32(MTK_WDT_NONRST_REG2) &
			MTK_WDT_REBOOT_MODE_MASK;

	/* hwt, kernel panic, and kernel panic die */
	if (ddr_rsvd || (reboot_mode == MTK_WDT_REBOOT_DDR_RSVD)) {
		tmp |= MTK_WDT_MODE_DDR_RESERVE;
		/* plat_rgu_pause_dvfsrc(1); */
	} else {
		tmp &= ~MTK_WDT_MODE_DDR_RESERVE;
		/* plat_rgu_dfd_count_en(0); */
		/* mtk_dbgtop_dram_reserved(0); */
	}

	/* setting of auto restart bit */
	if ((reboot_mode == MTK_WDT_REBOOT_BOOT_DM_VERITY) ||
	    (!reboot_mode)) {
		mmio_write_32(MTK_WDT_NONRST_REG2,
		 mmio_read_32(MTK_WDT_NONRST_REG2) | MTK_WDT_BYPASS_PWR_KEY);
	}

	tmp &= ~(MTK_WDT_MODE_DUAL_MODE | MTK_WDT_MODE_IRQ);
	tmp |= MTK_WDT_MODE_KEY | MTK_WDT_MODE_EXTEN;
	mmio_write_32(MTK_WDT_BASE, tmp);

	plat_rgu_request_dis(MTK_WDT_REQ_MODE_SYSRST);

	mmio_write_32(MTK_WDT_SWRST, MTK_WDT_SWRST_KEY);
	while (1) {
		if (i++ < RGU_SW_RESET_DELAY_CNT) {
			INFO("[RGU-ATF] wait for system reset\n");
			mdelay(RGU_SW_RESET_DELAY_MS);
		}
	}
}

unsigned int plat_rgu_read_status(void)
{
	return mmio_read_32(MTK_WDT_STATUS) | mmio_read_32(MTK_WDT_NONRST_REG);
}

bool plat_rgu_irq_lvl(void)
{
	return ((mmio_read_32(MTK_WDT_MODE) & MTK_WDT_MODE_IRQ_LEVEL_EN) ? true : false);
}

/* allow dump once by one CPU to avoid dummy dump by all other CPUs */

void plat_rgu_dump_reg(void)
{
	unsigned int wdt_status;

	/* dump RGU registers */

	INFO("[RGU-ATF] MODE:0x%x\n", mmio_read_32(MTK_WDT_MODE));
	INFO("[RGU-ATF] LENGTH:0x%x\n", mmio_read_32(MTK_WDT_LENGTH));

	wdt_status = plat_rgu_read_status();

	INFO("[RGU-ATF] STATUS:0x%x\n", wdt_status);
	INFO("[RGU-ATF] INTERVAL:0x%x\n", mmio_read_32(MTK_WDT_INTERVAL));
	INFO("[RGU-ATF] NONRST_REG:0x%x\n", mmio_read_32(MTK_WDT_NONRST_REG));
	INFO("[RGU-ATF] NONRST_REG2:0x%x\n", mmio_read_32(MTK_WDT_NONRST_REG2));
	INFO("[RGU-ATF] REQ_MODE:0x%x\n", mmio_read_32(MTK_WDT_REQ_MODE));
	INFO("[RGU-ATF] REQ_IRQ_EN:0x%x\n", mmio_read_32(MTK_WDT_REQ_IRQ_EN));
	INFO("[RGU-ATF] EXT_REQ_CON:0x%x\n", mmio_read_32(MTK_WDT_EXT_REQ_CON));
	INFO("[RGU-ATF] DEBUG_CTL:0x%x\n", mmio_read_32(MTK_WDT_DEBUG_CTL));
	INFO("[RGU-ATF] LATCH_CTL:0x%x\n", mmio_read_32(MTK_WDT_LATCH_CTL));
	INFO("[RGU-ATF] DEBUG_CTL2:0x%x\n", mmio_read_32(MTK_WDT_DEBUG_CTL2));
	INFO("[RGU-ATF] COUNTER:0x%x\n", mmio_read_32(MTK_WDT_COUNTER));

	/* analyze interrupt source */

	INFO("[RGU-ATF] Status:\n");

	if (wdt_status & MTK_WDT_STATUS_SPM_THERMAL_RST)
		INFO("[RGU-ATF]   SPM_THERMAL\n");

	if (wdt_status & MTK_WDT_STATUS_SPM_RST)
		INFO("[RGU-ATF]   SPM\n");

	if (wdt_status & MTK_WDT_STATUS_EINT_RST)
		INFO("[RGU-ATF]   EINT\n");

	if (wdt_status & MTK_WDT_STATUS_SYSRST_RST)
		INFO("[RGU-ATF]   SYSRST\n");

	if (wdt_status & MTK_WDT_STATUS_DVFSP_RST)
		INFO("[RGU-ATF]   DVFSP\n");

	if (wdt_status & MTK_WDT_STATUS_PMCU_RST)
		INFO("[RGU-ATF]   SSPM\n");

	if (wdt_status & MTK_WDT_STATUS_MDDBG_RST)
		INFO("[RGU-ATF]   MDDBG\n");

	if (wdt_status & MTK_WDT_STATUS_THERMAL_DIRECT_RST)
		INFO("[RGU-ATF]   THERMAL_DIRECT\n");

	if (wdt_status & MTK_WDT_STATUS_DEBUG_RST)
		INFO("[RGU-ATF]   DBG\n");

	if (wdt_status & MTK_WDT_STATUS_SECURITY_RST)
		INFO("[RGU-ATF]   SEC\n");

	if (wdt_status & MTK_WDT_STATUS_IRQ_ASSERT)
		INFO("[RGU-ATF]   IRQ\n");

	if (wdt_status & MTK_WDT_STATUS_SW_WDT_RST)
		INFO("[RGU-ATF]   SW_WDT\n");

	if (wdt_status & MTK_WDT_STATUS_HW_WDT_RST)
		INFO("[RGU-ATF]   HW_WDT\n");
}
