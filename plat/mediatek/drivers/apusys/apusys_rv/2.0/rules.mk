#
# Copyright (c) 2025, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

#Prologue, init variable
LOCAL_DIR := $(call GET_LOCAL_DIR)

#Define your module name
MODULE := apusys_rv_${MTK_SOC}

PLAT_INCLUDES += -I${MTK_PLAT}/drivers/apusys/${MTK_SOC}

#Add your source code here
LOCAL_SRCS-$(CONFIG_MTK_APUSYS_RV) := ${LOCAL_DIR}/apusys_rv.c
LOCAL_SRCS-$(CONFIG_MTK_APUSYS_KERNEL_LOAD_IMAGE) += ${LOCAL_DIR}/apusys_rv_load_img.c

#Epilogue, build as module
$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))

