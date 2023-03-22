/*
 * Copyright (c) 2021, MediaTek Inc. All rights reserved.
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

//#ifdef CONFIG_MTK_APUSYS_RV_SECURE_BOOT
#include <mtk_iommu_public.h>
//#endif


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
static bool apusys_rv_load_image_called;
static uint64_t apusys_rv_sec_buf_pa;
static uint64_t apusys_rv_sec_buf_iova;
static uint64_t apusys_rv_sec_buf_sz;
static uint64_t apusys_rv_aee_coredump_buf_pa;
static uint64_t apusys_rv_aee_coredump_buf_sz;
static uint64_t apu_img_base_pa;
static uint64_t apu_img_base_sz;

static struct apusys_secure_info_t *apusys_secure_info;
static struct apusys_aee_coredump_info_t *apusys_aee_coredump_info;

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
	return 0;
}

int apusys_kernel_apusys_rv_cg_ungating(void)
{
	INFO("%s: enter\n", __func__);
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
	WARN(MODULE_TAG "%s not support\n", __func__);

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

static bool is_valid_pa_dram_range(uint64_t res_mem_start,
			uint64_t res_mem_size, uint64_t addr, uint64_t size)
{
	if ((res_mem_start != APUSYS_RESERVED_MEM_START) ||
		(res_mem_size != APUSYS_RESERVED_MEM_SZ)) {
		return false;
	}

	INFO("%s: 0x%llx, 0x%llx, 0x%llx, 0x%llx\n",
		__func__, res_mem_start, res_mem_size, addr, size);

	return (addr >= res_mem_start &&
		addr < (res_mem_start + res_mem_size) &&
		(addr + size) < (res_mem_start + res_mem_size));
}

static int apusys_rv_ns_mem_emi_protect_en(uint64_t pa, uint32_t size)
{
#if ENABLE_APUSYS_EMI_PROTECTION
    /*
     * setup EMI MPU
     * domain 0: APMCU
     * domain 5: APUSYS
     */
	struct emi_region_info_t region_info;

	region_info.start = (unsigned long long) pa;
	region_info.end = (unsigned long long) (pa + size) - 1;
	region_info.region = APUSYS_NS_FW_EMI_REGION;

	SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
					FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW,
					FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW,
					FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW,
					FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW, SEC_RW_NSEC_R);

	return emi_mpu_set_protection(&region_info);
#else
	return 0;
#endif
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

	region_info.start = (unsigned long long) pa;
	region_info.end = (unsigned long long) (pa + size) - 1;
	region_info.region = APUSYS_SEC_FW_EMI_REGION;

	SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
					FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW,
					FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW,
					FORBIDDEN_RW, FORBIDDEN_RW, SEC_RW,       FORBIDDEN_RW,
					FORBIDDEN_RW, FORBIDDEN_RW, FORBIDDEN_RW, SEC_RW);

	return emi_mpu_set_protection(&region_info);
#else
	return 0;
#endif
}

/*
 * apusys_rv_setup_apu_img_mem() - setup apu img region
 */
