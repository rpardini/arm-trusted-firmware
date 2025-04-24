/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright (c) 2025 MediaTek Inc.
 */

#ifndef MT_SPM_HWREQ_H
#define MT_SPM_HWREQ_H

#include <drivers/spm/mt_spm_resource_req.h>

/* Resource requirement which HW CG support */
enum {
	HWCG_DDREN = 0,
	HWCG_VRF18,
	HWCG_INFRA,
	HWCG_F26M,
	HWCG_PMIC,
	HWCG_VCORE,
	HWCG_MAX
};

/* Signal that monitor by HW CG  */
enum spm_hwcg_setting {
	HWCG_PWR,
	HWCG_PWR_MSB,
	HWCG_MODULE_BUSY,
	HWCG_SETTING_MAX
};

enum spm_pwr_status {
	HWCG_PWR_MD1 = 0,
	HWCG_PWR_CONN,
	HWCG_PWR_IFR,
	HWCG_PWR_PERI,
	HWCG_PWR_UFS0,
	HWCG_PWR_UFS0_PHY,
	HWCG_PWR_AUDIO,
	HWCG_PWR_ADSP_TOP,
	HWCG_PWR_ADSP_INFRA = 8,
	HWCG_PWR_ADSP_AO,
	HWCG_PWR_ISP_IMG1,
	HWCG_PWR_ISP_IMG2,
	HWCG_PWR_ISP_IPE,
	HWCG_PWR_ISP_VCORE,
	HWCG_PWR_VDE0,
	HWCG_PWR_VDE1,
	HWCG_PWR_VEN0 = 16,
	HWCG_PWR_VEN1,
	HWCG_PWR_CAM_MAIN,
	HWCG_PWR_CAM_MRAW,
	HWCG_PWR_CAM_SUBA,
	HWCG_PWR_CAM_SUBB,
	HWCG_PWR_CAM_SUBC,
	HWCG_PWR_CAM_VCORE,
	HWCG_PWR_CAM_CCU = 24,
	HWCG_PWR_CAM_CCU_AO,
	HWCG_PWR_MDP0,
	HWCG_PWR_MDP1,
	HWCG_PWR_DIS0,
	HWCG_PWR_DIS1,
	HWCG_PWR_MM_INFRA,
	HWCG_PWR_MM_PROC,
	HWCG_PWR_MAX
};

enum spm_pwr_msb_status {
	HWCG_PWR_DP_TX = 0,
	HWCG_PWR_SCP_CORE,
	HWCG_PWR_SCP_PERI,
	HWCG_PWR_DPM0,
	HWCG_PWR_DPM1,
	HWCG_PWR_EMI0,
	HWCG_PWR_EMI1,
	HWCG_PWR_CSI_RX,
	HWCG_PWR_SSRSYS = 8,
	HWCG_PWR_SSPM,
	HWCG_PWR_SSUSB,
	HWCG_PWR_SSUSB_PHY,
	HWCG_PWR_EDP_TX,
	HWCG_PWR_PCIE,
	HWCG_PWR_PCIE_PHY,
	HWCG_PWR_MSB_EMPTY_BIT15,
	HWCG_PWR_MSB_EMPTY_BIT16 = 16,
	HWCG_PWR_MSB_EMPTY_BIT17,
	HWCG_PWR_MSB_EMPTY_BIT18,
	HWCG_PWR_MSB_EMPTY_BIT19,
	HWCG_PWR_MSB_EMPTY_BIT20,
	HWCG_PWR_MSB_EMPTY_BIT21,
	HWCG_PWR_MSB_EMPTY_BIT22,
	HWCG_PWR_MSB_EMPTY_BIT23,
	HWCG_PWR_MSB_EMPTY_BIT24 = 24,
	HWCG_PWR_MSB_EMPTY_BIT25,
	HWCG_PWR_MSB_EMPTY_BIT26,
	HWCG_PWR_MSB_EMPTY_BIT27,
	HWCG_PWR_MSB_EMPTY_BIT28,
	HWCG_PWR_MSB_EMPTY_BIT29,
	HWCG_PWR_MSB_EMPTY_BIT30,
	HWCG_PWR_MSB_EMPTY_BIT31,
	HWCG_PWR_MSB_MAX
};

enum spm_hwcg_module_busy {
	HWCG_MODULE_AUDIO = 0,
	HWCG_MODULE_MMPLL,
	HWCG_MODULE_UFSPLL,
	HWCG_MODULE_MSDCPLL,
	HWCG_MODULE_UNIVPLL,
	HWCG_MODULE_MAX
};

enum spm_hwcg_sta_type {
	HWCG_STA_DEFAULT_MASK,
	HWCG_STA_MASK
};

/* Signal that monitor by HW CG  */
enum spm_peri_req_setting {
	PERI_REQ_EN  = 0,
	PERI_REQ_SETTING_MAX
};

/* Resource requirement which PERI REQ support */
enum spm_peri_req {
	PERI_REQ_F26M = 0,
	PERI_REQ_INFRA,
	PERI_REQ_SYSPLL,
	PERI_REQ_APSRC,
	PERI_REQ_DDREN,
	PERI_REQ_EMI,
	PERI_REQ_PMIC,
	PERI_REQ_MAX
};

