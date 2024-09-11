// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2022 MediaTek Inc. All rights reserved.
 */

#include <common/fdt_wrappers.h>
#include <common/debug.h>
#include <drivers/delay_timer.h>
#include <lib/mmio.h>
#include <platform_def.h>
#include <drivers/spi_mem.h>
#include <lib/utils_def.h>
#include <errno.h>


#define MTK_NOR_REG_CMD			0x00
#define MTK_NOR_CMD_WRITE		BIT(4)
#define MTK_NOR_CMD_PROGRAM		BIT(2)
#define MTK_NOR_CMD_READ		BIT(0)
#define MTK_NOR_CMD_MASK		GENMASK(5, 0)

#define MTK_NOR_REG_PRG_CNT		0x04
#define MTK_NOR_PRG_CNT_MAX		56
#define MTK_NOR_REG_RDATA		0x0c

#define MTK_NOR_REG_RADR0		0x10
#define MTK_NOR_REG_RADR(n)		(MTK_NOR_REG_RADR0 + 4 * (n))
#define MTK_NOR_REG_RADR3		0xc8

#define MTK_NOR_REG_WDATA		0x1c

#define MTK_NOR_REG_PRGDATA0		0x20
#define MTK_NOR_REG_PRGDATA(n)		(MTK_NOR_REG_PRGDATA0 + 4 * (n))
#define MTK_NOR_REG_PRGDATA_MAX		5

#define MTK_NOR_REG_SHIFT0		0x38
#define MTK_NOR_REG_SHIFT(n)		(MTK_NOR_REG_SHIFT0 + 4 * (n))
#define MTK_NOR_REG_SHIFT_MAX		9

#define MTK_NOR_REG_CFG1		0x60
#define MTK_NOR_FAST_READ		BIT(0)

#define MTK_NOR_REG_CFG2		0x64
#define MTK_NOR_WR_CUSTOM_OP_EN		BIT(4)
#define MTK_NOR_WR_BUF_EN		BIT(0)

#define MTK_NOR_REG_PP_DATA		0x98

#define MTK_NOR_REG_IRQ_STAT		0xa8
#define MTK_NOR_REG_IRQ_EN		0xac
#define MTK_NOR_IRQ_DMA			BIT(7)
#define MTK_NOR_IRQ_MASK		GENMASK(7, 0)

#define MTK_NOR_REG_CFG3		0xb4
#define MTK_NOR_DISABLE_WREN		BIT(7)
#define MTK_NOR_DISABLE_SR_POLL		BIT(5)

#define MTK_NOR_REG_WP			0xc4
#define MTK_NOR_ENABLE_SF_CMD		0x30

#define MTK_NOR_REG_BUSCFG		0xcc
#define MTK_NOR_4B_ADDR			BIT(4)
#define MTK_NOR_QUAD_ADDR		BIT(3)
#define MTK_NOR_QUAD_READ		BIT(2)
#define MTK_NOR_DUAL_ADDR		BIT(1)
#define MTK_NOR_DUAL_READ		BIT(0)
#define MTK_NOR_BUS_MODE_MASK		GENMASK(4, 0)

#define MTK_NOR_REG_DMA_CTL		0x718
#define MTK_NOR_DMA_START		BIT(0)

#define MTK_NOR_REG_DMA_FADR		0x71c
#define MTK_NOR_REG_DMA_DADR		0x720
#define MTK_NOR_REG_DMA_END_DADR	0x724
#define MTK_NOR_REG_DMA_DADR_HB		0x738
#define MTK_NOR_REG_DMA_END_DADR_HB	0x73c

#define MTK_NOR_PRG_MAX_SIZE		6
/* Reading DMA src/dst addresses have to be 16-byte aligned */
#define MTK_NOR_DMA_ALIGN		16
#define MTK_NOR_DMA_ALIGN_MASK		(MTK_NOR_DMA_ALIGN - 1)
/* and we allocate a bounce buffer if destination address isn't aligned */
#define MTK_NOR_BOUNCE_BUF_SIZE		PAGE_SIZE

/* Buffered page program can do one 128-byte transfer */
#define MTK_NOR_PP_SIZE			128

