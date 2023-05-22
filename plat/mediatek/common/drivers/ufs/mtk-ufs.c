#include <stddef.h>
#include <stdint.h>
#include <drivers/ufs.h>
#include <lib/mmio.h>
#include <common/debug.h>

#include "mtk-ufs.h"

static int mtk_ufs_phy_init(ufs_params_t *params)
{
	return 0;
}

static int mtk_ufs_phy_set_pwr_mode(ufs_params_t *params)
{
	unsigned int data, tx_lanes, rx_lanes;
	uintptr_t base;
	unsigned int flags;

	base = params->reg_base;
	flags = params->flags;
	if ((flags & UFS_FLAGS_VENDOR_SKHYNIX) != 0U) {
		NOTICE("ufs: H**** device must set VS_DebugSaveConfigTime 0x10\n");
		/* VS_DebugSaveConfigTime */
		ufshc_dme_set(0xd0a0, 0x0, 0x10);
		/* sync length */
		ufshc_dme_set(0x1556, 0x0, 0x48);
	}

	ufshc_dme_get(PA_TACTIVATE_OFFSET, 0, &data);
	if (data < 7)
		ufshc_dme_set(PA_TACTIVATE_OFFSET, 0, 7);

	ufshc_dme_get(PA_CONNECTED_TX_DATA_LANES_OFFSET, 0, &tx_lanes);
	ufshc_dme_get(PA_CONNECTED_RX_DATA_LANES_OFFSET, 0, &rx_lanes);

	ufshc_dme_set(PA_TX_SKIP_OFFSET, 0, 0);
	ufshc_dme_set(PA_TX_GEAR_OFFSET, 0, 3);
	ufshc_dme_set(PA_RX_GEAR_OFFSET, 0, 3);
	ufshc_dme_set(PA_HS_SERIES_OFFSET, 0, 2);
	ufshc_dme_set(PA_TX_TERMINATION_OFFSET, 0, 1);
	ufshc_dme_set(PA_RX_TERMINATION_OFFSET, 0, 1);
	ufshc_dme_set(PA_SCRAMBLING_OFFSET, 0, 0);
	ufshc_dme_set(PA_ACTIVE_TX_DATA_LANES_OFFSET, 0, tx_lanes);
	ufshc_dme_set(PA_ACTIVE_RX_DATA_LANES_OFFSET, 0, rx_lanes);
	ufshc_dme_set(PA_PWR_MODE_USER_DATA0_OFFSET, 0, 8191);
	ufshc_dme_set(PA_PWR_MODE_USER_DATA1_OFFSET, 0, 65535);
	ufshc_dme_set(PA_PWR_MODE_USER_DATA2_OFFSET, 0, 32767);
	ufshc_dme_set(DME_FC0_PROTECTION_TIMEOUT_OFFSET, 0, 8191);
	ufshc_dme_set(DME_TC0_REPLAY_TIMEOUT_OFFSET, 0, 65535);
	ufshc_dme_set(DME_AFC0_REQ_TIMEOUT_OFFSET, 0, 32767);
	ufshc_dme_set(PA_PWR_MODE_USER_DATA3_OFFSET, 0, 8191);
	ufshc_dme_set(PA_PWR_MODE_USER_DATA4_OFFSET, 0, 65535);
	ufshc_dme_set(PA_PWR_MODE_USER_DATA5_OFFSET, 0, 32767);
	ufshc_dme_set(DME_FC1_PROTECTION_TIMEOUT_OFFSET, 0, 8191);
	ufshc_dme_set(DME_TC1_REPLAY_TIMEOUT_OFFSET, 0, 65535);
	ufshc_dme_set(DME_AFC1_REQ_TIMEOUT_OFFSET, 0, 32767);

	ufshc_dme_set(PA_PWR_MODE_OFFSET, 0, 0x11);
	do {
		data = mmio_read_32(base + IS);
	} while ((data & UFS_INT_UPMS) == 0);
	mmio_write_32(base + IS, UFS_INT_UPMS);
	data = mmio_read_32(base + HCS);
	if ((data & HCS_UPMCRS_MASK) == HCS_PWR_LOCAL)
		INFO("ufs: change power mode success\n");
	else
		WARN("ufs: HCS.UPMCRS error, HCS:0x%x\n", data);

	return 0;
}

static ufs_ops_t mtk_ufs_ops = {
	.phy_init = mtk_ufs_phy_init,
	.phy_set_pwr_mode = mtk_ufs_phy_set_pwr_mode,
};

void mtk_ufs_init(ufs_params_t *params)
{
	ufs_init(&mtk_ufs_ops, params);
}
