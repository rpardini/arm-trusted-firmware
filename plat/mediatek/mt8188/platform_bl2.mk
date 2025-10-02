#
# Copyright (c) 2022, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

PLAT_INCLUDES += -I${MTK_PLAT}/common/drivers

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
	       ${MTK_PLAT}/common/mtk_rgu.c

MODULES-BL2-y += $(MTK_PLAT)/drivers/uart
MODULES-BL2-y += $(MTK_PLAT_SOC)/drivers/libdram
MODULES-BL2-y += ${MTK_PLAT}/drivers/wdt
MODULES-BL2-y += ${MTK_PLAT_SOC}/drivers/pll
MODULES-BL2-y += $(MTK_PLAT_SOC)/drivers/pmic_bl2
MODULES-BL2-y += $(MTK_PLAT_SOC)/drivers/pmic_wrap_bl2
MODULES-BL2-y += ${MTK_PLAT}/common/drivers/blkdev
ifeq (${STORAGE_NOR},1)
MODULES-BL2-y += $(MTK_PLAT_SOC)/drivers/snfc
endif

BL2_LIBS += ${LIBDRAM} \
	    ${LIBBASE}

ifeq (${PLAT_HW_CRYPTO},1)
BL2_LIBS += ${MTK_PLAT}/lib/crypt/libarmcrypt.a
$(eval $(call add_define,PLAT_HW_CRYPTO))
endif

ifeq (${PLAT_AB_BOOT_ENABLE},1)
include lib/zlib/zlib.mk
BL2_SOURCES += ${MTK_PLAT}/common/mtk_ab.c \
	        $(ZLIB_SOURCES) \
$(eval $(call add_define,PLAT_AB_BOOT_ENABLE))
endif

ifeq (${STORAGE_NOR},1)
BL2_STORAGE_NOR_START_ADDR := 0x400000
BL2_STORAGE_NOR_LENGTH := 0x400000
BL2_STORAGE_NOR_BOOTCTRL := 0x1480000
$(eval $(call add_define,BL2_STORAGE_NOR_START_ADDR))
$(eval $(call add_define,BL2_STORAGE_NOR_LENGTH))
$(eval $(call add_define,BL2_STORAGE_NOR_BOOTCTRL))
endif

ifeq (${STORAGE_NOR},1)
BL2_SOURCES += drivers/mtd/nor/spi_nor.c \
            drivers/mtd/spi-mem/spi_mem.c \
            ${MTK_PLAT}/common/drivers/nor/mtk-snfc.c
PLAT_PARTITION_BLOCK_SIZE := 4096
$(eval $(call add_define,STORAGE_NOR))
include lib/libfdt/libfdt.mk
else
BL2_SOURCES += drivers/mmc/mmc.c \
            ${MTK_PLAT}/common/drivers/mmc/mtk-sd.c
ifeq (${STORAGE_APPEND_FIP},1)
$(eval $(call add_define,STORAGE_APPEND_FIP))
$(eval $(call add_define_val,STORAGE_FIP_OFFSET,${STORAGE_FIP_OFFSET}))
$(eval $(call add_define_val,STORAGE_BOOT_LENGTH,${STORAGE_BOOT_LENGTH}))
endif
endif
