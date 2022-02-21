/*
 * Copyright (c) 2022, BayLibre
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MTK_HWID_H
#define MTK_HWID_H

#include <stdint.h>

extern
int mtk_get_hwid(uint64_t io_base, uint32_t** hardware_id, uint8_t *hwid_size);

#endif /* MTK_HWID_H */
