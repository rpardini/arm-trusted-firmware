/*
 * Copyright (c) 2020, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PMIC_SETTING_H
#define PMIC_SETTING_H

struct pmic_setting {
  unsigned short addr;
  unsigned short val;
  unsigned short mask;
  unsigned char shift;
};

void pmic_initial_setting(void);

#endif /* PMIC_SETTING_H */

