#
# Copyright (c) 2022, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LOCAL_DIR := $(call GET_LOCAL_DIR)

MODULE := apusys_${MTK_SOC}
LOCAL_SRCS-$(CONFIG_MTK_APUSYS) := $(LOCAL_DIR)/apusys_devapc.c
LOCAL_SRCS-$(CONFIG_MTK_APUSYS) += $(LOCAL_DIR)/apusys_power.c
LOCAL_SRCS-$(CONFIG_MTK_APUSYS) += $(LOCAL_DIR)/apusys_regdump.c
LOCAL_SRCS-$(CONFIG_MTK_APUSYS) += $(LOCAL_DIR)/apusys_security_ctrl_plat.c

MTK_APUSYS_RV_VERSION := 2.0

PLAT_INCLUDES += -I${LOCAL_DIR}
PLAT_INCLUDES += -I$(MTK_PLAT)/drivers/apusys/devapc

$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))

#Include sub rules.mk
SUB_RULES-$(CONFIG_MTK_APUSYS) += $(LOCAL_DIR)/apusys_rv/$(MTK_APUSYS_RV_VERSION)

#Expand sub rules.mk
$(eval $(call INCLUDE_MAKEFILE,$(SUB_RULES-y)))