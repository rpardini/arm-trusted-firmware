// SPDX-License-Identifier: GPL-2.0-only OR MIT

/*
 * Copyright (C) 2024 MediaTek Inc.
 */

#include <arch_helpers.h>
#include <common/debug.h>
#include <lib/libc/errno.h>
#include <platform_def.h>
#include <lib/mmio.h>
#include <api_hre_mminfra.h>
#include <mminfra.h>
#include <mminfra_comm.h>
#include <mminfra_priv.h>

typedef uint32_t u32;

static const struct mm_lut mdp_rec[] = {
	/* pipe 0*/
	{0x10, 0x11, 0x0},	/* RDMA0/RDMA1 */
	{0x12, 0x13, 0x1},	/* WROT0/WROT1 */
	{0x14, 0x15, 0x2},	/* DUMMY0/DUMMY1 */
	{0x16, 0x17, 0x3},	/* RDMA2/RDMA3 */
	{0x18, 0x19, 0x4},	/* WROT2/WROT3 */
	{0x1a, 0x1a, 0x5},	/* FAKE0 */
};

static const struct mm_lut disp_rec[] = {
	{0x0, 0x7, 0x0},	/* OVL0 */
	{0x8, 0xf, 0x1},	/* OVL1 */
	{0x10, 0x10, 0x2},	/* RDMA0 */
	{0x11, 0x11, 0x3},	/* RDMA1 */
	{0x12, 0x12, 0x4},	/* WDMA0 */
	{0x13, 0x13, 0x5},	/* WDMA1 */
	{0x14, 0x14, 0x6},	/* DISP_FAKE0 */
	{0x15, 0x15, 0x7},	/* DISP_FAKE1 */
};

static const struct mm_lut gce_rec[] = {
	{0x0, 0x1f, 0x0},	/* GCE_D */
	{0x20, 0x3f, 0x1},	/* GCE_M */
};

static const struct mm_lut venc_rec[] = {
	{0x0, 0x0, 0x0},
	{0x1, 0x1, 0x1},
	{0x2, 0x2, 0x2},
	{0x3, 0x3, 0x3},
	{0x4, 0x4, 0x4},
	{0x5, 0x5, 0x5},
	{0x6, 0x6, 0x6},
	{0x7, 0x7, 0x7},
	{0x8, 0x8, 0x8},
	{0x9, 0x9, 0x9},
	{0xa, 0xa, 0xa},
	{0xb, 0xb, 0xb},
	{0xc, 0xc, 0xc},
	{0xd, 0xd, 0xd},
	{0xe, 0xe, 0xe},
	{0xf, 0xf, 0xf},
};

static const struct mm_lut vdec_rec[] = {
	{0x0, 0x0, 0x0},
	{0x1, 0x1, 0x1},
	{0x2, 0x2, 0x2},
	{0x3, 0x3, 0x3},
	{0x4, 0x4, 0x4},
	{0x5, 0x5, 0x5},
	{0x6, 0x6, 0x6},
	{0x7, 0x7, 0x7},
	{0x8, 0x8, 0x8},
	{0x9, 0x9, 0x9},
	{0xa, 0xa, 0xa},
	{0xb, 0xb, 0xb},
	{0xc, 0xc, 0xc},
	{0xd, 0xd, 0xd},
	{0xe, 0xe, 0xe},
	{0xf, 0xf, 0xf},
};

static void set_uid2aid_remap(const struct mm_lut *rec, size_t array_size,
			      uintptr_t aid_addr, u32 aid_width, u32 aid_mask)
{
	int i, j;
	u32 offset, shift, val;

	for (i = 0; i < array_size; i++) {
		for (j = rec[i].in_start; j <= rec[i].in_end; j++) {
			offset = (aid_width * j) / 32 * 4;
			shift = (aid_width * j) % 32;
			val = mmio_read_32(aid_addr + offset) & ~(aid_mask << shift);
			val = ((u32)rec[i].out_id << shift) | val;
			mmio_write_32(aid_addr + offset, val);
			NOTICE("%s 1 write(0x%lx)=0x%x shift=%d\n",
				__func__, aid_addr + offset, val, shift);

			if ((aid_width * (j+1) % 32 < aid_width) &&
			    (aid_width * (j+1) % 32 != 0)) {
				offset += 4;
				shift = aid_width * (j + 1) % 32;
				val = mmio_read_32(aid_addr + offset) &
						   ~(aid_mask >> (aid_width - shift));
				val = ((u32)rec[i].out_id >> (aid_width - shift)) | val;
				mmio_write_32(aid_addr + offset, val);
				NOTICE("%s 2 write(0x%lx)=0x%x shift=%d\n",
					__func__, aid_addr + offset, val, shift);
			}
		}
	}
}

