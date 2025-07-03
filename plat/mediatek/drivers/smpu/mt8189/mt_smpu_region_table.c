// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (C) 2025 MediaTek Inc.
 */

#include <smpu.h>

const struct smpu_region_info smpu_region_info_table[MAX_REGION_NUM] = {
	{
		/* NSR */
		.region_num = 0,
		.region_perm = {
			{1, RW}, {8, RW},
			{15, RW}, {31, RW}, {33, RW}, {56, RW}, {57, RW}, {65, RW},
			{67, RW}, {68, RW}, {69, RW}, {70, RW}, {71, RW}, {73, RW},
			{81, RW}, {91, RW}, {97, RW}, {98, RW}, {99, RW}, {101, RW},
			{103, RW}, {105, RW}, {107, RW}, {109, RW}, {111, RW}, {113, RW},
			{115, RW}, {119, RW}, {121, RW}, {123, RW}, {125, RW}, {127, RW},
			{129, RW}, {131, RW}, {132, RW}, {133, RW}, {134, RW}, {135, RW},
			{136, RW}, {137, RW}, {139, RW}, {141, RW}, {143, RW}, {145, RW},
			{161, RW}, {163, RW}, {165, RW}, {167, RW}, {169, RW}, {176, RW},
			{177, RW}, {178, RW}, {179, RW}, {184, RW}, {185, RW}, {193, RW},
			{195, RW}, {197, RW}, {199, RW}, {201, RW}, {203, RW}, {205, RW},
			{207, RW}, {209, RW}, {211, RW}, {213, RW}, {215, RW}, {216, RW},
			{217, RW}, {219, RW}, {221, RW}, {223, RW}, {225, RW}, {227, RW},
			{232, RW}, {233, RW}, {234, RW}, {235, RW}, {238, RW}, {239, RW},
			{240, RW}, {241, RW}, {243, RW}, {248, RW}, {249, RW}, {250, RW},
			{255, RW}},
		.region_perm_drm = {{52, READ}, {240, RW}, {241, RW}, {252, RW}},
	},
	{
		/* BL31-reserved */
		.region_num = 1,
		.region_perm = {{8, RW}, {52, RW}, {240, RW}},
		.region_perm_drm = {{240, RW}},
	},
	{
		/* TEE-reserved */
		.region_num = 2,
		.region_perm = {{8, RW}, {52, RW}, {240, RW}},
		.region_perm_drm = {{240, RW}},
	},
	{
		.region_num = 3,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 4,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 5,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 6,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 7,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 8,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 9,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 10,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 11,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 12,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 13,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 14,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 15,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 16,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		/* SSPM-reserved */
		.region_num = 17,
		.region_perm = {{48, RW}},
		.region_perm_drm = {{240, RW}, {241, RW}},
	},
	{
		/* sspm_ap-shared */
		.region_num = 18,
		.region_perm = {{48, RW}, {49, RW}, {240, RW}, {241, RW}},
		.region_perm_drm = {{240, RW}, {241, RW}},
	},
	{
		.region_num = 19,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 20,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 21,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 22,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		/* apu_apusys-rv_secure */
		.region_num = 23,
		.region_perm = {{10, RW}, {28, RW}, {30, RW}, {240, RW}},
		.region_perm_drm = {{240, RW}, {241, RW}},
	},
	{
		/* apu_sapu_apurvgzctrlshm */
		.region_num = 24,
		.region_perm = {{10, RW}, {243, RW}},
		.region_perm_drm = {{240, RW}, {241, RW}},
	},
	{
		.region_num = 25,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 26,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 27,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 28,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 29,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 30,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 31,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 32,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 33,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 34,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 35,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 36,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 37,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 38,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 39,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 40,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 41,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 42,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 43,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 44,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 45,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 46,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 47,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 48,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 49,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 50,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 51,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 52,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 53,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 54,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 55,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 56,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 57,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 58,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 59,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 60,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		.region_num = 61,
		.region_perm = {},
		.region_perm_drm = {},
	},
	{
		/* consys_wifi_emi */
		.region_num = 62,
		.region_perm = {{81, RW}, {241, READ}},
		.region_perm_drm = {{240, RW}, {241, RW}},
	},
	{
		/* debug_met_res-ram */
		.region_num = 63,
		.region_perm = {},
		.region_perm_drm = {},
	},
};
