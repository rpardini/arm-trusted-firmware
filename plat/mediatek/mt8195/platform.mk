#
# Copyright (c) 2021-2023, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

MTK_PLAT     := plat/mediatek
MTK_PLAT_SOC := ${MTK_PLAT}/${PLAT}
MTK_SOC := ${PLAT}

include plat/mediatek/build_helpers/mtk_build_helpers.mk

PLAT_INCLUDES := -I${MTK_PLAT}/common/                            \
                 -I${MTK_PLAT}/common/drivers/                    \
                 -I${MTK_PLAT}/drivers/apusys/secure_boot/         \
                 -I${MTK_PLAT}/drivers/apusys/secure_boot/include/ \
                 -I${MTK_PLAT}/drivers/cirq/                      \
                 -I${MTK_PLAT}/drivers/dp/                        \
                 -I${MTK_PLAT}/drivers/gic600/                    \
                 -I${MTK_PLAT}/drivers/gpio/                      \
                 -I${MTK_PLAT}/drivers/iommu/                     \
                 -I${MTK_PLAT}/drivers/pmic/                      \
                 -I${MTK_PLAT}/drivers/pmic_wrap/                 \
                 -I${MTK_PLAT}/drivers/ptp3/                      \
                 -I${MTK_PLAT}/drivers/rtc/                       \
                 -I${MTK_PLAT}/drivers/timer/                     \
                 -I${MTK_PLAT}/drivers/uart/                      \
                 -I${MTK_PLAT}/drivers/wdt/                       \
                 -I${MTK_PLAT}/include/                           \
                 -I${MTK_PLAT_SOC}/drivers/                       \
                 -I${MTK_PLAT}/include/lpm/                       \
                 -I${MTK_PLAT_SOC}/drivers/apusys/                \
                 -I${MTK_PLAT_SOC}/drivers/apusys/apusys_rv/2.0   \
                 -I${MTK_PLAT_SOC}/drivers/dcm                    \
                 -I${MTK_PLAT_SOC}/drivers/dfd                    \
                 -I${MTK_PLAT_SOC}/drivers/emi_mpu/               \
                 -I${MTK_PLAT_SOC}/drivers/gpio/                  \
                 -I${MTK_PLAT_SOC}/drivers/iommu/                 \
                 -I${MTK_PLAT_SOC}/drivers/mcdi/                  \
                 -I${MTK_PLAT_SOC}/drivers/pmic/                  \
                 -I${MTK_PLAT_SOC}/drivers/mcupm/                 \
                 -I${MTK_PLAT_SOC}/drivers/sspm/                  \
                 -I${MTK_PLAT_SOC}/drivers/dpm/                   \
                 -I${MTK_PLAT_SOC}/drivers/spmc/                  \
                 -I${MTK_PLAT_SOC}/drivers/ptp3/                  \
                 -I${MTK_PLAT_SOC}/drivers/devapc/                \
                 -I${MTK_PLAT_SOC}/drivers/eint/                  \
                 -I${MTK_PLAT_SOC}/include/

GICV3_SUPPORT_GIC600        :=      1
include drivers/arm/gic/v3/gicv3.mk
include lib/xlat_tables_v2/xlat_tables.mk

PLAT_BL_COMMON_SOURCES := ${GICV3_SOURCES}                              \
                          ${XLAT_TABLES_LIB_SRCS}                       \
                          lib/cpus/aarch64/cortex_a55.S                 \
                          lib/cpus/aarch64/cortex_a78.S                 \
                          plat/common/aarch64/crash_console_helpers.S   \
                          plat/common/plat_psci_common.c

