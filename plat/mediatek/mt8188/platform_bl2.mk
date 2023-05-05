#
# Copyright (c) 2022, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

PLAT_INCLUDES += -I${MTK_PLAT_SOC}/drivers/pmic/

BL2_SOURCES += lib/cpus/aarch64/cortex_a55.S \
	       lib/cpus/aarch64/cortex_a78.S \
	       ${GICV3_SOURCES} \
	       ${XLAT_TABLES_LIB_SRCS} \
	       plat/common/aarch64/crash_console_helpers.S \
	       drivers/delay_timer/delay_timer.c \
	       drivers/delay_timer/generic_delay_timer.c \
	       drivers/io/io_storage.c \
	       drivers/io/io_block.c \
	       drivers/io/io_fip.c \
	       drivers/partition/gpt.c \
	       drivers/partition/partition.c \
	       lib/libc/memset.c \
	       ${MTK_PLAT}/common/mtk_plat_common.c \
	       $(MTK_PLAT_SOC)/plat_mmap.c \
	       ${MTK_PLAT_SOC}/bl2_plat_setup.c \
	       drivers/mmc/mmc.c \
	       ${MTK_PLAT}/common/drivers/mmc/mtk-sd.c

MODULES-BL2-y += $(MTK_PLAT)/drivers/uart
MODULES-BL2-y += $(MTK_PLAT)/drivers/pmic
MODULES-BL2-y += ${MTK_PLAT}/drivers/wdt
MODULES-BL2-y += ${MTK_PLAT_SOC}/drivers/pll
MODULES-BL2-y += $(MTK_PLAT_SOC)/drivers/pmic_bl2
MODULES-BL2-y += $(MTK_PLAT_SOC)/drivers/pmic_wrap_bl2

BL2_LIBS += ${LIBDRAM} \
	    ${LIBBASE}

ifeq (${PLAT_HW_CRYPTO},1)
BL2_LIBS += ${MTK_PLAT}/lib/crypt/libarmcrypt.a
$(eval $(call add_define,PLAT_HW_CRYPTO))
endif

