/*
 * Copyright (c) 2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __APUSYS_DEVAPC_H__
#define __APUSYS_DEVAPC_H__

#define APUSYS_DAPC_BL31_INIT

/******************************************************************************
 * FUNCTION DEFINITION
 ******************************************************************************/
int32_t start_apusys_devapc_ao(void);
int32_t apusys_devapc_hyp_ctrl(uint64_t x1, uint64_t x2,
				uint64_t x3, uint64_t x4);
int32_t apusys_devapc_sdsp_domain_switch(uint64_t vpu_ns, uint64_t domain,
				uint64_t core_num);
int32_t apusys_devapc_vpu_ctrl_permission_switch(uint64_t to_gz);
int32_t apusys_kernel_reviser_set_boundary(uint32_t mdla,
				uint32_t vpu, uint32_t edma);
int32_t apusys_devapc_apu_power_ctrl_permission_switch(uint64_t to_gz);
int32_t apusys_kernel_set_ao_dbg_sel(uint32_t val);
int32_t apusys_kernel_reviser_chk_value(uint32_t offset, uint32_t *value);
int32_t apusys_kernel_reviser_set_default_iova(uint32_t set_value);
int32_t apusys_kernel_reviser_get_interrupt_status(uint32_t offset,
				uint32_t *value);
int32_t apusys_kernel_reviser_set_context_id(uint32_t offset,
				uint32_t context_id);
int32_t apusys_kernel_reviser_set_remap_table(uint32_t offset,
				uint32_t need_valid, uint32_t set_value);
int32_t apusys_kernel_reviser_init_ip(void);

#define HYP_CTRL_VPU_ACCESS_PERMISSION		1
#define HYP_CTRL_VPU_DOMAIN_NS_BOUNDARY		2
#define HYP_CTRL_APU_POWER_ACCESS_PERMISSION	3

#define MAX_MDLA		2
#define MAX_VPU			2
#define MAX_EDMA		2
#define MAX_EDMA_CH		8

#define VPU_NORMAL_DOMAIN	5
#define VPU_NORMAL_NS		1

#define NS_MASK			0x1
#define NS_OFFSET		0x4
#define DOMAIN_MASK		0xF
#define DOMAIN_OFFSET		0x0
#define BDY_MASK		0x3
#define BDY_OFFSET		0x0
#define CNTX_ID_MASK		0x1F	/* b'25-b'21 */
#define CNTX_ID_OFFSET		0x15	/* BIT21 */
#define VPU_BDY_0		0
#define VPU_BDY_1		1
#define VPU_BDY_2		2
#define VPU_BDY_3		3
#define MIN_VPU_CORE		2
#define MAX_VPU_CORE		3

#define APUSYS_SCTRL_VPU6_CORE0_DOMAIN_NS	((APUSYS_SCTRL_REVISER_BASE)+0x014)
#define APUSYS_SCTRL_VPU6_CORE1_DOMAIN_NS	((APUSYS_SCTRL_REVISER_BASE)+0x01C)
#define APUSYS_SCTRL_VPU6_CORE2_DOMAIN_NS	((APUSYS_SCTRL_REVISER_BASE)+0x024)
#define APUSYS_SCTRL_MDLA_CORE0_CTXT		((APUSYS_SCTRL_REVISER_BASE)+0x108)
#define APUSYS_SCTRL_MDLA_CORE1_CTXT		((APUSYS_SCTRL_REVISER_BASE)+0x10C)
#define APUSYS_SCTRL_VPU6_CORE0_CTXT		((APUSYS_SCTRL_REVISER_BASE)+0x114)
#define APUSYS_SCTRL_VPU6_CORE1_CTXT		((APUSYS_SCTRL_REVISER_BASE)+0x11C)
#define APUSYS_SCTRL_VPU6_CORE2_CTXT		((APUSYS_SCTRL_REVISER_BASE)+0x124)
#define APUSYS_SCTRL_EDMA0_CTXT			((APUSYS_SCTRL_REVISER_BASE)+0x12C)
#define APUSYS_SCTRL_EDMA1_CTXT			((APUSYS_SCTRL_REVISER_BASE)+0x14C)
#define APUSYS_AO_DBG_SEL			((APUSYS_APU_S_S_4_BASE)+0x034)

#define APUSYS_SCTRL_VIRLM_MVABASE		((APUSYS_SCTRL_REVISER_BASE)+0x300)
#define VLM_REMAP_TABLE_0			((APUSYS_SCTRL_VIRLM_MVABASE) + 0x04)
#define VLM_REMAP_VALID_OFFSET			(31)

#define bits_clr(x, m, o)       (x & (~(m << o)))
#define bits_get(x, m, o)       ((x & (m << o)) >> o)
#define bits_set(x, v, m, o)    ((bits_clr(x, m, o)) | ((v & m) << o))

#define REMAP_TABLE_VALID			(0x80000000)
#define VIRLM_MVABASE_DEFAULT_VALUE		(0x8000000)

#endif /* __APUSYS_DEVAPC_H__ */