static const struct smi_comm_item mtk_smi_mminfra_comm_init[] = {
	/* smi_disp_common */
	[0] = {
		.base = IO_PHYS + 0x0e801000,
		.regs = {{SMI_L1LEN, 0xb}, {SMI_BUS_SEL, 0x4444}, {SMI_M4U_TH, 0xe100e10},
			 {SMI_FIFO_TH1, 0x506090a}, {SMI_FIFO_TH2, 0x506090a}, {SMI_DCM, 0x4f1},
			 {SMI_DUMMY, 0x1}},
	},
	/* ssc0 */
	[1] = {
		.base = IO_PHYS + 0x0e807000,
		.regs = {{SMI_DUMMY, 0x1},},
	},
	/* ssc1 */
	[2] = {
		.base = IO_PHYS + 0x0e808000,
		.regs = {{SMI_L1LEN, 0xa},},
	},
	/* mdp_sub_common0 */
	[3] = {
		.base = IO_PHYS + 0x0e809000,
		.regs = {{SMI_L1LEN, 0xa},},
	},
	/* mdp_sub_common1 */
	[4] = {
		.base = IO_PHYS + 0x0e80a000,
		.regs = {{SMI_L1LEN, 0xa},},
	},
};

static int set_smi_golden(void)
{
	const struct smi_comm_item *comm;
	const struct smi_reg_pair *reg;
	int i, j;
	uint32_t val;

	for (i = 0; i < ARRAY_SIZE(mtk_smi_mminfra_comm_init); i++) {
		comm = &mtk_smi_mminfra_comm_init[i];

		for (j = 0; j < SMI_MAX_REG; j++) {
			reg = &comm->regs[j];
			if (!reg->value)
				break;
			mmio_write_32(comm->base + reg->offset, reg->value);
			val = mmio_read_32(comm->base + reg->offset);
			if (val != reg->value) {
				ERROR("%s comm: %d - %d set fail. offset: 0x%x\n",
				      __func__, i, j, reg->offset);
				ERROR("value = 0x%x real value = 0x%x\n",
				      reg->value, val);
				return -EPERM;
			}
		}
	}

	return 0;
}

static void mminfra_hre_sram_init(void)
{
	uint32_t val;

	/* Set to HW default value */
	mmio_write_32(MM_HRE_SRAM_CON, MM_HRE_SRAM_CON_DEF_VAL);
	/* Power on MMInfra HRE SRAM */
	val = mmio_read_32(MM_HRE_SRAM_CON) & ~MM_HRE_SRAM_CON_PWR_ON_MASK;
	mmio_write_32(MM_HRE_SRAM_CON, val);
}

void mminfra_enable_clk(void)
{
	u32 val;

	/* enable CLK_MMINFRA_SMI */
	val = mmio_read_32(MMINFRA_CONFIG_BASE + 0x100) & ~CLK_MMINFRA_SMI_MASK;
	mmio_write_32(MMINFRA_CONFIG_BASE + 0x100, val);
}

void mminfra_init(void)
{
	mminfra_hre_sram_init();

	/* Initialize HRE setting */
	mminfra_hre_bkrs(INIT_HRE_BKRS_CONFIG);
	mminfra_hre_bkrs(SAVE_HRE_BK_CONFIG);
	mminfra_hre_bkrs(SAVE_HRE_RS_CONFIG);

	/* UID to AID remap */
	set_uid2aid_remap(disp_rec, ARRAY_SIZE(disp_rec), MMINFRA_DISP_AID_REMAP,
			  DISP_AID_WIDTH, DISP_AID_MASK);
	set_uid2aid_remap(mdp_rec, ARRAY_SIZE(mdp_rec), MMINFRA_MDP_AID_REMAP,
			  MDP_AID_WIDTH, MDP_AID_MASK);
	set_uid2aid_remap(gce_rec, ARRAY_SIZE(gce_rec), MMINFRA_GCE_AID_REMAP,
			  GCE_AID_WIDTH, GCE_AID_MASK);
	set_uid2aid_remap(venc_rec, ARRAY_SIZE(venc_rec), MMINFRA_VENC_AID_REMAP,
			  VENC_AID_WIDTH, VENC_AID_MASK);
	set_uid2aid_remap(vdec_rec, ARRAY_SIZE(vdec_rec), MMINFRA_VDEC_AID_REMAP,
			  VDEC_AID_WIDTH, VDEC_AID_MASK);

	/* Enable dom2aid and dom2aid_infra_en */
	mmio_write_32(MMINFRA_AID_REMAP_DOM2AID_EN, 0x1 | DOM2AID_INFRA_EN);

	mminfra_enable_clk();
	set_smi_golden();

	NOTICE("%s done\n", __func__);
}
