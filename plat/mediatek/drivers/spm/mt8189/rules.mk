#
# Copyright (c) 2025, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

#Prologue, init variable
LOCAL_DIR := $(call GET_LOCAL_DIR)

#Define your module name
MODULE := spm_${MTK_SOC}

MT_SPM_FEATURE_SUPPORT := y

#Add your source code here
LOCAL_SRCS-${MT_SPM_FEATURE_SUPPORT} += ${LOCAL_DIR}/mt_spm_hwreq.c

#Epilogue, build as module
$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))
