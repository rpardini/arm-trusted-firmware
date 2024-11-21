// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#include "common/debug.h"
#include <platform_def.h>
#include <libdram.h>
#include "pmic_initial_setting.h"
#include "upmu_hw.h"

/*
 * CONFIG OPTION SET
 */
/* Enable this option when pmic initial setting is verified */
#define INIT_SETTING_VERIFIED	1

/* Enable this option when pmic need efuse sw load */
/* if enable, please also check pmic_efuse.c or pmic_efuse_xxxx.c */
#define EFUSE_SW_LOAD		0

static void wk_vs2_voter_setting(void)
{
	/*
	 * 1. Set VS2_VOTER_VOSEL = 1.35V
	 * 2. Clear VS2_VOTER
	 * 3. Set VS2_VOSEL = 1.45V
	 **/
	pmic_config_interface(PMIC_RG_BUCK_VS2_VOTER_VOSEL_ADDR, 0x2C,
		PMIC_RG_BUCK_VS2_VOTER_VOSEL_MASK, PMIC_RG_BUCK_VS2_VOTER_VOSEL_SHIFT);
	pmic_config_interface(PMIC_RG_BUCK_VS2_VOTER_EN_ADDR, 0,
		PMIC_RG_BUCK_VS2_VOTER_EN_MASK, PMIC_RG_BUCK_VS2_VOTER_EN_SHIFT);
	pmic_config_interface(PMIC_RG_BUCK_VS2_VOSEL_ADDR, 0x34,
		PMIC_RG_BUCK_VS2_VOSEL_MASK, PMIC_RG_BUCK_VS2_VOSEL_SHIFT);
}

