#include <stddef.h>
#include <drivers/mmc.h>
#include <stdint.h>
#include <stdbool.h>
#include <lib/mmio.h>
#include <common/debug.h>
#include <errno.h>

#include "mtk-sd.h"

static int xfer_lba;
static uintptr_t xfer_buf;
static size_t xfer_size;

/* u-boot MMC */

#define MMC_CMD_GO_IDLE_STATE		0
#define MMC_CMD_SEND_OP_COND		1
#define MMC_CMD_ALL_SEND_CID		2
#define MMC_CMD_SET_RELATIVE_ADDR	3
#define MMC_CMD_SET_DSR			4
#define MMC_CMD_SWITCH			6
#define MMC_CMD_SELECT_CARD		7
#define MMC_CMD_SEND_EXT_CSD		8
#define MMC_CMD_SEND_CSD		9
#define MMC_CMD_SEND_CID		10
#define MMC_CMD_STOP_TRANSMISSION	12
#define MMC_CMD_SEND_STATUS		13
#define MMC_CMD_SET_BLOCKLEN		16
#define MMC_CMD_READ_SINGLE_BLOCK	17
#define MMC_CMD_READ_MULTIPLE_BLOCK	18
#define MMC_CMD_SEND_TUNING_BLOCK		19
#define MMC_CMD_SEND_TUNING_BLOCK_HS200	21
#define MMC_CMD_SET_BLOCK_COUNT         23
#define MMC_CMD_WRITE_SINGLE_BLOCK	24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK	25
#define MMC_CMD_ERASE_GROUP_START	35
#define MMC_CMD_ERASE_GROUP_END		36
#define MMC_CMD_ERASE			38
#define MMC_CMD_APP_CMD			55
#define MMC_CMD_SPI_READ_OCR		58
#define MMC_CMD_SPI_CRC_ON_OFF		59
#define MMC_CMD_RES_MAN			62

#define MMC_CMD62_ARG1			0xefac62ec
#define MMC_CMD62_ARG2			0xcbaea7

#define SD_CMD_SEND_RELATIVE_ADDR	3
#define SD_CMD_SWITCH_FUNC		6
#define SD_CMD_SEND_IF_COND		8
#define SD_CMD_SWITCH_UHS18V		11

#define SD_CMD_APP_SET_BUS_WIDTH	6
#define SD_CMD_APP_SD_STATUS		13
#define SD_CMD_ERASE_WR_BLK_START	32
#define SD_CMD_ERASE_WR_BLK_END		33
#define SD_CMD_APP_SEND_OP_COND		41
#define SD_CMD_APP_SEND_SCR		51

enum bus_mode {
	MMC_LEGACY,
	SD_LEGACY,
	MMC_HS,
	SD_HS,
	MMC_HS_52,
	MMC_DDR_52,
	UHS_SDR12,
	UHS_SDR25,
	UHS_SDR50,
	UHS_DDR50,
	UHS_SDR104,
	MMC_HS_200,
	MMC_HS_400,
	MMC_MODES_END
};

/* MSDC_CFG */
#define MSDC_CFG_HS400_CK_MODE_EXT	BIT(22)
#define MSDC_CFG_CKMOD_EXT_M		0x300000
#define MSDC_CFG_CKMOD_EXT_S		20
#define MSDC_CFG_CKDIV_EXT_M		0xfff00
#define MSDC_CFG_CKDIV_EXT_S		8
#define MSDC_CFG_HS400_CK_MODE		BIT(18)
#define MSDC_CFG_CKMOD_M		0x30000
#define MSDC_CFG_CKMOD_S		16
#define MSDC_CFG_CKDIV_M		0xff00
#define MSDC_CFG_CKDIV_S		8
#define MSDC_CFG_CKSTB			BIT(7)
#define MSDC_CFG_PIO			BIT(3)
#define MSDC_CFG_RST			BIT(2)
#define MSDC_CFG_CKPDN			BIT(1)
#define MSDC_CFG_MODE			BIT(0)

/* MSDC_IOCON */
#define MSDC_IOCON_W_DSPL		BIT(8)
#define MSDC_IOCON_DSPL			BIT(2)
#define MSDC_IOCON_RSPL			BIT(1)

/* MSDC_PS */
#define MSDC_PS_DAT0			BIT(16)
#define MSDC_PS_CDDBCE_M		0xf000
#define MSDC_PS_CDDBCE_S		12
#define MSDC_PS_CDSTS			BIT(1)
#define MSDC_PS_CDEN			BIT(0)

