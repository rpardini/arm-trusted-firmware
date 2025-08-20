/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <common/debug.h>
#include <drivers/delay_timer.h>
#include <lib/mtk_init/mtk_init.h>
#include <lib/spinlock.h>
#include <lib/xlat_tables/xlat_tables_v2.h>

#if defined(CONFIG_MTK_DEVINFO)
#include <drivers/chip_id.h>
#endif
#include <drivers/apusys_rv_public.h>
#include <drivers/iommu/mtk_iommu_public.h>
#include <mtk_sip_svc.h>
#include <mtk_mmap_pool.h>

#include "apusys_rv.h"
#include "apusys_rv_pwr_ctl.h"
#include "apusys_rv_coredump.h"
#include "apusys_rv_reg_map.h"
#ifdef CONFIG_MTK_APUSYS_PWR
#include "apusys_power.h"
#endif

#ifdef CONFIG_MTK_APUSYS_SEC_CTRL
#include "apusys_security_ctrl_perm.h"
#endif

#ifdef CONFIG_MTK_APUSYS_RV_APUMMU_SUPPORT
#include "apusys_ammu.h"
#endif

#ifdef CONFIG_MTK_APUSYS_RV_CUSTOM_MBOX_MPU
#include "apusys_rv_custom_mbox_mpu.h"
#else
#include "apusys_rv_mbox_mpu.h"
#endif

#define MODULE_TAG "[APUSYS]"
#define APU_SECURE_FW_IOVA (0x200000)

#pragma weak apusys_regdump
#pragma weak apu_infra_dcm_setup
#pragma weak apusys_ce_debug_regdump
#pragma weak apusys_ce_sram_dump
#pragma weak apusys_kernel_apusys_ce_regdump
#pragma weak apusys_kernel_apusys_ce_mask_init
#pragma weak apusys_kernel_apusys_ce_reg_write


/* static variable declaration */
static spinlock_t apusys_rv_lock;
static bool apusys_rv_setup_reviser_called;
static bool apusys_rv_setup_apummu_called;
static bool apusys_rv_reset_mp_called;
static bool apusys_rv_setup_boot_called;
static bool apusys_rv_start_mp_called;
static bool apusys_rv_stop_mp_called;
static uint64_t apusys_rv_sec_buf_pa;
static uint64_t apusys_rv_sec_buf_iova;
static uint64_t apusys_rv_sec_buf_sz;
static uint64_t apusys_rv_aee_coredump_buf_pa;
static uint64_t apusys_rv_aee_coredump_buf_sz;
static uint64_t apusys_rv_tcm_log_buf_pa;
static uint64_t apusys_rv_tcm_log_buf_sz;


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

int apu_infra_dcm_setup(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);

	return -EOPNOTSUPP;
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
		(1 << UP_NS_SHIFT));
	mmio_write_32(UP_PRI_DOMAIN_NS, (UP_PRI_DOMAIN << UP_DOMAIN_SHIFT) |
		(0 << UP_NS_SHIFT));

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

	spin_unlock(&apusys_rv_lock);

	return 0;
}

int apusys_kernel_apusys_rv_setup_apummu(void)
{
#ifdef CONFIG_MTK_APUSYS_RV_APUMMU_SUPPORT
	uint32_t apummu_tcm_sz_select = 0;
#endif

	if (apusys_rv_setup_apummu_called) {
		ERROR("%s: only permitted called once\n", __func__);
		return -EPERM;
	}

	apusys_rv_setup_apummu_called = true;

#ifdef CONFIG_MTK_APUSYS_SEC_CTRL
	sec_set_rv_dns();
#endif

	/* todo: call apummu provided setup API */
#ifdef CONFIG_MTK_APUSYS_RV_APUMMU_SUPPORT
	if (APU_MD32_TCM_SZ <= 0x20000)
		apummu_tcm_sz_select = APUMMU_PAGE_LEN_128KB;
	else if (APU_MD32_TCM_SZ <= 0x40000)
		apummu_tcm_sz_select = APUMMU_PAGE_LEN_256KB;
	else if (APU_MD32_TCM_SZ <= 0x80000)
		apummu_tcm_sz_select = APUMMU_PAGE_LEN_512KB;
	else if (APU_MD32_TCM_SZ <= 0x100000)
		apummu_tcm_sz_select = APUMMU_PAGE_LEN_1MB;
	else {
		ERROR("%s: APU_MD32_TCM_SZ = 0x%x > 1MB", __func__, APU_MD32_TCM_SZ);
		return -EINVAL;
	}

	INFO("%s: apummu_tcm_sz_select = %u\n", __func__, apummu_tcm_sz_select);
	rv_boot(apusys_rv_sec_buf_iova, 0, 0, APUMMU_PAGE_LEN_1MB,
		APU_MD32_TCM, apummu_tcm_sz_select);
#endif
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

	dsb();
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

	INFO("%s: apusys_kernel_apusys_rv_setup_boot = 0x%x, enter\n",
		__func__, code_da);

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

	#if defined(CONFIG_MTK_DEVINFO)
		if (mt_get_chip_sw_ver() == CHIP_VER_E2)
			apu_infra_dcm_setup();
	#endif

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
	int ret = 0;

	INFO("%s: enter\n", __func__);

	if (apusys_rv_stop_mp_called) {
		ERROR("%s: only permitted called once\n", __func__);
		return -EPERM;
	}

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		ret = -EPERM;
		goto out;
	}

	apusys_rv_stop_mp_called = true;

	spin_lock(&apusys_rv_lock);

	/* hold runstall */
	mmio_write_32(MD32_RUNSTALL, 0x1);

	spin_unlock(&apusys_rv_lock);

