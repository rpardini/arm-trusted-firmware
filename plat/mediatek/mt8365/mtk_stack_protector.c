/*
 * Copyright (c) 2017-2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>

#include <lib/utils.h>
#include <lib/utils_def.h>
#include <common/debug.h>
#include <rng.h>

u_register_t plat_get_stack_protector_canary(void)
{
	size_t i;
	u_register_t ret = 0U;

	if(!plat_get_rnd(&ret))
		return ret;
	else
		return 0U;
}