/* #define MSDC_INT(EN) */
#define MSDC_INT_ACMDRDY		BIT(3)
#define MSDC_INT_ACMDTMO		BIT(4)
#define MSDC_INT_ACMDCRCERR		BIT(5)
#define MSDC_INT_CMDRDY			BIT(8)
#define MSDC_INT_CMDTMO			BIT(9)
#define MSDC_INT_RSPCRCERR		BIT(10)
#define MSDC_INT_XFER_COMPL		BIT(12)
#define MSDC_INT_DATTMO			BIT(14)
#define MSDC_INT_DATCRCERR		BIT(15)

/* MSDC_FIFOCS */
#define MSDC_FIFOCS_CLR			BIT(31)
#define MSDC_FIFOCS_TXCNT_M		0xff0000
#define MSDC_FIFOCS_TXCNT_S		16
#define MSDC_FIFOCS_RXCNT_M		0xff
#define MSDC_FIFOCS_RXCNT_S		0

/* #define SDC_CFG */
#define SDC_CFG_DTOC_M			0xff000000
#define SDC_CFG_DTOC_S			24
#define SDC_CFG_SDIOIDE			BIT(20)
#define SDC_CFG_SDIO			BIT(19)
#define SDC_CFG_BUSWIDTH_M		0x30000
#define SDC_CFG_BUSWIDTH_S		16

/* SDC_CMD */
#define SDC_CMD_BLK_LEN_M		0xfff0000
#define SDC_CMD_BLK_LEN_S		16
#define SDC_CMD_STOP			BIT(14)
#define SDC_CMD_WR			BIT(13)
#define SDC_CMD_DTYPE_M			0x1800
#define SDC_CMD_DTYPE_S			11
#define SDC_CMD_RSPTYP_M		0x380
#define SDC_CMD_RSPTYP_S		7
#define SDC_CMD_CMD_M			0x3f
#define SDC_CMD_CMD_S			0

/* SDC_STS */
#define SDC_STS_CMDBUSY			BIT(1)
#define SDC_STS_SDCBUSY			BIT(0)

/* SDC_ADV_CFG0 */
#define SDC_RX_ENHANCE_EN		BIT(20)

/* PATCH_BIT0 */
#define MSDC_INT_DAT_LATCH_CK_SEL_M	0x380
#define MSDC_INT_DAT_LATCH_CK_SEL_S	7

/* PATCH_BIT1 */
#define MSDC_PB1_STOP_DLY_M		0xf00
#define MSDC_PB1_STOP_DLY_S		8

/* PATCH_BIT2 */
#define MSDC_PB2_CRCSTSENSEL_M		0xe0000000
#define MSDC_PB2_CRCSTSENSEL_S		29
#define MSDC_PB2_CFGCRCSTS		BIT(28)
#define MSDC_PB2_RESPSTSENSEL_M		0x70000
#define MSDC_PB2_RESPSTSENSEL_S		16
#define MSDC_PB2_CFGRESP		BIT(15)
#define MSDC_PB2_RESPWAIT_M		0x0c
#define MSDC_PB2_RESPWAIT_S		2

/* PAD_TUNE */
#define MSDC_PAD_TUNE_CMDRRDLY_M	0x7c00000
#define MSDC_PAD_TUNE_CMDRRDLY_S	22
#define MSDC_PAD_TUNE_CMD_SEL		BIT(21)
#define MSDC_PAD_TUNE_CMDRDLY_M		0x1f0000
#define MSDC_PAD_TUNE_CMDRDLY_S		16
#define MSDC_PAD_TUNE_RXDLYSEL		BIT(15)
#define MSDC_PAD_TUNE_RD_SEL		BIT(13)
#define MSDC_PAD_TUNE_DATRRDLY_M	0x1f00
#define MSDC_PAD_TUNE_DATRRDLY_S	8
#define MSDC_PAD_TUNE_DATWRDLY_M	0x1f
#define MSDC_PAD_TUNE_DATWRDLY_S	0

/* EMMC50_CFG0 */
#define EMMC50_CFG_CFCSTS_SEL		BIT(4)

/* SDC_FIFO_CFG */
#define SDC_FIFO_CFG_WRVALIDSEL		BIT(24)
#define SDC_FIFO_CFG_RDVALIDSEL		BIT(25)

/* SDC_CFG_BUSWIDTH */
#define MSDC_BUS_1BITS			0x0
#define MSDC_BUS_4BITS			0x1
#define MSDC_BUS_8BITS			0x2

