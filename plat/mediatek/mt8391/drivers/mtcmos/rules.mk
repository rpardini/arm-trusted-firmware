#
# Copyright (c) 2024, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LOCAL_DIR := $(call GET_LOCAL_DIR)

MODULE := mt8189_mtcmos
LOCAL_SRCS-y := $(LOCAL_DIR)/spm_mtcmos.c

PLAT_INCLUDES += -I${LOCAL_DIR}/include

$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))
