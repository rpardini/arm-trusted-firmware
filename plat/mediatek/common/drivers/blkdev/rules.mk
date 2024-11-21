#
# Copyright (c) 2024, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LOCAL_DIR := $(call GET_LOCAL_DIR)

MODULE := blkdev

PLAT_INCLUDES += -I${LOCAL_DIR}

LOCAL_SRCS-y := $(LOCAL_DIR)/blkdev-mmc.c
LOCAL_SRCS-$(or $(if $(STORAGE_UFS),y), $(CONFIG_MTK_UFS_SUPPORT)) += $(LOCAL_DIR)/blkdev-ufs.c
LOCAL_SRCS-$(or $(if $(STORAGE_NOR),y), $(CONFIG_MTK_NOR_SUPPORT)) += $(LOCAL_DIR)/blkdev-nor.c

$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))
