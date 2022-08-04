#
# Copyright (c) 2022, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

ifneq (${TRUSTED_BOARD_BOOT},0)

include drivers/auth/mbedtls/mbedtls_crypto.mk
include drivers/auth/mbedtls/mbedtls_x509.mk

AUTH_SOURCES :=	drivers/auth/auth_mod.c 		\
				drivers/auth/crypto_mod.c 		\
				drivers/auth/img_parser_mod.c 	\
				drivers/auth/tbbr/tbbr_cot_common.c

BL2_SOURCES += ${AUTH_SOURCES} 					\
				plat/common/tbbr/plat_tbbr.c 	\
				${MTK_PLAT}/common/mtk_tbbr.c 		\
				${MTK_PLAT}/common/mtk_rotpk.S 	\
				drivers/auth/tbbr/tbbr_cot_bl2.c

ROT_KEY = $(BUILD_PLAT)/rot_key.pem
ROTPK_HASH = $(BUILD_PLAT)/rotpk_sha256.bin

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
