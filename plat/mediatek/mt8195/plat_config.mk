#
# Copyright (c) 2022, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Configs for A78 and A55
HW_ASSISTED_COHERENCY := 1
USE_COHERENT_MEM := 0
CTX_INCLUDE_AARCH32_REGS := 0
ERRATA_A55_1530923 := 1

ERRATA_A78_1688305 := 1
ERRATA_A78_1941498 := 1
ERRATA_A78_1951500 := 1
ERRATA_A78_1821534 := 1
ERRATA_A78_2132060 := 1
ERRATA_A78_2242635 := 1

# indicate the reset vector address can be programmed
PROGRAMMABLE_RESET_ADDRESS := 1

COLD_BOOT_SINGLE_CPU := 1

BL2_AT_EL3 := 1

#Enable dynamic memory mapping
PLAT_XLAT_TABLES_DYNAMIC :=1
$(eval $(call add_define,PLAT_XLAT_TABLES_DYNAMIC))

MACH_MT8195 := 1
$(eval $(call add_define,MACH_MT8195))
