#
# Copyright (c) 2025, Grinn sp. z o.o. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LOCAL_DIR := $(call GET_LOCAL_DIR)

MODULE := libdram

PLAT_INCLUDES += -I${LOCAL_DIR}/
$(eval $(call MAKE_MODULE,$(MODULE),$(LOCAL_SRCS-y),$(MTK_BL)))
