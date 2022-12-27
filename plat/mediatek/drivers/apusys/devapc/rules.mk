#
# Copyright (c) 2021, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

#Prologue, init variable
LOCAL_DIR := $(call GET_LOCAL_DIR)

#Define your module name
MODULE := apusys_devapc

#Add your source code here
LOCAL_SRCS-$(CONFIG_MTK_APUSYS) := ${LOCAL_DIR}/apusys_dapc_v1.c

#Epilogue, build as module
$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))