static struct pmic_setting init_setting[] = {
	{0x20, 0xA, 0xA, 0},
	{0x24, 0x1F00, 0x1F00, 0},
	{0x30, 0x1, 0x1, 0},
	{0x32, 0x1, 0x1, 0},
	{0x94, 0x0, 0xFFFF, 0},
	{0x10C, 0x10, 0x10, 0},
	{0x112, 0x4, 0x4, 0},
	{0x118, 0x8, 0x8, 0},
	{0x14A, 0x20, 0x20, 0},
	{0x198, 0x0, 0x1FF, 0},
	{0x1B2, 0x3, 0x3, 0},
	{0x3B0, 0x0, 0x300, 0},
	{0x790, 0x3, 0x3, 0},
	{0x796, 0x1750, 0x3FFF, 0},
	{0x798, 0x1750, 0x3FFF, 0},
	{0x7A6, 0xF800, 0xFC00, 0},
	{0x7A8, 0x80, 0x280, 0},
	{0x98A, 0x80, 0x80, 0},
	{0x992, 0xF00, 0xF00, 0},
	{0xA08, 0x1, 0x1, 0},
	{0xA0C, 0x300, 0x300, 0},
	{0xA10, 0x0, 0x4000, 0},
	{0xA12, 0x1E0, 0x1E0, 0},
	{0xA24, 0xFFFF, 0xFFFF, 0},
	{0xA26, 0xFFE0, 0xFFE0, 0},
	{0xA2C, 0xC0DF, 0xC0DF, 0},
	{0xA2E, 0xEBE0, 0xEBE0, 0},
	{0xA34, 0x8000, 0x8000, 0},
	{0xA3C, 0x1500, 0x1F00, 0},
	{0xA3E, 0x5641, 0x7FFF, 0},
	{0xA40, 0x3C63, 0x7FFF, 0},
	{0xA42, 0x9AE, 0x7FFF, 0},
	{0xA44, 0x20E4, 0x7FFF, 0},
	{0xA46, 0x25AD, 0x7FFF, 0},
	{0xA48, 0x416A, 0x7FFF, 0},
	{0xA4A, 0x3A0C, 0x7FFF, 0},
	{0xA4C, 0x3A45, 0x7FFF, 0},
	{0xA4E, 0x4E34, 0x7FFF, 0},
	{0xA50, 0x7CDF, 0x7FFF, 0},
	{0xA9C, 0x4000, 0x4000, 0},
	{0xA9E, 0x2E11, 0xFF11, 0},
	{0xF8C, 0x115, 0x115, 0},
	{0x1188, 0x0, 0x8000, 0},
	{0x1198, 0x13, 0x3FF, 0},
	{0x119E, 0x6000, 0x7000, 0},
	{0x11D4, 0x0, 0x2, 0},
	{0x1212, 0x0, 0x2, 0},
	{0x1224, 0x0, 0x2, 0},
	{0x1238, 0x0, 0x2, 0},
	{0x124A, 0x0, 0x2, 0},
	{0x125C, 0x0, 0x2, 0},
	{0x125E, 0x0, 0x8000, 0},
	{0x1260, 0x1, 0xFFF, 0},
	{0x1262, 0x4, 0x4, 0},
	{0x1412, 0x8, 0x8, 0},
	{0x148E, 0x38, 0x7F, 0},
	{0x1492, 0xF1F, 0x7F7F, 0},
	{0x150E, 0x1C, 0x7F, 0},
	{0x1512, 0xF1F, 0x7F7F, 0},
	{0x158E, 0x18, 0x7F, 0},
	{0x1592, 0xF00, 0x7F00, 0},
	{0x160E, 0x18, 0x7F, 0},
	{0x168E, 0x18, 0x7F, 0},
	{0x1692, 0xF1F, 0x7F7F, 0},
	{0x170E, 0x1C, 0x7F, 0},
	{0x1712, 0xF0F, 0x7F7F, 0},
	{0x178E, 0x18, 0x7F, 0},
	{0x1792, 0xF1F, 0x7F7F, 0},
	{0x1918, 0x0, 0x3F3F, 0},
	{0x191A, 0x0, 0x3F00, 0},
	{0x198A, 0x5004, 0x502C, 0},
	{0x198C, 0x3E, 0x3F, 0},
	{0x198E, 0x1E0, 0x1E0, 0},
	{0x1990, 0xFD, 0xFF, 0},
	{0x1994, 0x10, 0x38, 0},
	{0x1996, 0x2004, 0xA02C, 0},
	{0x1998, 0x3E, 0x3F, 0},
	{0x199A, 0xFB78, 0xFF78, 0},
	{0x199E, 0x2, 0x7, 0},
	{0x19A0, 0x1050, 0x10F1, 0},
	{0x19A2, 0x3E, 0x3F, 0},
	{0x19A4, 0xFD0F, 0xFF0F, 0},
	{0x19A6, 0x20, 0xFF, 0},
	{0x19AC, 0x4208, 0x4698, 0},
	{0x19AE, 0x6E, 0x7E, 0},
	{0x19B0, 0x3C00, 0x3C00, 0},
	{0x19B4, 0x20FD, 0xFFFF, 0},
	{0x1A08, 0x4208, 0x4698, 0},
	{0x1A0A, 0x6E, 0x7E, 0},
	{0x1A0C, 0x3C00, 0x3C00, 0},
	{0x1A10, 0x20FD, 0xFFFF, 0},
	{0x1A14, 0x4208, 0x4698, 0},
	{0x1A16, 0x6E, 0x7E, 0},
	{0x1A18, 0x3C00, 0x3C00, 0},
	{0x1A1C, 0x20FD, 0xFFFF, 0},
	{0x1A1E, 0x0, 0x200, 0},
	{0x1A20, 0x4208, 0x4698, 0},
	{0x1A22, 0x4A, 0x7E, 0},
	{0x1A24, 0x3C00, 0x3C00, 0},
	{0x1A28, 0x20FD, 0xFFFF, 0},
	{0x1A2C, 0x20, 0x74, 0},
	{0x1A2E, 0x1E, 0x1E, 0},
	{0x1A30, 0x42, 0xFF, 0},
	{0x1A32, 0x480, 0x7E0, 0},
	{0x1A34, 0x20, 0x74, 0},
	{0x1A36, 0x1E, 0x1E, 0},
	{0x1A38, 0x42, 0xFF, 0},
	{0x1A3A, 0x480, 0x7E0, 0},
	{0x1A3C, 0x14C, 0x3CC, 0},
	{0x1A3E, 0x23C, 0x3FC, 0},
	{0x1A40, 0xC400, 0xFF00, 0},
	{0x1A42, 0x80, 0xFF, 0},
	{0x1A44, 0x702C, 0xFF2C, 0},
	{0x1B0E, 0xF, 0xF, 0},
	{0x1B10, 0x1, 0x1, 0},
	{0x1B14, 0xFFFF, 0xFFFF, 0},
	{0x1B1A, 0x3FFF, 0x3FFF, 0},
	{0x1B32, 0x8, 0x8, 0},
	{0x1B8A, 0x30, 0x8030, 0},
	{0x1B9C, 0x10, 0x8010, 0},
	{0x1BA0, 0x4000, 0x4000, 0},
	{0x1BAE, 0x1410, 0x9C10, 0},
	{0x1BB2, 0x2, 0x2, 0},
	{0x1BC0, 0x10, 0x8010, 0},
	{0x1BD2, 0x13, 0x8013, 0},
	{0x1BE4, 0x10, 0x8010, 0},
	{0x1C0A, 0x10, 0x8010, 0},
	{0x1C1E, 0x10, 0x8010, 0},
	{0x1C30, 0x10, 0x8010, 0},
	{0x1C42, 0x10, 0x8010, 0},
	{0x1C54, 0x32, 0x8033, 0},
	{0x1C66, 0x10, 0x8010, 0},
	{0x1C8A, 0x10, 0x8010, 0},
	{0x1C8E, 0x4000, 0x4000, 0},
	{0x1C9C, 0x10, 0x8010, 0},
	{0x1CAE, 0x10, 0x8010, 0},
	{0x1CC0, 0x10, 0x8010, 0},
	{0x1CD2, 0x33, 0x8033, 0},
	{0x1CE4, 0x33, 0x8033, 0},
	{0x1D0A, 0x10, 0x8010, 0},
	{0x1D1E, 0x10, 0x8010, 0},
	{0x1D22, 0x4000, 0x4000, 0},
	{0x1D30, 0x10, 0x8010, 0},
	{0x1D34, 0x4000, 0x4000, 0},
	{0x1D42, 0x30, 0x8030, 0},
	{0x1D46, 0x4000, 0x4000, 0},
	{0x1D54, 0x30, 0x8030, 0},
	{0x1D66, 0x32, 0x8033, 0},
	{0x1D8A, 0x10, 0x8010, 0},
	{0x1D9C, 0x10, 0x8010, 0},
	{0x1E8A, 0x10, 0x8010, 0},
	{0x1E8E, 0x10, 0x7F, 0},
	{0x1E92, 0xF1F, 0x7F7F, 0},
	{0x1EAA, 0x10, 0x8010, 0},
	{0x1EAE, 0x10, 0x7F, 0},
	{0x1EB2, 0xF15, 0x7F7F, 0},
	{0x1F0A, 0x10, 0x8010, 0},
	{0x1F0E, 0x8, 0x7F, 0},
	{0x1F12, 0xF15, 0x7F7F, 0},
	{0x1F30, 0x10, 0x8010, 0},
	{0x1F34, 0x8, 0x7F, 0},
	{0x1F38, 0xF15, 0x7F7F, 0},
	{0x200A, 0x8, 0xC, 0},
	{0x202C, 0x8, 0xC, 0},
	{0x208C, 0x100, 0xF00, 0},
	{0x209C, 0x80, 0x1E0, 0},
};

