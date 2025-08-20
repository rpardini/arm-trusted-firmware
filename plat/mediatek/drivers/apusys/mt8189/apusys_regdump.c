/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <common/debug.h>
#include <lib/mmio.h>
#include <lib/utils_def.h>
#include <mtk_mmap_pool.h>
#include <platform_def.h>

#define ACX0_SUPPORT 0
#define ACX1_SUPPORT 0
#define NCX_SUPPORT 0

#define VCORE               (SPM_BASE + 0x414)
#define PWR_RDY             (APU_RPCTOP + 0x44)
#define VCORE_CLK_CRTL      (APU_RCX_VCORE_CONFIG)
#define RCX_CLK_CRTL        (APU_RCX_CONFIG)

#if ACX0_SUPPORT
#define ACX0_PWR_RDY        (APU_RPCTOP_LITE_ACX0 + 0x44)
#define ACX0_CLK_CRTL       (APU_ACX0_CONFIG)
#endif

#if ACX1_SUPPORT
#define ACX1_PWR_RDY        (APU_RPCTOP_LITE_ACX1 + 0x44)
#define ACX1_CLK_CRTL       (APU_ACX1_CONFIG)
#endif

#if NCX_SUPPORT
#define NCX_PWR_RDY         (APU_RPCTOP_LITE_NCX + 0x44)
#define NCX_CLK_CRTL        (APU_NCX_CONFIG)
#endif

#define bits_get(x, m, o)   ((x & (m << o)) >> o)
#define VCORE_STATUS(x)     bits_get(x, 1, 8)
#define PWR_STATUS(x)       bits_get(x, 1, 0)

/*
 * NOTE: if these are modified,
 * also change them in kernel driver
 */
#define NAME_MAX_LEN        30
#define REGION_MAX_NUM      70

enum apusys_region_name {
	APU_MD32_SYSCTRL_RGN,
	APU_RCX_AO_CTRL_RGN,
	APU_MD32_WDT_RGN,
	APU0_IOMMU_BANK0_RGN,
	APU0_IOMMU_BANK1_RGN,
	APU0_IOMMU_BANK2_RGN,
	APU0_IOMMU_BANK3_RGN,
	APU0_IOMMU_BANK4_RGN,
	APU_SEC_CON_RGN,
	APU_ARE_RGN,
	APU_ARE_AO_RGN,
	APU_RPCTOP_RGN,
	APU_PCUTOP_RGN,
	APU_MNOC_PLL_RGN,
	APU_MDLA_PLL_RGN,
	APU_LOGTOP_RGN,
	APU_CE_REG_RGN,
	APU_ACS_RCX_RGN,
	APU_MBOX0_RGN,
#if ACX0_SUPPORT
	APU_ACS_ACX0_RGN,
	APU_MVPU0_TR1_RGN,
	APU_MVPU0_TR2_RGN,
	APU_MVPU0_KR1_RGN,
	APU_MVPU0_KR2_RGN,
#endif
#if ACX1_SUPPORT
	APU_ACS_ACX1_RGN,
#endif
#if NCX_SUPPORT
	APU_ACS_NCX_RGN,
#endif
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
	{APU_MD32_WDT_RGN,     0x19002000, 0x10,  true},
	{APU0_IOMMU_BANK0_RGN, 0x19010000, 0xB88, true},
	{APU0_IOMMU_BANK1_RGN, 0x19011000, 0x158, true},
	{APU0_IOMMU_BANK2_RGN, 0x19012000, 0x158, true},
	{APU0_IOMMU_BANK3_RGN, 0x19013000, 0x158, true},
	{APU0_IOMMU_BANK4_RGN, 0x19014000, 0xF14, true},
	{APU_LOGTOP_RGN,       0x19024000, 0xE0,  true},
	{APU_ACS_RCX_RGN,      0x1903C000, 0x14,  true},
	{APU_ARE_RGN,          0x190A0000, 0x40,  true},
	{APU_ARE_AO_RGN,       0x190B0000, 0x500, true},
	{APU_CE_REG_RGN,       0x190B0400, 0xA80, true},
	{APU_MBOX0_RGN,        0x190E1000, 0xF4,  true},
	{APU_RPCTOP_RGN,       0x190F0000, 0x60,  true},
	{APU_PCUTOP_RGN,       0x190F1000, 0x100, true},
	{APU_RCX_AO_CTRL_RGN,  0x190F2000, 0x314, true},
	{APU_SEC_CON_RGN,      0x190F5000, 0x34,  true},
	{APU_MNOC_PLL_RGN,     0x1000C604, 0x10,  true},
	{APU_MDLA_PLL_RGN,     0x1000C614, 0x10,  true},
#if ACX0_SUPPORT
	{APU_ACS_ACX0_RGN,     0x19106600, 0x14,  true},
	{APU_MVPU0_TR1_RGN,    0x19120020, 0x3F4, true},
	{APU_MVPU0_TR2_RGN,    0x19120800, 0x8F4, true},
	{APU_MVPU0_KR1_RGN,    0x19121FD0, 0xA0,  true},
	{APU_MVPU0_KR2_RGN,    0x191223F0, 0x5A0, true},
#endif
#if ACX1_SUPPORT
	{APU_ACS_ACX1_RGN,     0x19212600, 0x14,  true},
#endif
#if NCX_SUPPORT
	{APU_ACS_NCX_RGN,      0x19312600, 0x14,  true},
#endif
};