#define MSDC_FIFO_SIZE			128

#define PAD_DELAY_MAX			32

#define DEFAULT_CD_DEBOUNCE		8

#define CMD_INTS_MASK	\
	(MSDC_INT_CMDRDY | MSDC_INT_RSPCRCERR | MSDC_INT_CMDTMO)

#define DATA_INTS_MASK	\
	(MSDC_INT_XFER_COMPL | MSDC_INT_DATTMO | MSDC_INT_DATCRCERR)

typedef uint32_t u32;
typedef unsigned int uint;

/* Register offset */
struct mtk_sd_regs {
	uint32_t msdc_cfg;
	uint32_t msdc_iocon;
	uint32_t msdc_ps;
	uint32_t msdc_int;
	uint32_t msdc_inten;
	uint32_t msdc_fifocs;
	uint32_t msdc_txdata;
	uint32_t msdc_rxdata;
	uint32_t reserved0[4];
	uint32_t sdc_cfg;
	uint32_t sdc_cmd;
	uint32_t sdc_arg;
	uint32_t sdc_sts;
	uint32_t sdc_resp[4];
	uint32_t sdc_blk_num;
	uint32_t sdc_vol_chg;
	uint32_t sdc_csts;
	uint32_t sdc_csts_en;
	uint32_t sdc_datcrc_sts;
	uint32_t sdc_adv_cfg0;
	uint32_t reserved1[2];
	uint32_t emmc_cfg0;
	uint32_t emmc_cfg1;
	uint32_t emmc_sts;
	uint32_t emmc_iocon;
	uint32_t sd_acmd_resp;
	uint32_t sd_acmd19_trg;
	uint32_t sd_acmd19_sts;
	uint32_t dma_sa_high4bit;
	uint32_t dma_sa;
	uint32_t dma_ca;
	uint32_t dma_ctrl;
	uint32_t dma_cfg;
	uint32_t sw_dbg_sel;
	uint32_t sw_dbg_out;
	uint32_t dma_length;
	uint32_t reserved2;
	uint32_t patch_bit0;
	uint32_t patch_bit1;
	uint32_t patch_bit2;
	uint32_t reserved3;
	uint32_t dat0_tune_crc;
	uint32_t dat1_tune_crc;
	uint32_t dat2_tune_crc;
	uint32_t dat3_tune_crc;
	uint32_t cmd_tune_crc;
	uint32_t sdio_tune_wind;
	uint32_t reserved4[5];
	uint32_t pad_tune;
	uint32_t pad_tune0;
	uint32_t pad_tune1;
	uint32_t dat_rd_dly[4];
	uint32_t reserved5[2];
	uint32_t hw_dbg_sel;
	uint32_t main_ver;
	uint32_t eco_ver;
	uint32_t reserved6[27];
	uint32_t pad_ds_tune;
	uint32_t reserved7[31];
	uint32_t emmc50_cfg0;
	uint32_t reserved8[7];
	uint32_t sdc_fifo_cfg;
};

struct msdc_delay_phase {
	uint8_t maxlen;
	uint8_t start;
	uint8_t final_phase;
};

struct msdc_tune_para {
	uint32_t iocon;
	uint32_t pad_tune;
};

struct msdc_host {
	struct mtk_sd_regs *base;
	struct msdc_compatible *dev_comp;

	uint32_t src_clk_freq;	/* source clock */
	uint32_t mclk;		/* mmc framework required bus clock */
	uint32_t sclk;		/* actual calculated bus clock */

	/* operation timeout clocks */
	uint32_t timeout_ns;
	uint32_t timeout_clks;

	/* tuning options */
	uint32_t hs400_ds_delay;
	uint32_t hs200_cmd_int_delay;
	uint32_t hs200_write_int_delay;
	uint32_t latch_ck;
	uint32_t r_smpl;		/* sample edge */
	bool hs400_mode;

	/* whether to use gpio detection or built-in hw detection */
	bool builtin_cd;

	unsigned int last_resp_type;
	unsigned int last_data_write;

	struct msdc_tune_para def_tune_para;
	struct msdc_tune_para saved_tune_para;
} msdc_host;

#define setbits_le32(addr, set) mmio_setbits_32((uintptr_t) addr, set)
#define clrbits_le32(addr, clr) mmio_clrbits_32((uintptr_t) addr, clr)
#define clrsetbits_le32(addr, clr, set) mmio_clrsetbits_32((uintptr_t) addr, clr, set)
#define readl(addr) mmio_read_32((uintptr_t) addr)
#define readb(addr) mmio_read_8((uintptr_t) addr)
#define writel(val, addr) mmio_write_32((uintptr_t) addr, val)

