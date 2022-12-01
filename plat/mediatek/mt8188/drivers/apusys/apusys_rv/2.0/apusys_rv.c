/*
 * Copyright (c) 2021, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include <common/debug.h>
#include <drivers/delay_timer.h>
#include <lib/spinlock.h>
#include <lib/xlat_tables/xlat_tables_v2.h>

#include "apusys_rv.h"
#include "apusys_rv_coredump.h"
#include "apusys_rv_mbox_mpu.h"
#include "apusys_rv_reg_map.h"

#if ENABLE_APUSYS_EMI_PROTECTION
#include <emi_mpu.h>
#endif

#ifdef CONFIG_MTK_APUSYS_RV_SECURE_BOOT
#include <mtk_iommu_public.h>
#endif


#define MODULE_TAG "[APUSYS]"

#define UNLOCK				(0)

#define ADDR_MAX			(0xC0000000)
#define APUSYS_MEM_LIMIT		(0x90000000)

#define UP_PRI_NS			(0)
#define UP_NORMAL_NS			(1)

#pragma weak apusys_regdump

/* static variable declaration */
static spinlock_t apusys_rv_lock;
static bool apusys_rv_setup_reviser_called;
static bool apusys_rv_reset_mp_called;
static bool apusys_rv_setup_boot_called;
static bool apusys_rv_start_mp_called;
static bool apusys_rv_stop_mp_called;
static uint64_t apusys_rv_sec_buf_pa;
static uint64_t apusys_rv_sec_buf_iova;
static uint64_t apusys_rv_sec_buf_sz;
static uint64_t apusys_rv_aee_coredump_buf_pa;
static uint64_t apusys_rv_aee_coredump_buf_sz;
static uint64_t apu_img_base_pa;
static uint64_t apu_img_base_sz;

static struct apusys_secure_info_t *apusys_secure_info;
static struct apusys_aee_coredump_info_t *apusys_aee_coredump_info;

static bool is_rcx_mtcmos_on(void)
{
	uint32_t pwr_ready = mmio_read_32(PWR_RDY);
	uint16_t vcore_clk_ctrl = mmio_read_16(VCORE_CLK_CRTL);
	uint32_t rcx_clk_ctrl = mmio_read_32(RCX_CLK_CRTL);

	if (PWR_STATUS(pwr_ready) &&
		vcore_clk_ctrl == 0 &&
		rcx_clk_ctrl == 0)
		return true;

	NOTICE("%s: pwr_ready 0x%x, vcore_clk_ctrl 0x%x, rcx_clk_ctrl 0x%x\n",
		__func__, pwr_ready, vcore_clk_ctrl, rcx_clk_ctrl);

	return false;
}

int apusys_kernel_apusys_rv_setup_reviser(void)
{
	uint32_t boundary = 0;
	uint32_t iommu_en = 1;
	uint32_t code_da = apusys_rv_sec_buf_iova;

	INFO("%s: enter\n", __func__);

	if (apusys_rv_setup_reviser_called) {
		ERROR("%s: only permitted called once\n", __func__);
		return -EPERM;
	}

	if (apusys_rv_sec_buf_iova == 0) {
		ERROR("%s: apusys_rv_sec_buf_iova = 0\n", __func__);
		return -ENOMEM;
	}

	apusys_rv_setup_reviser_called = true;

	spin_lock(&apusys_rv_lock);

	/* setup boundary */
	mmio_write_32(USERFW_CTXT, CFG_4GB_SEL_EN | boundary);
	mmio_write_32(SECUREFW_CTXT, CFG_4GB_SEL_EN | boundary);

	/* setup iommu ctrl(mmu_ctrl | mmu_en) */
	mmio_write_32(UP_IOMMU_CTRL, MMU_CTRL_LOCK | MMU_CTRL |
		(iommu_en << MMU_EN_SHIFT));

	/* setup ns/domain */
	mmio_write_32(UP_NORMAL_DOMAIN_NS, (UP_NORMAL_DOMAIN << UP_DOMAIN_SHIFT) |
		(UP_NORMAL_NS << UP_NS_SHIFT));
	mmio_write_32(UP_PRI_DOMAIN_NS, (UP_PRI_DOMAIN << UP_DOMAIN_SHIFT) |
		(UP_PRI_NS << UP_NS_SHIFT));

	/* setup VDRAM for privilege mode */
	/* vld=1, partial_enable=1, thread_num=1 */
	mmio_write_32(UP_CORE0_VABASE0, VLD | PARTIAL_ENABLE |
		(1 << THREAD_NUM_SHIFT));
	/* for 34 bit mva */
	mmio_write_32(UP_CORE0_MVABASE0, VASIZE_1MB | (code_da >> 2));

	/* setup VDRAM for normal mode */
	/* vld=1, partial_enable=1 */
	mmio_write_32(UP_CORE0_VABASE1, VLD | PARTIAL_ENABLE |
		(0 << THREAD_NUM_SHIFT));
	/* for 34 bit mva */
	mmio_write_32(UP_CORE0_MVABASE1, VASIZE_1MB | (code_da >> 2));

#ifdef CONFIG_MTK_APUSYS_RV_MNOC_OST_DBG
	mmio_write_32(APU_NOC_MNI_RCX + 0x16c, 0x8080);
	mmio_write_32(APU_NOC_MNI_RCX + 0x1ac, 0xffffffff);
#endif

	spin_unlock(&apusys_rv_lock);

	return 0;
}