static const struct mtk_regulator empty_regulator;
void pmic_default_voltage(void)
{
	int ret = 0;
	struct mtk_regulator reg_vsram_apu = empty_regulator;
	struct mtk_regulator reg_vsram_gpu = empty_regulator;
	struct mtk_regulator reg_vsram_cpub = empty_regulator;
	struct mtk_regulator reg_vsram_cpul = empty_regulator;
	struct mtk_regulator reg_vapu = empty_regulator;
	struct mtk_regulator reg_vgpu = empty_regulator;
	struct mtk_regulator reg_vcore = empty_regulator;
	struct mtk_regulator reg_vsram_core = empty_regulator;
	struct mtk_regulator reg_vcpul = empty_regulator;
	struct mtk_regulator reg_vmddr = empty_regulator;
	struct mtk_regulator reg_vrf12 = empty_regulator;
	struct mtk_regulator reg_vio18 = empty_regulator;
	struct mtk_regulator reg_vcpub = empty_regulator;
	struct mtk_regulator reg_vdd2h = empty_regulator;
	struct mtk_regulator reg_vmddq = empty_regulator;

	/*--Get regulator handle--*/
	if (mtk_regulator_get("vsram_apu", &reg_vsram_apu))
		ret |= (1 << 0);

	if (mtk_regulator_get("vsram_gpu", &reg_vsram_gpu))
		ret |= (1 << 1);

	if (mtk_regulator_get("vsram_cpub", &reg_vsram_cpub))
		ret |= (1 << 2);

	if (mtk_regulator_get("vsram_cpul", &reg_vsram_cpul))
		ret |= (1 << 3);

	if (mtk_regulator_get("vapu", &reg_vapu))
		ret |= (1 << 4);

	if (mtk_regulator_get("vgpu", &reg_vgpu))
		ret |= (1 << 5);

	if (mtk_regulator_get("vcore", &reg_vcore))
		ret |= (1 << 6);

	if (mtk_regulator_get("vsram_core", &reg_vsram_core))
		ret |= (1 << 7);

	if (mtk_regulator_get("vcpul", &reg_vcpul))
		ret |= (1 << 8);

	if (mtk_regulator_get("vmddr", &reg_vmddr))
		ret |= (1 << 9);

	if (mtk_regulator_get("vrf12", &reg_vrf12))
		ret |= (1 << 10);

	if (mtk_regulator_get("vio18", &reg_vio18))
		ret |= (1 << 11);

	if (mtk_regulator_get("vcpub", &reg_vcpub))
		ret |= (1 << 12);

	if (mtk_regulator_get("vdd2h", &reg_vdd2h))
		ret |= (1 << 13);

	if (mtk_regulator_get("vmddq", &reg_vmddq))
		ret |= (1 << 14);

	if (ret) {
		ERROR("mtk_regulator_get failed.(0x%x)\n", ret);
		return;
	}

/*
 * Bring up only, for voltage adjustment experiment
 * Modify 2nd parameter to the voltage(uV) you want
 */
#if BRINGUP
	mtk_regulator_set_voltage(&reg_vsram_apu, 750000, 750000);
	mtk_regulator_set_voltage(&reg_vsram_gpu, 750000, 750000);
	mtk_regulator_set_voltage(&reg_vsram_cpub, 750000, 750000);
	mtk_regulator_set_voltage(&reg_vsram_cpul, 750000, 750000);
	mtk_regulator_set_voltage(&reg_vapu, 750000, 750000);
	mtk_regulator_set_voltage(&reg_vgpu, 750000, 750000);
	mtk_regulator_set_voltage(&reg_vcore, 750000, 750000);
	mtk_regulator_set_voltage(&reg_vsram_core, 750000, 750000);
	mtk_regulator_set_voltage(&reg_vcpul, 750000, 750000);
	mtk_regulator_set_voltage(&reg_vmddr, 750000, 750000);
	mtk_regulator_set_voltage(&reg_vrf12, 1200000, 1200000);
	mtk_regulator_set_voltage(&vio18, 1800000, 1800000);
	mtk_regulator_set_voltage(&vcpub, 750000, 750000);
	mtk_regulator_set_voltage(&vdd2h, 1050000, 1050000);
	mtk_regulator_set_voltage(&vmddq, 506250, 506250);
#endif

	/*--Get voltage--*/
	INFO("vsram_apu = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vsram_apu),
		mtk_regulator_is_enabled(&reg_vsram_apu) ? "enabled" : "disabled");
	INFO("vsram_gpu = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vsram_gpu),
		mtk_regulator_is_enabled(&reg_vsram_gpu) ? "enabled" : "disabled");
	INFO("vsram_cpub = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vsram_cpub),
		mtk_regulator_is_enabled(&reg_vsram_cpub) ? "enabled" : "disabled");
	INFO("vsram_cpul = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vsram_cpul),
		mtk_regulator_is_enabled(&reg_vsram_cpul) ? "enabled" : "disabled");
	INFO("vapu = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vapu),
		mtk_regulator_is_enabled(&reg_vapu) ? "enabled" : "disabled");
	INFO("vgpu = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vgpu),
		mtk_regulator_is_enabled(&reg_vgpu) ? "enabled" : "disabled");
	INFO("vcore = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vcore),
		mtk_regulator_is_enabled(&reg_vcore) ? "enabled" : "disabled");
	INFO("vsram_core = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vsram_core),
		mtk_regulator_is_enabled(&reg_vsram_core) ? "enabled" : "disabled");
	INFO("vcpul = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vcpul),
		mtk_regulator_is_enabled(&reg_vcpul) ? "enabled" : "disabled");
	INFO("vmddr = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vmddr),
		mtk_regulator_is_enabled(&reg_vmddr) ? "enabled" : "disabled");
	INFO("vrf12 = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vrf12),
		mtk_regulator_is_enabled(&reg_vrf12) ? "enabled" : "disabled");
	INFO("vio18 = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vio18),
		mtk_regulator_is_enabled(&reg_vio18) ? "enabled" : "disabled");
	INFO("vcpub = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vcpub),
		mtk_regulator_is_enabled(&reg_vcpub) ? "enabled" : "disabled");
	INFO("vdd2h = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vdd2h),
		mtk_regulator_is_enabled(&reg_vdd2h) ? "enabled" : "disabled");
	INFO("vmddq = %d uV, %s\n", mtk_regulator_get_voltage(&reg_vmddq),
		mtk_regulator_is_enabled(&reg_vmddq) ? "enabled" : "disabled");
}

