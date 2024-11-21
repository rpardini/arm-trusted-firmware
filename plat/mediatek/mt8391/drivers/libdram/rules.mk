#
# Copyright (c) 2024, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LOCAL_DIR := $(call GET_LOCAL_DIR)

MODULE := libdram
PLAT_INCLUDES += -I${LOCAL_DIR}/
LDLIBS += ${LIBDRAM}