int apusys_kernel_apusys_rv_reset_mp(void)
{
	INFO("%s: enter\n", __func__);

	if (apusys_rv_reset_mp_called) {
		ERROR("%s: only permitted called once\n", __func__);
		return -EPERM;
	}

	apusys_rv_reset_mp_called = true;

	spin_lock(&apusys_rv_lock);

	/* reset uP */
	mmio_write_32(MD32_SYS_CTRL, 0x0);

	udelay(10);

	/* enable IOMMU only(iommu_tr_en = 1/acp_en = 0) */
	mmio_write_32(MD32_SYS_CTRL, MD32_G2B_CG_EN | MD32_DBG_EN |
		MD32_DM_AWUSER_IOMMU_EN | MD32_DM_ARUSER_IOMMU_EN |
		MD32_PM_AWUSER_IOMMU_EN | MD32_PM_ARUSER_IOMMU_EN |
		MD32_SOFT_RSTN);

	/* md32 clk enable */
	mmio_write_32(MD32_CLK_EN, 0x1);
	/* set up_wake_host_mask0 for wdt irq */
	mmio_write_32(UP_WAKE_HOST_MASK0, WDT_IRQ_EN);
	/* set up_wake_host_mask1 for mbox irq */
	mmio_write_32(UP_WAKE_HOST_MASK1, MBOX0_IRQ_EN | MBOX1_IRQ_EN |
		MBOX2_IRQ_EN);

	spin_unlock(&apusys_rv_lock);

	return 0;
}

int apusys_kernel_apusys_rv_setup_boot(void)
{
	uint32_t code_da = apusys_rv_sec_buf_iova;

	INFO("%s: enter\n", __func__);

	if (apusys_rv_setup_boot_called) {
		ERROR("%s: only permitted called once\n", __func__);
		return -EPERM;
	}

	if (apusys_rv_sec_buf_iova == 0) {
		ERROR("%s: apusys_rv_sec_buf_iova = 0\n", __func__);
		return -ENOMEM;
	}

	apusys_rv_setup_boot_called = true;

	spin_lock(&apusys_rv_lock);

	mmio_write_32(MD32_BOOT_CTRL, code_da);

	/* set predefined MPU region for cache access */
	mmio_write_32(MD32_PRE_DEFINE, (PREDEFINE_CACHE_TCM << PREDEF_1G_OFS) |
		(PREDEFINE_CACHE << PREDEF_2G_OFS) |
		(PREDEFINE_CACHE << PREDEF_3G_OFS) |
		(PREDEFINE_CACHE << PREDEF_4G_OFS));

	spin_unlock(&apusys_rv_lock);

	return 0;
}

int apusys_kernel_apusys_rv_start_mp(void)
{
	INFO("%s: enter\n", __func__);

	if (apusys_rv_start_mp_called) {
		ERROR("%s: only permitted called once\n", __func__);
		return -EPERM;
	}

	apusys_rv_start_mp_called = true;

	spin_lock(&apusys_rv_lock);

	/* release runstall */
	mmio_write_32(MD32_RUNSTALL, 0x0);

	spin_unlock(&apusys_rv_lock);

	return 0;
}

int apusys_kernel_apusys_rv_stop_mp(void)
{
	INFO("%s: enter\n", __func__);

	if (apusys_rv_stop_mp_called) {
		ERROR("%s: only permitted called once\n", __func__);
		return -EPERM;
	}

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		return -EPERM;
	}

	apusys_rv_stop_mp_called = true;

	spin_lock(&apusys_rv_lock);

	/* hold runstall */
	mmio_write_32(MD32_RUNSTALL, 0x1);

	spin_unlock(&apusys_rv_lock);

	return 0;
}

static int is_valid_pa_dram_range(uint64_t addr, uint64_t size)
{
	uint64_t res_mem_start = APUSYS_RESERVED_MEM_START;
	uint64_t res_mem_size  = APUSYS_RESERVED_MEM_SZ;

	return (addr >= res_mem_start &&
		addr < (res_mem_start + res_mem_size) &&
		(addr + size) < (res_mem_start + res_mem_size));
}

int apusys_rv_mbox_mpu_init(void)
{
	int i;

	for (i = 0; i < APU_MBOX_NUM; i++) {
		mmio_write_32(APU_MBOX_FUNC_CFG(i),
			MBOX_CTRL_LOCK |
			(mbox_mpu_setting_tab[i].no_mpu << MBOX_NO_MPU_SHIFT));
		mmio_write_32(APU_MBOX_DOMAIN_CFG(i),
		MBOX_CTRL_LOCK |
		(mbox_mpu_setting_tab[i].rx_ns << MBOX_RX_NS_SHIFT) |
		(mbox_mpu_setting_tab[i].rx_domain << MBOX_RX_DOMAIN_SHIFT) |
		(mbox_mpu_setting_tab[i].tx_ns << MBOX_TX_NS_SHIFT) |
		(mbox_mpu_setting_tab[i].tx_domain << MBOX_TX_DOMAIN_SHIFT));
	}

	return 0;
}

/*
 * apusys_rv driver initialization (from bootloader)
 *
 * initialize static variables
 *
 */
int apusys_rv_init(void)
{
	INFO("%s: enter\n", __func__);

	apusys_rv_setup_reviser_called = 0;
	apusys_rv_reset_mp_called = 0;
	apusys_rv_setup_boot_called = 0;
	apusys_rv_start_mp_called = 0;
	apusys_rv_stop_mp_called = 0;

	apusys_rv_sec_buf_pa = 0;
	apusys_rv_sec_buf_iova = 0;
	apusys_rv_sec_buf_sz = 0;
	apusys_rv_aee_coredump_buf_pa = 0;
	apusys_rv_aee_coredump_buf_sz = 0;

	apusys_secure_info = NULL;
	apusys_aee_coredump_info = NULL;

	return 0;
}

int apusys_kernel_apusys_rv_disable_wdt_isr(void)
{
	INFO("%s: enter\n", __func__);

	spin_lock(&apusys_rv_lock);
	/* disable apu wdt */
	mmio_write_32(WDT_CTRL0, mmio_read_32(WDT_CTRL0) & (~WDT_EN));
	spin_unlock(&apusys_rv_lock);

	return 0;
}

int apusys_kernel_apusys_rv_clear_wdt_isr(void)
{
	INFO("%s: enter\n", __func__);

	spin_lock(&apusys_rv_lock);
	/* clear bit 31 to disable debug APB access */
	mmio_clrbits_32(UP_INT_EN2, DBG_APB_EN);
	/* clear wdt interrupt */
	mmio_write_32(WDT_INT, 0x1);
	spin_unlock(&apusys_rv_lock);

	return 0;
}

#ifdef CONFIG_MTK_APUSYS_RV_MNOC_OST_DBG
#define DUMP_REG(addr) INFO("%s: 0x%x = 0x%x\n", __func__, addr, \
							mmio_read_32(addr))