BL2_SOURCES     += common/desc_image_load.c                           \
                   drivers/delay_timer/delay_timer.c                  \
                   drivers/delay_timer/generic_delay_timer.c          \
                   drivers/io/io_storage.c                            \
                   drivers/io/io_block.c                              \
                   drivers/io/io_fip.c                                \
                   drivers/partition/gpt.c                            \
                   drivers/partition/partition.c                      \
                   drivers/ti/uart/aarch64/16550_console.S            \
                   lib/libc/memset.c                                  \
                   ${MTK_PLAT}/common/mtk_plat_common.c               \
                   ${MTK_PLAT}/drivers/rtc/rtc_common.c               \
                   ${MTK_PLAT}/drivers/uart/uart.c                    \
                   ${MTK_PLAT}/drivers/wdt/wdt.c                      \
                   ${MTK_PLAT}/drivers/wdt/mt8195/wdt_platform.c      \
                   ${MTK_PLAT_SOC}/aarch64/plat_helpers.S             \
                   ${MTK_PLAT_SOC}/aarch64/platform_common.c          \
                   ${MTK_PLAT_SOC}/bl2_plat_setup.c                   \
                   ${MTK_PLAT_SOC}/drivers/pll/pll.c                  \
                   ${MTK_PLAT_SOC}/drivers/pll/spm_mtcmos.c           \
                   ${MTK_PLAT_SOC}/drivers/pmic/pmic_wrap_init.c      \
                   ${MTK_PLAT_SOC}/drivers/pmic/pmic_initial_setting.c\

ifeq (${STORAGE_UFS},1)
BL2_SOURCES     += drivers/ufs/ufs.c                                  \
                   ${MTK_PLAT}/common/drivers/ufs/mtk-ufs.c
PLAT_PARTITION_BLOCK_SIZE := 4096
$(eval $(call add_define,STORAGE_UFS))
else
ifeq (${STORAGE_NOR},1)
BL2_SOURCES     += drivers/mtd/nor/spi_nor.c                          \
                   drivers/mtd/spi-mem/spi_mem.c                      \
                   ${MTK_PLAT}/common/drivers/nor/mtk-snfc.c          \
                   ${MTK_PLAT_SOC}/drivers/snfc/mtk_snfc_plat.c
PLAT_PARTITION_BLOCK_SIZE := 4096
$(eval $(call add_define,STORAGE_NOR))
include lib/libfdt/libfdt.mk
else
BL2_SOURCES     += drivers/mmc/mmc.c                                  \
                   ${MTK_PLAT}/common/drivers/mmc/mtk-sd.c
PLAT_PARTITION_BLOCK_SIZE := 512
endif
endif

MODULES-BL2-y += ${MTK_PLAT}/common/drivers/blkdev

$(eval $(call add_define,PLAT_PARTITION_BLOCK_SIZE))

BL2_LIBS += ${LIBDRAM} \
            ${LIBBASE}

ifeq (${PLAT_HW_CRYPTO},1)
BL2_LIBS += ${MTK_PLAT}/lib/crypt/libarmcrypt.a
$(eval $(call add_define,PLAT_HW_CRYPTO))
endif

ifeq (${PLAT_AB_BOOT_ENABLE},1)
include lib/zlib/zlib.mk
BL2_SOURCES     += ${MTK_PLAT}/common/mtk_ab.c                        \
                   $(ZLIB_SOURCES)                                    \
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

ifeq (${STORAGE_APPEND_FIP},1)
$(eval $(call add_define,STORAGE_APPEND_FIP))
$(eval $(call add_define_val,STORAGE_FIP_OFFSET,${STORAGE_FIP_OFFSET}))
$(eval $(call add_define_val,STORAGE_BOOT_LENGTH,${STORAGE_BOOT_LENGTH}))
endif

