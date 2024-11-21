/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 MediaTek Inc.
 */

#ifndef PMIC_SETTING_H
#define PMIC_SETTING_H

struct pmic_setting {
	unsigned short addr;
	unsigned short val;
	unsigned short mask;
	unsigned char shift;
};

struct regulator_ctrl;
struct mtk_regulator {
	const char *name;
	unsigned char id;
	struct regulator_ctrl *reg_ops;
};

extern int mtk_regulator_get(const char *id, struct mtk_regulator *mreg);
extern int mtk_regulator_enable(struct mtk_regulator *mreg, unsigned char enable);
extern int mtk_regulator_is_enabled(struct mtk_regulator *mreg);
extern int mtk_regulator_set_voltage(struct mtk_regulator *mreg, int min_uv, int max_uv);
extern int mtk_regulator_get_voltage(struct mtk_regulator *mreg);
extern int mtk_regulator_set_mode(struct mtk_regulator *mreg, unsigned char mode);
extern int mtk_regulator_get_mode(struct mtk_regulator *mreg);

void pmic_initial_setting(void);

#endif /* PMIC_SETTING_H */