#define DBG_DUMP_TIMES (3)
#define MD32_DBG_SEL_MAX (22)

static void dump_slave_handshake_staus(uint32_t offset)
{
	mmio_write_32(APU_NOC_MNI_RCX + offset, 0x1);
	DUMP_REG(APU_NOC_MNI_RCX + offset);
	DUMP_REG(APU_NOC_MNI_RCX + offset + 0x54);
	DUMP_REG(APU_NOC_MNI_RCX + offset + 0x58);
}

static void dump_dbg_status(void)
{
	int i;

	for (i = 0; i < DBG_DUMP_TIMES; i++) {
		DUMP_REG(MD32_MON_PC);
		udelay(1);
	}

	DUMP_REG(MD32_CLK_EN);
	DUMP_REG(APU_NOC_MNI_RCX + 0x16c);
	DUMP_REG(APU_NOC_MNI_RCX + 0x1ac);
	DUMP_REG(APU_NOC_MNI_RCX + 0x100);

	for (i = 0; i < DBG_DUMP_TIMES; i++) {
		INFO("==========================\n");
		DUMP_REG(APU_NOC_MNI_RCX + 0x1c4);
		DUMP_REG(APU_NOC_MNI_RCX + 0x1f8);
		DUMP_REG(APU_NOC_MNI_RCX + 0x4);
		DUMP_REG(APU_NOC_MNI_RCX + 0x8);
		DUMP_REG(APU_NOC_MNI_RCX + 0xc);

		DUMP_REG(APU_RCX_VCORE_CONFIG + 0x6c);
		DUMP_REG(APU_RCX_VCORE_CONFIG + 0x70);
		udelay(1);
	}

	/* check DRAM slv0 status */
	dump_slave_handshake_staus(0x700);
	/* check DRAM slv1 status */
	dump_slave_handshake_staus(0x800);
	/* check uP slv status */
	dump_slave_handshake_staus(0x900);
	/* check logger slv status */
	dump_slave_handshake_staus(0xa00);

	/* check MGALS slv status */
	mmio_write_32(APU_MGALS + 0x390, 0x84);
	DUMP_REG(APU_MGALS + 0x390);
	DUMP_REG(APU_MGALS + 0x370);

	DUMP_REG(APU_NOC_MNI_RCX + 0x1e8);
	DUMP_REG(APU_NOC_MNI_RCX + 0x1d4);
	DUMP_REG(APU_NOC_MNI_RCX + 0x1f4);
	DUMP_REG(APU_NOC_MNI_RCX + 0x3c4);
	DUMP_REG(APU_NOC_MNI_RCX + 0x3e8);

	mmio_write_32(APU_NOC_MNI_RCX + 0x100, 0x700001);
	DUMP_REG(APU_NOC_MNI_RCX + 0x100);
	DUMP_REG(APU_NOC_MNI_RCX + 0x1f0);

	for (i = 0; i <= MD32_DBG_SEL_MAX; i++) {
		mmio_write_32(APU_MD32_SYSCTRL + 0x98, 0xff00);
		mmio_setbits_32(APU_MD32_SYSCTRL + 0x98, i);
		INFO("%s: i = %d\n", __func__, i);
		DUMP_REG(APU_MD32_SYSCTRL + 0x98);
		DUMP_REG(APU_MD32_SYSCTRL + 0x9c);
	}

}
#endif

int apusys_kernel_apusys_rv_cg_gating(void)
{
	INFO("%s: enter\n", __func__);

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		return -EPERM;
	}

#ifdef CONFIG_MTK_APUSYS_RV_MNOC_OST_DBG
	dump_dbg_status();
#endif

	spin_lock(&apusys_rv_lock);
	mmio_write_32(MD32_CLK_EN, 0x0);
	spin_unlock(&apusys_rv_lock);

	return 0;
}

int apusys_kernel_apusys_rv_cg_ungating(void)
{
	INFO("%s: enter\n", __func__);

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		return -EPERM;
	}

	spin_lock(&apusys_rv_lock);
	mmio_write_32(MD32_CLK_EN, 0x1);
	spin_unlock(&apusys_rv_lock);

	return 0;
}

int apusys_kernel_apusys_rv_coredump_shadow_copy(void)
{
	INFO("%s: enter\n", __func__);

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		return -EPERM;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		return -ENOMEM;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		return -ENOMEM;
	}

	if (apusys_rv_sec_buf_pa == 0) {
		ERROR("%s: apusys_rv_sec_buf_pa == 0\n", __func__);
		return -ENOMEM;
	}

	if (apusys_secure_info == NULL) {
		ERROR("%s: apusys_secure_info == NULL\n", __func__);
		return -ENOMEM;
	}

	memcpy((void *) (apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->up_coredump_ofs),
		(void *) apusys_rv_sec_buf_pa +
		apusys_secure_info->up_coredump_ofs,
		sizeof(struct apu_coredump));

	return 0;
}

int apusys_kernel_apusys_rv_tcmdump(void)
{
	INFO("%s: enter\n", __func__);

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		return -EPERM;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		return -ENOMEM;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		return -ENOMEM;
	}

	if (!is_rcx_mtcmos_on()) {
		ERROR("%s: rcx not on\n", __func__);
		return -EPERM;
	}

	memcpy((void *) (apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->up_coredump_ofs +
		offsetof(struct apu_coredump, tcmdump)),
		(void *) APU_MD32_TCM, TCM_SIZE);

	return 0;
}

int apusys_kernel_apusys_rv_ramdump(void)
{
	INFO("%s: enter\n", __func__);

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		return -EPERM;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		return -ENOMEM;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		return -ENOMEM;
	}

	if (apusys_rv_sec_buf_pa == 0) {
		ERROR("%s: apusys_rv_sec_buf_pa == 0\n", __func__);
		return -ENOMEM;
	}

	if (apusys_secure_info == NULL) {
		ERROR("%s: apusys_secure_info == NULL\n", __func__);
		return -ENOMEM;
	}

	memcpy((void *) (apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->up_coredump_ofs +
		offsetof(struct apu_coredump, ramdump)),
		(void *) (apusys_rv_sec_buf_pa +
		apusys_secure_info->up_code_buf_ofs + TCM_SIZE),
		DRAM_DUMP_SIZE);

	return 0;
}