/*
 * This metadata is shared with kernel driver.
 *
 * NOTE:
 * region_info's max length is REGION_MAX_NUM,
 * and its name is NAME_MAX_LEN.
 */
static const struct apusys_regdump_info g_regdump_info = {
	.size = sizeof(struct apusys_regdump_info),
	.region_info_num = REGION_NUM,
	.region_info = {
		{"APU_MD32_SYSCTRL", 0x19001000, 0x848},
		{"APU_MD32_WDT",     0x19002000, 0x10},
		{"APU0_IOMMU_BANK0", 0x19010000, 0xB88},
		{"APU0_IOMMU_BANK1", 0x19011000, 0x158},
		{"APU0_IOMMU_BANK2", 0x19012000, 0x158},
		{"APU0_IOMMU_BANK3", 0x19013000, 0x158},
		{"APU0_IOMMU_BANK4", 0x19014000, 0xF14},
		{"APU_LOGTOP",       0x19024000, 0xE0},
		{"APU_ACS_RCX",      0x1903C000, 0x14},
		{"APU_ARE",          0x190A0000, 0x40},
		{"APU_ARE_AO",       0x190B0000, 0x500},
		{"APU_CE_REG",       0x190B0400, 0xA80},
		{"APU_MBOX0",        0x190E1000, 0xF4},
		{"APU_RPCTOP",       0x190F0000, 0x60},
		{"APU_PCUTOP",       0x190F1000, 0x100},
		{"APU_RCX_AO_CTRL",  0x190F2000, 0x314},
		{"APU_SEC_CON",      0x190F5000, 0x34},
		{"APU_MNOC_PLL",     0x190F6800, 0x10},
		{"APU_UP_PLL",       0x190F6C00, 0x10},
#if ACX0_SUPPORT
		{"APU_ACS_ACX0",     0x19106600, 0x14},
		{"APU_MVPU0_TR1",    0x19120020, 0x3F4},
		{"APU_MVPU0_TR2",    0x19120800, 0x8F4},
		{"APU_MVPU0_KR1",    0x19121FD0, 0xA0},
		{"APU_MVPU0_KR2",    0x191223F0, 0x5A0},
#endif
#if ACX1_SUPPORT
		{"APU_ACS_ACX1",     0x19212600, 0x14},
#endif
#if NCX_SUPPORT
		{"APU_ACS_NCX",      0x19312600, 0x14},
#endif
	},
};

static bool is_vcore_on(void)
{
	uint32_t vcore = mmio_read_32(VCORE);
	bool ret = false;

	if (VCORE_STATUS(vcore)) {
		ret = true;
	} else {
		NOTICE("%s: VCORE 0x%x\n", __func__, vcore);
		ret = false;
	}

	return ret;
}

static bool is_rcx_power_clock_on(void)
{
	uint32_t pwr_ready = mmio_read_32(PWR_RDY);
	uint16_t vcore_clk_ctrl = mmio_read_16(VCORE_CLK_CRTL);
	uint32_t rcx_clk_ctrl = mmio_read_32(RCX_CLK_CRTL);
	bool ret = false;

	if (PWR_STATUS(pwr_ready) &&
		vcore_clk_ctrl == 0 &&
		rcx_clk_ctrl == 0) {
		ret = true;
	} else {
		NOTICE("%s: PWR_RDY 0x%x, VCORE_CLK_CRTL 0x%x, RCX_CLK_CRTL 0x%x\n",
			__func__, pwr_ready, vcore_clk_ctrl, rcx_clk_ctrl);
		ret = false;
	}

	return ret;
}