#define BITS_PER_BYTE			8

#define MTK_NOR_TIMEOUT         5000000

#define readl(addr)                mmio_read_32(addr)
#define writel(val, addr)          mmio_write_32(addr, val)

#define readb(addr)                mmio_read_8(addr)
#define writeb(val, addr)          mmio_write_8(addr, val)

struct mtk_nor {
	uintptr_t base;
	bool wbuf_en;
	bool has_irq;
	bool high_dma;
};

static struct mtk_nor snfc_op = {
	SNFC_REG_BASE,
	false,
	false,
	false,
};


static int polling_cmd(unsigned int reg, unsigned int val)
{
	unsigned int count = 0;

	do {
		if (!(readl(reg) & val))
			return 0;
		udelay(1);
		count++;
	} while (count < MTK_NOR_TIMEOUT);

	return -1;
}

static inline mtk_nor_rmw(struct mtk_nor *sp, unsigned int reg, unsigned int set, unsigned int clr)
{
	unsigned int val = readl(sp->base + reg);

	val &= ~clr;
	val |= set;
	writel(val, sp->base + reg);
}

static inline int mtk_nor_cmd_exec(struct mtk_nor *sp, unsigned int cmd)
{
	unsigned int reg;
	int ret;

	writel(cmd, sp->base + MTK_NOR_REG_CMD);
	ret = polling_cmd(sp->base + MTK_NOR_REG_CMD, cmd);
	if (ret < 0)
		INFO("command %u timeout.\n", cmd);
	return ret;
}

static mtk_nor_set_addr(struct mtk_nor *sp, const struct spi_mem_op *op)
{
	unsigned int addr = op->addr.val;
	int i;

	for (i = 0; i < 3; i++) {
		writeb(addr & 0xff, sp->base + MTK_NOR_REG_RADR(i));
		addr >>= 8;
	}
	if (op->addr.nbytes == 4) {
		writeb(addr & 0xff, sp->base + MTK_NOR_REG_RADR3);
		mtk_nor_rmw(sp, MTK_NOR_REG_BUSCFG, MTK_NOR_4B_ADDR, 0);
	} else {
		mtk_nor_rmw(sp, MTK_NOR_REG_BUSCFG, 0, MTK_NOR_4B_ADDR);
	}
}

static bool mtk_nor_match_read(const struct spi_mem_op *op)
{
	int dummy = 0;

	if (op->dummy.nbytes)
		dummy = op->dummy.nbytes * BITS_PER_BYTE / op->dummy.buswidth;

	if ((op->data.buswidth == 2) || (op->data.buswidth == 4)) {
		if (op->addr.buswidth == 1)
			return dummy == 8;
		else if (op->addr.buswidth == 2)
			return dummy == 4;
		else if (op->addr.buswidth == 4)
			return dummy == 6;
	} else if ((op->addr.buswidth == 1) && (op->data.buswidth == 1)) {
		if (op->cmd.opcode == 0x03)
			return dummy == 0;
		else if (op->cmd.opcode == 0x0b)
			return dummy == 8;
	}
	return false;
}

static mtk_nor_setup_bus(struct mtk_nor *sp, const struct spi_mem_op *op)
{
	unsigned int reg = 0;

	if (op->addr.nbytes == 4)
		reg |= MTK_NOR_4B_ADDR;

	if (op->data.buswidth == 4) {
		reg |= MTK_NOR_QUAD_READ;
		writeb(op->cmd.opcode, sp->base + MTK_NOR_REG_PRGDATA(4));
		if (op->addr.buswidth == 4)
			reg |= MTK_NOR_QUAD_ADDR;
	} else if (op->data.buswidth == 2) {
		reg |= MTK_NOR_DUAL_READ;
		writeb(op->cmd.opcode, sp->base + MTK_NOR_REG_PRGDATA(3));
		if (op->addr.buswidth == 2)
			reg |= MTK_NOR_DUAL_ADDR;
	} else {
		if (op->cmd.opcode == 0x0b)
			mtk_nor_rmw(sp, MTK_NOR_REG_CFG1, MTK_NOR_FAST_READ, 0);
		else
			mtk_nor_rmw(sp, MTK_NOR_REG_CFG1, 0, MTK_NOR_FAST_READ);
	}
	mtk_nor_rmw(sp, MTK_NOR_REG_BUSCFG, reg, MTK_NOR_BUS_MODE_MASK);
}

