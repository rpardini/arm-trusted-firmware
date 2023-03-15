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
	bool use_dma_mode;
	uintptr_t top_base;
};

void mtk_mmc_init(uintptr_t reg_base, struct msdc_compatible *compat,
				  uint32_t src_clk);

enum dma_data_direction {
	DMA_BIDIRECTIONAL = 0,
	DMA_TO_DEVICE = 1,
	DMA_FROM_DEVICE = 2,
	DMA_NONE = 3,
};

typedef uint32_t dma_addr_t;

#endif