static int apusys_rv_setup_apu_img_mem(uint64_t addr, uint64_t size)
{
	int ret;

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

/*
 * apusys_rv_setup_apu_img_mem() - setup apu sec mem region
 *
 * 1. dynamic map secure memory
 * 2. get iova of secure memory from iommu mapping
 */
static int apusys_rv_setup_secure_mem(uint64_t addr, uint64_t size)
{
	int ret;

	apusys_rv_sec_buf_pa = addr;
	apusys_rv_sec_buf_sz = size;

	INFO("%s: addr = 0x%llx, size = 0x%llx, realsize = 0x%lx\n",
		__func__, addr, size,
		(size_t)round_up(size + APUSYS_IOVA_PAGE, PAGE_SIZE));

	/* create mapping */
	ret = mmap_add_dynamic_region((unsigned long long)addr,
				(uintptr_t)addr,
				(size_t)round_up(size + APUSYS_IOVA_PAGE,
							PAGE_SIZE),
				(unsigned int)MT_MEMORY | MT_RW | MT_SECURE);
	if (ret) {
		ERROR("%s: mmap_add_dynamic_region() fail, ret=0x%x\n",
			__func__, ret);
		return ret;
	}

	apusys_secure_info =
		(struct apusys_secure_info_t *)(addr + CODE_BUF_SIZE);

//#ifdef CONFIG_MTK_APUSYS_RV_SECURE_BOOT

	INFO("%s: GET SECURE IOVA\n", __func__);
	apusys_rv_sec_buf_iova = mtk_iommu_linear_secure_map(addr,
		size, SECURE_MEM, MOD_APU_FW);

	if (apusys_rv_sec_buf_iova == 0) {
	//	ERROR("%s: Fail to map iova, addr(0x%llx), size(0x%llx)\n",
	//		__func__, addr, size);
		//apusys_rv_sec_buf_iova = 0x200000;
		//mmap_remove_dynamic_region((uintptr_t)addr, (size_t)size);
		//return -ENOMEM;
	}
	INFO("%s: GET SECURE IOVA, addr(0x%llx)\n", __func__, apusys_rv_sec_buf_iova);
//#else
//	apusys_rv_sec_buf_iova = 0x200000;
//#endif

	apusys_rv_sec_mem_emi_protect_en(addr, size);

	return 0;
}

/*
 * apusys_rv_setup_aee_coredump_mem() - setup apu aee coredump mem region
 */
static int apusys_rv_setup_aee_coredump_mem(uint64_t addr, uint64_t size)
{
	int ret;

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

	apusys_aee_coredump_info = (struct apusys_aee_coredump_info_t *)addr;

	return 0;
}
/*
 * apusys_rv_fill_sec_info() - fill sec_info struct
 */
static void apusys_rv_fill_sec_info(uint64_t apusys_part_size,
					struct apusys_secure_info_t *sec_info)
{
	size_t tmp_ofs = 0;
	uint64_t coredump_buf_sz = 0, sec_mem_sz = 0;

	/* fill in sec_info structure */
	sec_info->up_code_buf_ofs = 0;
	sec_info->up_code_buf_sz = UP_CODE_BUF_SZ;
	tmp_ofs = sec_info->up_code_buf_ofs + sec_info->up_code_buf_sz;
	INFO("%s: up_code_buf_sz is 0x%x\n",
		__func__, sec_info->up_code_buf_sz);

	sec_info->up_coredump_ofs = apusys_part_size - (0x200) + tmp_ofs;
	sec_info->up_coredump_sz = UP_COREDUMP_BUF_SZ;
	tmp_ofs = sec_info->up_coredump_ofs + sec_info->up_coredump_sz;
	INFO("%s: up_coredump_ofs is 0x%x, up_coredump_sz is 0x%x\n",
		__func__,
		sec_info->up_coredump_ofs, sec_info->up_coredump_sz);

	sec_info->mdla_coredump_ofs = tmp_ofs;
	sec_info->mdla_coredump_sz = MDLA_COREDUMP_BUF_SZ;
	tmp_ofs = sec_info->mdla_coredump_ofs + sec_info->mdla_coredump_sz;
	INFO("%s: mdla_coredump_ofs is 0x%x, mdla_coredump_sz is 0x%x\n",
		__func__,
		sec_info->mdla_coredump_ofs, sec_info->mdla_coredump_sz);

	sec_info->mvpu_coredump_ofs = tmp_ofs;
	sec_info->mvpu_coredump_sz = MVPU_COREDUMP_BUF_SZ;
	tmp_ofs = sec_info->mvpu_coredump_ofs + sec_info->mvpu_coredump_sz;
	INFO("%s: mvpu_coredump_ofs is 0x%x, mvpu_coredump_sz is 0x%x\n",
		__func__,
		sec_info->mvpu_coredump_ofs, sec_info->mvpu_coredump_sz);

	sec_info->mvpu_sec_coredump_ofs = tmp_ofs;
	sec_info->mvpu_sec_coredump_sz = MVPU_SEC_COREDUMP_BUF_SZ;
	INFO("%s: mvpu_sec_coredump_ofs is 0x%x, mvpu_sec_coredump_sz is 0x%x\n",
		__func__,
		sec_info->mvpu_sec_coredump_ofs,
		sec_info->mvpu_sec_coredump_sz);

	coredump_buf_sz = sec_info->up_coredump_sz +
				sec_info->mdla_coredump_sz +
				sec_info->mvpu_coredump_sz +
				sec_info->mvpu_sec_coredump_sz;

	sec_mem_sz = ROUNDUP(apusys_part_size - (0x200) +
				sec_info->up_code_buf_sz + coredump_buf_sz,
				APUSYS_MEM_IOVA_ALIGN);

	sec_info->total_sz = sec_mem_sz;
}

/*
 * apusys_rv_load_apu_img() - load apusys image
 */
static int apusys_rv_load_apu_img(uint64_t apu_secure_info_pa)
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
	INFO("%s: apu_partition_sz is 0x%x\n", __func__, *img_size);
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

	INFO("%s APUSYS part load finished: PM:0x%llx(up_fw_ofs = 0x%x),",
			__func__, (uint64_t)apusys_pmimg, sec_info->up_fw_ofs);
	INFO("XM:0x%llx(up_xfile_ofs = 0x%x)\n",
			(uint64_t)apusys_ximg, sec_info->up_xfile_ofs);

	/* copy uP firmware to code buffer */
	memcpy((void *) (apusys_rv_sec_buf_pa + sec_info->up_code_buf_ofs),
		(void *) (apusys_rv_sec_buf_pa + sec_info->up_fw_ofs),
		sec_info->up_fw_sz);
	INFO("copy uP firmware to code buffer\n");

	/* initialize apusys sec_info */
	memcpy((void *)(apusys_rv_sec_buf_pa + sec_info->up_code_buf_ofs +
			sec_info->up_code_buf_sz),
			(void *)sec_info, sizeof(*sec_info));

	flush_dcache_range((uintptr_t)apu_secure_info_pa, sizeof(*sec_info));
	flush_dcache_range((uintptr_t)apusys_rv_sec_buf_pa, apusys_rv_sec_buf_sz);
	INFO("apusys_kernel_apusys_rv_load_apu_img end\n");

	return 0;
}