#define readl_poll_timeout(_addr, _reg, _test, _timeout) \
			({ do { \
				_reg = readl(_addr); \
				(void) _timeout; \
			} while (!(_test)); 0; })


static void msdc_reset_hw(struct msdc_host *host)
{
	setbits_le32(&host->base->msdc_cfg, MSDC_CFG_RST);

	while (readl(&host->base->msdc_cfg) & MSDC_CFG_RST)
		;
}


static int msdc_ops_set_ios(unsigned int clock, unsigned int bus_width)
;
static void msdc_init_hw(void)
{
	u32 val;
	struct msdc_host *host = &msdc_host;
	u32 *tune_reg = &host->base->pad_tune;

	if (host->dev_comp->pad_tune0)
		tune_reg = &host->base->pad_tune0;

	/* Configure to MMC/SD mode, clock free running */
	setbits_le32(&host->base->msdc_cfg, MSDC_CFG_MODE);

	/* Use PIO mode */
	setbits_le32(&host->base->msdc_cfg, MSDC_CFG_PIO);

	/* Reset */
	msdc_reset_hw(host);

	/* Enable/disable hw card detection according to fdt option */
	if (host->builtin_cd)
		clrsetbits_le32(&host->base->msdc_ps,
			MSDC_PS_CDDBCE_M,
			(DEFAULT_CD_DEBOUNCE << MSDC_PS_CDDBCE_S) |
			MSDC_PS_CDEN);
	else
		clrbits_le32(&host->base->msdc_ps, MSDC_PS_CDEN);

	/* Clear all interrupts */
	val = readl(&host->base->msdc_int);
	writel(val, &host->base->msdc_int);

	/* Enable data & cmd interrupts */
	writel(DATA_INTS_MASK | CMD_INTS_MASK, &host->base->msdc_inten);

	writel(0, tune_reg);
	writel(0, &host->base->msdc_iocon);

	if (host->r_smpl)
		setbits_le32(&host->base->msdc_iocon, MSDC_IOCON_RSPL);
	else
		clrbits_le32(&host->base->msdc_iocon, MSDC_IOCON_RSPL);

	writel(0x403c0046, &host->base->patch_bit0);
	writel(0xffff4089, &host->base->patch_bit1);

	if (host->dev_comp->stop_clk_fix)
		clrsetbits_le32(&host->base->patch_bit1, MSDC_PB1_STOP_DLY_M,
				3 << MSDC_PB1_STOP_DLY_S);

	if (host->dev_comp->busy_check)
		clrbits_le32(&host->base->patch_bit1, (1 << 7));

	setbits_le32(&host->base->emmc50_cfg0, EMMC50_CFG_CFCSTS_SEL);

	if (host->dev_comp->async_fifo) {
		clrsetbits_le32(&host->base->patch_bit2, MSDC_PB2_RESPWAIT_M,
				3 << MSDC_PB2_RESPWAIT_S);

		if (host->dev_comp->enhance_rx) {
			setbits_le32(&host->base->sdc_adv_cfg0,
				     SDC_RX_ENHANCE_EN);
		} else {
			clrsetbits_le32(&host->base->patch_bit2,
					MSDC_PB2_RESPSTSENSEL_M,
					2 << MSDC_PB2_RESPSTSENSEL_S);
			clrsetbits_le32(&host->base->patch_bit2,
					MSDC_PB2_CRCSTSENSEL_M,
					2 << MSDC_PB2_CRCSTSENSEL_S);
		}

		/* use async fifo to avoid tune internal delay */
		clrbits_le32(&host->base->patch_bit2,
			     MSDC_PB2_CFGRESP);
		clrbits_le32(&host->base->patch_bit2,
			     MSDC_PB2_CFGCRCSTS);
	}

	if (host->dev_comp->data_tune) {
		setbits_le32(tune_reg,
			     MSDC_PAD_TUNE_RD_SEL | MSDC_PAD_TUNE_CMD_SEL);
		clrsetbits_le32(&host->base->patch_bit0,
				MSDC_INT_DAT_LATCH_CK_SEL_M,
				host->latch_ck <<
				MSDC_INT_DAT_LATCH_CK_SEL_S);
	} else {
		/* choose clock tune */
		setbits_le32(tune_reg, MSDC_PAD_TUNE_RXDLYSEL);
	}

	/* Configure to enable SDIO mode otherwise sdio cmd5 won't work */
	setbits_le32(&host->base->sdc_cfg, SDC_CFG_SDIO);

	/* disable detecting SDIO device interrupt function */
	clrbits_le32(&host->base->sdc_cfg, SDC_CFG_SDIOIDE);

	/* Configure to default data timeout */
	clrsetbits_le32(&host->base->sdc_cfg, SDC_CFG_DTOC_M,
			3 << SDC_CFG_DTOC_S);

	if (host->dev_comp->stop_clk_fix) {
		clrbits_le32(&host->base->sdc_fifo_cfg,
			     SDC_FIFO_CFG_WRVALIDSEL);
		clrbits_le32(&host->base->sdc_fifo_cfg,
			     SDC_FIFO_CFG_RDVALIDSEL);
	}

	host->def_tune_para.iocon = readl(&host->base->msdc_iocon);
	host->def_tune_para.pad_tune = readl(&host->base->pad_tune);

	msdc_ops_set_ios(260000, 1);
}

