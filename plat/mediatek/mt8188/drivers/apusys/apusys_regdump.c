/*
 * Copyright (c) 2021, Mediatek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <string.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <lib/utils_def.h>
#include <mtk_mmap_pool.h>
#include <platform_def.h>
//#include <mblock.h>

#define VCORE               (SPM_BASE + 0x414)
#define PWR_RDY             (APU_RPCTOP + 0x44)
#define VCORE_CLK_CRTL      (APU_RCX_VCORE_CONFIG)
#define RCX_CLK_CRTL        (APU_RCX_CONFIG)
#define ACX0_PWR_RDY        (APU_ACX0_RPC_LITE + 0x44)
#define ACX0_CLK_CRTL       (APU_ACX0_CONFIG)

#define bits_get(x, m, o)   ((x & (m << o)) >> o)
#define VCORE_STATUS(x)     bits_get(x, 1, 1)
#define PWR_STATUS(x)       bits_get(x, 1, 0)

/*
 * NOTE: if these are modified,
 * also change them in kernel driver
 */
#define NAME_MAX_LEN        30
#define REGION_MAX_NUM      50

#if 0
static const mmap_region_t apusys_regdump_mmap[] MTK_MMAP_SECTION = {
	MAP_REGION_FLAT(APU_ACS_RCX, APU_ACS_RCX_SZ,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APU_ACX0_RPC_LITE, APU_RPCTOP_LITE_ACX0_SZ,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APU_ACX0_CONFIG, APU_ACX0_CONFIG_SZ,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APU_ACS_ACX0, APU_ACS_ACX0_SZ,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APU_SEC_CON, APU_SEC_CON_SZ,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APU_ARETOP_ARE0, APU_ARETOP_ARE0_SZ,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APU_ARETOP_ARE1, APU_ARETOP_ARE1_SZ,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APU_ARETOP_ARE2, APU_ARETOP_ARE2_SZ,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APU_PCUTOP, APU_PCUTOP_SZ,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APU_PLL, APU_PLL_COMMON_SZ,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(APU_LOGTOP, APU_LOGTOP_SZ,
		MT_DEVICE | MT_RW | MT_SECURE),
	{0}
};
DECLARE_MTK_MMAP_REGIONS(apusys_regdump_mmap);
#endif

enum apusys_region_name {
	APU_MD32_SYSCTRL_RGN,
	APU_RCX_AO_CTRL_RGN,
	APU_MD32_WDT_RGN,
	APU0_IOMMU_BANK0_RGN,
	APU0_IOMMU_BANK1_RGN,
	APU0_IOMMU_BANK2_RGN,
	APU0_IOMMU_BANK3_RGN,
	APU0_IOMMU_BANK4_RGN,
	APU_ACS_RCX_RGN,
	APU_ACS_ACX0_RGN,
	APU_SEC_CON_RGN,
	INFRA_CFG_AO_RGN,
	APU_ARE0_SRAM_L_RGN,
	APU_ARE0_SRAM_H_RGN,
	APU_ARE1_SRAM_L_RGN,
	APU_ARE1_SRAM_H_RGN,
	APU_ARE2_SRAM_L_RGN,
	APU_ARE2_SRAM_H_RGN,
	APU_ARE0_HW_CFG_RGN,
	APU_ARE1_HW_CFG_RGN,
	APU_ARE2_HW_CFG_RGN,
	APU_RPCTOP_RGN,
	APU_PCUTOP_RGN,
	APU_MNOC_PLL_RGN,
	APU_UP_PLL_RGN,
	APU_LOGTOP_RGN,
	REGION_NUM
};

struct apusys_regdump_region {
	enum apusys_region_name name;
	uint32_t start;
	uint32_t size;
	bool is_dump;
};

struct apusys_regdump_region_info {
	char name[NAME_MAX_LEN + 1];
	uint32_t start;
	uint32_t size;
};

struct apusys_regdump_info {
	uint64_t size;
	uint32_t region_info_num;
	struct apusys_regdump_region_info region_info[REGION_MAX_NUM];
};

/* The region will not be dumped if is_dump is false */
static const struct apusys_regdump_region g_reg_region[] = {
	{APU_MD32_SYSCTRL_RGN, 0x19001000, 0x848, true},
	{APU_RCX_AO_CTRL_RGN,  0x190F2000, 0x314, true},
	{APU_MD32_WDT_RGN,     0x19002000, 0x10,  true},
	{APU0_IOMMU_BANK0_RGN, 0x19010000, 0xB88, true},
	{APU0_IOMMU_BANK1_RGN, 0x19011000, 0x158, true},
	{APU0_IOMMU_BANK2_RGN, 0x19012000, 0x158, true},
	{APU0_IOMMU_BANK3_RGN, 0x19013000, 0x158, true},
	{APU0_IOMMU_BANK4_RGN, 0x19014000, 0xF14, true},
	{APU_ACS_RCX_RGN,      0x19044400, 0x14,  true},
	{APU_ACS_ACX0_RGN,     0x19112600, 0x14,  true},
	{APU_SEC_CON_RGN,      0x190F5000, 0x34,  true},
	{INFRA_CFG_AO_RGN,     0x10001CB0, 0x04,  true},
	{APU_ARE0_SRAM_L_RGN,  0x190F6800, 0x400, true},
	{APU_ARE0_SRAM_H_RGN,  0x190F6C00, 0x400, true},
	{APU_ARE1_SRAM_L_RGN,  0x190F7800, 0x400, true},
	{APU_ARE1_SRAM_H_RGN,  0x190F7C00, 0x400, true},
	{APU_ARE2_SRAM_L_RGN,  0x190F8800, 0x400, true},
	{APU_ARE2_SRAM_H_RGN,  0x190F8C00, 0x400, true},
	{APU_ARE0_HW_CFG_RGN,  0x190F6000, 0x60,  true},
	{APU_ARE1_HW_CFG_RGN,  0x190F7000, 0x60,  true},
	{APU_ARE2_HW_CFG_RGN,  0x190F8000, 0x60,  true},
	{APU_RPCTOP_RGN,       0x190F0000, 0x60,  true},
	{APU_PCUTOP_RGN,       0x190F1000, 0x100, true},
	{APU_MNOC_PLL_RGN,     0x190F3800, 0x10,  true},
	{APU_UP_PLL_RGN,       0x190F3C00, 0x10,  true},
	{APU_LOGTOP_RGN,       0x19024000, 0xE0,  true},
};

/*
 * This metadata is shared with kernel driver.
 *
 * NOTE:
 * 1. region_info's max length is REGION_MAX_NUM,
 *    and its name is NAME_MAX_LEN.
 * 2. The register dump size now is 0x45F0 bytes.
 */
static const struct apusys_regdump_info g_regdump_info = {
	.size = sizeof(struct apusys_regdump_info),
	.region_info_num = REGION_NUM,
	.region_info = {
		{"APU_MD32_SYSCTRL", 0x19001000, 0x848},
		{"APU_RCX_AO_CTRL",  0x190F2000, 0x314},
		{"APU_MD32_WDT",     0x19002000, 0x10},
		{"APU0_IOMMU_BANK0", 0x19010000, 0xB88},
		{"APU0_IOMMU_BANK1", 0x19011000, 0x158},
		{"APU0_IOMMU_BANK2", 0x19012000, 0x158},
		{"APU0_IOMMU_BANK3", 0x19013000, 0x158},
		{"APU0_IOMMU_BANK4", 0x19014000, 0xF14},
		{"APU_ACS_RCX",      0x19044400, 0x14},
		{"APU_ACS_ACX0",     0x19112600, 0x14},
		{"APU_SEC_CON",      0x190F5000, 0x34},
		{"INFRA_CFG_AO",     0x10001CB0, 0x04},
		{"APU_ARE0_SRAM_L",  0x190F6800, 0x400},
		{"APU_ARE0_SRAM_H",  0x190F6C00, 0x400},
		{"APU_ARE1_SRAM_L",  0x190F7800, 0x400},
		{"APU_ARE1_SRAM_H",  0x190F7C00, 0x400},
		{"APU_ARE2_SRAM_L",  0x190F8800, 0x400},
		{"APU_ARE2_SRAM_H",  0x190F8C00, 0x400},
		{"APU_ARE0_HW_CFG",  0x190F6000, 0x60},
		{"APU_ARE1_HW_CFG",  0x190F7000, 0x60},
		{"APU_ARE2_HW_CFG",  0x190F8000, 0x60},
		{"APU_RPCTOP",       0x190F0000, 0x60},
		{"APU_PCUTOP",       0x190F1000, 0x100},
		{"APU_MNOC_PLL",     0x190F3800, 0x10},
		{"APU_UP_PLL",       0x190F3C00, 0x10},
		{"APU_LOGTOP",       0x19024000, 0xE0},
	},
};

static bool is_vcore_on(void)
{
	uint32_t vcore = mmio_read_32(VCORE);

	if (VCORE_STATUS(vcore)) {
		NOTICE("%s: VCORE 0x%x\n", __func__, vcore);
		return false;
	}

	return true;
}

static bool is_rcx_power_clock_on(void)
{
	uint32_t pwr_ready = mmio_read_32(PWR_RDY);
	uint16_t vcore_clk_ctrl = mmio_read_16(VCORE_CLK_CRTL);
	uint32_t rcx_clk_ctrl = mmio_read_32(RCX_CLK_CRTL);

	if (PWR_STATUS(pwr_ready) &&
		vcore_clk_ctrl == 0 &&
		rcx_clk_ctrl == 0)
		return true;

	NOTICE("%s: PWR_RDY 0x%x, VCORE_CLK_CRTL 0x%x, RCX_CLK_CRTL 0x%x\n",
		__func__, pwr_ready, vcore_clk_ctrl, rcx_clk_ctrl);

	return false;
}

static bool is_acx0_power_clock_on(void)
{
	uint32_t pwr_ready = mmio_read_32(ACX0_PWR_RDY);
	uint32_t clk_ctrl = mmio_read_32(ACX0_CLK_CRTL);

	if (PWR_STATUS(pwr_ready) && clk_ctrl == 0)
		return true;

	NOTICE("%s: ACX0_PWR_RDY 0x%x, ACX0_CLK_CRTL 0x%x\n",
		__func__, pwr_ready, clk_ctrl);

	return false;
}

static bool is_power_on(enum apusys_region_name region_name)
{
	const struct apusys_regdump_info *info = &g_regdump_info;
	bool result = false;

	switch (region_name) {
	case APU_RCX_AO_CTRL_RGN:
	case APU_SEC_CON_RGN:
		if (!is_vcore_on())
			NOTICE("%s: vcore is not enable, do not dump %s\n",
				__func__, info->region_info[region_name].name);
		else
			result = true;
		break;
	case APU_MD32_SYSCTRL_RGN:
	case APU_MD32_WDT_RGN:
	case APU0_IOMMU_BANK0_RGN:
	case APU0_IOMMU_BANK1_RGN:
	case APU0_IOMMU_BANK2_RGN:
	case APU0_IOMMU_BANK3_RGN:
	case APU0_IOMMU_BANK4_RGN:
	case APU_ACS_RCX_RGN:
	case APU_LOGTOP_RGN:
		if (!is_rcx_power_clock_on())
			NOTICE("%s: rcx mtcmos is not on, do not dump %s\n",
				__func__, info->region_info[region_name].name);
		else
			result = true;
		break;
	case APU_ACS_ACX0_RGN:
		if (!is_acx0_power_clock_on())
			NOTICE("%s: acx0 mtcmos is not on, do not dump %s\n",
				__func__, info->region_info[region_name].name);
		else
			result = true;
		break;
	case INFRA_CFG_AO_RGN:
	case APU_ARE0_SRAM_L_RGN:
	case APU_ARE0_SRAM_H_RGN:
	case APU_ARE1_SRAM_L_RGN:
	case APU_ARE1_SRAM_H_RGN:
	case APU_ARE2_SRAM_L_RGN:
	case APU_ARE2_SRAM_H_RGN:
	case APU_ARE0_HW_CFG_RGN:
	case APU_ARE1_HW_CFG_RGN:
	case APU_ARE2_HW_CFG_RGN:
	case APU_RPCTOP_RGN:
	case APU_PCUTOP_RGN:
	case APU_MNOC_PLL_RGN:
	case APU_UP_PLL_RGN:
		result = true;
		break;
	default:
		ERROR("%s: not dump unknown region %d\n",
			__func__, region_name);
		break;
	}

	return result;
}

static uint32_t region_regdump(uint64_t dst_pa, uint32_t region_idx)
{
	const struct apusys_regdump_region *reg_region = g_reg_region;
	uint32_t *dst_ptr = (uint32_t *)dst_pa;
	uint32_t reg_len_bytes = 4;
	uint32_t reg_addr = 0;
	uint32_t reg_num = 0;
	uint32_t i;

	reg_num = reg_region[region_idx].size / reg_len_bytes;

	if (reg_region[region_idx].is_dump) {
		if (is_power_on(reg_region[region_idx].name)) {
			for (i = 0; i < reg_num; i++) {
				reg_addr = reg_region[region_idx].start + i * reg_len_bytes;
				dst_ptr[i] = mmio_read_32(reg_addr);
			}
		} else {
			for (i = 0; i < reg_num; i++)
				dst_ptr[i] = 0x0;
		}
	} else {
		for (i = 0; i < reg_num; i++)
			dst_ptr[i] = 0x0;
	}

	return reg_region[region_idx].size;
}

static int is_valid_pa_dram_range(uint64_t addr, unsigned int size)
{
#if 0
	uint64_t res_mem_start = mblock_get_memory_start();
	uint64_t res_mem_size = mblock_get_memory_size();

	INFO("%s: 0x%llx, 0x%llx, 0x%llx, 0x%x\n",
		__func__, res_mem_start, res_mem_size, addr, size);

	return (addr >= res_mem_start &&
		addr < (res_mem_start + res_mem_size) &&
		(addr + size) < (res_mem_start + res_mem_size));
#else
	return true;
#endif
}

int apusys_regdump(uint64_t dst_pa, unsigned int buffer_size)
{
	const struct apusys_regdump_info *info = &g_regdump_info;
	const struct apusys_regdump_region *reg_region = g_reg_region;
	uint32_t region_num = 0;
	unsigned int dump_size = 0;
	int i;

	INFO("%s\n", __func__);

	if (!is_valid_pa_dram_range(dst_pa, buffer_size)) {
		ERROR("%s: invalid dst_pa(0x%llx), buffer_size(0x%x)\n",
			__func__, dst_pa, buffer_size);
		return -EINVAL;
	}

	if (info->size > buffer_size) {
		ERROR("%s: no enough regdump buffer(0x%llx, 0x%x)\n",
			__func__, info->size, buffer_size);
		return -ENOMEM;
	}

	if (info->region_info_num > REGION_MAX_NUM) {
		ERROR("%s: region_info_num 0x%x > REGION_MAX_NUM 0x%x\n",
			__func__, info->region_info_num, REGION_MAX_NUM);
		return -EPERM;
	}

	for (i = 0; i < info->region_info_num; i++) {
		if (strlen(info->region_info[i].name) > NAME_MAX_LEN) {
			ERROR("%s: region_info[%d].name len 0x%lx > NAME_MAX_LEN 0x%x\n",
				__func__, i, strlen(info->region_info[i].name),
				NAME_MAX_LEN);
			return -EPERM;
		}
	}

	/* Copy info to regdump memory */
	memcpy((void *) dst_pa, info, info->size);
	dump_size = (unsigned int) info->size;

	/* Dump register contents to regdump memory */
	region_num = ARRAY_SIZE(g_reg_region);
	for (i = 0; i < region_num; i++) {
		buffer_size -= dump_size;
		if (reg_region[i].size > buffer_size) {
			ERROR("%s: no enough regdump buffer(%d, 0x%x, 0x%x)\n",
				__func__, i, reg_region[i].size, buffer_size);
			return -ENOMEM;
		}

		dst_pa += dump_size;
		dump_size = region_regdump(dst_pa, i);
	}

	return 0;
}
