#
# Copyright (c) 2022, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LOCAL_DIR := $(call GET_LOCAL_DIR)

MODULE := 8250_console

PLAT_INCLUDES += -I${LOCAL_DIR}

LOCAL_SRCS-y := $(LOCAL_DIR)/8250_console.S
LOCAL_SRCS-y += $(LOCAL_DIR)/uart.c

$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))