static void msdc_fifo_clr(struct msdc_host *host)
{
	u32 reg;

	setbits_le32(&host->base->msdc_fifocs, MSDC_FIFOCS_CLR);

	readl_poll_timeout(&host->base->msdc_fifocs, reg,
			   !(reg & MSDC_FIFOCS_CLR), 1000000);
}

static u32 msdc_fifo_rx_bytes(struct msdc_host *host)
{
	return (readl(&host->base->msdc_fifocs) &
		MSDC_FIFOCS_RXCNT_M) >> MSDC_FIFOCS_RXCNT_S;
}

static u32 msdc_cmd_find_resp(struct msdc_host *host, struct mmc_cmd *cmd)
{
	u32 resp;

	switch (cmd->resp_type) {
		/* Actually, R1, R5, R6, R7 are the same */
	case MMC_RESPONSE_R1:
		resp = 0x1;
		break;
	case MMC_RESPONSE_R1B:
		resp = 0x7;
		break;
	case MMC_RESPONSE_R2:
		resp = 0x2;
		break;
	case MMC_RESPONSE_R3:
		resp = 0x3;
		break;
	default:
		resp = 0x0;
		break;
	}

	return resp;
}

static u32 msdc_cmd_prepare_raw_cmd(struct msdc_host *host,
				    struct mmc_cmd *cmd)
{
	u32 opcode = cmd->cmd_idx;
	u32 resp_type = msdc_cmd_find_resp(host, cmd);
	uint blocksize = 0;
	u32 dtype = 0;
	u32 rawcmd = 0;

	switch (opcode) {
	case MMC_CMD_WRITE_MULTIPLE_BLOCK:
	case MMC_CMD_READ_MULTIPLE_BLOCK:
		dtype = 2;
		break;
	case MMC_CMD_WRITE_SINGLE_BLOCK:
	case MMC_CMD_READ_SINGLE_BLOCK:
	case SD_CMD_APP_SEND_SCR:
		dtype = 1;
		break;
#if 1
case SD_CMD_SWITCH_FUNC: /* same as MMC_CMD_SWITCH */
	case SD_CMD_SEND_IF_COND: /* same as MMC_CMD_SEND_EXT_CSD */
	case SD_CMD_APP_SD_STATUS: /* same as MMC_CMD_SEND_STATUS */
		if (xfer_size)
			dtype = 1;
#endif
	default:
		break;
}

	if (xfer_size) {
		blocksize = 512;
		if (xfer_size / blocksize > 1)
			dtype = 2;
	}

#if 0
	if (data) {
		if (data->flags == MMC_DATA_WRITE)
			rawcmd |= SDC_CMD_WR;

		if (data->blocks > 1)
			dtype = 2;

		blocksize = data->blocksize;
	}
#endif

	rawcmd |= ((opcode << SDC_CMD_CMD_S) & SDC_CMD_CMD_M) |
		((resp_type << SDC_CMD_RSPTYP_S) & SDC_CMD_RSPTYP_M) |
		((blocksize << SDC_CMD_BLK_LEN_S) & SDC_CMD_BLK_LEN_M) |
		((dtype << SDC_CMD_DTYPE_S) & SDC_CMD_DTYPE_M);

	if (opcode == MMC_CMD_STOP_TRANSMISSION)
		rawcmd |= SDC_CMD_STOP;

	return rawcmd;
}

