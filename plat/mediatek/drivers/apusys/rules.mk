#
# Copyright (c) 2022, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LOCAL_DIR := $(call GET_LOCAL_DIR)

MODULE := apusys
LOCAL_SRCS-y := $(LOCAL_DIR)/apusys.c \
				${LOCAL_DIR}/secure_boot/apusys_secure_boot.c

#Add your library here
#LDLIBS needs to be evaluated right away
$(eval LDLIBS += ${LOCAL_DIR}/secure_boot/lib/sec.a)

PLAT_INCLUDES += -I${LOCAL_DIR} \
				 -I${LOCAL_DIR}/secure_boot\
				 -I${LOCAL_DIR}/secure_boot/include

$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))

#Include sub rules.mk
SUB_RULES-$(CONFIG_MTK_APUSYS) += $(LOCAL_DIR)/devapc

#Expand sub rules.mk
$(eval $(call INCLUDE_MAKEFILE,$(SUB_RULES-y)))