/*
 * apusys_rv_initialize_aee_coredump_buf() -  initialize_aee_coredump memory
 */
static int apusys_rv_initialize_aee_coredump_buf(
					struct apusys_secure_info_t *sec_info)
{
	struct apusys_aee_coredump_info_t *aee_coredump_info =
					((struct apusys_aee_coredump_info_t *)
						apusys_rv_aee_coredump_buf_pa);

	aee_coredump_info->up_coredump_ofs = sizeof(*aee_coredump_info);
	aee_coredump_info->up_coredump_sz = sec_info->up_coredump_sz;
	aee_coredump_info->regdump_ofs = aee_coredump_info->up_coredump_ofs +
					aee_coredump_info->up_coredump_sz;
	aee_coredump_info->regdump_sz = REGDUMP_BUF_SZ;
	aee_coredump_info->mdla_coredump_ofs = aee_coredump_info->regdump_ofs +
						aee_coredump_info->regdump_sz;
	aee_coredump_info->mdla_coredump_sz = sec_info->mdla_coredump_sz;
	aee_coredump_info->mvpu_coredump_ofs =
		aee_coredump_info->mdla_coredump_ofs +
		aee_coredump_info->mdla_coredump_sz;
	aee_coredump_info->mvpu_coredump_sz = sec_info->mvpu_coredump_sz;
	aee_coredump_info->mvpu_sec_coredump_ofs =
		aee_coredump_info->mvpu_coredump_ofs +
		aee_coredump_info->mvpu_coredump_sz;
	aee_coredump_info->mvpu_sec_coredump_sz =
		sec_info->mvpu_sec_coredump_sz;

	aee_coredump_info->up_xfile_ofs =
		aee_coredump_info->mvpu_sec_coredump_ofs +
		aee_coredump_info->mvpu_sec_coredump_sz;
	aee_coredump_info->up_xfile_sz = sec_info->up_xfile_sz;
	aee_coredump_info->mdla_xfile_ofs =
		aee_coredump_info->up_xfile_ofs +
		aee_coredump_info->up_xfile_sz;
	aee_coredump_info->mdla_xfile_sz = sec_info->mdla_xfile_sz;
	aee_coredump_info->mvpu_xfile_ofs =
		aee_coredump_info->mdla_xfile_ofs +
		aee_coredump_info->mdla_xfile_sz;
	aee_coredump_info->mvpu_xfile_sz = sec_info->mvpu_xfile_sz;
	aee_coredump_info->mvpu_sec_xfile_ofs =
		aee_coredump_info->mvpu_xfile_ofs +
		aee_coredump_info->mvpu_xfile_sz;
	aee_coredump_info->mvpu_sec_xfile_sz = sec_info->mvpu_sec_xfile_sz;