static int msdc_cmd_done(struct msdc_host *host, int events,
			 struct mmc_cmd *cmd)
{
	u32 *rsp = cmd->resp_data;
	int ret = 0;

	if (cmd->resp_type & MMC_RSP_48) {
		if (cmd->resp_type & MMC_RSP_136) {
			rsp[0] = readl(&host->base->sdc_resp[3]);
			rsp[1] = readl(&host->base->sdc_resp[2]);
			rsp[2] = readl(&host->base->sdc_resp[1]);
			rsp[3] = readl(&host->base->sdc_resp[0]);
		} else {
			rsp[0] = readl(&host->base->sdc_resp[0]);
		}
	}

	if (!(events & MSDC_INT_CMDRDY)) {
		if (cmd->cmd_idx != MMC_CMD_SEND_TUNING_BLOCK &&
		    cmd->cmd_idx != MMC_CMD_SEND_TUNING_BLOCK_HS200)
			/*
			 * should not clear fifo/interrupt as the tune data
			 * may have alreay come.
			 */
			msdc_reset_hw(host);

		if (events & MSDC_INT_CMDTMO)
			ret = -ETIMEDOUT;
		else
			ret = -EIO;
	}

	return ret;
}


static bool msdc_cmd_is_ready(struct msdc_host *host)
{
	int ret;
	u32 reg;

	/* The max busy time we can endure is 20ms */
	ret = readl_poll_timeout(&host->base->sdc_sts, reg,
				 !(reg & SDC_STS_CMDBUSY), 20000);

	if (ret) {
		ERROR("CMD bus busy detected\n");
		msdc_reset_hw(host);
		return false;
	}

	if (host->last_resp_type == MMC_RESPONSE_R1B && host->last_data_write) {
		ret = readl_poll_timeout(&host->base->msdc_ps, reg,
					 reg & MSDC_PS_DAT0, 1000000);

		if (ret) {
			ERROR("Card stuck in programming state!\n");
			msdc_reset_hw(host);
			return false;
		}
	}

	return true;
}

static int msdc_start_command(struct msdc_host *host, struct mmc_cmd *cmd)
{
	u32 rawcmd;
	u32 status;
	u32 blocks = 0;
	int ret;

	if (!msdc_cmd_is_ready(host))
		return -EIO;

	msdc_fifo_clr(host);

	host->last_resp_type = cmd->resp_type;
	host->last_data_write = 0;

	rawcmd = msdc_cmd_prepare_raw_cmd(host, cmd);

	if (xfer_size) {
		blocks = xfer_size / 512;
	}

	writel(CMD_INTS_MASK, &host->base->msdc_int);
	writel(blocks, &host->base->sdc_blk_num);
	writel(cmd->cmd_arg, &host->base->sdc_arg);
	writel(rawcmd, &host->base->sdc_cmd);

	ret = readl_poll_timeout(&host->base->msdc_int, status,
				 status & CMD_INTS_MASK, 1000000);
	if (ret)
		status = MSDC_INT_CMDTMO;

	xfer_size = 0;
	return msdc_cmd_done(host, status, cmd);
}

static int mtk_mmc_send_cmd(struct mmc_cmd *cmd)
{
	int ret;
	struct msdc_host *host = &msdc_host;

	ret = msdc_start_command(host, cmd);
	if (ret) {
		ERROR("mmc_send_cmd; %d\n", ret);
		return ret;
	}

	return 0;
}

static void msdc_set_buswidth(struct msdc_host *host, u32 width)
{
	u32 val = readl(&host->base->sdc_cfg);

	val &= ~SDC_CFG_BUSWIDTH_M;

	switch (width) {
	default:
	case MMC_BUS_WIDTH_1:
		val |= (MSDC_BUS_1BITS << SDC_CFG_BUSWIDTH_S);
		break;
	case MMC_BUS_WIDTH_4:
		val |= (MSDC_BUS_4BITS << SDC_CFG_BUSWIDTH_S);
		break;
	case MMC_BUS_WIDTH_8:
		val |= (MSDC_BUS_8BITS << SDC_CFG_BUSWIDTH_S);
		break;
	}

	writel(val, &host->base->sdc_cfg);
}

