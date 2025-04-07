#
# Copyright (c) 2025, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LOCAL_DIR := $(call GET_LOCAL_DIR)

MODULE := mtk-ufs

PLAT_INCLUDES += -I${LOCAL_DIR}

LOCAL_SRCS-y := $(LOCAL_DIR)/mtk-ufs.c \
		drivers/ufs/ufs.c

$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))
