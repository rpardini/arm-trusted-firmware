#
# Copyright (c) 2019, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

MTK_PLAT      := plat/mediatek
MTK_PLAT_SOC  := ${MTK_PLAT}/${PLAT}

PLAT_INCLUDES := -I.                                              \
                 -Idrivers/arm/gic/v3/                            \
                 -Iinclude/bl31/                                  \
                 -Iinclude/common/                                \
                 -Iinclude/drivers/                               \
                 -Iinclude/drivers/arm/                           \
                 -Iinclude/lib/                                   \
                 -Iinclude/lib/el3_runtime/                       \
                 -Iinclude/plat/common                            \
                 -I${MTK_PLAT}/common/                            \
                 -I${MTK_PLAT}/common/drivers/                    \
                 -I${MTK_PLAT}/common/drivers/timer/              \
                 -I${MTK_PLAT_SOC}/drivers/                       \
                 -I${MTK_PLAT_SOC}/drivers/devapc/                \
                 -I${MTK_PLAT_SOC}/drivers/mcdi/                  \
                 -I${MTK_PLAT_SOC}/drivers/spmc/                  \
                 -I${MTK_PLAT_SOC}/drivers/uart/                  \
                 -I${MTK_PLAT_SOC}/drivers/rtc/                  \
                 -I${MTK_PLAT_SOC}/drivers/pmic/                  \
                 -I${MTK_PLAT_SOC}/drivers/spm/                   \
                 -I${MTK_PLAT_SOC}/drivers/spmc/                  \
                 -I${MTK_PLAT_SOC}/drivers/sspm/                  \
		 -I${MTK_PLAT_SOC}/drivers/rng/                  \
                 -I${MTK_PLAT_SOC}/include/

include drivers/arm/gic/v3/gicv3.mk
include lib/xlat_tables_v2/xlat_tables.mk

PLAT_BL_COMMON_SOURCES := ${GICV3_SOURCES}                              \
                          ${XLAT_TABLES_LIB_SRCS}                       \
                          plat/common/aarch64/crash_console_helpers.S   \
                          plat/common/plat_psci_common.c                \
                          plat/common/plat_gicv3.c			\
			  ${MTK_PLAT_SOC}/drivers/rng/rng.c

ifneq (${ENABLE_STACK_PROTECTOR},0)
PLAT_BL_COMMON_SOURCES	+=	${MTK_PLAT_SOC}/mtk_stack_protector.c
endif

BL2_SOURCES     += common/desc_image_load.c                              \
                   drivers/delay_timer/delay_timer.c                     \
                   drivers/delay_timer/generic_delay_timer.c             \
                   drivers/io/io_storage.c                               \
                   drivers/io/io_block.c                                 \
                   drivers/io/io_fip.c                                   \
                   drivers/mmc/mmc.c                                     \
                   drivers/partition/gpt.c                               \
                   drivers/partition/partition.c                         \
                   drivers/ti/uart/aarch64/16550_console.S               \
                   lib/libc/memset.c                                     \
                   lib/cpus/aarch64/cortex_a53.S                         \
                   ${MTK_PLAT}/common/mtk_plat_common.c                  \
                   ${MTK_PLAT}/common/drivers/mmc/mtk-sd.c               \
                   ${MTK_PLAT}/common/drivers/rtc/rtc_common.c           \
                   ${MTK_PLAT_SOC}/aarch64/plat_helpers.S                \
                   ${MTK_PLAT_SOC}/aarch64/platform_common.c             \
                   ${MTK_PLAT_SOC}/bl2_plat_setup.c                      \
                   ${MTK_PLAT_SOC}/drivers/pll/pll.c                     \
                   ${MTK_PLAT_SOC}/drivers/rtc/rtc.c                     \
                   ${MTK_PLAT_SOC}/drivers/pmic/pmic_wrap_init.c

BL2_LIBS += ${LIBDRAM} \
            ${LIBBASE}

