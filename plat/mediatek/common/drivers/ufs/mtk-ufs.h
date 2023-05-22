#ifndef __MTK_UFS__

#include <drivers/ufs.h>

#define RX_MIN_ACTIVATETIME_CAP_OFFSET          0x008F
#define RX_HS_G2_SYNC_LENGTH_CAP_OFFSET         0x0094
#define RX_HS_G3_SYNC_LENGTH_CAP_OFFSET         0x0095

#define PA_AVAIL_TX_DATA_LANES_OFFSET           0x1520
#define PA_TX_SKIP_OFFSET                       0x155C
#define PA_TX_SKIP_PERIOD_OFFSET                0x155D
#define PA_LOCAL_TX_LCC_ENABLE_OFFSET           0x155E
#define PA_ACTIVE_TX_DATA_LANES_OFFSET          0x1560
#define PA_CONNECTED_TX_DATA_LANES_OFFSET       0x1561
#define PA_TX_TRAILING_CLOCKS_OFFSET            0x1564
#define PA_TX_GEAR_OFFSET                       0x1568
#define PA_TX_TERMINATION_OFFSET                0x1569
#define PA_HS_SERIES_OFFSET                     0x156A
#define PA_PWR_MODE_OFFSET                      0x1571
#define PA_ACTIVE_RX_DATA_LANES_OFFSET          0x1580
#define PA_CONNECTED_RX_DATA_LANES_OFFSET       0x1581
#define PA_RX_PWR_STATUS_OFFSET                 0x1582
#define PA_RX_GEAR_OFFSET                       0x1583
#define PA_RX_TERMINATION_OFFSET                0x1584
#define PA_SCRAMBLING_OFFSET                    0x1585
#define PA_MAX_RX_PWM_GEAR_OFFSET               0x1586
#define PA_MAX_RX_HS_GEAR_OFFSET                0x1587
#define PA_PACP_REQ_TIMEOUT_OFFSET              0x1590
#define PA_PACP_REQ_EOB_TIMEOUT_OFFSET          0x1591
#define PA_REMOTE_VER_INFO_OFFSET               0x15A0
#define PA_LOGICAL_LANE_MAP_OFFSET              0x15A1
#define PA_TACTIVATE_OFFSET                     0x15A8
#define PA_PWR_MODE_USER_DATA0_OFFSET           0x15B0
#define PA_PWR_MODE_USER_DATA1_OFFSET           0x15B1
#define PA_PWR_MODE_USER_DATA2_OFFSET           0x15B2
#define PA_PWR_MODE_USER_DATA3_OFFSET           0x15B3
#define PA_PWR_MODE_USER_DATA4_OFFSET           0x15B4
#define PA_PWR_MODE_USER_DATA5_OFFSET           0x15B5

#define DL_TC0_TX_FC_THRESHOLD_OFFSET           0x2040
#define DL_AFC0_CREDIT_THRESHOLD_OFFSET         0x2044
#define DL_TC0_OUT_ACK_THRESHOLD_OFFSET         0x2045

#define DME_FC0_PROTECTION_TIMEOUT_OFFSET       0xD041
#define DME_TC0_REPLAY_TIMEOUT_OFFSET           0xD042
#define DME_AFC0_REQ_TIMEOUT_OFFSET             0xD043
#define DME_FC1_PROTECTION_TIMEOUT_OFFSET       0xD044
#define DME_TC1_REPLAY_TIMEOUT_OFFSET           0xD045
#define DME_AFC1_REQ_TIMEOUT_OFFSET             0xD046

void mtk_ufs_init(ufs_params_t *params);

#endif