int apusys_kernel_apusys_rv_tbufdump(void)
{
	int i, j;
	uint32_t tbuf_cur_ptr;
	uint32_t tbuf_dump[TBUF_SIZE/sizeof(uint32_t)];

	INFO("%s: enter\n", __func__);

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		return -EPERM;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		return -ENOMEM;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		return -ENOMEM;
	}

	if (!is_rcx_mtcmos_on()) {
		ERROR("%s: rcx not on\n", __func__);
		return -EPERM;
	}

	tbuf_cur_ptr = ((mmio_read_32(MD32_STATUS) >> MD32_TBUF_ADDR_SHIFT) &
							MD32_TBUF_ADDR_MASK);
	for (i = 0; i < NR_TBUF_DBG_DATA*2; i++) {
		spin_lock(&apusys_rv_lock);
		mmio_write_32(MD32_TBUF_DBG_SEL, tbuf_cur_ptr);
		spin_unlock(&apusys_rv_lock);
		if (tbuf_cur_ptr > 0)
			tbuf_cur_ptr--;
		else
			tbuf_cur_ptr = NR_TBUF_DBG_DATA*2 - 1;
		for (j = 0; j < NR_TBUF_DBG_DATA; j++) {
			tbuf_dump[i*NR_TBUF_DBG_DATA + j] =
				mmio_read_32(MD32_TBUF_DBG_DAT3 -
							j * NR_TBUF_DBG_DATA);
		}
	}
	memcpy((void *) (apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->up_coredump_ofs +
		offsetof(struct apu_coredump, tbufdump)),
		tbuf_dump, sizeof(tbuf_dump));

	return 0;
}

int apusys_kernel_apusys_rv_cachedump(void)
{
	int offset;
	struct apu_coredump *coredump;

	INFO("%s: enter\n", __func__);

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		return -EPERM;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		return -ENOMEM;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		return -ENOMEM;
	}

	if (!is_rcx_mtcmos_on()) {
		ERROR("%s: rcx not on\n", __func__);
		return -EPERM;
	}

	coredump = (struct apu_coredump *) (apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->up_coredump_ofs);

	spin_lock(&apusys_rv_lock);
	/* set APU_UP_SYS_DBG_EN for cache dump enable through normal APB */
	mmio_write_32(MD32_DBG_BUS_SEL,
		mmio_read_32(MD32_DBG_BUS_SEL) | APU_UP_SYS_DBG_EN);
	spin_unlock(&apusys_rv_lock);

	for (offset = 0; offset < CACHE_DUMP_SIZE/sizeof(uint32_t); offset++)
		coredump->cachedump[offset] =
			mmio_read_32(APU_CACHE_DUMP + offset*sizeof(uint32_t));

	spin_lock(&apusys_rv_lock);
	/* clear APU_UP_SYS_DBG_EN */
	mmio_write_32(MD32_DBG_BUS_SEL,
		mmio_read_32(MD32_DBG_BUS_SEL) & ~(APU_UP_SYS_DBG_EN));
	spin_unlock(&apusys_rv_lock);

	return 0;
}

int apusys_regdump(uint64_t dst_pa, unsigned int buffer_size)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_regdump(void)
{
	uint64_t regdump_pa = 0;
	int ret = 0;

	INFO("%s: enter\n", __func__);

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		return -ENOMEM;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		return -ENOMEM;
	}

	regdump_pa = apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->regdump_ofs;

	ret = apusys_regdump(regdump_pa, apusys_aee_coredump_info->regdump_sz);

	return ret;
}

static void dbg_apb_dw(uint32_t dbg_reg, uint32_t val)
{
	spin_lock(&apusys_rv_lock);

	mmio_write_32(MD32_DBG_APB_INSTR, dbg_reg);
	mmio_write_32(MD32_DBG_APB_INSTR_WR, 0x1);
	mmio_write_32(MD32_DBG_APB_WDATA, val);
	mmio_write_32(MD32_DBG_APB_WDATA_WR, 0x1);

	spin_unlock(&apusys_rv_lock);
}

static void dbg_apb_iw(uint32_t dbg_cmd)
{
	spin_lock(&apusys_rv_lock);

	mmio_write_32(MD32_DBG_APB_INSTR, dbg_cmd);
	mmio_write_32(MD32_DBG_APB_INSTR_WR, 0x1);

	spin_unlock(&apusys_rv_lock);
}

static uint32_t dbg_apb_dr(uint32_t dbg_reg)
{
	uint32_t ret;

	spin_lock(&apusys_rv_lock);

	mmio_write_32(MD32_DBG_APB_INSTR, dbg_reg);
	mmio_write_32(MD32_DBG_APB_INSTR_WR, 0x1);
	ret = mmio_read_32(MD32_DBG_APB_RDATA);

	spin_unlock(&apusys_rv_lock);

	return ret;
}

static uint32_t dbg_read_csr(uint32_t csr_id)
{
	dbg_apb_dw(DBG_INSTR_REG_INSTR, CSR_ID_CMD | (csr_id << CSR_ID_SHIFT));
	dbg_apb_iw(DBG_EXECUTE_INSTR);
	dbg_apb_dw(DBG_INSTR_REG_INSTR, CSR_READ_CMD);
	dbg_apb_iw(DBG_EXECUTE_INSTR);

	return dbg_apb_dr(DBG_DATA_REG_INSTR);
}