static void msdc_set_timeout(struct msdc_host *host, u32 ns, u32 clks)
{
	u32 timeout, clk_ns;
	u32 mode = 0;

	host->timeout_ns = ns;
	host->timeout_clks = clks;

	if (host->sclk == 0) {
		timeout = 0;
	} else {
		clk_ns = 1000000000UL / host->sclk;
		timeout = (ns + clk_ns - 1) / clk_ns + clks;
		/* unit is 1048576 sclk cycles */
		timeout = (timeout + (0x1 << 20) - 1) >> 20;
		if (host->dev_comp->clk_div_bits == 8)
			mode = (readl(&host->base->msdc_cfg) &
				MSDC_CFG_CKMOD_M) >> MSDC_CFG_CKMOD_S;
		else
			mode = (readl(&host->base->msdc_cfg) &
				MSDC_CFG_CKMOD_EXT_M) >> MSDC_CFG_CKMOD_EXT_S;
		/* DDR mode will double the clk cycles for data timeout */
		timeout = mode >= 2 ? timeout * 2 : timeout;
		timeout = timeout > 1 ? timeout - 1 : 0;
		timeout = timeout > 255 ? 255 : timeout;
	}

	clrsetbits_le32(&host->base->sdc_cfg, SDC_CFG_DTOC_M,
			timeout << SDC_CFG_DTOC_S);
}

static void msdc_set_mclk(struct msdc_host *host, u32 hz)
{
	u32 mode;
	u32 div;
	u32 sclk;
	u32 reg;
	int timing = 2;

	if (!hz) {
		host->mclk = 0;
		clrbits_le32(&host->base->msdc_cfg, MSDC_CFG_CKPDN);
		return;
	}

	if (host->dev_comp->clk_div_bits == 8)
		clrbits_le32(&host->base->msdc_cfg, MSDC_CFG_HS400_CK_MODE);
	else
		clrbits_le32(&host->base->msdc_cfg,
			     MSDC_CFG_HS400_CK_MODE_EXT);

	if (timing == UHS_DDR50 || timing == MMC_DDR_52 ||
	    timing == MMC_HS_400) {
		if (timing == MMC_HS_400)
			mode = 0x3;
		else
			mode = 0x2; /* ddr mode and use divisor */

		if (hz >= (host->src_clk_freq >> 2)) {
			div = 0; /* mean div = 1/4 */
			sclk = host->src_clk_freq >> 2; /* sclk = clk / 4 */
		} else {
			div = (host->src_clk_freq + ((hz << 2) - 1)) /
			       (hz << 2);
			sclk = (host->src_clk_freq >> 2) / div;
			div = (div >> 1);
		}

		if (timing == MMC_HS_400 && hz >= (host->src_clk_freq >> 1)) {
			if (host->dev_comp->clk_div_bits == 8)
				setbits_le32(&host->base->msdc_cfg,
					     MSDC_CFG_HS400_CK_MODE);
			else
				setbits_le32(&host->base->msdc_cfg,
					     MSDC_CFG_HS400_CK_MODE_EXT);

			sclk = host->src_clk_freq >> 1;
			div = 0; /* div is ignore when bit18 is set */
		}
	} else if (hz >= host->src_clk_freq) {
		mode = 0x1; /* no divisor */
		div = 0;
		sclk = host->src_clk_freq;
	} else {
		mode = 0x0; /* use divisor */
		if (hz >= (host->src_clk_freq >> 1)) {
			div = 0; /* mean div = 1/2 */
			sclk = host->src_clk_freq >> 1; /* sclk = clk / 2 */
		} else {
			div = (host->src_clk_freq + ((hz << 2) - 1)) /
			       (hz << 2);
			sclk = (host->src_clk_freq >> 2) / div;
		}
	}

	clrbits_le32(&host->base->msdc_cfg, MSDC_CFG_CKPDN);

	if (host->dev_comp->clk_div_bits == 8) {
		div = MIN(div, (u32)(MSDC_CFG_CKDIV_M >> MSDC_CFG_CKDIV_S));
		clrsetbits_le32(&host->base->msdc_cfg,
				MSDC_CFG_CKMOD_M | MSDC_CFG_CKDIV_M,
				(mode << MSDC_CFG_CKMOD_S) |
				(div << MSDC_CFG_CKDIV_S));
	} else {
		div = MIN(div, (u32)(MSDC_CFG_CKDIV_EXT_M >>
				      MSDC_CFG_CKDIV_EXT_S));
		clrsetbits_le32(&host->base->msdc_cfg,
				MSDC_CFG_CKMOD_EXT_M | MSDC_CFG_CKDIV_EXT_M,
				(mode << MSDC_CFG_CKMOD_EXT_S) |
				(div << MSDC_CFG_CKDIV_EXT_S));
	}

	readl_poll_timeout(&host->base->msdc_cfg, reg,
			   reg & MSDC_CFG_CKSTB, 1000000);

	setbits_le32(&host->base->msdc_cfg, MSDC_CFG_CKPDN);
	host->sclk = sclk;
	host->mclk = hz;

	/* needed because clk changed. */
	msdc_set_timeout(host, host->timeout_ns, host->timeout_clks);

	/*
	 * mmc_select_hs400() will drop to 50Mhz and High speed mode,
	 * tune result of hs200/200Mhz is not suitable for 50Mhz
	 */
	if (host->sclk <= 52000000) {
		writel(host->def_tune_para.iocon, &host->base->msdc_iocon);
		writel(host->def_tune_para.pad_tune,
		       &host->base->pad_tune);
	} else {
		writel(host->saved_tune_para.iocon, &host->base->msdc_iocon);
		writel(host->saved_tune_para.pad_tune,
		       &host->base->pad_tune);
	}

	VERBOSE("sclk: %d, timing: %d\n", host->sclk, timing);
}

