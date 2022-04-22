#ifndef MT8365_DEVAPC_H
#define MT8365_DEVAPC_H

#define DEVAPC_AO_INFRA_BASE		(0x1000E000)
#define DEVAPC_AO_MM_BASE		(0x1001C000)
#define DEVAPC_AO_MD_BASE		(0x10019000)

#define MOD_NO_IN_1_DEVAPC		16
#define MASTER_MOD_NO_IN_1_DEVAPC	8
#define SLAVE_INFRA_MAX_INDEX		160
#define SLAVE_MM_MAX_INDEX		67

/* Master index */
#define DAPC_MASTER_INDEX_MSDC0		5
#define DAPC_MASTER_INDEX_SPM		10
#define DAPC_MASTER_INDEX_MAX		34

#define PERMISSION(DEV_NAME, ATTR1) \
{ .permission = { [0] = ATTR1, [1 ... (DAPC_MAX_DOMAIN -1)] = 0 } }

static const struct device_info D_APC_INFRA_Devices[] = {
    /* module,                          AP permission */

    /* 0 */
    PERMISSION("INFRA_AO_TOPCKGEN",                   NO_SEC),
    PERMISSION("INFRA_AO_INFRASYS_CONFIG_REGS",       NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_PERICFG",                    NO_SEC),
    PERMISSION("INFRA_AO_EFUSE_AO_DEBUG",             NO_SEC),
    PERMISSION("INFRA_AO_GPIO",                       NO_SEC),
    PERMISSION("INFRA_AO_SLEEP_CONTROLER",            NO_SEC),
    PERMISSION("INFRA_AO_TOPRGU",                     NO_SEC),
    PERMISSION("INFRA_AO_APXGPT",                     NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),

    /* 10 */
    PERMISSION("INFRA_AO_SEJ",                        S_RW_ONLY),
    PERMISSION("INFRA_AO_AP_CIRQ_EINT",               NO_SEC),
    PERMISSION("INFRA_AO_APMIXEDSYS",                 NO_SEC),
    PERMISSION("INFRA_AO_PMIC_WRAP",                  NO_SEC),
    PERMISSION("INFRA_AO_DEVICE_APC_AO_INFRA_PERI",   S_RW_ONLY),
    PERMISSION("INFRA_AO_SLEEP_CONTROLER_MD",         NO_SEC),
    PERMISSION("INFRA_AO_KEYPAD",                     NO_SEC),
    PERMISSION("INFRA_AO_TOP_MISC",                   NO_SEC),
    PERMISSION("INFRA_AO_DVFS_CTRL_PROC",             NO_SEC),
    PERMISSION("INFRA_AO_MBIST_AO_REG",               NO_SEC),

    /* 20 */
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_SYS_TIMER",                  NO_SEC),
    PERMISSION("INFRA_AO_MDEM_TEMP_SHARE",            NO_SEC),
    PERMISSION("INFRA_AO_DEVICE_APC_AO_MD",           S_RW_ONLY),
    PERMISSION("INFRA_AO_SECURITY_AO",                NO_SEC),
    PERMISSION("INFRA_AO_TOPCKGEN_REG",               NO_SEC),
    PERMISSION("INFRA_AO_DEVICE_APC_AO_MM",           S_RW_ONLY),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),

    /* 30 */
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_MCUSYS_REG0",                NO_SEC),
    PERMISSION("INFRASYS_MCUSYS_REG1",                NO_SEC),
    PERMISSION("INFRASYS_MCUSYS_REG2",                NO_SEC),
    PERMISSION("INFRASYS_MCUSYS_REG3",                NO_SEC),
    PERMISSION("INFRASYS_SYS_CIRQ",                   NO_SEC),
    PERMISSION("INFRASYS_MM_IOMMU",                   NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_DEVICE_APC",                 NO_SEC),

    /* 40 */
    PERMISSION("INFRASYS_DBG_TRACKER",                NO_SEC),
    PERMISSION("INFRASYS_CCIF0_AP",                   NO_SEC),
    PERMISSION("INFRASYS_CCIF0_MD",                   NO_SEC),
    PERMISSION("INFRASYS_CCIF1_MD",                   NO_SEC),
    PERMISSION("INFRASYS_CLDMA_MD",                   NO_SEC),
    PERMISSION("INFRASYS_MBIST",                      NO_SEC),
    PERMISSION("INFRASYS_INFRA_PDN_REGISTER",         NO_SEC),
    PERMISSION("INFRASYS_TRNG",                       NO_SEC),
    PERMISSION("INFRASYS_CGPU",                       NO_SEC),
    PERMISSION("INFRASYS_MCUPM_SRAM2",                NO_SEC),

    /* 50 */
    PERMISSION("INFRASYS_CQ_DMA",                     NO_SEC),
    PERMISSION("INFRASYS_MCUPM_SRAM3",                NO_SEC),
    PERMISSION("INFRASYS_SRAMROM",                    NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_MCUPM_REG",                  NO_SEC),
    PERMISSION("INFRASYS_MCUPM_SRAM0",                NO_SEC),
    PERMISSION("INFRASYS_MCUPM_SRAM1",                NO_SEC),
    PERMISSION("INFRASYS_EMI",                        NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_CLDMA_PDN_AP",               NO_SEC),

    /* 60 */
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_EMI_MPU",                    S_RW_ONLY),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_DRAMC_CH0_TOP0",             NO_SEC),
    PERMISSION("INFRASYS_DRAMC_CH0_TOP1",             NO_SEC),
    PERMISSION("INFRASYS_DRAMC_CH0_TOP2",             NO_SEC),

    /* 70 */
    PERMISSION("INFRASYS_DRAMC_CH0_TOP3",             NO_SEC),
    PERMISSION("INFRASYS_DRAMC_CH0_TOP4",             NO_SEC),
    PERMISSION("INFRASYS_DRAMC_CH1_TOP0",             NO_SEC),
    PERMISSION("INFRASYS_DRAMC_CH1_TOP1",             NO_SEC),
    PERMISSION("INFRASYS_DRAMC_CH1_TOP2",             NO_SEC),
    PERMISSION("INFRASYS_DRAMC_CH1_TOP3",             NO_SEC),
    PERMISSION("INFRASYS_DRAMC_CH1_TOP4",             NO_SEC),
    PERMISSION("INFRASYS_GCE",                        NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),

    /* 80 */
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INNFRA_ECC_TOP",                      NO_SEC),
    PERMISSION("INFRA_GCPU",                          NO_SEC),
    PERMISSION("INFRA_GCPU",                          NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),

    /* 90 */
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_MCUCFG",                     NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("PERISYS_APDMA",                       NO_SEC),
    PERMISSION("PERISYS_AUXADC",                      NO_SEC),

    /* 100 */
    PERMISSION("PERISYS_UART0",                       NO_SEC),
    PERMISSION("PERISYS_UART1",                       NO_SEC),
    PERMISSION("PERISYS_UART2",                       NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("PERISYS_PWM",                         NO_SEC),
    PERMISSION("PERISYS_I2C0",                        NO_SEC),
    PERMISSION("PERISYS_I2C1",                        NO_SEC),
    PERMISSION("PERISYS_I2C2",                        NO_SEC),
    PERMISSION("PERISYS_SPI0",                        NO_SEC),
    PERMISSION("PERISYS_PTP",                         NO_SEC),

    /* 110 */
    PERMISSION("PERISYS_BTIF",                        NO_SEC),
    PERMISSION("PERISYS_I2C6",                        NO_SEC),
    PERMISSION("PERISYS_DISP_PWM",                    NO_SEC),
    PERMISSION("PERISYS_I2C3",                        NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),

    /* 120 */
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("INFRA_AO_RESERVE",                    NO_SEC),
    PERMISSION("PERISYS_NFI",                         NO_SEC),
    PERMISSION("PERISYS_NFIECC",                      NO_SEC),
    PERMISSION("PERISYS_USB",                         NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("PERISYS_AUDIO",                       NO_SEC),
    PERMISSION("PERISYS_MSDC0",                       NO_SEC),
    PERMISSION("PERISYS_MSDC1",                       NO_SEC),

    /* 130 */
    PERMISSION("PERISYS_MSDC2",                       NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("INFRASYS_RESERVE",                    NO_SEC),
    PERMISSION("PSRISYS_NIC",                         NO_SEC),
    PERMISSION("EAST_RESERVE",                        NO_SEC),
    PERMISSION("EAST_CSI_TOP_AO",                     NO_SEC),
    PERMISSION("EAST_RESERVE",                        NO_SEC),
    PERMISSION("EAST_RESERVE",                        NO_SEC),

    /* 140 */
    PERMISSION("SOUTH_RESERVE",                       NO_SEC),
    PERMISSION("SOUTH_EFUSE",                         NO_SEC),
    PERMISSION("SOUTH_RESERVE",                       NO_SEC),
    PERMISSION("SOUTH_RESERVE",                       NO_SEC),
    PERMISSION("WEST_MIPI_TX_CONFIG",                 NO_SEC),
    PERMISSION("WEST_MSDC1",                          NO_SEC),
    PERMISSION("WEST_RESERVE_1",                      NO_SEC),
    PERMISSION("WEST_RESERVE_2",                      NO_SEC),
    PERMISSION("NORTH_USBSIF_TOP",                    NO_SEC),
    PERMISSION("NORTH_MSDC0",                         NO_SEC),

    /* 150 */
    PERMISSION("NORTH_RESERVE_0",                     NO_SEC),
    PERMISSION("NORTH_RESERVE_1",                     NO_SEC),
    PERMISSION("PERISYS_CONN",                        NO_SEC),
    PERMISSION("DSP_TIMER",                           NO_SEC),
    PERMISSION("DSP_UART",                            NO_SEC),
    PERMISSION("DSP_CFG",                             NO_SEC),
    PERMISSION("DSPIRQ_CFG",                          NO_SEC),
    PERMISSION("DSP_DTCM_REGION_0",                   NO_SEC),
    PERMISSION("DSP_DTCM_REGION_1",                   NO_SEC),
    PERMISSION("DSP_ITCM",                            NO_SEC),

    /* 160 */
    PERMISSION("APUSYS",                              NO_SEC),
};

static const struct device_info D_APC_MM_Devices[] = {
    /* module,                   AP permission */

    /* 0 */
    PERMISSION("GPU",                      NO_SEC),
    PERMISSION("GPUDVFSMON",               NO_SEC),
    PERMISSION("GPUCONFIG",                NO_SEC),
    PERMISSION("MFG_OTHERS",               NO_SEC),
    PERMISSION("MMSYS_CONFIG",             NO_SEC),
    PERMISSION("DISP_MUTEX",               NO_SEC),
    PERMISSION("SMI_COMMON",               NO_SEC),
    PERMISSION("SMI_LARB0",                NO_SEC),
    PERMISSION("MDP_RDMA0",                NO_SEC),
    PERMISSION("MDP_CCORR0",               NO_SEC),

    /* 10 */
    PERMISSION("MDP_RSZ0",                 NO_SEC),
    PERMISSION("MDP_RSZ1",                 NO_SEC),
    PERMISSION("MDP_WROT1",                NO_SEC),
    PERMISSION("MDP_WROT0",                NO_SEC),
    PERMISSION("MDP_TDSHP0",               NO_SEC),
    PERMISSION("DISP_OVL0",                NO_SEC),
    PERMISSION("DISP_OVL0_2L",             NO_SEC),
    PERMISSION("DISP_RDMA0",               NO_SEC),
    PERMISSION("DISP_WDMA0",               NO_SEC),
    PERMISSION("DISP_COLOR0",              NO_SEC),

    /* 20 */
    PERMISSION("DISP_CCORR0",              NO_SEC),
    PERMISSION("DISP_AAL0",                NO_SEC),
    PERMISSION("DISP_GAMMA0",              NO_SEC),
    PERMISSION("DISP_DITHER0",             NO_SEC),
    PERMISSION("DSI0",                     NO_SEC),
    PERMISSION("DISP_RSZ0",                NO_SEC),
    PERMISSION("DISP_RDMA1",               NO_SEC),
    PERMISSION("MDP_RDMA1",                NO_SEC),
    PERMISSION("DPI0",                     NO_SEC),
    PERMISSION("LVDS",                     NO_SEC),

    /* 30 */
    PERMISSION("CAMSYS_CAMSYS_TOP",        NO_SEC),
    PERMISSION("CAMSYS_LARB2",             NO_SEC),
    PERMISSION("CAMSYS_RESERVED",          NO_SEC),
    PERMISSION("CAMSYS_RESERVED",          NO_SEC),
    PERMISSION("CAMSYS_MAIN",              NO_SEC),
    PERMISSION("CAMSYS_LUT",               NO_SEC),
    PERMISSION("CAMSYS_D",                 NO_SEC),
    PERMISSION("CAMSYS_DMA",               NO_SEC),
    PERMISSION("CAMSYS_RESERVED",          NO_SEC),
    PERMISSION("CAMSYS_RESERVED",          NO_SEC),

    /* 40 */
    PERMISSION("CAMSYS_RESERVED",          NO_SEC),
    PERMISSION("CAMSYS_FDVT",              NO_SEC),
    PERMISSION("CAMSYS_RESERVED",          NO_SEC),
    PERMISSION("CAMSYS_MAIN_INNER",        NO_SEC),
    PERMISSION("CAMSYS_D_INNER",           NO_SEC),
    PERMISSION("CAMSYS_DMA_INNER",         NO_SEC),
    PERMISSION("CAMSYS_SENINF_A",          NO_SEC),
    PERMISSION("CAMSYS_SENINF_B",          NO_SEC),
    PERMISSION("CAMSYS_SENINF_C",          NO_SEC),
    PERMISSION("CAMSYS_SENINF_D",          NO_SEC),

    /* 50 */
    PERMISSION("CAMSYS_SENINF_E",          NO_SEC),
    PERMISSION("CAMSYS_SENINF_F",          NO_SEC),
    PERMISSION("CAMSYS_SENINF_G",          NO_SEC),
    PERMISSION("CAMSYS_SENINF_H",          NO_SEC),
    PERMISSION("CAMSYS_CAMSV_TOP_0",       NO_SEC),
    PERMISSION("CAMSYS_CAMSV_TOP_1",       NO_SEC),
    PERMISSION("CAMSYS_CAMSV_TOP_2",       NO_SEC),
    PERMISSION("CAMSYS_CAMSV_TOP_3",       NO_SEC),
    PERMISSION("VDEC_TOP_CLOBAL_CON",      NO_SEC),
    PERMISSION("SMI_LARB3",                NO_SEC),

    /* 60 */
    PERMISSION("VDEC_FULL_TOP",            NO_SEC),
    PERMISSION("VDEC_RESERVE",             NO_SEC),
    PERMISSION("VENC_GLOBAL_CON",          NO_SEC),
    PERMISSION("VENC_SMI_LARB1",           NO_SEC),
    PERMISSION("VENC",                     NO_SEC),
    PERMISSION("VENC_JPGENC",              NO_SEC),
    PERMISSION("VENC_MBIST_CTRL",          NO_SEC),

    /* 67 */
    PERMISSION("VENC_RESERVE",             NO_SEC)
};

static const struct master_domain master_domains[] = {
	{ DAPC_MASTER_INDEX_SPM, DOMAIN_0 },
};

static struct dapc_conf mt8365_dapc_conf = {
	.default_master = DAPC_MASTER_INDEX_MSDC0,
	.infra = D_APC_INFRA_Devices,
	.infra_size = ARRAY_SIZE(D_APC_INFRA_Devices),
	.mm = D_APC_MM_Devices,
	.mm_size = ARRAY_SIZE(D_APC_MM_Devices),
	.master = master_domains,
	.master_size = ARRAY_SIZE(master_domains),
};

static inline void devapc_init(void)
{
	devapc_common_init(&mt8365_dapc_conf);
}

#endif /* MT8365_DEVAPC_H */