int apusys_kernel_apusys_rv_dbg_apb_attach(void)
{
	int status;
	unsigned int retry;

	INFO("%s: enter\n", __func__);

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		return -EPERM;
	}

	if (!is_rcx_mtcmos_on()) {
		ERROR("%s: rcx not on\n", __func__);
		return -EPERM;
	}

	spin_lock(&apusys_rv_lock);
	/* set bit 31 to enable debug APB access */
	mmio_setbits_32(UP_INT_EN2, DBG_APB_EN);
	/* set DBG_EN */
	mmio_write_32(MD32_DBG_APB_EN, 0x1);
	/* set DBG MODE */
	mmio_write_32(MD32_DBG_APB_MODE, 0x0);
	spin_unlock(&apusys_rv_lock);
	/* ATTACH */
	dbg_apb_iw(DBG_ATTACH_INSTR);
	/* REQUEST */
	dbg_apb_iw(DBG_REQUEST_INSTR);
	/* Read DBG STATUS Register */
	status = dbg_apb_dr(DBG_STATUS_REG_INSTR);

	INFO("%s: status = 0x%x\n", __func__, status);

	retry = 0;
	/* Check if RV33 go into DEBUG mode */
	while (retry++ < RETRY_CNT_MAX) {
		status = dbg_apb_dr(DBG_STATUS_REG_INSTR);
		if (status & 0x1)
			return status;
		udelay(1);
	}

	ERROR("%s: timeout\n", __func__);

	return status;
}

int apusys_kernel_apusys_rv_regdump(uint32_t dbg_apb_status)
{
	int i;
	uint32_t reg_dump[REG_SIZE/sizeof(uint32_t)] = {0};
	uint32_t val;
	struct apu_coredump *coredump;

	INFO("%s: enter\n", __func__);

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		return -EPERM;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		return -ENOMEM;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		return -ENOMEM;
	}

	if (!is_rcx_mtcmos_on()) {
		ERROR("%s: rcx not on\n", __func__);
		return -EPERM;
	}

	coredump = (struct apu_coredump *) (apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->up_coredump_ofs);

	reg_dump[IDX_REG_DUMP_SIZE] = REG_SIZE; /* reg dump size */
	reg_dump[IDX_REG_DUMP_GPR_LR] = mmio_read_32(MD32_MON_LR);
	reg_dump[IDX_REG_DUMP_GPR_SP] = mmio_read_32(MD32_MON_SP);
	reg_dump[IDX_REG_DUMP_PC] = mmio_read_32(MD32_MON_PC);
	reg_dump[IDX_REG_DUMP_SP] = mmio_read_32(MD32_MON_SP);

	if (dbg_apb_status != 1)
		goto exit;

	/* Read GPRs */
	for (i = 1; i < NUM_GPR_REG; i++) {
		val = GPR_READ_CMD | ((uint32_t) i << GPR_ID_SHIFT);
		dbg_apb_dw(DBG_INSTR_REG_INSTR, val);
		dbg_apb_iw(DBG_EXECUTE_INSTR);
		reg_dump[i] = dbg_apb_dr(DBG_DATA_REG_INSTR);
	}

	/* Read CSRs */
	for (i = 0 ; i < REG_SIZE/sizeof(uint32_t) - IDX_CSR_DUMP_START; i++)
		reg_dump[i + IDX_CSR_DUMP_START] = dbg_read_csr(rv_reg_map[i]);

exit:
	spin_lock(&apusys_rv_lock);
	/* clear bit 31 to disable debug APB access */
	mmio_clrbits_32(UP_INT_EN2, DBG_APB_EN);
	spin_unlock(&apusys_rv_lock);
	memcpy(coredump->regdump, reg_dump, sizeof(reg_dump));

	return 0;
}

/*
 * apusys_rv load apusys image (from kernel)
 *
 * 1. load apusys image
 *
 */
