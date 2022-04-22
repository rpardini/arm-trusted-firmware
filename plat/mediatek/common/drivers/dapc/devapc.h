/*
 * Copyright (c) 2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef COMMON_DEVAPC_H
#define COMMON_DEVAPC_H

#include <stdint.h>

#define DEVAPC_INFRA_D0_APC_0     (DEVAPC_AO_INFRA_BASE + 0x0000)
#define DEVAPC_INFRA_MAS_DOM_0    (DEVAPC_AO_INFRA_BASE + 0x0A00)
#define DEVAPC_INFRA_MAS_SEC_0    (DEVAPC_AO_INFRA_BASE + 0x0B00)
#define DEVAPC_INFRA_DOM_RMP_0    (DEVAPC_AO_INFRA_BASE + 0x0D00)
#define DEVAPC_INFRA_DOM_RMP_1    (DEVAPC_AO_INFRA_BASE + 0x0D04)
#define DEVAPC_INFRA_APC_CON      (DEVAPC_AO_INFRA_BASE + 0x0F00)

#define DEVAPC_MD_APC_CON         (DEVAPC_AO_MD_BASE + 0x0F00)

#define DEVAPC_MM_D0_APC_0        (DEVAPC_AO_MM_BASE + 0x0000)
#define DEVAPC_MM_DOM_RMP_0       (DEVAPC_AO_MM_BASE + 0x0D00)
#define DEVAPC_MM_APC_CON         (DEVAPC_AO_MM_BASE + 0x0F00)

#define DAPC_MASTER_INDEX_INVALID (0xffffffff)

enum MASK_DOM {
	DOMAIN_0 = 0,
	DOMAIN_1,
	DOMAIN_2,
	DOMAIN_3,
	DOMAIN_4,
	DOMAIN_5,
	DOMAIN_6,
	DOMAIN_7,
	DOMAIN_8,
	DOMAIN_9,
	DOMAIN_10,
	DOMAIN_11,
	DAPC_MAX_DOMAIN
};

enum TRANSACTION {
	NON_SECURE_TRANSACTION = 0,
	SECURE_TRANSACTION
};

enum DAPC_SLAVE_TYPE {
	DAPC_INFRA_SLAVE = 0,
	DAPC_MM_SLAVE
};

enum APC_ATTR {
	NO_SEC = 0,
	S_RW_ONLY,
	S_RW_NS_R,
	FORBID,
};

struct device_info {
	uint8_t permission[DAPC_MAX_DOMAIN];
};

struct master_domain {
	uint32_t master_id;
	uint32_t domain_id;
};

struct dapc_conf {
	uint32_t default_master;

	const struct device_info *infra;
	uint32_t infra_size;

	const struct device_info *mm;
	uint32_t mm_size;

	const struct master_domain *master;
	uint32_t master_size;
};

void devapc_common_init(struct dapc_conf *conf);

#include <plat_devapc.h>

#endif /* COMMON_DEVAPC_H */

