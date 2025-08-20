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

ifeq ($(CONFIG_MTK_APUSYS_COMMON), y)
	PLAT_INCLUDES += -I${MTK_PLAT}/drivers/apusys/apusys_rv/${MTK_APUSYS_RV_VERSION}
endif

$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))

#Include sub rules.mk
ifeq ($(CONFIG_MTK_APUSYS_COMMON), y)
	SUB_RULES-${CONFIG_MTK_APUSYS} := ${LOCAL_DIR}/${MTK_SOC}
	SUB_RULES-${CONFIG_MTK_APUSYS_RV} += $(LOCAL_DIR)/apusys_rv/$(MTK_APUSYS_RV_VERSION)
	SUB_RULES-${CONFIG_MTK_APUSYS_DEVAPC} += $(LOCAL_DIR)/devapc
	SUB_RULES-${CONFIG_MTK_APUSYS_SEC_CTRL} += $(LOCAL_DIR)/security_ctrl/$(MTK_APUSYS_SEC_CTRL_VERSION)
	SUB_RULES-${CONFIG_MTK_APUSYS_AOV} += $(LOCAL_DIR)/apusys_aov
else
	SUB_RULES-$(CONFIG_MTK_APUSYS) += $(LOCAL_DIR)/devapc
endif

#Expand sub rules.mk
$(eval $(call INCLUDE_MAKEFILE,$(SUB_RULES-y)))