int apusys_kernel_apusys_rv_load_apu_img(uint64_t apu_secure_info_pa)
{
	unsigned int apusys_pmsize, apusys_xsize;
	void *apusys_pmimg, *apusys_ximg;
	void *tmp_addr;
	void *img;
	struct ptimg_hdr_t *hdr;
	unsigned int *img_size;
	struct apusys_secure_info_t *sec_info =
			(struct apusys_secure_info_t *)apu_secure_info_pa;

	INFO("%s: start apusys_rv_load_fw\n", __func__);
	/* initialize apusys sec_info */
	tmp_addr = sec_info->up_code_buf_ofs + sec_info->up_code_buf_sz +
				(void *)apusys_rv_sec_buf_pa +
				ROUNDUP(sizeof(*sec_info), APUSYS_FW_ALIGN);
//return apu_secure_info_pa;

	INFO("tmp_addr is 0x%llx\n", (uint64_t)tmp_addr);
	INFO("sec_info ROUNDUP is 0x%lx\n",
		ROUNDUP(sizeof(*sec_info), APUSYS_FW_ALIGN));
	INFO("up_code_buf_ofs is 0x%x, up_code_buf_sz is 0x%x\n",
		sec_info->up_code_buf_ofs, sec_info->up_code_buf_sz);
	INFO("total_sz is 0x%x, up_fw_ofs is 0x%x\n",
		sec_info->total_sz, sec_info->up_fw_ofs);

	/* separate ptimg */
	apusys_pmimg = apusys_ximg = NULL;
	apusys_pmsize = apusys_xsize = 0;
	hdr = (void *)apu_img_base_pa + (0x200);
	INFO("%s: hdr->magic is 0x%x\n", __func__, hdr->magic);
	img_size = (void *)apu_img_base_pa + (0x4);
	INFO("%s: apu_partition_sz is 0x%x\n", __func__, *img_size );
	memcpy(tmp_addr, ((void *)apu_img_base_pa + (0x200)), (*img_size));

	hdr = tmp_addr;

	while (hdr->magic == PT_MAGIC) {
		img = ((void *) hdr) + hdr->hdr_size;
		INFO("Rhdr->hdr_size= 0x%x\n", hdr->hdr_size);
		INFO("img address is 0x%llx\n", (uint64_t)img);

		switch (hdr->id) {
		case PT_ID_APUSYS_FW:
			INFO("PT_ID_APUSYS_FW\n");
			apusys_pmimg = img;
			apusys_pmsize = hdr->img_size;
			sec_info->up_fw_ofs = ((uint64_t)img -
						(uint64_t)apusys_rv_sec_buf_pa);
			sec_info->up_fw_sz = apusys_pmsize;
			INFO("up_fw_ofs = 0x%x, up_fw_sz = 0x%x\n",
				sec_info->up_fw_ofs, sec_info->up_fw_sz);
			break;
		case PT_ID_APUSYS_XFILE:
			INFO("PT_ID_APUSYS_XFILE\n");
			apusys_ximg = img;
			apusys_xsize = hdr->img_size;
			sec_info->up_xfile_ofs =
				(uint64_t)hdr - (uint64_t)apusys_rv_sec_buf_pa;
			sec_info->up_xfile_sz = hdr->hdr_size + apusys_xsize;
			INFO("up_xfile_ofs = 0x%x, up_xfile_sz = 0x%x\n",
				sec_info->up_xfile_ofs, sec_info->up_xfile_sz);
			break;
		case PT_ID_MDLA_FW_BOOT:
			INFO("PT_ID_MDLA_FW_BOOT\n");
			sec_info->mdla_fw_boot_ofs =
				(uint64_t)img - (uint64_t)apusys_rv_sec_buf_pa;
			sec_info->mdla_fw_boot_sz = hdr->img_size;
			INFO("mdla_fw_boot_ofs = 0x%x, mdla_fw_boot_sz = 0x%x\n",
				sec_info->mdla_fw_boot_ofs,
				sec_info->mdla_fw_boot_sz);
			break;
		case PT_ID_MDLA_FW_MAIN:
			INFO("PT_ID_MDLA_FW_MAIN\n");
			sec_info->mdla_fw_main_ofs =
				(uint64_t)img - (uint64_t)apusys_rv_sec_buf_pa;
			sec_info->mdla_fw_main_sz = hdr->img_size;
			INFO("mdla_fw_main_ofs = 0x%x, mdla_fw_main_sz = 0x%x\n",
				sec_info->mdla_fw_main_ofs,
				sec_info->mdla_fw_main_sz);
			break;
		case PT_ID_MDLA_XFILE:
			INFO("PT_ID_MDLA_XFILE\n");
			sec_info->mdla_xfile_ofs =
				(uint64_t)hdr - (uint64_t)apusys_rv_sec_buf_pa;
			sec_info->mdla_xfile_sz = hdr->hdr_size + hdr->img_size;
			INFO("mdla_xfile_ofs = 0x%x, mdla_xfile_sz = 0x%x\n",
				sec_info->mdla_xfile_ofs,
				sec_info->mdla_xfile_sz);
			break;
		case PT_ID_MVPU_FW:
			INFO("PT_ID_MVPU_FW\n");
			sec_info->mvpu_fw_ofs =
				(uint64_t)img - (uint64_t)apusys_rv_sec_buf_pa;
			sec_info->mvpu_fw_sz = hdr->img_size;
			INFO("mvpu_fw_ofs = 0x%x, mvpu_fw_sz = 0x%x\n",
				sec_info->mvpu_fw_ofs, sec_info->mvpu_fw_sz);
			break;
		case PT_ID_MVPU_XFILE:
			INFO("PT_ID_MVPU_XFILE\n");
			sec_info->mvpu_xfile_ofs =
				(uint64_t)hdr - (uint64_t)apusys_rv_sec_buf_pa;
			sec_info->mvpu_xfile_sz = hdr->hdr_size + hdr->img_size;
			INFO("mvpu_xfile_ofs = 0x%x, mvpu_xfile_sz = 0x%x\n",
				sec_info->mvpu_xfile_ofs,
				sec_info->mvpu_xfile_sz);
			break;
		case PT_ID_MVPU_SEC_FW:
			INFO("PT_ID_MVPU_SEC_FW\n");
			sec_info->mvpu_sec_fw_ofs =
				(uint64_t)img - (uint64_t)apusys_rv_sec_buf_pa;
			sec_info->mvpu_sec_fw_sz = hdr->img_size;
			INFO("mvpu_sec_fw_ofs = 0x%x, mvpu_sec_fw_sz = 0x%x\n",
				sec_info->mvpu_sec_fw_ofs,
				sec_info->mvpu_sec_fw_sz);
			break;
		case PT_ID_MVPU_SEC_XFILE:
			INFO("PT_ID_MVPU_SEC_XFILE\n");
			sec_info->mvpu_sec_xfile_ofs =
				(uint64_t)hdr - (uint64_t)apusys_rv_sec_buf_pa;
			sec_info->mvpu_sec_xfile_sz =
						hdr->hdr_size + hdr->img_size;
			INFO("mvpu_sec_xfile_ofs = 0x%x, mvpu_sec_xfile_sz = 0x%x\n",
				sec_info->mvpu_sec_xfile_ofs,
				sec_info->mvpu_sec_xfile_sz);
			break;
		default:
			ERROR("Warning: Ignore unknown APUSYS image_%d\n",
				hdr->id);
			break;
		}

		INFO("hdr->img_size = 0x%x, ROUNDUP(hdr->img_size, hdr->align) = 0x%x\n",
			hdr->img_size, ROUNDUP(hdr->img_size, hdr->align));
		img += ROUNDUP(hdr->img_size, hdr->align);
		hdr = (struct ptimg_hdr_t *) img;
	}

	if (!apusys_pmimg || !apusys_ximg) {
		ERROR("APUSYS partition missing - PM:0x%llx, XM:0x%llx (@0x%llx)\n",
			(uint64_t)apusys_pmimg,
			(uint64_t)apusys_ximg, (uint64_t)tmp_addr);
		return -ENOENT;
	}

	INFO("%s APUSYS part load finished: PM:0x%llx(up_fw_ofs = 0x%x), XM:0x%llx(up_xfile_ofs = 0x%x)\n",
		__func__, (uint64_t)apusys_pmimg, sec_info->up_fw_ofs,
		(uint64_t)apusys_ximg, sec_info->up_xfile_ofs);


	/* copy uP firmware to code buffer */
	memcpy((void *) (apusys_rv_sec_buf_pa + sec_info->up_code_buf_ofs),
		(void *) (apusys_rv_sec_buf_pa + sec_info->up_fw_ofs),
		sec_info->up_fw_sz);
	INFO("copy uP firmware to code buffer\n");

	/* initialize apusys sec_info */
	memcpy((void *)(sec_info->up_code_buf_ofs +
			sec_info->up_code_buf_sz + apusys_rv_sec_buf_pa),
		(void *)apu_secure_info_pa, sizeof(*sec_info));
	INFO("initialize apusys sec_info\n");
	INFO("apusys_kernel_apusys_rv_load_apu_img end\n");

	flush_dcache_range(apu_secure_info_pa, sizeof(*sec_info));
	flush_dcache_range(apusys_rv_sec_buf_pa, apusys_rv_sec_buf_sz);

	return 0;
}