enum spm_peri_req_sta_type {
	PERI_REQ_STA_DEFAULT_MASK,
	PERI_REQ_STA_MASK,
	PERI_REQ_STA_MAX
};

enum spm_peri_req_status {
	PERI_RES_REQ_EN,
	PERI_REQ_STATUS_MAX
};

enum spm_peri_req_status_raw {
	PERI_REQ_STATUS_RAW_NUM,
	PERI_REQ_STATUS_RAW_NAME,
	PERI_REQ_STATUS_RAW_STA,
	PERI_REQ_STATUS_RAW_MAX
};

enum spm_peri_req_en {
	PERI_REQ_EN_DMA = 0,
	PERI_REQ_EN_UART0,
	PERI_REQ_EN_UART1,
	PERI_REQ_EN_UART2,
	PERI_REQ_EN_PWM,
	PERI_REQ_EN_SPI0,
	PERI_REQ_EN_SPI1,
	PERI_REQ_EN_SPI2,
	PERI_REQ_EN_SPI3 = 8,
	PERI_REQ_EN_SPI4,
	PERI_REQ_EN_SPI5,
	PERI_REQ_EN_SPI6,
	PERI_REQ_EN_SPI7,
	PERI_REQ_EN_I2C,
	PERI_REQ_EN_MSDC0,
	PERI_REQ_EN_MSDC1,
	PERI_REQ_EN_SSUSB = 16,
	PERI_REQ_EN_AFE,
	PERI_REQ_EN_MAX = 19,
	PERI_REQ_EN_PCIE = 22,
};

struct spm_peri_req_sta {
	uint32_t sta;
};

struct spm_peri_req_info {
	uint32_t req_en;
	uint32_t req_sta;
};

struct spm_hwcg_sta {
	uint32_t sta;
};

#define MT_SPM_HW_CG_STA_INIT(_x)	({ if (_x) _x->sta = 0; })

#define INFRA_AO_OFFSET(offset)	(INFRACFG_AO_BASE + offset)
#define INFRA_SW_CG_0_MASK	INFRA_AO_OFFSET(0x060)
#define INFRA_SW_CG_1_MASK	INFRA_AO_OFFSET(0x064)
#define INFRA_SW_CG_2_MASK	INFRA_AO_OFFSET(0x068)
#define INFRA_SW_CG_3_MASK	INFRA_AO_OFFSET(0x0CC)
#define INFRA_SW_CG_4_MASK	INFRA_AO_OFFSET(0x0EC)

#define REG_PERI_REQ_EN(N)	(PERICFG_AO_BASE + 0x050 + 0x4 * N)
#define REG_PERI_REQ_STA(N)	(PERICFG_AO_BASE + 0x06C + 0x4 * N)

int spm_hwreq_init(void);

/* res:
 *	Please refer the mt_spm_resource_req.h.
 *	Section of SPM resource request internal bit_mask.
 */
void spm_hwcg_ctrl(uint32_t res, enum spm_hwcg_setting type,
				uint32_t is_set, uint32_t val);

/* idx:
 *	index of HWCG setting.
 */
void spm_hwcg_ctrl_by_index(uint32_t idx, enum spm_hwcg_setting type,
				uint32_t is_set, uint32_t val);


/* res:
 *	Please refer the mt_spm_resource_req.h.
 *	Section of SPM resource request internal bit_mask.
 */
int spm_hwcg_get_setting(uint32_t res, enum spm_hwcg_sta_type sta_type,
					enum spm_hwcg_setting type, struct spm_hwcg_sta *sta);

/* idx:
 *	index of HWCG setting.
 */
int spm_hwcg_get_setting_by_index(uint32_t idx, enum spm_hwcg_sta_type sta_type,
					enum spm_hwcg_setting type, struct spm_hwcg_sta *sta);

uint32_t spm_hwcg_get_status(uint32_t idx, enum spm_hwcg_setting type);

int spm_hwcg_name(uint32_t idex, char *name, size_t sz);

static inline uint32_t spm_hwcg_num(void)
{
	return HWCG_MAX;
}

static inline uint32_t spm_hwcg_setting_num(void)
{
	return HWCG_SETTING_MAX;
}

uint32_t spm_peri_req_get_status(uint32_t idx,
					enum spm_peri_req_status type);

uint32_t spm_peri_req_get_status_raw(enum spm_peri_req_status_raw type,
					uint32_t idx,
					char *name, size_t sz);

static inline uint32_t spm_peri_req_num(void)
{
	return PERI_REQ_MAX;
}

static inline uint32_t spm_peri_req_setting_num(void)
{
	return PERI_REQ_SETTING_MAX;
}

int spm_peri_req_get_setting_by_index(uint32_t idx,
					enum spm_peri_req_sta_type sta_type,
					struct spm_peri_req_sta *sta);

void spm_peri_req_ctrl_by_index(uint32_t idx,
				uint32_t is_set, uint32_t val);

int spm_peri_req_name(uint32_t idex, char *name, size_t sz);

#endif