BL31_SOURCES    += common/desc_image_load.c                              \
                   drivers/delay_timer/delay_timer.c                     \
                   drivers/delay_timer/generic_delay_timer.c             \
                   drivers/ti/uart/aarch64/16550_console.S               \
                   lib/bl_aux_params/bl_aux_params.c                     \
                   lib/cpus/aarch64/cortex_a53.S                         \
                   ${MTK_PLAT}/common/mtk_plat_common.c                  \
                   ${MTK_PLAT}/common/params_setup.c                     \
                   ${MTK_PLAT}/common/drivers/timer/mt_timer.c           \
                   ${MTK_PLAT}/common/mtk_gic_v3_main.c                  \
                   ${MTK_PLAT_SOC}/plat_mt_cirq.c                        \
                   ${MTK_PLAT}/common/drivers/rtc/rtc_common.c           \
                   ${MTK_PLAT_SOC}/drivers/rtc/rtc.c                     \
                   ${MTK_PLAT_SOC}/drivers/pmic/pmic_wrap_init.c         \
                   ${MTK_PLAT_SOC}/aarch64/platform_common.c             \
                   ${MTK_PLAT_SOC}/aarch64/plat_helpers.S                \
                   ${MTK_PLAT_SOC}/drivers/mcdi/mtk_mcdi.c               \
                   ${MTK_PLAT_SOC}/drivers/mcdi/mtk_mcdi_fw.c            \
                   ${MTK_PLAT_SOC}/drivers/spm/mt_spm.c                  \
                   ${MTK_PLAT_SOC}/drivers/spm/mt_spm_sleep.c            \
                   ${MTK_PLAT_SOC}/drivers/spm/mt_spm_dpidle.c           \
                   ${MTK_PLAT_SOC}/drivers/spm/mt_spm_sodi.c             \
                   ${MTK_PLAT_SOC}/drivers/spm/mt_spm_idle.c             \
                   ${MTK_PLAT_SOC}/drivers/spm/mt_spm_internal.c         \
                   ${MTK_PLAT_SOC}/drivers/spm/mt_spm_vcorefs.c          \
                   ${MTK_PLAT_SOC}/drivers/spm/mt_spm_pmic_wrap.c        \
                   ${MTK_PLAT_SOC}/drivers/spm/mt_spm_mc_dsr.c           \
                   ${MTK_PLAT_SOC}/drivers/spmc/mtspmc.c                 \
                   ${MTK_PLAT_SOC}/drivers/uart/uart.c                   \
                   ${MTK_PLAT_SOC}/plat_mt_gic.c                         \
                   ${MTK_PLAT_SOC}/bl31_plat_setup.c                     \
                   ${MTK_PLAT_SOC}/drivers/pmic/pmic.c                   \
                   ${MTK_PLAT_SOC}/plat_pm.c                             \
                   ${MTK_PLAT_SOC}/plat_topology.c                       \
                   ${MTK_PLAT_SOC}/plat_dfd.c                            \
                   ${MTK_PLAT_SOC}/plat_sip_svc.c                        \
                   ${MTK_PLAT}/common/mtk_sip_svc.c                      \
                   ${MTK_PLAT_SOC}/scu.c

ifndef LIBBASE
BL31_SOURCES    += ${MTK_PLAT}/common/mtk_hwid.c
endif

BL31_LIBS += ${LIBDRAM} ${LIBBASE}

# Enable workarounds for selected Cortex-A53 erratas.
ERRATA_A53_826319 := 0
ERRATA_A53_836870 := 1
ERRATA_A53_855873 := 1

BL2_AT_EL3 := 1

# indicate the reset vector address can be programmed
PROGRAMMABLE_RESET_ADDRESS := 1
USE_COHERENT_MEM := 0

COLD_BOOT_SINGLE_CPU := 1

MULTI_CONSOLE_API := 1

MACH_MT8365 := 1
$(eval $(call add_define,MACH_MT8365))

ifneq (${TRUSTED_BOARD_BOOT},0)

include drivers/auth/mbedtls/mbedtls_crypto.mk
include drivers/auth/mbedtls/mbedtls_x509.mk

AUTH_SOURCES		:=	drivers/auth/auth_mod.c			\
				drivers/auth/crypto_mod.c		\
				drivers/auth/img_parser_mod.c		\
				drivers/auth/tbbr/tbbr_cot_common.c

BL2_SOURCES		+=	${AUTH_SOURCES}				\
				plat/common/tbbr/plat_tbbr.c	\
				${MTK_PLAT_SOC}/mtk_tbbr.c	\
				${MTK_PLAT_SOC}/mtk_rotpk.S \
				drivers/auth/tbbr/tbbr_cot_bl2.c

ROT_KEY		=	$(BUILD_PLAT)/rot_key.pem
ROTPK_HASH		=	$(BUILD_PLAT)/rotpk_sha256.bin

$(eval $(call add_define_val,ROTPK_HASH,'"$(ROTPK_HASH)"'))
$(BUILD_PLAT)/bl1/mtk_rotpk.o: $(ROTPK_HASH)
$(BUILD_PLAT)/bl2/mtk_rotpk.o: $(ROTPK_HASH)

certificates: $(ROT_KEY)
$(ROT_KEY): | $(BUILD_PLAT)
	@echo "  OPENSSL $@"
	$(Q)openssl genrsa 2048 > $@ 2>/dev/null

$(ROTPK_HASH): $(ROT_KEY)
	@echo "  OPENSSL $@"
	$(Q)openssl rsa -in $< -pubout -outform DER 2>/dev/null |\
	openssl dgst -sha256 -binary > $@ 2>/dev/null
endif

# Enable dynamic memory mapping
PLAT_XLAT_TABLES_DYNAMIC :=    1

# Enable dynamic memory mapping
$(eval $(call add_define,PLAT_XLAT_TABLES_DYNAMIC))

# Use the EFUSE HWID as HUK
# Warning: Unsafe for secure storage
ifeq (${HUK_EFUSE_HWID},1)
TF_CFLAGS += -DHUK_EFUSE_HWID
endif