int apusys_rv_ns_mem_emi_protect_en(uint64_t pa, uint32_t size)
{
#if ENABLE_APUSYS_EMI_PROTECTION
    /*
     * setup EMI MPU
     * domain 0: APMCU
     * domain 5: APUSYS
     */
    struct emi_region_info_t region_info;

    if (!is_valid_pa_dram_range(pa, size)) {
	    ERROR("%s: invalid addr(0x%llx), size(0x%llx)\n",
		    __func__, pa, size);
	    return -EINVAL;
    }

    region_info.start = (unsigned long long) pa;
    region_info.end = (unsigned long long) (pa + size) - 1;
    region_info.region = APUSYS_NS_FW_EMI_REGION;

    SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, SEC_RW_NSEC_R);

    return emi_mpu_set_protection(&region_info);
#else
    return 0;
#endif
}

/*
 * apusys_rv get apusys.sig.img for kernel
 *
 *  dynamic map secure apusys.sig.img memory
 *
 */
int apusys_kernel_apusys_rv_setup_apu_img_mem(uint64_t addr, uint64_t size)
{
	int ret;

	if (!is_valid_pa_dram_range(addr, size)) {
		ERROR("%s: invalid addr(0x%llx), size(0x%llx)\n",
			__func__, addr, size);
		return -EINVAL;
	}

	apu_img_base_pa = addr;
	apu_img_base_sz = size;

	INFO("%s: addr = 0x%llx, size = 0x%llx\n", __func__, addr, size);

	/* create apusys img mapping */
	ret = mmap_add_dynamic_region((unsigned long long)addr,
				(uintptr_t)addr,
				(size_t)round_up(size, PAGE_SIZE),
				(unsigned int)MT_MEMORY | MT_RW | MT_SECURE);
	if (ret) {
		ERROR("%s: mmap_add_dynamic_region() fail, ret=0x%x\n",
			__func__, ret);
		return ret;
	}

	apusys_rv_ns_mem_emi_protect_en(addr, size);

	return 0;
}

static int apusys_rv_sec_mem_emi_protect_en(uint64_t pa, uint32_t size)
{
#if ENABLE_APUSYS_EMI_PROTECTION
    /*
     * setup EMI MPU
     * domain 0: APMCU
     * domain 5: APUSYS
     */
    struct emi_region_info_t region_info;

    if (!is_valid_pa_dram_range(pa, size)) {
	    ERROR("%s: invalid addr(0x%llx), size(0x%llx)\n",
		    __func__, pa, size);
	    return -EINVAL;
    }

    region_info.start = (unsigned long long) pa;
    region_info.end = (unsigned long long) (pa + size) - 1;
    region_info.region = APUSYS_SEC_FW_EMI_REGION;

    SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
                          FORBIDDEN, FORBIDDEN, SEC_RW,    FORBIDDEN,
                          FORBIDDEN, FORBIDDEN, FORBIDDEN, SEC_RW);

    return emi_mpu_set_protection(&region_info);
#else
    return 0;
#endif
}

/*
 * apusys_rv secure memory mapping (from kernel)
 *
 * 1. dynamic map secure memory
 * 2. get iova of secure memory from iommu mapping
 *
 */
int apusys_kernel_apusys_rv_setup_secure_mem(uint64_t addr, uint64_t size)
{
	int ret;

	if (!is_valid_pa_dram_range(addr, size)) {
		ERROR("%s: invalid addr(0x%llx), size(0x%llx)\n",
			__func__, addr, size);
		return -EINVAL;
	}

	apusys_rv_sec_buf_pa = addr;
	apusys_rv_sec_buf_sz = size;

	INFO("%s: addr = 0x%llx, size = 0x%llx, realsize = 0x%lx\n",
		__func__, addr, size,
		(size_t)round_up(size + APUSYS_IOVA_PAGE, PAGE_SIZE));

	/* create mapping */
	ret = mmap_add_dynamic_region((unsigned long long)addr,
			(uintptr_t)addr,
			(size_t)round_up(size + APUSYS_IOVA_PAGE, PAGE_SIZE),
			(unsigned int)MT_MEMORY | MT_RW | MT_SECURE);
	if (ret) {
		ERROR("%s: mmap_add_dynamic_region() fail, ret=0x%x\n",
			__func__, ret);
		return ret;
	}

	apusys_secure_info = (struct apusys_secure_info_t *)
						(addr + CODE_BUF_SIZE);

#ifdef CONFIG_MTK_APUSYS_RV_SECURE_BOOT
	/* get iova */

	INFO("%s: GET SECURE IOVA\n", __func__);
	apusys_rv_sec_buf_iova = mtk_iommu_linear_secure_map(addr,
		size, SECURE_MEM, MOD_APU_FW);

	if (apusys_rv_sec_buf_iova == 0) {
		ERROR("%s: Fail to map iova, addr(0x%llx), size(0x%llx)\n",
			__func__, addr, size);
		mmap_remove_dynamic_region((uintptr_t)addr, (size_t)size);
		return -ENOMEM;
	}
#else
	apusys_rv_sec_buf_iova = 0x200000;
#endif

	apusys_rv_sec_mem_emi_protect_en(addr, size);

	return 0;
}

/*
 * apusys_rv aee_coredump memory mapping (from kernel)
 *
 * dynamic map aee_coredump memory
 *
 */
int apusys_kernel_apusys_rv_setup_aee_coredump_mem(uint64_t addr, uint64_t size)
{
	int ret;

	if (!is_valid_pa_dram_range(addr, size)) {
		ERROR("%s: invalid addr(0x%llx), size(0x%llx)\n",
			__func__, addr, size);
		return -EINVAL;
	}

	apusys_rv_aee_coredump_buf_pa = addr;
	apusys_rv_aee_coredump_buf_sz = size;

	INFO("%s: addr = 0x%llx, size = 0x%llx\n",
		__func__, addr, size);

	/* create mapping */
	ret = mmap_add_dynamic_region((unsigned long long)addr, /* PA */
				(uintptr_t)addr, /* VA */
				(size_t)round_up(size, PAGE_SIZE), /* size */
				(unsigned int)MT_MEMORY | MT_RW | MT_NS); /* attrs */
	if (ret) {
		ERROR("%s: mmap_add_dynamic_region() fail, ret=0x%x\n",
			__func__, ret);
		return ret;
	}

	apusys_aee_coredump_info = (struct apusys_aee_coredump_info_t *) addr;

	return 0;
}