	/* copy uP xfile to aee_coredump buffer */
	INFO("copy uP xfile to aee_coredump buffer\n");
	memcpy((void *)(apusys_rv_aee_coredump_buf_pa +
				aee_coredump_info->up_xfile_ofs),
		(void *)(apusys_rv_sec_buf_pa + sec_info->up_xfile_ofs),
		sec_info->up_xfile_sz);

	/* copy mdla xfile to aee_coredump buffer */
	INFO("copy mdla xfile to aee_coredump buffer\n");
	memcpy((void *)(apusys_rv_aee_coredump_buf_pa +
				aee_coredump_info->mdla_xfile_ofs),
		(void *)(apusys_rv_sec_buf_pa + sec_info->mdla_xfile_ofs),
		sec_info->mdla_xfile_sz);

	/* copy mvpu xfile to aee_coredump buffer */
	INFO("copy mvpu xfile to aee_coredump buffer\n");
	memcpy((void *)(apusys_rv_aee_coredump_buf_pa +
				aee_coredump_info->mvpu_xfile_ofs),
		(void *)(apusys_rv_sec_buf_pa + sec_info->mvpu_xfile_ofs),
		sec_info->mvpu_xfile_sz);

	/* copy mvpu_sec xfile to aee_coredump buffer */
	INFO("copy mvpu_sec xfile to aee_coredump buffer\n");
	memcpy((void *)(apusys_rv_aee_coredump_buf_pa +
			aee_coredump_info->mvpu_sec_xfile_ofs),
		(void *)(apusys_rv_sec_buf_pa + sec_info->mvpu_sec_xfile_ofs),
		sec_info->mvpu_sec_xfile_sz);

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

int apusys_kernel_apusys_rv_setup_reviser(void)
{
	uint32_t boundary = 0;

	INFO("%s: enter\n", __func__);

	if (apusys_rv_setup_reviser_called) {
		ERROR("%s: only permitted called once\n", __func__);
		return -EPERM;
	}

//	if (apusys_rv_sec_buf_iova == 0) {
//		ERROR("%s: apusys_rv_sec_buf_iova = 0\n", __func__);
		//return -ENOMEM;
//	}

	apusys_rv_setup_reviser_called = true;

	spin_lock(&apusys_rv_lock);

	/* setup boundary */
	mmio_write_32(USERFW_CTXT, boundary);
	mmio_write_32(SECUREFW_CTXT, boundary);

	/* setup ns/domain */
	mmio_write_32(UP_NORMAL_DOMAIN_NS, (UP_NORMAL_DOMAIN << UP_DOMAIN_SHIFT) |
		(UP_NORMAL_NS << UP_NS_SHIFT)); //1<<4 |7 1:non-secure bit:4
	mmio_write_32(UP_PRI_DOMAIN_NS, (UP_PRI_DOMAIN << UP_DOMAIN_SHIFT) |
		(UP_PRI_NS << UP_NS_SHIFT));//0<<4 |5 0:secure
	INFO("%s: UP_NORMAL_DOMAIN_NS = 0x%x, UP_PRI_DOMAIN_NS=0x%x\n", __func__,
		mmio_read_32(UP_NORMAL_DOMAIN_NS), mmio_read_32(UP_PRI_DOMAIN_NS));

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
	INFO("%s: read MD32_SYS_CTRL = 0x%x\n", __func__, mmio_read_32(MD32_SYS_CTRL));
	spin_unlock(&apusys_rv_lock);

	return 0;
}

int apusys_kernel_apusys_rv_setup_boot(void)
{
	INFO("%s: +++\n", __func__);
	uint32_t code_da = apusys_rv_sec_buf_iova;

	if (apusys_rv_setup_boot_called) {
		ERROR("%s: only permitted called once\n", __func__);
		return -EPERM;
	}

	//if (apusys_rv_sec_buf_iova == 0) {
		//ERROR("%s: apusys_rv_sec_buf_iova = 0\n", __func__);
		//return -ENOMEM;
	//}

	apusys_rv_setup_boot_called = true;

	spin_lock(&apusys_rv_lock);

	mmio_write_32(MD32_BOOT_CTRL, code_da << 1);

	/* set predefined MPU region for cache access */
	mmio_write_32(MD32_PRE_DEFINE, (PREDEFINE_CACHE_TCM << PREDEF_1G_OFS) |
		(PREDEFINE_CACHE << PREDEF_2G_OFS) |
		(PREDEFINE_CACHE << PREDEF_3G_OFS) |
		(PREDEFINE_CACHE << PREDEF_4G_OFS)); //10101011
	INFO("%s: read MD32_PRE_DEFINE=%08x\n", __func__, mmio_read_32(MD32_PRE_DEFINE));
	spin_unlock(&apusys_rv_lock);
	INFO("%s: ---\n", __func__);

	return 0;
}

int apusys_kernel_apusys_rv_start_mp(void)
{
	INFO("%s: +++\n", __func__);

	if (apusys_rv_start_mp_called) {
		ERROR("%s: only permitted called once\n", __func__);
		//return -EPERM;
	}

	apusys_rv_start_mp_called = true;

	spin_lock(&apusys_rv_lock);

	//release runstall
	mmio_write_32(MD32_RUNSTALL, 0x0);
	for (int i = 0; i < 5; i++) {
		udelay(20);
		INFO("apu boot: pc=%08x, sp=%08x\n",
					mmio_read_32(APU_MD32_SYSCTRL + 0x838),
					mmio_read_32(APU_MD32_SYSCTRL + 0x840));
	}

	spin_unlock(&apusys_rv_lock);
	INFO("%s: ---\n", __func__);

	return 0;
}

int apusys_kernel_apusys_rv_stop_mp(void)
{
	INFO("%s: enter\n", __func__);

	if (apusys_rv_stop_mp_called) {
		ERROR("%s: only permitted called once\n", __func__);
		//return -EPERM;
	}

	apusys_rv_stop_mp_called = true;

	spin_lock(&apusys_rv_lock);

	/* hold runstall */
	mmio_write_32(MD32_RUNSTALL, 0x1);

	spin_unlock(&apusys_rv_lock);

	return 0;
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

	return 0;
}

/*
 * apusys_rv driver initialization (from atf)
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
	apusys_rv_load_image_called = 0;

	apu_img_base_pa = 0;
	apu_img_base_sz = 0;

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
 * apusys_kernel_apusys_rv_load_image() - public api for apusys secure boot flow
 * @apusys_part_size: apu fw size
 *
 * APU map three mem region in ATF for diff usage. The memory layout is
 * described as below:
 *
 *             PA                            offset (ALIGN)
 *
 *  1|apu_img_base_pa            || -- || 0 (APUSYS_MEM_ALIGN)
 *  1|                           || -- ||
 *  1|sec_info_pa                || -- || apusys_part_size (APUSYS_MEM_ALIGN)
 *  1|                           || -- ||
 *  2|sec_mem_addr_pa            || -- || sec_mem_addr_ofs
 *  2|                           || -- || (APUSYS_MEM_IOVA_ALIGN)
 *  2|                           || -- ||
 *  3|aee_coredump_mem_addr_pa   || -- || sec_mem_addr_ofs + sec_mem_size +
 *  3|                           || -- ||       APUSYS_IOVA_PAGE
 *  3|                           || -- || (APUSYS_MEM_IOVA_ALIGN)
 *
 *
 * @return: 0 if success, otherwise failed
 */
int32_t apusys_kernel_apusys_rv_load_image(uint64_t res_mem_start,
			uint64_t res_mem_size, uint64_t apusys_part_size)
{
	struct apusys_secure_info_t *sec_info = NULL;
	struct apusys_aee_coredump_info_t *aee_coredump_info = NULL;
	size_t tmp_ofs = 0;
	size_t ret = 0;
	uint64_t sec_info_pa = 0, sec_info_mem_sz = 0;
	uint64_t sec_mem_addr_pa = 0, sec_mem_sz = 0, sec_mem_addr_ofs = 0;
	uint64_t xfile_buf_sz = 0, coredump_buf_sz = 0, regdump_buf_sz = 0;
	uint64_t aee_coredump_mem_addr_pa = 0, aee_coredump_mem_size = 0;
	char *sig = NULL;

	if (apusys_rv_load_image_called) {
		ERROR("%s: only permitted called once\n", __func__);
		return -EPERM;
	}
	apusys_rv_load_image_called = true;

	apu_img_base_pa = APUSYS_RESERVED_MEM_START;
	if (!is_valid_pa_dram_range(res_mem_start, res_mem_size,
					apu_img_base_pa, apusys_part_size)) {
		ERROR("%s: invalid res_mem_start(0x%llx), res_mem_size(0x%llx),",
			__func__, res_mem_start, res_mem_size);
		ERROR("invalid addr(0x%llx), size(0x%llx)\n",
			apu_img_base_pa, apusys_part_size);
		return -EINVAL;
	}
	tmp_ofs = ROUNDUP(apusys_part_size, APUSYS_MEM_ALIGN);

	sec_info_pa = apu_img_base_pa + tmp_ofs;
	sec_info_mem_sz = ROUNDUP(sizeof(struct apusys_secure_info_t),
					APUSYS_MEM_ALIGN);
	tmp_ofs += sec_info_mem_sz;

	sec_mem_addr_ofs = ROUNDUP(tmp_ofs, APUSYS_MEM_IOVA_ALIGN);
	sec_mem_addr_pa = apu_img_base_pa + sec_mem_addr_ofs;
	apu_img_base_sz = sec_mem_addr_ofs;

	INFO("%s: apu_img_base_pa is 0x%llx\, size=0x%xn",
		__func__, apu_img_base_pa, apu_img_base_sz);
	INFO("%s: sec_info_pa is 0x%llx, sec_info_mem_sz is 0x%llx\n",
		__func__, sec_info_pa, sec_info_mem_sz);

	/* map non-sec apu mem region */
	ret = apusys_rv_setup_apu_img_mem(apu_img_base_pa, apu_img_base_sz);
	if (ret) {
		ERROR("%s: apusys_rv_setup_apu_img_mem failed, ret(%d)\n",
			__func__, (int)ret);
		return ret;
	}

	/* fill in dts property sec_info */
	sec_info = (struct apusys_secure_info_t *)sec_info_pa;
	apusys_rv_fill_sec_info(apusys_part_size, sec_info);

	/* verify img */
	sig = (char *)(apu_img_base_pa + apusys_part_size - 256);

	ret = apusys_image_verify((uint64_t *)apu_img_base_pa,
					(uint32_t)apusys_part_size, sig);
	if (ret) {
		ERROR("%s: apusys_image_verify failed, ret(%d)\n",
			__func__, (int)ret);
		return ret;
	}

	/* map sec apu mem region */
	sec_mem_sz = sec_info->total_sz;
	INFO("%s: sec_mem_addr_pa is 0x%llx\, size=0x%x\n",
		__func__, sec_mem_addr_pa, sec_mem_sz);
	ret = apusys_rv_setup_secure_mem(sec_mem_addr_pa, sec_mem_sz);
	if (ret) {
		ERROR("%s: apusys_rv_setup_secure_mem failed, ret(%d)\n",
			__func__, (int)ret);
		return ret;
	}

	/* load apu img */
	ret = apusys_rv_load_apu_img(sec_info_pa);
	if (ret) {
		ERROR("%s: apusys_rv_load_apu_img failed, ret(%d)\n",
			__func__, (int)ret);
		return ret;
	}

	/* prepare & map aee coredump mem region */
	xfile_buf_sz = sec_info->up_xfile_sz + sec_info->mdla_xfile_sz +
			sec_info->mvpu_xfile_sz + sec_info->mvpu_sec_xfile_sz;
	coredump_buf_sz = sec_info->up_coredump_sz +
				sec_info->mdla_coredump_sz +
				sec_info->mvpu_coredump_sz +
				sec_info->mvpu_sec_coredump_sz;
	regdump_buf_sz = REGDUMP_BUF_SZ;

	aee_coredump_mem_addr_pa = apusys_rv_sec_buf_pa +
					ROUNDUP(sec_mem_sz + APUSYS_IOVA_PAGE,
						APUSYS_MEM_ALIGN);
	aee_coredump_mem_size = ROUNDUP(sizeof(*aee_coredump_info) +
					xfile_buf_sz + coredump_buf_sz +
					regdump_buf_sz, APUSYS_MEM_ALIGN);

	ret = apusys_rv_setup_aee_coredump_mem(aee_coredump_mem_addr_pa,
						aee_coredump_mem_size);
	if (ret) {
		ERROR("%s: apusys_rv_setup_aee_coredump_mem failed, ret(%d)\n",
			__func__, (int)ret);
		return ret;
	}

	/* init aee coredump buf info */
	INFO("%s: apusys_rv_initialize_aee_coredump_buf...\n", __func__);
	apusys_rv_initialize_aee_coredump_buf(sec_info);
	if (ret) {
		ERROR("%s: apusys_rv_initialize_aee_coredump_buf failed, ret(%d)\n",
			__func__, (int)ret);
		return ret;
	}
	return 0;
}