out:
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return ret;
}

static int is_valid_apusys_aretop_reg_range(unsigned int addr, unsigned int size)
{
#if CONFIG_MTK_APUSYS_PWR
	return (addr >= APU_ARETOP_VALID_START &&
		(addr + size) < APU_ARETOP_VALID_END);
#else
	return 1;
#endif
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

		INFO("%s: 0x%x = 0x%x, 0x%x = 0x%x\n", __func__,
			APU_MBOX_FUNC_CFG(i), mmio_read_32(APU_MBOX_FUNC_CFG(i)),
			APU_MBOX_DOMAIN_CFG(i), mmio_read_32(APU_MBOX_DOMAIN_CFG(i)));
	}

#ifdef APU_MBOX_DBG_EN
	//bypass mbox register Dump for secure master
	mmio_write_32(APU_MBOX_DBG_EN, 1);
#endif

	return 0;
}

int apusys_setup_ce_bin(void)
{
	struct ce_main_hdr_t *ce_main_hdr;
	struct ce_sub_hdr_t *ce_sub_hdr;
	unsigned int cnt, i, reg_val;

	if (apusys_secure_info == NULL) {
		ERROR("%s: invalid para\n", __func__);
		return -ENOMEM;
	}

	ce_main_hdr = (struct ce_main_hdr_t *)(apusys_rv_sec_buf_pa +
		(uint64_t)apusys_secure_info->ce_bin_ofs);
	ce_sub_hdr = (struct ce_sub_hdr_t *)((uint64_t)ce_main_hdr +
		(uint64_t)ce_main_hdr->hdr_size);

	if (ce_main_hdr->magic != CE_MAIN_MAGIC) {
		ERROR("%s: invalid header\n", __func__);
		return -EINVAL;
	}

	cnt = 0;

	while (ce_sub_hdr->magic == CE_SUB_MAGIC &&
		cnt < ce_main_hdr->bin_count) {
		INFO("%s: job (%d), magic (0x%x)\n", __func__,
			ce_sub_hdr->ce_enum, ce_sub_hdr->magic);

		if (!is_valid_apusys_aretop_reg_range(ce_sub_hdr->mem_st,
			ce_sub_hdr->bin_size)) {
			ERROR("%s: invalid job %d, mem_st(0x%x), size(0x%x)\n",
				__func__, ce_sub_hdr->ce_enum, ce_sub_hdr->mem_st,
				ce_sub_hdr->bin_size);
			return -EINVAL;
		}

		for (i = 0; i < ce_sub_hdr->bin_size; i += 4) {
			reg_val = *(unsigned int *)((uint64_t)ce_sub_hdr +
				(uint64_t)(ce_sub_hdr->bin_offset + i));
			mmio_write_32(ce_sub_hdr->mem_st + i, reg_val);
		}

		if (ce_sub_hdr->hw_entry) {
			if (!is_valid_apusys_aretop_reg_range(ce_sub_hdr->hw_entry, 4)) {
				ERROR("%s: invalid job %d, mem_st(0x%x)\n",
					__func__, ce_sub_hdr->ce_enum, ce_sub_hdr->hw_entry);
				return -EINVAL;
			}
			reg_val = mmio_read_32(ce_sub_hdr->hw_entry);
			reg_val = (reg_val &
				~(ce_sub_hdr->hw_entry_mask << ce_sub_hdr->hw_entry_bit))
				| ((ce_sub_hdr->hw_entry_val & ce_sub_hdr->hw_entry_mask)
				<< ce_sub_hdr->hw_entry_bit);
			mmio_write_32(ce_sub_hdr->hw_entry, reg_val);
			INFO("%s: HW_ENTRY(0x%x) = 0x%x\n", __func__,
				ce_sub_hdr->hw_entry, reg_val);
		}

		ce_sub_hdr = (struct ce_sub_hdr_t *)((uint64_t)ce_sub_hdr +
			(uint64_t)ce_sub_hdr->bin_offset + (uint64_t)ce_sub_hdr->bin_size);
		cnt++;
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
	apusys_rv_setup_apummu_called = 0;
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

/*
 * apusys_rv secure memory mapping (from bootloader)
 *
 * 1. dynamic map secure memory
 * 2. get pa of secure memory
 * 3. iommu iova mapping (only for sp product line iommu)
 */
int apusys_rv_setup_secure_mem(uint64_t addr, uint64_t size)
{
	int ret;
	int mmap_size;

	apusys_rv_sec_buf_pa = addr;
	apusys_rv_sec_buf_sz = size;

	INFO("%s: addr = 0x%" PRIx64 ", size = 0x%" PRIx64 "\n",
		__func__, addr, size);

	apusys_secure_info = (struct apusys_secure_info_t *) (addr + CODE_BUF_SIZE);
	INFO("%s: apusys_secure_info = 0x%p, total_sz = 0x%x\n",
		__func__, apusys_secure_info, apusys_secure_info->total_sz);
	INFO("%s: up_coredump_ofs = 0x%x, up_coredump_sz = 0x%x\n",
		__func__, apusys_secure_info->up_coredump_ofs,
		apusys_secure_info->up_coredump_sz);

#ifdef CONFIG_MTK_APUSYS_RV_SECURE_BOOT_MT8XXX
	return 0;
#endif

#ifdef CONFIG_MTK_APUSYS_RV_SECURE_BOOT
	/* get iova */
	apusys_rv_sec_buf_iova = mtk_iommu_linear_secure_map(addr,
		size, SECURE_MEM, MOD_APU_FW);

	if (apusys_rv_sec_buf_iova == 0) {
		ERROR("%s: Fail to map iova, addr(0x%" PRIx64 "), size(0x%" PRIx64 ")\n",
			__func__, addr, size);
		mmap_remove_dynamic_region((uintptr_t)addr, (size_t)size);
		return -ENOMEM;
	}
#else
	apusys_rv_sec_buf_iova = APU_SECURE_FW_IOVA;
#endif

	return 0;
}

/*
 * apusys_rv secure iova mapping (from kernel)
 *
 * This func is used to compatible w/ MT8xxx iommu. Set the cfg
 * `CONFIG_MTK_APUSYS_RV_SECURE_BOOT_MT8XXX :=y` in tfa to support this func.
 * `mtk_iommu_linear_secure_map()` func need apu is power on to setup apu-iommu
 * reg, and apu kernel driver will use smc call to do this job.
 */
int apusys_kernel_apusys_map_secure_iova(void)
{
	uint64_t addr, size;

#ifndef CONFIG_MTK_APUSYS_RV_SECURE_BOOT_MT8XXX
	ERROR("%s: not supported", __func__);
	return -EOPNOTSUPP;
#endif


	addr = apusys_rv_sec_buf_pa;
	size = apusys_rv_sec_buf_sz;

	INFO("%s: addr = 0x%" PRIx64 ", size = 0x%" PRIx64 "\n",
		__func__, addr, size);

#ifdef CONFIG_MTK_APUSYS_RV_SECURE_BOOT
	/* get iova */
	apusys_rv_sec_buf_iova = mtk_iommu_linear_secure_map(addr,
		size, SECURE_MEM, MOD_APU_FW);

	if (apusys_rv_sec_buf_iova == 0) {
		ERROR("%s: Fail to map iova, addr(0x%lx), size(0x%lx)\n",
			__func__, addr, size);
		return -ENOMEM;
	}
#endif

	return 0;
}

/*
 * apusys_rv aee_coredump memory mapping (from bootloader)
 *
 * dynamic map aee_coredump memory
 *
 */
int apusys_rv_setup_aee_coredump_mem(uint64_t addr, uint64_t size)
{
	int ret;

	apusys_rv_aee_coredump_buf_pa = addr;
	apusys_rv_aee_coredump_buf_sz = size;

	INFO("%s: addr = 0x%" PRIx64 ", size = 0x%" PRIx64 "\n",
		__func__, addr, size);

	apusys_aee_coredump_info = (struct apusys_aee_coredump_info_t *) addr;

#ifdef CONFIG_MTK_APUSYS_KERNEL_LOAD_IMAGE
	apusys_aee_coredump_info->up_xfile_sz = apusys_secure_info->up_xfile_sz;
#endif

	if (apusys_aee_coredump_info->up_xfile_sz == 0) {
		ERROR("%s: apusys_aee_coredump_info->up_xfile_sz == 0\n", __func__);
		mmap_remove_dynamic_region((uintptr_t)addr, (size_t)size);
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

/*
 * apusys_rv tcm log memory mapping (from kernel)
 *
 * dynamic map tcm log memory
 *
 */
int apusys_kernel_apusys_rv_setup_tcm_log_mem(uint64_t addr, uint64_t size)
{
	int ret;

	ERROR("%s: addr = 0x%" PRIx64 ", size = 0x%" PRIx64 "\n",
		__func__, addr, size);

	if (size < APUSYS_TCM_LP_LOG_SIZE + APUSYS_TCM_LP_LOG_HEADER_SIZE) {
		ERROR("%s: size %lu must >= %d\n", __func__, size,
			APUSYS_TCM_LP_LOG_SIZE + APUSYS_TCM_LP_LOG_HEADER_SIZE);
		return -EINVAL;
	}

	apusys_rv_tcm_log_buf_pa = addr;
	apusys_rv_tcm_log_buf_sz = size;


	/* create mapping */
	ret = mmap_add_dynamic_region((unsigned long long)addr, /* PA */
				(uintptr_t)addr, /* VA */
				(size_t)round_up(size, PAGE_SIZE), /* size */
				(unsigned int)MT_MEMORY | MT_RW | MT_NS); /* attrs */
	if (ret) {
		ERROR("%s: mmap_add_dynamic_region() fail, ret=0x%x\n", __func__, ret);
		return ret;
	}

	return 0;
}

int apusys_kernel_apusys_rv_disable_wdt_isr(void)
{
	int ret = 0;

	INFO("%s: enter\n", __func__);

	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, 0, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout (rcx off)\n", __func__);
		return ret;
	}

	spin_lock(&apusys_rv_lock);
	/* disable apu wdt */
	mmio_write_32(WDT_CTRL0, mmio_read_32(WDT_CTRL0) & (~WDT_EN));
	spin_unlock(&apusys_rv_lock);

	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, 0, 0);
	if (ret) {
		ERROR("%s: sem release timeout\n", __func__);
		return ret;
	}

	return 0;
}

int apusys_kernel_apusys_rv_clear_wdt_isr(void)
{
	int ret = 0;

	INFO("%s: enter\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	spin_lock(&apusys_rv_lock);
	/* clear bit 31 to disable debug APB access */
	mmio_clrbits_32(UP_INT_EN2, DBG_APB_EN);
	/* clear wdt interrupt */
	mmio_write_32(WDT_INT, 0x1);
	spin_unlock(&apusys_rv_lock);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return ret;
}

int apusys_kernel_apusys_rv_cg_gating(void)
{
	int ret = 0;

	INFO("%s: enter\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		ret = -EPERM;
		goto out;
	}

	spin_lock(&apusys_rv_lock);
	mmio_write_32(MD32_CLK_EN, 0x0);
	spin_unlock(&apusys_rv_lock);

out:
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return ret;
}

int apusys_kernel_apusys_rv_cg_ungating(void)
{
	int ret = 0;

	INFO("%s: enter\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		ret = -EPERM;
		goto out;
	}

	spin_lock(&apusys_rv_lock);
	mmio_write_32(MD32_CLK_EN, 0x1);
	spin_unlock(&apusys_rv_lock);

out:
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return ret;
}

int apusys_kernel_apusys_rv_coredump_shadow_copy(void)
{
	int ret = 0;

	INFO("%s: enter\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		ret = -EPERM;
		goto out;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (apusys_rv_sec_buf_pa == 0) {
		ERROR("%s: apusys_rv_sec_buf_pa == 0\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (apusys_secure_info == NULL) {
		ERROR("%s: apusys_secure_info == NULL\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	memcpy((void *) (apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->up_coredump_ofs),
		(void *) apusys_rv_sec_buf_pa +
		apusys_secure_info->up_coredump_ofs,
		sizeof(struct apu_coredump));

out:
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return ret;
}

int apusys_kernel_apusys_rv_tcmdump(void)
{
	int ret = 0;

	INFO("%s: enter\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		ret = -EPERM;
		goto out;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (!is_rcx_mtcmos_on()) {
		ERROR("%s: rcx not on\n", __func__);
		ret = -EPERM;
		goto out;
	}

	memcpy((void *) (apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->up_coredump_ofs +
		offsetof(struct apu_coredump, tcmdump)),
		(void *) APU_MD32_TCM, APU_MD32_TCM_SZ);

out:
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return ret;
}

int apusys_kernel_apusys_rv_tcmdump_wa(void)
{
	int ret = 0;

	INFO("%s: enter\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		ret = -EPERM;
		goto out;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (!is_rcx_mtcmos_on()) {
		ERROR("%s: rcx not on\n", __func__);
		ret = -EPERM;
		goto out;
	}

	/* reset uP */
	mmio_write_32(MD32_SYS_CTRL, 0x0);
	udelay(10);

	memcpy((void *) (apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->up_coredump_ofs +
		offsetof(struct apu_coredump, tcmdump)),
		(void *) APU_MD32_TCM, APU_MD32_TCM_SZ);

out:
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return ret;
}

int apusys_kernel_apusys_rv_ramdump(void)
{
	int ret = 0;

	INFO("%s: enter\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		ret = -EPERM;
		goto out;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (apusys_rv_sec_buf_pa == 0) {
		ERROR("%s: apusys_rv_sec_buf_pa == 0\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (apusys_secure_info == NULL) {
		ERROR("%s: apusys_secure_info == NULL\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	memcpy((void *) (apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->up_coredump_ofs +
		offsetof(struct apu_coredump, ramdump)),
		(void *) (apusys_rv_sec_buf_pa +
		apusys_secure_info->up_code_buf_ofs + APU_MD32_TCM_SZ),
		DRAM_DUMP_SIZE);

out:
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return ret;
}

int apusys_kernel_apusys_rv_tbufdump(void)
{
	int i, j;
	uint32_t tbuf_cur_ptr;
	uint32_t tbuf_dump[TBUF_SIZE/sizeof(uint32_t)];
	int ret = 0;

	INFO("%s: enter\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		ret = -EPERM;
		goto out;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		ret = -ENOMEM;
		goto out;
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
				mmio_read_32(MD32_TBUF_DBG_DAT3 - j*NR_TBUF_DBG_DATA);
		}
	}
	memcpy((void *) (apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->up_coredump_ofs +
		offsetof(struct apu_coredump, tbufdump)),
		tbuf_dump, sizeof(tbuf_dump));

out:
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return ret;
}

int apusys_kernel_apusys_rv_cachedump(void)
{
	int offset;
	struct apu_coredump *coredump;
	int ret = 0;

	INFO("%s: enter\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		ret = -EPERM;
		goto out;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (!is_rcx_mtcmos_on()) {
		ERROR("%s: rcx not on\n", __func__);
		ret = -EPERM;
		goto out;
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

out:
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return ret;
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

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	regdump_pa = apusys_rv_aee_coredump_buf_pa +
		apusys_aee_coredump_info->regdump_ofs;

	ret = apusys_regdump(regdump_pa, apusys_aee_coredump_info->regdump_sz);
	if (ret)
		ERROR("%s: apusys_regdump fail\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

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
	int status = 0;
	unsigned int retry;
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	int ret = 0;
#endif

	INFO("%s: enter\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		goto out;
	}

	if (!is_rcx_mtcmos_on()) {
		ERROR("%s: rcx not on\n", __func__);
		goto out;
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
		if (status & 0x1) {
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
			ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
			if (ret)
				ERROR("%s: sem release timeout\n", __func__);
#endif
			return status;
		}
		udelay(1);
	}

	ERROR("%s: timeout\n", __func__);

out:
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return status;
}

int apusys_kernel_apusys_rv_regdump(uint32_t dbg_apb_status)
{
	int i;
	uint32_t reg_dump[REG_SIZE/sizeof(uint32_t)] = {0};
	uint32_t val;
	struct apu_coredump *coredump;
	int ret = 0;

	INFO("%s: enter\n", __func__);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	if (mmio_read_32(WDT_INT) != 1) {
		ERROR("%s: WDT not timeout\n", __func__);
		ret = -EPERM;
		goto out;
	}

	if (apusys_rv_aee_coredump_buf_pa == 0) {
		ERROR("%s: apusys_rv_aee_coredump_buf_pa == 0\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (apusys_aee_coredump_info == NULL) {
		ERROR("%s: apusys_aee_coredump_info == NULL\n", __func__);
		ret = -ENOMEM;
		goto out;
	}

	if (!is_rcx_mtcmos_on()) {
		ERROR("%s: rcx not on\n", __func__);
		ret = -EPERM;
		goto out;
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

out:
#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return ret;
}

int apusys_kernel_apusys_rv_dump_tcm_log(void)
{
#ifdef CONFIG_MTK_APUSYS_AOV_TCM_DUMP_SUPPORT
	int ret;
	unsigned int buf;
	unsigned long long i, log_buf_pa = apusys_rv_tcm_log_buf_pa;
	char header[APUSYS_TCM_LP_LOG_HEADER_SIZE] = {0};

	INFO("%s: enter\n", __func__);

	if (log_buf_pa == 0)
		return -EINVAL;

	if (snprintf(header, APUSYS_TCM_LP_LOG_HEADER_SIZE,
				"Dump APUSYS TCM from 0x%08x to 0x%08x\n",
				APU_TCM + APUSYS_TCM_LP_LOG_OFFSET,
				APU_TCM + APUSYS_TCM_LP_LOG_OFFSET + APUSYS_TCM_LP_LOG_SIZE) < 0
		)
		return -EINVAL;

	memset((void *) (log_buf_pa), 0, APUSYS_TCM_LP_LOG_HEADER_SIZE + APUSYS_TCM_LP_LOG_SIZE);

	memcpy((void *) (log_buf_pa), header, APUSYS_TCM_LP_LOG_HEADER_SIZE);

	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, 0, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout (rcx off)\n", __func__);
		return ret;
	}

	/* only copy log to dram */
	for (i = 0; i < APUSYS_TCM_LP_LOG_SIZE; i += APU_LOG_ROW_SIZE) {
		buf = mmio_read_32(APU_TCM + APUSYS_TCM_LP_LOG_OFFSET + i);
		if ((buf & APU_LOG_SYNC_MASK) == APU_LOG_SYNC_CODE)
			memcpy((void *) (log_buf_pa + APUSYS_TCM_LP_LOG_HEADER_SIZE + i),
				(void *) (APU_TCM + APUSYS_TCM_LP_LOG_OFFSET + i),
				APU_LOG_ROW_SIZE);
	}

	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, 0, 0);
	if (ret) {
		ERROR("%s: sem release timeout\n", __func__);
		return ret;
	}

#endif

	return 0;
}

int apusys_ce_debug_regdump(uint64_t aee_coredump_buf_pa,
	struct apusys_aee_coredump_info_t *aee_coredump_info)
{
	WARN(MODULE_TAG "%s not support\n", __func__);
	return -EOPNOTSUPP;
}

int apusys_ce_sram_dump(uint64_t aee_coredump_buf_pa,
	struct apusys_aee_coredump_info_t *aee_coredump_info)
{
	WARN(MODULE_TAG "%s not support\n", __func__);
	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_ce_debug_regdump(void)
{
	return apusys_ce_debug_regdump(apusys_rv_aee_coredump_buf_pa,
		apusys_aee_coredump_info);
}

int apusys_kernel_apusys_ce_sram_dump(void)
{
	return apusys_ce_sram_dump(apusys_rv_aee_coredump_buf_pa,
		apusys_aee_coredump_info);
}

int apusys_kernel_apusys_ce_regdump(uint32_t op, struct smccc_res *smccc_ret)
{
	WARN(MODULE_TAG "%s not support\n", __func__);
	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_ce_mask_init(void)
{
	WARN(MODULE_TAG "%s not support\n", __func__);
	return -EOPNOTSUPP;
}

int apusys_kernel_apusys_ce_reg_write(uint32_t op, uint32_t write_val)
{
	WARN(MODULE_TAG "%s not support\n", __func__);
	return -EOPNOTSUPP;
}

#ifdef CONFIG_MTK_APUSYS_RV_IOMMU_HW_SEM_SUPPORT
/* return value:
 *     0: semaphore acquired successfully
 *     non 0: semaphore acquired fail
 */
int apusys_rv_iommu_hw_sem_trylock(void)
{
	return rv_iommu_hw_sem_trylock();
}

/* return value:
 *     0: semaphore release successfully
 *     non 0: semaphore release fail
 */
int apusys_rv_iommu_hw_sem_unlock(void)
{
	return rv_iommu_hw_sem_unlock();
}
#endif /* CONFIG_MTK_APUSYS_RV_IOMMU_HW_SEM_SUPPORT */

int apusys_kernel_apusys_rv_pwr_ctrl(uint32_t op)
{
	return apusys_rv_pwr_ctrl(op);
}

#ifdef CONFIG_MTK_APUSYS_LOGTOP_SUPPORT
int apusys_kernel_apusys_logtop_reg_dump(uint32_t op, struct smccc_res *smccc_ret)
{
	int ret = 0;
	uint8_t smc_op;
	uint32_t reg_addr[MAX_SMC_OP_NUM];
	uint32_t i;

	if (op == 0) {
		ERROR("%s empty op = 0x%08x\n", MODULE_TAG, op);
		return -EINVAL;
	}

	for (i = 0; i < MAX_SMC_OP_NUM; i++) {
		smc_op = (0xFF) & (op >> (8 * i));
		switch (smc_op) {
		case SMC_OP_APU_LOG_BUF_NULL:
			reg_addr[i] = 0x0;
			break;
		case SMC_OP_APU_LOG_BUF_T_SIZE:
			reg_addr[i] = APU_LOG_BUF_T_SIZE;
			break;
		case SMC_OP_APU_LOG_BUF_W_PTR:
			reg_addr[i] = APU_LOG_BUF_W_PTR;
			break;
		case SMC_OP_APU_LOG_BUF_R_PTR:
			reg_addr[i] = APU_LOG_BUF_R_PTR;
			break;
		case SMC_OP_APU_LOG_BUF_CON:
			reg_addr[i] = APU_LOGTOP_CON;
			break;
		default:
			ERROR("%s unknown op = 0x%08x\n", MODULE_TAG, smc_op);
			return -EINVAL;
		}
	}

	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_LOGGER_USER, 1, 0, 0);
	if (ret)
		return ret;

	for (i = 0; i < MAX_SMC_OP_NUM; i++) {
		if (reg_addr[i] != 0) {
			switch (i) {
			case 0:
				smccc_ret->a1 = mmio_read_32(reg_addr[i]);
				break;
			case 1:
				smccc_ret->a2 = mmio_read_32(reg_addr[i]);
				break;
			case 2:
				smccc_ret->a3 = mmio_read_32(reg_addr[i]);
				break;
			}
		}
	}

	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_LOGGER_USER, 0, 0, 0);
	if (ret) {
		ERROR("%s(%d): sem release timeout\n", __func__, op);
		return ret;
	}

	return ret;
}

static int apusys_kernel_apusys_logtop_reg_rw(uint32_t op, uint32_t write_val,
	bool w1c, struct smccc_res *smccc_ret)
{
	int ret = 0;
	uint32_t reg_addr = 0, reg_val = 0;

	switch (op) {
	case SMC_OP_APU_LOG_BUF_R_PTR:
		reg_addr = APU_LOG_BUF_R_PTR;
		break;
	case SMC_OP_APU_LOG_BUF_CON:
		reg_addr = APU_LOGTOP_CON;
		break;
	}

	if (reg_addr == 0) {
		ERROR("%s unknown or not support op = %x\n", MODULE_TAG, op);
		return -EINVAL;
	}

	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_LOGGER_USER, 1, 0, 0);
	if (ret)
		return ret;

	if (w1c) {
		reg_val = mmio_read_32(reg_addr);
		mmio_write_32(reg_addr, reg_val);
		smccc_ret->a1 = reg_val;
	} else {
		mmio_write_32(reg_addr, write_val);
	}

	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_LOGGER_USER, 0, 0, 0);
	if (ret) {
		ERROR("%s(%d): sem release timeout\n", __func__, op);
		return ret;
	}

	return ret;
}

int apusys_kernel_apusys_logtop_reg_write(uint32_t op, uint32_t write_val,
	struct smccc_res *smccc_ret)
{
	return apusys_kernel_apusys_logtop_reg_rw(op, write_val, false, smccc_ret);
}

int apusys_kernel_apusys_logtop_reg_w1c(uint32_t op, struct smccc_res *smccc_ret)
{
	return apusys_kernel_apusys_logtop_reg_rw(op, 0, true, smccc_ret);
}

#endif /* CONFIG_MTK_APUSYS_LOGTOP_SUPPORT */

int apusys_kernel_apusys_release_scp_hw_sem(void)
{
	return release_scp_hw_sem();
}

int apusys_kernel_apusys_decode_apu_exp_irq(void)
{
	uint32_t reg_val = 0;

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	int ret = 0;
#endif

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 1, HW_SEM_TIMEOUT, 0);
	if (ret) {
		ERROR("%s: sem acquire timeout\n", __func__);
		return ret;
	}
#endif

	mmio_write_32(MD32_DBG_BUS_SEL, 0x3);
	reg_val = mmio_read_32(MD32_DBG_BUS_OUT);
	INFO("%s: decode_apu_exp_irq = %x\n", __func__, reg_val);

#ifdef CONFIG_MTK_APUSYS_RV_COREDUMP_WA_SUPPORT
	ret = apu_hw_sema_ctl(HW_SEMA2, HW_SEMA_USER, 0, HW_SEM_TIMEOUT, 0);
	if (ret)
		ERROR("%s: sem release timeout\n", __func__);
#endif

	return reg_val;

}

int apusys_rv_cold_boot_clr_mbox_dummy(void)
{
	mmio_write_32(APU_MBOX(APU_HW_SEM_SYS_APMCU) + APU_MBOX_DUMMY, 0);
	return 0;
}
static int apusys_rv_ops_init(void)
{
	apusys_rv_ops.apusys_rv_ops_setup_reviser = apusys_kernel_apusys_rv_setup_reviser;
	apusys_rv_ops.apusys_rv_ops_setup_apummu = apusys_kernel_apusys_rv_setup_apummu;
	apusys_rv_ops.apusys_rv_ops_pwr_ctrl = apusys_kernel_apusys_rv_pwr_ctrl;
	apusys_rv_ops.apusys_rv_ops_reset_mp = apusys_kernel_apusys_rv_reset_mp;
	apusys_rv_ops.apusys_rv_ops_setup_boot = apusys_kernel_apusys_rv_setup_boot;
	apusys_rv_ops.apusys_rv_ops_start_mp = apusys_kernel_apusys_rv_start_mp;
	apusys_rv_ops.apusys_rv_ops_stop_mp = apusys_kernel_apusys_rv_stop_mp;
	apusys_rv_ops.apusys_rv_ops_drv_init = apusys_rv_init;
//	apusys_rv_ops.apusys_rv_ops_mbox_mpu_init = apusys_rv_mbox_mpu_init;
	apusys_rv_ops.apusys_rv_ops_setup_ce_bin = apusys_setup_ce_bin;
	apusys_rv_ops.apusys_rv_ops_setup_secure_mem = apusys_rv_setup_secure_mem;
	apusys_rv_ops.apusys_rv_ops_kernel_map_secure_iova = apusys_kernel_apusys_map_secure_iova;
	apusys_rv_ops.apusys_rv_ops_setup_aee_coredump_mem = apusys_rv_setup_aee_coredump_mem;
	apusys_rv_ops.apusys_rv_ops_disable_wdt_isr = apusys_kernel_apusys_rv_disable_wdt_isr;
	apusys_rv_ops.apusys_rv_ops_clear_wdt_isr = apusys_kernel_apusys_rv_clear_wdt_isr;
	apusys_rv_ops.apusys_rv_ops_cg_gating = apusys_kernel_apusys_rv_cg_gating;
	apusys_rv_ops.apusys_rv_ops_cg_ungating = apusys_kernel_apusys_rv_cg_ungating;
	apusys_rv_ops.apusys_rv_ops_coredump_shadow_copy =
		apusys_kernel_apusys_rv_coredump_shadow_copy;
	apusys_rv_ops.apusys_rv_ops_tcmdump = apusys_kernel_apusys_rv_tcmdump;
	apusys_rv_ops.apusys_rv_ops_tcmdump_wa = apusys_kernel_apusys_rv_tcmdump_wa;
	apusys_rv_ops.apusys_rv_ops_ramdump = apusys_kernel_apusys_rv_ramdump;
	apusys_rv_ops.apusys_rv_ops_tbufdump = apusys_kernel_apusys_rv_tbufdump;
	apusys_rv_ops.apusys_rv_ops_cachedump = apusys_kernel_apusys_rv_cachedump;
	apusys_rv_ops.apusys_rv_ops_dbg_apb_attach = apusys_kernel_apusys_rv_dbg_apb_attach;
	apusys_rv_ops.apusys_rv_ops_regdump = apusys_kernel_apusys_rv_regdump;
	apusys_rv_ops.apusys_rv_ops_ce_regdump = apusys_kernel_apusys_ce_regdump;
	apusys_rv_ops.apusys_rv_ops_ce_mask_init = apusys_kernel_apusys_ce_mask_init;
	apusys_rv_ops.apusys_rv_ops_ce_sram_dump = apusys_kernel_apusys_ce_sram_dump;
	apusys_rv_ops.apusys_rv_ops_ce_debug_regdump = apusys_kernel_apusys_ce_debug_regdump;
	apusys_rv_ops.apusys_rv_ops_ce_reg_write = apusys_kernel_apusys_ce_reg_write;
	apusys_rv_ops.apusys_rv_ops_release_scp_hw_sem = apusys_kernel_apusys_release_scp_hw_sem;
#ifdef CONFIG_MTK_APUSYS_LOGTOP_SUPPORT
	apusys_rv_ops.apusys_rv_ops_logtop_reg_dump = apusys_kernel_apusys_logtop_reg_dump;
	apusys_rv_ops.apusys_rv_ops_logtop_reg_write = apusys_kernel_apusys_logtop_reg_write;
	apusys_rv_ops.apusys_rv_ops_logtop_reg_w1c = apusys_kernel_apusys_logtop_reg_w1c;
#endif
	apusys_rv_ops.apusys_rv_ops_decode_apu_exp_irq = apusys_kernel_apusys_decode_apu_exp_irq;
	apusys_rv_ops.apusys_rv_ops_setup_tcm_log_mem = apusys_kernel_apusys_rv_setup_tcm_log_mem;
	apusys_rv_ops.apusys_rv_ops_dump_tcm_log = apusys_kernel_apusys_rv_dump_tcm_log;
	apusys_rv_ops.apusys_rv_ops_cold_boot_clr_mbox_dummy = apusys_rv_cold_boot_clr_mbox_dummy;

	return 0;
}
MTK_PLAT_RUNTIME_INIT(apusys_rv_ops_init);

