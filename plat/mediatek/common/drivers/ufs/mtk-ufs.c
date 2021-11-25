#include <stddef.h>
#include <stdint.h>
#include <drivers/ufs.h>
#include <common/debug.h>

static int mtk_ufs_phy_init(ufs_params_t *params)
{
	return 0;
}

static int mtk_ufs_phy_set_pwr_mode(ufs_params_t *params)
{
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
