/*
 * Copyright (c) 2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <common/debug.h>
#include <devapc.h>
#include <drivers/console.h>
#include <lib/mmio.h>

#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))

static void set_master_transaction(uint32_t master_index,
				   enum TRANSACTION transaction_type)
{
	uintptr_t base;
	uint32_t master_register_index;
	uint32_t master_set_index;
	uint32_t set_bit;

	master_register_index = master_index / (MOD_NO_IN_1_DEVAPC * 2);
	master_set_index = master_index % (MOD_NO_IN_1_DEVAPC * 2);

	base = DEVAPC_INFRA_MAS_SEC_0 + master_register_index * 4;

	set_bit = 0x1 << master_set_index;
	if (transaction_type == SECURE_TRANSACTION)
		mmio_setbits_32(base, set_bit);
	else
		mmio_clrbits_32(base, set_bit);
}

static void set_master_domain(uint32_t master_index, enum MASK_DOM domain)
{
	uintptr_t base;
	uint32_t domain_reg;
	uint32_t domain_index;
	uint32_t clr_bit;
	uint32_t set_bit;

	domain_reg = master_index / MASTER_MOD_NO_IN_1_DEVAPC;
	domain_index = master_index % MASTER_MOD_NO_IN_1_DEVAPC;
	clr_bit = 0xF << (4 * domain_index);
	set_bit = domain << (4 * domain_index);

	base = DEVAPC_INFRA_MAS_DOM_0 + domain_reg * 4;
	mmio_clrsetbits_32(base, clr_bit, set_bit);
}

static void set_master_domain_remap_infra(enum MASK_DOM domain_emi_view,
					enum MASK_DOM domain_infra_view)
{
	uintptr_t base;
	uint32_t clr_bit;
	uint32_t set_bit;

	if (domain_emi_view < DOMAIN_10) {
		base = DEVAPC_INFRA_DOM_RMP_0;
		clr_bit = 0x7 << (domain_emi_view * 3);
		set_bit = domain_infra_view << (domain_emi_view * 3);
		mmio_clrsetbits_32(base, clr_bit, set_bit);
	} else if (domain_emi_view > DOMAIN_10) {
		base = DEVAPC_INFRA_DOM_RMP_1;
		domain_emi_view = domain_emi_view - DOMAIN_11;
		clr_bit = 0x7 << (domain_emi_view * 3 + 1);
		set_bit = domain_infra_view << (domain_emi_view * 3 + 1);
		mmio_clrsetbits_32(base, clr_bit, set_bit);
	} else {
		base = DEVAPC_INFRA_DOM_RMP_0;
		clr_bit = 0x3 << (domain_emi_view * 3);
		set_bit = domain_infra_view << (domain_emi_view * 3);
		mmio_clrsetbits_32(base, clr_bit, set_bit);

		base = DEVAPC_INFRA_DOM_RMP_1;
		set_bit = (domain_infra_view & 0x4) >> 2;
		mmio_clrsetbits_32(base, 0x1, set_bit);
	}
}

static void set_master_domain_remap_mm(enum MASK_DOM domain_emi_view,
					enum MASK_DOM domain_mm_view)
{
	uintptr_t base;
	uint32_t clr_bit;
	uint32_t set_bit;

	base = DEVAPC_MM_DOM_RMP_0;
	clr_bit = 0x3 << (domain_emi_view * 2);
	set_bit = domain_mm_view << (domain_emi_view * 2);

	mmio_clrsetbits_32(base, clr_bit, set_bit);
}

static void set_module_apc(enum DAPC_SLAVE_TYPE slave_type, uint32_t module,
			   enum MASK_DOM domain_num,
			   enum APC_ATTR permission_control)
{
	uintptr_t base;
	uint32_t apc_index;
	uint32_t apc_set_index;
	uint32_t clr_bit;
	uint32_t set_bit;

	apc_index = module / MOD_NO_IN_1_DEVAPC;
	apc_set_index = module % MOD_NO_IN_1_DEVAPC;
	clr_bit = 0x3 << (apc_set_index * 2);
	set_bit = permission_control << (apc_set_index * 2);

	if (slave_type == DAPC_INFRA_SLAVE && module <= SLAVE_INFRA_MAX_INDEX)
		base = DEVAPC_INFRA_D0_APC_0 + domain_num * 0x100 +
					       apc_index * 4;
	else if (slave_type == DAPC_MM_SLAVE && module <= SLAVE_MM_MAX_INDEX)
		base = DEVAPC_MM_D0_APC_0 + domain_num * 0x100 + apc_index * 4;
	else
		return;

	mmio_clrsetbits_32(base, clr_bit, set_bit);
}

static void set_default_master_transaction(uint32_t master)
{
	set_master_transaction(master, SECURE_TRANSACTION);
}

static void set_default_master_domain(const struct master_domain *master,
				      uint32_t master_size)
{
	uint32_t i;
	for (i = 0; i < master_size; i++) {
		uint32_t domain = master[i].domain_id;
		set_master_domain(master[i].master_id, domain);
		set_master_domain_remap_infra(domain, domain);
		set_master_domain_remap_mm(domain, domain);
	}
}

static void set_default_slave_permission(const struct device_info *infra,
					 uint32_t infra_size,
					 const struct device_info *mm,
					 uint32_t mm_size)
{
	uint32_t module_index;
	uint32_t domain_index;
	uint8_t permission;

	for (module_index = 0; module_index < infra_size; module_index++) {
		for (domain_index = 0; domain_index < DAPC_MAX_DOMAIN; domain_index++) {
			permission = infra[module_index].permission[domain_index];
			if (permission > 0) {
				set_module_apc(DAPC_INFRA_SLAVE, module_index,
					       domain_index, permission);
			}
		}
	}

	for (module_index = 0; module_index < mm_size; module_index++) {
		for (domain_index = 0; domain_index < DAPC_MAX_DOMAIN; domain_index++) {
			permission = mm[module_index].permission[domain_index];
			if (permission > 0) {
				set_module_apc(DAPC_INFRA_SLAVE, module_index,
					       domain_index, permission);
			}
		}
	}
}

static void dump_devapc(uint32_t infra_size, uint32_t mm_size)
{
	int i;

	INFO("[DEVAPC] dump DEVAPC registers:\n");

	for (i = 0; i < DIV_ROUND_UP(infra_size, MOD_NO_IN_1_DEVAPC); i++) {
		INFO("[DEVAPC] (INFRA)D0_APC_%d = 0x%x, "
			       "(INFRA)D1_APC_%d = 0x%x, "
			       "(INFRA)D2_APC_%d = 0x%x\n",
		i, mmio_read_32(DEVAPC_INFRA_D0_APC_0 + i * 4),
		i, mmio_read_32(DEVAPC_INFRA_D0_APC_0 + 0x100 + i * 4),
		i, mmio_read_32(DEVAPC_INFRA_D0_APC_0 + 0x200 + i * 4));
	}

	for (i = 0; i < DIV_ROUND_UP(mm_size, MOD_NO_IN_1_DEVAPC); i++) {
		INFO("[DEVAPC] (MM)D0_APC_%d = 0x%x, "
			       "(MM)D1_APC_%d = 0x%x, "
			       "(MM)D2_APC_%d = 0x%x\n",
		i, mmio_read_32(DEVAPC_MM_D0_APC_0 + i * 4),
		i, mmio_read_32(DEVAPC_MM_D0_APC_0 + 0x100 + i * 4),
		i, mmio_read_32(DEVAPC_MM_D0_APC_0 + 0x200 + i * 4));
	}

	for (i = 0; i < DIV_ROUND_UP(DAPC_MASTER_INDEX_MAX, MASTER_MOD_NO_IN_1_DEVAPC); i++) {
		INFO("[DEVAPC] MAS_DOM_%d = 0x%x\n", i,
			mmio_read_32(DEVAPC_INFRA_MAS_DOM_0 + i * 4));
	}

	INFO("[DEVAPC] MAS_SEC_0 = 0x%x\n",
			mmio_read_32(DEVAPC_INFRA_MAS_SEC_0));

	INFO("[DEVAPC]  (INFRA)MAS_DOMAIN_REMAP_0 = 0x%x, "
			"(INFRA)MAS_DOMAIN_REMAP_1 = 0x%x\n",
			mmio_read_32(DEVAPC_INFRA_DOM_RMP_0),
			mmio_read_32(DEVAPC_INFRA_DOM_RMP_1));

	INFO("[DEVAPC]  (MM)MAS_DOMAIN_REMAP_0 = 0x%x\n",
			mmio_read_32(DEVAPC_MM_DOM_RMP_0));
}

void devapc_common_init(struct dapc_conf *conf)
{
	mmio_write_32(DEVAPC_INFRA_APC_CON, 0x80000001);
	mmio_write_32(DEVAPC_MM_APC_CON, 0x80000001);
	mmio_write_32(DEVAPC_MD_APC_CON, 0x80000001);

	set_default_master_transaction(conf->default_master);
	set_default_master_domain(conf->master, conf->master_size);
	set_default_slave_permission(conf->infra, conf->infra_size,
				     conf->mm, conf->mm_size);

	dump_devapc(conf->infra_size, conf->mm_size);
}

