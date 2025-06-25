#
# Copyright (c) 2025, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LOCAL_DIR := $(call GET_LOCAL_DIR)

MODULE := mcupm
LOCAL_SRCS-y := $(LOCAL_DIR)/mt_mcupm.c
LOCAL_SRCS-y += $(LOCAL_DIR)/mcupm_bin.S

PLAT_INCLUDES += -I${LOCAL_DIR}

$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))
