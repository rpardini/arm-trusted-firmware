#
# Copyright (c) 2022, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LOCAL_DIR := $(call GET_LOCAL_DIR)

MODULE := wdt

PLAT_INCLUDES += -I${LOCAL_DIR}

LOCAL_SRCS-y := $(LOCAL_DIR)/wdt.c
LOCAL_SRCS-y += $(LOCAL_DIR)/$(MTK_SOC)/wdt_platform.c

$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))