static int msdc_ops_set_ios(unsigned int clock, unsigned int bus_width)
{
	struct msdc_host *host = &msdc_host;

	msdc_set_buswidth(host, bus_width);

#if 0
	if (mmc->clk_disable)
		clock = 0;
	else if (clock < mmc->cfg->f_min)
		clock = mmc->cfg->f_min;
#endif

	if (host->mclk != clock /*|| host->timing != mmc->selected_mode*/)
		msdc_set_mclk(host, clock);

	return 0;
}

static int mtk_mmc_prepare(int lba, uintptr_t buf, size_t size)
{
	xfer_lba = lba;
	xfer_buf = buf;
	xfer_size = size;

	return 0;
}


static void msdc_fifo_read(struct msdc_host *host, uint8_t *buf, size_t size)
{
	u32 *wbuf;

	while ((size_t)buf % 4) {
		*buf++ = readb(&host->base->msdc_rxdata);
		size--;
	}

	wbuf = (uint32_t *)buf;
	while (size >= 4) {
		*wbuf++ = readl(&host->base->msdc_rxdata);
		size -= 4;
	}

	buf = (uint8_t *)wbuf;
	while (size) {
		*buf++ = readb(&host->base->msdc_rxdata);
		size--;
	}
}

static int mtk_mmc_read(int lba, uintptr_t buf, size_t size)
{
	u32 status;
	u32 chksz;
	int ret = 0;
	struct msdc_host *host = &msdc_host;

	while (1) {
		status = readl(&host->base->msdc_int);
		writel(status, &host->base->msdc_int);
		status &= DATA_INTS_MASK;

		if (status & MSDC_INT_DATCRCERR) {
			ret = -EIO;
			break;
		}

		if (status & MSDC_INT_DATTMO) {
			ret = -ETIMEDOUT;
			break;
		}

		chksz = MIN(size, (size_t)MSDC_FIFO_SIZE);

		if (msdc_fifo_rx_bytes(host) >= chksz) {
			msdc_fifo_read(host, (uint8_t*) buf, chksz);
			buf += chksz;
			size -= chksz;
		}

		if (status & MSDC_INT_XFER_COMPL) {
			if (size) {
				ERROR("data not fully read\n");
				ret = -EIO;
			}
			break;
		}
	}

	return ret;
}

static int mtk_mmc_write(int lba, uintptr_t buf, size_t size)
{
	INFO("mmc_write: %d:%zu\n", lba, size);
	return -1;
}

static const struct mmc_ops mtk_mmc_ops = {
	.init = msdc_init_hw,
	.send_cmd = mtk_mmc_send_cmd,
	.set_ios = msdc_ops_set_ios,
	.prepare = mtk_mmc_prepare,
	.read = mtk_mmc_read,
	.write = mtk_mmc_write,
};

static struct mmc_device_info mtk_mmc_device_info = {
	.mmc_dev_type = MMC_IS_EMMC,
};

void mtk_mmc_init(uintptr_t reg_base, struct msdc_compatible *compat,
				  uint32_t src_clk)
{
	msdc_host.base = (struct mtk_sd_regs*) reg_base;
	msdc_host.dev_comp = compat;

	msdc_host.src_clk_freq = src_clk;
	msdc_host.timeout_ns = 100000000;
	msdc_host.timeout_clks = 3 * 1048576;

	mmc_init(&mtk_mmc_ops, 50000000, MMC_BUS_WIDTH_1, 0, &mtk_mmc_device_info);
}