static int mtk_nor_write_buffer_enable(struct mtk_nor *sp)
{
	unsigned int val;

	if (sp->wbuf_en)
		return 0;

	val = readl(sp->base + MTK_NOR_REG_CFG2);
	writel(val | MTK_NOR_WR_BUF_EN, sp->base + MTK_NOR_REG_CFG2);

	return 0;
}

static int mtk_nor_write_buffer_disable(struct mtk_nor *sp)
{
	unsigned int val;

	val = readl(sp->base + MTK_NOR_REG_CFG2);
	writel(val & ~MTK_NOR_WR_BUF_EN, sp->base + MTK_NOR_REG_CFG2);
	return 0;
}

static int mtk_nor_pp_buffered(struct mtk_nor *sp, const struct spi_mem_op *op)
{
	const unsigned char *buf = op->data.buf;
	unsigned int val;
	int ret, i;

	ret = mtk_nor_write_buffer_enable(sp);
	if (ret < 0)
		return ret;

	for (i = 0; i < op->data.nbytes; i += 4) {
		val = buf[i + 3] << 24 | buf[i + 2] << 16 | buf[i + 1] << 8 |
		      buf[i];
		writel(val, sp->base + MTK_NOR_REG_PP_DATA);
	}
	return mtk_nor_cmd_exec(sp, MTK_NOR_CMD_WRITE);
}

static int mtk_nor_pp_unbuffered(struct mtk_nor *sp,
				 const struct spi_mem_op *op)
{
	const unsigned char *buf = op->data.buf;
	int ret;

	ret = mtk_nor_write_buffer_disable(sp);
	if (ret < 0)
		return ret;
	writeb(buf[0], sp->base + MTK_NOR_REG_WDATA);
	return mtk_nor_cmd_exec(sp, MTK_NOR_CMD_WRITE);
}

static int mtk_nor_spi_mem_prg(struct mtk_nor *sp, const struct spi_mem_op *op)
{
	int rx_len = 0;
	int reg_offset = MTK_NOR_REG_PRGDATA_MAX;
	int tx_len, prg_len;
	int i, ret;
	unsigned int reg;
	unsigned char bufbyte;

	tx_len = 1 + op->addr.nbytes;

	/* count dummy bytes only if we need to write data after it */
	if (op->data.dir == SPI_MEM_DATA_OUT)
		tx_len += op->dummy.nbytes + op->data.nbytes;
	else if (op->data.dir == SPI_MEM_DATA_IN)
		rx_len = op->data.nbytes;

	prg_len = 1 + op->addr.nbytes + op->dummy.nbytes +
		  op->data.nbytes;

	/*
	 * an invalid op may reach here if the caller calls exec_op without
	 * adjust_op_size. return -EINVAL instead of -ENOTSUPP so that
	 * spi-mem won't try this op again with generic spi transfers.
	 */
	if ((tx_len > MTK_NOR_REG_PRGDATA_MAX + 1) ||
	    (rx_len > MTK_NOR_REG_SHIFT_MAX + 1) ||
	    (prg_len > MTK_NOR_PRG_CNT_MAX / 8))
		return -EINVAL;

	/* fill tx data */
	for (i = 1; i > 0; i--, reg_offset--) {
		reg = sp->base + MTK_NOR_REG_PRGDATA(reg_offset);
		bufbyte = (op->cmd.opcode >> ((i - 1) * BITS_PER_BYTE)) & 0xff;
		writeb(bufbyte, reg);
	}

	for (i = op->addr.nbytes; i > 0; i--, reg_offset--) {
		reg = sp->base + MTK_NOR_REG_PRGDATA(reg_offset);
		bufbyte = (op->addr.val >> ((i - 1) * BITS_PER_BYTE)) & 0xff;
		writeb(bufbyte, reg);
	}

	if (op->data.dir == SPI_MEM_DATA_OUT) {
		for (i = 0; i < op->dummy.nbytes; i++, reg_offset--) {
			reg = sp->base + MTK_NOR_REG_PRGDATA(reg_offset);
			writeb(0, reg);
		}

		for (i = 0; i < op->data.nbytes; i++, reg_offset--) {
			reg = sp->base + MTK_NOR_REG_PRGDATA(reg_offset);
			writeb(((const unsigned char *)(op->data.buf))[i], reg);
		}
	}

	for (; reg_offset >= 0; reg_offset--) {
		reg = sp->base + MTK_NOR_REG_PRGDATA(reg_offset);
		writeb(0, reg);
	}

	/* trigger op */
	writel(prg_len * BITS_PER_BYTE, sp->base + MTK_NOR_REG_PRG_CNT);
	ret = mtk_nor_cmd_exec(sp, MTK_NOR_CMD_PROGRAM);
	if (ret)
		return ret;

	/* fetch read data */
	reg_offset = 0;
	if (op->data.dir == SPI_MEM_DATA_IN) {
		for (i = op->data.nbytes - 1; i >= 0; i--, reg_offset++) {
			reg = sp->base + MTK_NOR_REG_SHIFT(reg_offset);
			((unsigned char *)(op->data.buf))[i] = readb(reg);
		}
	}

	return 0;
}

