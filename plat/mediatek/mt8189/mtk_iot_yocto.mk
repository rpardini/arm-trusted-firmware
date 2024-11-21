#
# Copyright (c) 2025, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

ifeq (${MTK_IOT_YOCTO},1)

# TODO: remove after libbase done
HUK_EFUSE_HWID := 1
$(eval $(call add_define,HUK_EFUSE_HWID))
BL31_SOURCES += ${MTK_PLAT}/common/mtk_hwid.c \
#

endif
