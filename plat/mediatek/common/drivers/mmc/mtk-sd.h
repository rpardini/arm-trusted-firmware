#ifndef __MTK_MMC_H__
#define __MTK_MMC_H__

struct msdc_compatible {
	uint8_t clk_div_bits;
	bool pad_tune0;
	bool async_fifo;
	bool data_tune;
	bool busy_check;
	bool stop_clk_fix;
	bool enhance_rx;
};

void mtk_mmc_init(uintptr_t reg_base, struct msdc_compatible *compat,
				  uint32_t src_clk);

#endif
