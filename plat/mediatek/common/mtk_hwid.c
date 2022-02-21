/*
 * Copyright (c) 2022, BayLibre
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <mtk_hwid.h>

/* stub function */
int mtk_get_hwid(uint64_t io_base, uint32_t** hardware_id, uint8_t *hwid_size)
{
	*hwid_size = 0;

	return 0;
}
