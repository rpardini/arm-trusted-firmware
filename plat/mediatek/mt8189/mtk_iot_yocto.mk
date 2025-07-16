#
# Copyright (c) 2025, MediaTek Inc. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

ifeq (${MTK_IOT_YOCTO},1)

# Use the EFUSE HWID as HUK
# Warning: Unsafe for secure storage
ifeq (${HUK_EFUSE_HWID},1)
$(eval $(call add_define,HUK_EFUSE_HWID))
endif

MODULES-y += $(MTK_PLAT_SOC)/drivers/binfo
MODULES-y += $(MTK_PLAT_SOC)/drivers/cpudvfs
MODULES-y += $(MTK_PLAT_SOC)/drivers/spmfw
MODULES-y += $(MTK_PLAT_SOC)/drivers/sspmfw
MODULES-y += $(MTK_PLAT_SOC)/drivers/dpmfw
MODULES-y += $(MTK_PLAT_SOC)/drivers/mcupmfw

endif