/* PMIC warm/cold reset for MT8188 APU */
static bool delayed_cold_reset;
void set_delayed_pmic_cold_reset(void)
{
	delayed_cold_reset = true;
}

bool has_delayed_pmic_cold_reset(void)
{
	return delayed_cold_reset;
}

void delayed_pmic_cold_reset(void)
{
	if (has_delayed_pmic_cold_reset()) {
		INFO("%s\n", __func__);
		pmic_config_interface(PMIC_RG_CRST_ADDR, 1,
				      PMIC_RG_CRST_MASK,
				      PMIC_RG_CRST_SHIFT);
	}
}

void pmic_initial_setting(void)
{
	uint16_t i;

#if INIT_SETTING_VERIFIED
	/* for AUXADC */
	pmic_config_interface(PMIC_HK_AUXADC_KEY_ADDR, 0x6359,
			      PMIC_HK_AUXADC_KEY_MASK,
			      PMIC_HK_AUXADC_KEY_SHIFT);
	/* for Power off sequence */
	pmic_config_interface(PMIC_RG_CPS_W_KEY_ADDR, 0x4729,
			      PMIC_RG_CPS_W_KEY_MASK,
			      PMIC_RG_CPS_W_KEY_SHIFT);
	/* for D type efuse */
	pmic_config_interface(PMIC_TMA_KEY_ADDR, 0x9CA6,
			      PMIC_TMA_KEY_MASK,
			      PMIC_TMA_KEY_SHIFT);
	/* for Buck */
	pmic_config_interface(PMIC_BUCK_TOP_WRITE_KEY_ADDR, 0x5543,
			      PMIC_BUCK_TOP_WRITE_KEY_MASK,
			      PMIC_BUCK_TOP_WRITE_KEY_SHIFT);
	/* for SPISLV */
	pmic_config_interface(PMIC_SPISLV_KEY_ADDR, 0xBADE,
			      PMIC_SPISLV_KEY_MASK,
			      PMIC_SPISLV_KEY_SHIFT);
	/* for BM */
	pmic_config_interface(PMIC_BM_FGADC_KEY_ADDR, 0x1605,
			      PMIC_BM_FGADC_KEY_MASK,
			      PMIC_BM_FGADC_KEY_SHIFT);
	pmic_config_interface(PMIC_BM_BATON_KEY_ADDR, 0x1706,
			      PMIC_BM_BATON_KEY_MASK,
			      PMIC_BM_BATON_KEY_SHIFT);
	pmic_config_interface(PMIC_BM_BIF_KEY_ADDR, 0x1807,
			      PMIC_BM_BIF_KEY_MASK,
			      PMIC_BM_BIF_KEY_SHIFT);

	for (i = 0; i < ARRAY_SIZE(init_setting); i++)
		pmic_config_interface(
			init_setting[i].addr, init_setting[i].val,
			init_setting[i].mask, init_setting[i].shift);

	pmic_config_interface(PMIC_BM_BIF_KEY_ADDR, 0,
			      PMIC_BM_BIF_KEY_MASK,
			      PMIC_BM_BIF_KEY_SHIFT);
	pmic_config_interface(PMIC_BM_BATON_KEY_ADDR, 0,
			      PMIC_BM_BATON_KEY_MASK,
			      PMIC_BM_BATON_KEY_SHIFT);
	pmic_config_interface(PMIC_BM_FGADC_KEY_ADDR, 0,
			      PMIC_BM_FGADC_KEY_MASK,
			      PMIC_BM_FGADC_KEY_SHIFT);
	pmic_config_interface(PMIC_SPISLV_KEY_ADDR, 0,
			      PMIC_SPISLV_KEY_MASK,
			      PMIC_SPISLV_KEY_SHIFT);
	pmic_config_interface(PMIC_BUCK_TOP_WRITE_KEY_ADDR, 0,
			      PMIC_BUCK_TOP_WRITE_KEY_MASK,
			      PMIC_BUCK_TOP_WRITE_KEY_SHIFT);
	pmic_config_interface(PMIC_TMA_KEY_ADDR, 0,
			      PMIC_TMA_KEY_MASK,
			      PMIC_TMA_KEY_SHIFT);
	pmic_config_interface(PMIC_RG_CPS_W_KEY_ADDR, 0,
			      PMIC_RG_CPS_W_KEY_MASK,
			      PMIC_RG_CPS_W_KEY_SHIFT);
	pmic_config_interface(PMIC_HK_AUXADC_KEY_ADDR, 0,
			      PMIC_HK_AUXADC_KEY_MASK,
			      PMIC_HK_AUXADC_KEY_SHIFT);

	INFO("[PMIC] pmic_init_setting end. v190923\n");
#endif /* INIT_SETTING_VERIFIED */

	/* MT6359P do not use HW tracking */
	pmic_default_voltage();

	wk_vs2_voter_setting();
}

