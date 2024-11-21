#
# Copyright (c) 2024, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

MTK_SOC_FAMILY := mt8189
MTK_IOT_YOCTO := 1

ifeq ($(MAKECMDGOALS),bl2)

MTK_PLAT := plat/mediatek
MTK_SOC := ${PLAT}

include plat/mediatek/build_helpers/mtk_build_helpers.mk
include drivers/arm/gic/v3/gicv3.mk
include lib/xlat_tables_v2/xlat_tables.mk

PLAT_INCLUDES := -I${MTK_PLAT}/common \
		 -I${MTK_PLAT}/include \
		 -I${MTK_PLAT}/common/drivers \
		 -I${MTK_PLAT}/include/${ARCH_VERSION} \
		 -I${MTK_PLAT} \
		 -I${MTK_PLAT}/${MTK_SOC_FAMILY}/include \
		 -Idrivers/arm/gic \

PLAT_BL_COMMON_SOURCES := common/desc_image_load.c \
			  drivers/ti/uart/aarch64/16550_console.S \
			  lib/bl_aux_params/bl_aux_params.c

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
	       ${MTK_PLAT}/common/mtk_plat_common.c \
	       ${MTK_PLAT}/common/mtk_bl2_common.c \
	       ${MTK_PLAT_SOC}/bl2_plat_setup.c \

MODULES-BL2-y				+= $(MTK_PLAT)/drivers/uart
MODULES-BL2-$(CONFIG_MTK_MMC_SUPPORT)	+= ${MTK_PLAT}/common/drivers/mmc

include plat/mediatek/build_helpers/mtk_build_helpers_epilogue.mk

else	# bl31

override PLAT := mt8189
include plat/mediatek/${PLAT}/platform.mk

endif
