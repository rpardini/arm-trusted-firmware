#
# Copyright (c) 2025, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

#Prologue, init variable
LOCAL_DIR := $(call GET_LOCAL_DIR)

#Define your module name
MODULE := apusys_${MTK_SOC}

PLAT_INCLUDES += -I${MTK_PLAT}/drivers/spm/${MTK_SOC}

#Add your source code here
LOCAL_SRCS-$(CONFIG_MTK_APUSYS) := ${LOCAL_DIR}/apusys_shared_mmap.c
LOCAL_SRCS-$(CONFIG_MTK_APUSYS_DEVAPC) += ${LOCAL_DIR}/apusys_devapc.c
LOCAL_SRCS-$(CONFIG_MTK_APUSYS_PWR) += ${LOCAL_DIR}/apusys_power.c
LOCAL_SRCS-$(CONFIG_MTK_APUSYS_KERNEL_LOAD_IMAGE) += ${LOCAL_DIR}/apupw.c
LOCAL_SRCS-$(CONFIG_MTK_APUSYS_SEC_CTRL) += ${LOCAL_DIR}/apusys_security_ctrl_plat.c
LOCAL_SRCS-$(CONFIG_MTK_APUSYS_SEC_CTRL) += ${LOCAL_DIR}/apusys_security_ctrl_perm_plat.c
LOCAL_SRCS-$(CONFIG_MTK_APUSYS_AMMU) += ${LOCAL_DIR}/apusys_ammu.c
ifeq ($(CONFIG_MTK_APUSYS_RV), y)
LOCAL_SRCS-$(CONFIG_MTK_APUSYS_REG_DUMP) += ${LOCAL_DIR}/apusys_regdump.c
endif
ifeq ($(CONFIG_MTK_APUSYS_CE_SUPPORT), y)
LOCAL_SRCS-$(CONFIG_MTK_APUSYS_CE_SUPPORT) += ${LOCAL_DIR}/apusys_ce.c
endif
LOCAL_SRCS-$(CONFIG_MTK_APUSYS_RV) += ${LOCAL_DIR}/apusys_rv_pwr_ctl.c
#Epilogue, build as module
$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))