/*
 * apusys_rv initialize_aee_coredump (from kernel)
 *
 * initialize_aee_coredump memory
 *
 */
int apusys_kernel_apusys_rv_initialize_aee_coredump_buf(uint64_t regdump_buf_sz)
{
	struct apusys_aee_coredump_info_t * aee_coredump_info =
		(struct apusys_aee_coredump_info_t *) \
			apusys_rv_aee_coredump_buf_pa;

	aee_coredump_info->up_coredump_ofs = sizeof(*aee_coredump_info);
	aee_coredump_info->up_coredump_sz = apusys_secure_info->up_coredump_sz;
	INFO("up_coredump_sz is 0x%x\n", apusys_secure_info->up_coredump_sz);
	aee_coredump_info->regdump_ofs =
				aee_coredump_info->up_coredump_ofs +
				aee_coredump_info->up_coredump_sz;
	aee_coredump_info->regdump_sz = regdump_buf_sz;
	aee_coredump_info->mdla_coredump_ofs =
				aee_coredump_info->regdump_ofs +
				aee_coredump_info->regdump_sz;
	aee_coredump_info->mdla_coredump_sz =
				apusys_secure_info->mdla_coredump_sz;
	aee_coredump_info->mvpu_coredump_ofs =
				aee_coredump_info->mdla_coredump_ofs +
				aee_coredump_info->mdla_coredump_sz;
	aee_coredump_info->mvpu_coredump_sz =
				apusys_secure_info->mvpu_coredump_sz;
	aee_coredump_info->mvpu_sec_coredump_ofs =
				aee_coredump_info->mvpu_coredump_ofs +
				aee_coredump_info->mvpu_coredump_sz;
	aee_coredump_info->mvpu_sec_coredump_sz =
				apusys_secure_info->mvpu_sec_coredump_sz;

	aee_coredump_info->up_xfile_ofs =
				aee_coredump_info->mvpu_sec_coredump_ofs +
				aee_coredump_info->mvpu_sec_coredump_sz;
	aee_coredump_info->up_xfile_sz = apusys_secure_info->up_xfile_sz;
	aee_coredump_info->mdla_xfile_ofs =
				aee_coredump_info->up_xfile_ofs +
				aee_coredump_info->up_xfile_sz;
	aee_coredump_info->mdla_xfile_sz = apusys_secure_info->mdla_xfile_sz;
	aee_coredump_info->mvpu_xfile_ofs =
				aee_coredump_info->mdla_xfile_ofs +
				aee_coredump_info->mdla_xfile_sz;
	aee_coredump_info->mvpu_xfile_sz = apusys_secure_info->mvpu_xfile_sz;
	aee_coredump_info->mvpu_sec_xfile_ofs =
				aee_coredump_info->mvpu_xfile_ofs +
				aee_coredump_info->mvpu_xfile_sz;
	aee_coredump_info->mvpu_sec_xfile_sz =
				apusys_secure_info->mvpu_sec_xfile_sz;

	INFO("copy uP xfile to aee_coredump buffer\n");
	/* copy uP xfile to aee_coredump buffer */
	memcpy((void *)(apusys_rv_aee_coredump_buf_pa +
				aee_coredump_info->up_xfile_ofs),
	       (void *)(apusys_rv_sec_buf_pa +
				apusys_secure_info->up_xfile_ofs),
	       apusys_secure_info->up_xfile_sz);

	/* copy mdla xfile to aee_coredump buffer */
	INFO("copy mdla xfile to aee_coredump buffer\n");
	memcpy((void *)(apusys_rv_aee_coredump_buf_pa +
			aee_coredump_info->mdla_xfile_ofs),
	       (void *)(apusys_rv_sec_buf_pa +
			apusys_secure_info->mdla_xfile_ofs),
	       apusys_secure_info->mdla_xfile_sz);

	/* copy mvpu xfile to aee_coredump buffer */
	INFO("copy mvpu xfile to aee_coredump buffer\n");
	memcpy((void *)(apusys_rv_aee_coredump_buf_pa +
			aee_coredump_info->mvpu_xfile_ofs),
	       (void *)(apusys_rv_sec_buf_pa +
			apusys_secure_info->mvpu_xfile_ofs),
		apusys_secure_info->mvpu_xfile_sz);

	/* copy mvpu_sec xfile to aee_coredump buffer */
	INFO("copy mvpu_sec xfile to aee_coredump buffer\n");
	memcpy((void *)(apusys_rv_aee_coredump_buf_pa +
			aee_coredump_info->mvpu_sec_xfile_ofs),
	       (void *)(apusys_rv_sec_buf_pa +
			apusys_secure_info->mvpu_sec_xfile_ofs),
	       apusys_secure_info->mvpu_sec_xfile_sz);

	if (apusys_aee_coredump_info->up_xfile_sz == 0) {
		ERROR("%s: apusys_aee_coredump_info->up_xfile_sz == 0\n",
			__func__);
		mmap_remove_dynamic_region(
			(uintptr_t)apusys_rv_aee_coredump_buf_pa,
			(size_t)apusys_rv_aee_coredump_buf_sz);

		return -EINVAL;
	}
	INFO("%s: apusys_aee_coredump_info = 0x%p, up_coredump_ofs = 0x%x\n",
		__func__, apusys_aee_coredump_info,
		apusys_aee_coredump_info->up_coredump_ofs);
	INFO("%s: up_xfile_ofs = 0x%x, up_xfile_sz = 0x%x\n",
		__func__, apusys_aee_coredump_info->up_xfile_ofs,
		apusys_aee_coredump_info->up_xfile_sz);

	return 0;
}

