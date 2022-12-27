#
# Copyright (c) 2022, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

#Prologue, init variable
LOCAL_DIR := $(call GET_LOCAL_DIR)

#Define your module name
MODULE := apusys_rv_${MTK_SOC}

PLAT_INCLUDES += -I${LOCAL_DIR}
PLAT_INCLUDES += -I$(MTK_PLAT)/drivers/iommu/${MTK_SOC}
PLAT_INCLUDES += -I$(MTK_PLAT)/drivers/emi_mpu

#Add your source code here
LOCAL_SRCS-$(CONFIG_MTK_APUSYS) := ${LOCAL_DIR}/apusys_rv.c

#Epilogue, build as module
$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))