static int mtk_nor_claim_bus(unsigned int cs)
{
	return 0;
}

static mtk_nor_release_bus(void)
{
}

static int mtk_nor_set_speed(unsigned int hz)
{

	return 0;
}

static int mtk_nor_set_mode(unsigned int mode)
{
	return 0;
}

static int mtk_nor_exec_op(const struct spi_mem_op *op)
{
	int ret;

	if ((op->data.nbytes == 0) ||
	    ((op->addr.nbytes != 3) && (op->addr.nbytes != 4)))
		return mtk_nor_spi_mem_prg(&snfc_op, op);

	if (op->data.dir == SPI_MEM_DATA_OUT) {
		mtk_nor_set_addr(&snfc_op, op);
		writeb(op->cmd.opcode, snfc_op.base + MTK_NOR_REG_PRGDATA0);
		if (op->data.nbytes == MTK_NOR_PP_SIZE)
			return mtk_nor_pp_buffered(&snfc_op, op);
		return mtk_nor_pp_unbuffered(&snfc_op, op);
	}

	if ((op->data.dir == SPI_MEM_DATA_IN) && mtk_nor_match_read(op)) {
		ret = mtk_nor_write_buffer_disable(&snfc_op);
		if (ret < 0)
			return ret;
		mtk_nor_setup_bus(&snfc_op, op);
		memcpy(op->data.buf, SNFC_MEM_BASE + op->addr.val, op->data.nbytes);
	}

	return mtk_nor_spi_mem_prg(&snfc_op, op);
}

static mtk_nor_init(struct mtk_nor *sp)
{
	writel(0, sp->base + MTK_NOR_REG_IRQ_EN);
	writel(MTK_NOR_IRQ_MASK, sp->base + MTK_NOR_REG_IRQ_STAT);

	writel(MTK_NOR_ENABLE_SF_CMD, sp->base + MTK_NOR_REG_WP);
	mtk_nor_rmw(sp, MTK_NOR_REG_CFG2, MTK_NOR_WR_CUSTOM_OP_EN, 0);
	mtk_nor_rmw(sp, MTK_NOR_REG_CFG3,
		    MTK_NOR_DISABLE_WREN | MTK_NOR_DISABLE_SR_POLL, 0);
}


static const struct spi_bus_ops mtk_nor_ops = {
	.claim_bus = mtk_nor_claim_bus,
	.release_bus = mtk_nor_release_bus,
	.set_speed = mtk_nor_set_speed,
	.set_mode = mtk_nor_set_mode,
	.exec_op = mtk_nor_exec_op,
};

static void *fdt;

int mtk_nor_probe(void)
{
	int nor_node;

	mtk_nor_init(&snfc_op);

	return spi_mem_init_slave(fdt, nor_node, &mtk_nor_ops);
}