BL31_SOURCES += common/desc_image_load.c                              \
                drivers/delay_timer/delay_timer.c                     \
                drivers/gpio/gpio.c                                   \
                drivers/delay_timer/generic_delay_timer.c             \
                drivers/ti/uart/aarch64/16550_console.S               \
                lib/bl_aux_params/bl_aux_params.c                     \
                plat/common/plat_gicv3.c                              \
                ${MTK_PLAT}/common/mtk_plat_common.c                  \
                ${MTK_PLAT}/common/mtk_rgu.c                          \
                ${MTK_PLAT}/common/mtk_sip_svc.c                      \
                ${MTK_PLAT}/common/params_setup.c                     \
                ${MTK_PLAT}/common/lpm/mt_lp_rm.c                     \
                ${MTK_PLAT}/drivers/apusys/secure_boot/apusys_secure_boot.c \
                ${MTK_PLAT}/drivers/cirq/mt_cirq.c                    \
                ${MTK_PLAT}/drivers/dp/mt_dp.c                        \
                ${MTK_PLAT}/drivers/gic600/mt_gic_v3.c                \
                ${MTK_PLAT}/drivers/gpio/mtgpio_common.c              \
                ${MTK_PLAT}/drivers/iommu/mtk_iommu_ptbl.c            \
                ${MTK_PLAT}/drivers/iommu/mtk_iommu_smc.c             \
                ${MTK_PLAT}/drivers/pmic/pmic.c                       \
                ${MTK_PLAT}/drivers/pmic_wrap/pmic_wrap_init_v2.c     \
                ${MTK_PLAT}/drivers/ptp3/ptp3_common.c                \
                ${MTK_PLAT}/drivers/rtc/rtc_common.c                  \
                ${MTK_PLAT}/drivers/rtc/rtc_mt6359p.c                 \
                ${MTK_PLAT}/drivers/timer/mt_timer.c                  \
                ${MTK_PLAT}/drivers/uart/uart.c                       \
                ${MTK_PLAT_SOC}/aarch64/platform_common.c             \
                ${MTK_PLAT_SOC}/aarch64/plat_helpers.S                \
                ${MTK_PLAT_SOC}/bl31_plat_setup.c                     \
                ${MTK_PLAT_SOC}/drivers/apusys/apusys.c                   \
                ${MTK_PLAT_SOC}/drivers/apusys/apusys_devapc.c            \
                ${MTK_PLAT_SOC}/drivers/apusys/apusys_regdump.c           \
                ${MTK_PLAT_SOC}/drivers/apusys/apusys_rv/2.0/apusys_rv.c  \
                ${MTK_PLAT_SOC}/drivers/dcm/mtk_dcm.c                 \
                ${MTK_PLAT_SOC}/drivers/dcm/mtk_dcm_utils.c           \
                ${MTK_PLAT_SOC}/drivers/dfd/plat_dfd.c                \
                ${MTK_PLAT_SOC}/drivers/emi_mpu/emi_mpu.c             \
                ${MTK_PLAT_SOC}/drivers/gpio/mtgpio.c                 \
                ${MTK_PLAT_SOC}/drivers/iommu/mtk_iommu_plat.c        \
                ${MTK_PLAT_SOC}/drivers/mcdi/mt_cpu_pm.c              \
                ${MTK_PLAT_SOC}/drivers/mcdi/mt_cpu_pm_cpc.c          \
                ${MTK_PLAT_SOC}/drivers/mcdi/mt_mcdi.c                \
                ${MTK_PLAT_SOC}/drivers/mcdi/mt_lp_irqremain.c        \
                ${MTK_PLAT_SOC}/drivers/gpio/mtgpio.c                 \
                ${MTK_PLAT_SOC}/drivers/mcupm/mt_mcupm.c	      \
                ${MTK_PLAT_SOC}/drivers/sspm/mt_sspm.c		      \
                ${MTK_PLAT_SOC}/drivers/dpm/mt_dpm.c		      \
                ${MTK_PLAT_SOC}/drivers/dpm/mt_dpm_4ch.c	      \
                ${MTK_PLAT_SOC}/drivers/spmc/mtspmc.c                 \
                ${MTK_PLAT_SOC}/drivers/devapc/devapc.c               \
                ${MTK_PLAT_SOC}/drivers/l2c/l2c.c                     \
                ${MTK_PLAT_SOC}/drivers/eint/eint_event.c             \
                ${MTK_PLAT_SOC}/plat_pm.c                             \
                ${MTK_PLAT_SOC}/plat_sip_calls.c                      \
                ${MTK_PLAT_SOC}/plat_topology.c

BL31_LIBS += ${LIBBASE}
BL31_LIBS += ${MTK_PLAT}/drivers/apusys/secure_boot/lib/sec.a
#$(eval LDLIBS += ${MTK_PLAT}/drivers/apusys/secure_boot/lib/sec.a)

# Build SPM drivers
include ${MTK_PLAT_SOC}/drivers/spm/build.mk

include plat/mediatek/build_helpers/mtk_build_helpers_epilogue.mk

include lib/coreboot/coreboot.mk
include ${MTK_PLAT}/secure-boot.mk

# Use the EFUSE HWID as HUK
# Warning: Unsafe for secure storage
ifeq (${HUK_EFUSE_HWID},1)
TF_CFLAGS += -DHUK_EFUSE_HWID
endif