#if ACX0_SUPPORT
static bool is_acx0_power_clock_on(void)
{
	uint32_t pwr_ready = mmio_read_32(ACX0_PWR_RDY);
	uint32_t clk_ctrl = mmio_read_32(ACX0_CLK_CRTL);
	bool ret = false;

	if (PWR_STATUS(pwr_ready) && clk_ctrl == 0) {
		ret = true;
	} else {
		NOTICE("%s: ACX0_PWR_RDY 0x%x, ACX0_CLK_CRTL 0x%x\n",
			__func__, pwr_ready, clk_ctrl);
		ret = false;
	}

	return ret;
}
#endif

#if ACX1_SUPPORT
static bool is_acx1_power_clock_on(void)
{
	uint32_t pwr_ready = mmio_read_32(ACX1_PWR_RDY);
	uint32_t clk_ctrl = mmio_read_32(ACX1_CLK_CRTL);
	bool ret = false;

	if (PWR_STATUS(pwr_ready) && clk_ctrl == 0) {
		ret = true;
	} else {
		NOTICE("%s: ACX1_PWR_RDY 0x%x, ACX1_CLK_CRTL 0x%x\n",
			__func__, pwr_ready, clk_ctrl);
		ret = false;
	}

	return ret;
}
#endif

#if NCX_SUPPORT
static bool is_ncx_power_clock_on(void)
{
	uint32_t pwr_ready = mmio_read_32(NCX_PWR_RDY);
	uint32_t clk_ctrl = mmio_read_32(NCX_CLK_CRTL);
	bool ret = false;

	if (PWR_STATUS(pwr_ready) && clk_ctrl == 0) {
		ret = true;
	} else {
		NOTICE("%s: NCX_PWR_RDY 0x%x, NCX_CLK_CRTL 0x%x\n",
			__func__, pwr_ready, clk_ctrl);
		ret = false;
	}

	return ret;
}
#endif

static bool is_power_on(enum apusys_region_name region_name)
{
	const struct apusys_regdump_info *info = &g_regdump_info;
	bool result = false;

	switch (region_name) {
	case APU_RCX_AO_CTRL_RGN:
	case APU_SEC_CON_RGN:
	case APU_RPCTOP_RGN:
	case APU_PCUTOP_RGN:
	case APU_ARE_AO_RGN:
	case APU_ARE_RGN:
	case APU_CE_REG_RGN:
	case APU_MBOX0_RGN:
	case APU_MNOC_PLL_RGN:
	case APU_MDLA_PLL_RGN:
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
#if ACX0_SUPPORT
	case APU_ACS_ACX0_RGN:
	case APU_MVPU0_TR1_RGN:
	case APU_MVPU0_TR2_RGN:
	case APU_MVPU0_KR1_RGN:
	case APU_MVPU0_KR2_RGN:
		if (!is_acx0_power_clock_on())
			NOTICE("%s: acx0 mtcmos is not on, do not dump %s\n",
				__func__, info->region_info[region_name].name);
		else
			result = true;
		break;
#endif
#if ACX1_SUPPORT
	case APU_ACS_ACX1_RGN:
		if (!is_acx1_power_clock_on())
			NOTICE("%s: acx1 mtcmos is not on, do not dump %s\n",
				__func__, info->region_info[region_name].name);
		else
			result = true;
		break;
#endif
#if NCX_SUPPORT
	case APU_ACS_NCX_RGN:
		if (!is_ncx_power_clock_on())
			NOTICE("%s: ncx mtcmos is not on, do not dump %s\n",
				__func__, info->region_info[region_name].name);
		else
			result = true;
		break;
#endif
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

int apusys_regdump(uint64_t dst_pa, unsigned int buffer_size)
{
	const struct apusys_regdump_info *info = &g_regdump_info;
	const struct apusys_regdump_region *reg_region = g_reg_region;
	uint32_t region_num = 0;
	unsigned int dump_size = 0;
	int i;

	INFO("%s\n", __func__);

	if (info->size > buffer_size) {
		ERROR("%s: no enough regdump buffer(0x%" PRIx64 ", 0x%x)\n",
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
