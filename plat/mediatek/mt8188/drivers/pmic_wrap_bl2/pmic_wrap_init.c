/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define PMIC_WRAP_PRELOADER 1
#define PMIC_WRAP_DEBUG 1

#include <pmic_wrap_init.h>
#include <reg_PMIC_WRAP.h>
#include <reg_PMIC_WRAP_mac.h>
#include <upmu_hw.h>
#include <platform_def.h>
#if (PMIC_WRAP_PRELOADER)
#elif (PMIC_WRAP_LK)
#elif (PMIC_WRAP_KERNEL)
#elif (PMIC_WRAP_CTP)
#include <gpio.h>
#include <upmu_hw.h>
#else
### Compile error, check SW ENV define
#endif


/************* marco    ******************************************************/
#if (PMIC_WRAP_PRELOADER)
#elif (PMIC_WRAP_LK)
#elif (PMIC_WRAP_KERNEL)
#elif (PMIC_WRAP_SCP)
#elif (PMIC_WRAP_CTP)
#else
### Compile error, check SW ENV define
#endif

#include <stdbool.h>
#define UINT32 uint32_t
#define TRUE true
#define ASSERT(x)

/*-----macro for read/write register -------------------------------------*/

#define WRAP_RD32(addr)            (*(volatile unsigned int *)(addr))
#define WRAP_WR32(addr,data)       ((*(volatile unsigned int *)(addr)) = (unsigned int)data)
#define WRAP_SET_BIT(BS,REG)       ((*(volatile unsigned int*)(REG)) |= (unsigned int)(BS))
#define WRAP_CLR_BIT(BS,REG)       ((*(volatile unsigned int*)(REG)) &= ~((unsigned int)(BS)))

/******  DEBUG marco define *******************************/
#define PWRAPTAG                "[PWRAP] "
#if (PMIC_WRAP_PRELOADER)
	#ifdef PMIC_WRAP_DEBUG
		#define PWRAPFUC(fmt, arg...)   printf(PWRAPTAG "%s\n", __func__)
		#define PWRAPLOG(fmt, arg...)   printf(PWRAPTAG fmt, ##arg)
		#define PWRAPERR(fmt, arg...)   printf(PWRAPTAG "ERR " fmt, ##arg)
	#else
		#define PWRAPFUC(fmt, arg...)
		#define PWRAPLOG(fmt, arg...)
		#define PWRAPERR(fmt, arg...)
	#endif /* end of #ifdef PMIC_WRAP_DEBUG */
	#define PWRAPCRI(fmt, arg...)   printf(PWRAPTAG fmt, ##arg)
#elif (PMIC_WRAP_LK)
	#ifdef PMIC_WRAP_DEBUG
		#define PWRAPFUC(fmt, arg...)   dprintf(CRITICAL, PWRAPTAG "%s\n", __func__)
		#define PWRAPLOG(fmt, arg...)   dprintf(CRITICAL, PWRAPTAG fmt, ##arg)
	#else
		#define PWRAPFUC(fmt, arg...)
		#define PWRAPLOG(fmt, arg...)
	#endif /* end of #ifdef PMIC_WRAP_DEBUG */
	#define PWRAPERR(fmt, arg...)   dprintf(CRITICAL, PWRAPTAG "ERROR,line=%d " fmt, __LINE__, ##arg)
#elif (PMIC_WRAP_KERNEL)
	#ifdef PMIC_WRAP_DEBUG
		#define PWRAPDEB(fmt, arg...)   pr_debug(PWRAPTAG "cpuid=%d," fmt, raw_smp_processor_id(), ##arg)
		#define PWRAPLOG(fmt, arg...)   pr_debug(PWRAPTAG fmt, ##arg)
		#define PWRAPFUC(fmt, arg...)   pr_debug(PWRAPTAG "cpuid=%d,%s\n", raw_smp_processor_id(), __func__)
		#define PWRAPREG(fmt, arg...)   pr_debug(PWRAPTAG fmt, ##arg)
	#else
		#define PWRAPDEB(fmt, arg...)
		#define PWRAPLOG(fmt, arg...)
		#define PWRAPFUC(fmt, arg...)
		#define PWRAPREG(fmt, arg...)
	#endif /* end of #ifdef PMIC_WRAP_DEBUG */
	#define PWRAPERR(fmt, arg...)   pr_err(PWRAPTAG "ERROR,line=%d " fmt, __LINE__, ##arg)
#elif (PMIC_WRAP_SCP)
	#ifdef PMIC_WRAP_DEBUG
		#define PWRAPFUC(fmt, arg...)   PRINTF_D(PWRAPTAG "%s\n", __func__)
		#define PWRAPLOG(fmt, arg...)   PRINTF_D(PWRAPTAG fmt, ##arg)
	#else
		#define PWRAPFUC(fmt, arg...)   /*PRINTF_D(PWRAPTAG "%s\n", __FUNCTION__)*/
		#define PWRAPLOG(fmt, arg...)   /*PRINTF_D(PWRAPTAG fmt, ##arg)*/
	#endif /* end of #ifdef PMIC_WRAP_DEBUG */
	#define PWRAPERR(fmt, arg...)   PRINTF_E(PWRAPTAG "ERROR, line=%d " fmt, __LINE__, ##arg)
#elif (PMIC_WRAP_CTP)
	#ifdef PMIC_WRAP_DEBUG
		#define PWRAPFUC(fmt, arg...)   dbg_print(PWRAPTAG "%s\n", __func__)
		#define PWRAPLOG(fmt, arg...)   dbg_print(PWRAPTAG fmt, ##arg)
	#else
		#define PWRAPFUC(fmt, arg...)   dbg_print(PWRAPTAG "%s\n", __func__)
		#define PWRAPLOG(fmt, arg...)   dbg_print(PWRAPTAG fmt, ##arg)
	#endif /* end of #ifdef PMIC_WRAP_DEBUG */
	#define PWRAPERR(fmt, arg...)   dbg_print(PWRAPTAG "ERROR,line=%d " fmt, __LINE__, ##arg)
#else
	### Compile error, check SW ENV define
#endif
/**********************************************************/


#ifdef PMIC_WRAP_NO_PMIC
#if !(PMIC_WRAP_KERNEL)
signed int pwrap_wacs2(unsigned int write, unsigned int adr,
	unsigned int wdata, unsigned int *rdata)
{
	PWRAPLOG("PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_read(unsigned int adr, unsigned int *rdata)
{
	PWRAPLOG("PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_write(unsigned int adr, unsigned int wdata)
{
	PWRAPLOG("PMIC_WRAP do Nothing.\n");
	return 0;
}
#endif
signed int pwrap_wacs2_read(unsigned int  adr, unsigned int *rdata)
{
	PWRAPLOG("PMIC_WRAP do Nothing.\n");
	return 0;
}

/* Provide PMIC write API */
signed int pwrap_wacs2_write(unsigned int  adr, unsigned int  wdata)
{
	PWRAPLOG("PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_read_nochk(unsigned int adr, unsigned int *rdata)
{
	PWRAPLOG("PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_write_nochk(unsigned int adr, unsigned int wdata)
{
	PWRAPLOG("PMIC_WRAP do Nothing.\n");
	return 0;
}

/*
 *pmic_wrap init,init wrap interface
 *
 */
signed int pwrap_init(void)
{
	PWRAPLOG("PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_init_preloader(void)
{
	PWRAPLOG("PMIC_WRAP do Nothing.\n");
	return 0;
}

void pwrap_dump_all_register(void)
{
	PWRAPLOG("PMIC_WRAP do Nothing.\n");
}

#else /* #ifdef PMIC_WRAP_NO_PMIC */
/*********************start ---internal API***********************************/
static bool _pwrap_timeout_ns(unsigned int start_time_ns,
	unsigned int timeout_time_ns);
static unsigned int _pwrap_get_current_time(void);
static unsigned int _pwrap_time2ns(unsigned int time_us);

static signed int _pwrap_reset_spislv(void);
static signed int _pwrap_init_cipher(void);
static signed int _pwrap_init_reg_clock(unsigned int regck_sel);
static signed int _pwrap_swinf_acc_nochk(unsigned int swinf_no, unsigned int cmd,
	unsigned int write, unsigned int pmifid, unsigned int slvid, unsigned int addr, unsigned int bytecnt,
	unsigned int wdata, unsigned int *rdata);
static signed int _pwrap_swinf_acc(unsigned int swinf_no, unsigned int cmd,
	unsigned int write, unsigned int pmifid, unsigned int slvid, unsigned int addr, unsigned int bytecnt,
	unsigned int wdata, unsigned int *rdata);
static void _pwrap_config_mpu(void);
signed int pwrap_enable_mpu(bool en);
static void _pwrap_config_hw_monitor(void);
/*********************test API************************************************/
static inline void pwrap_dump_ap_register(void);
static unsigned int pwrap_write_test(void);
static unsigned int pwrap_read_test(void);
signed int pwrap_wacs2_read(unsigned int  adr, unsigned int *rdata);
signed int pwrap_wacs2_write(unsigned int  adr, unsigned int  wdata);
/************* end--internal API**********************************************/
/*********************** external API for pmic_wrap user *********************/
signed int pwrap_wacs2_read(unsigned int adr, unsigned int *rdata)
{
	_pwrap_swinf_acc(PMIF_SPI_AP_SWINF_NO, DEFAULT_CMD, 0, PMIF_SPI_PMIFID,
			  DEFAULT_SLVID, adr, DEFAULT_BYTECNT, 0x0, rdata);
	return 0;
}

/* Provide PMIC write API */
signed int pwrap_wacs2_write(unsigned int adr, unsigned int wdata)
{
#ifdef CONFIG_MTK_TINYSYS_SSPM_SUPPORT
	unsigned int flag;

	flag = WRITE_CMD | (1 << WRITE_PMIC);
	pwrap_wacs2_ipi(adr, wdata, flag);
#else
	_pwrap_swinf_acc(PMIF_SPI_AP_SWINF_NO, DEFAULT_CMD, 1, PMIF_SPI_PMIFID,
			DEFAULT_SLVID, adr, DEFAULT_BYTECNT, wdata, 0x0);
#endif
	return 0;
}

signed int pwrap_read(unsigned int adr, unsigned int *rdata)
{
	return _pwrap_swinf_acc(PMIF_SPI_AP_SWINF_NO, DEFAULT_CMD, 0, PMIF_SPI_PMIFID,
			DEFAULT_SLVID, adr, DEFAULT_BYTECNT, 0x0, rdata);
}

signed int pwrap_write(unsigned int adr, unsigned int wdata)
{
	return _pwrap_swinf_acc(PMIF_SPI_AP_SWINF_NO, DEFAULT_CMD, 1, PMIF_SPI_PMIFID,
			DEFAULT_SLVID, adr, DEFAULT_BYTECNT, wdata, 0x0);
}
/******************************************************************************
 * wrapper timeout
 *****************************************************************************/
/* use the same API name with kernel driver
 * however,the timeout API in uboot use tick instead of ns
 */
#ifdef PWRAP_TIMEOUT
static unsigned int _pwrap_get_current_time(void)
{
	return gpt4_get_current_tick();
}

static bool _pwrap_timeout_ns(unsigned int start_time_ns,
	unsigned int timeout_time_ns)
{
	return gpt4_timeout_tick(start_time_ns, timeout_time_ns);
}

static unsigned int _pwrap_time2ns(unsigned int time_us)
{
	return gpt4_time2tick_us(time_us);
}

#else
static unsigned int _pwrap_get_current_time(void)
{
	return 0;
}
static bool _pwrap_timeout_ns(unsigned int start_time_ns,
	unsigned int elapse_time)
{
	return 0;
}

static unsigned int _pwrap_time2ns(unsigned int time_us)
{
	return 0;
}

#endif /* #ifdef PWRAP_TIMEOUT */

/* ##################################################################### */
/* define macro and inline function (for do while loop) */
/* ##################################################################### */
typedef unsigned int(*loop_condition_fp) (unsigned int);  /* define a function pointer */

static inline unsigned int wait_for_fsm_idle(unsigned int x)
{
	return GET_SWINF_2_FSM(x) != WACS_FSM_IDLE;
}

static inline unsigned int wait_for_fsm_vldclr(unsigned int x)
{
	return GET_SWINF_2_FSM(x) != WACS_FSM_WFVLDCLR;
}

static inline unsigned int wait_for_sync(unsigned int x)
{
	return GET_SYNC_IDLE2(x) != WACS_SYNC_IDLE;
}

static inline unsigned int wait_for_idle_and_sync(unsigned int x)
{
	return (GET_WACS2_FSM(x) != WACS_FSM_IDLE) ||
		(GET_SYNC_IDLE2(x) != WACS_SYNC_IDLE);
}

static inline unsigned int wait_for_wrap_idle(unsigned int x)
{
	return (GET_WRAP_FSM(x) != 0x0) || (GET_WRAP_CH_DLE_RESTCNT(x) != 0x0);
}

static inline unsigned int wait_for_wrap_state_idle(unsigned int x)
{
	return GET_WRAP_AG_DLE_RESTCNT(x) != 0;
}

static inline unsigned int wait_for_man_idle_and_noreq(unsigned int x)
{
	return (GET_MAN_REQ(x) != MAN_FSM_NO_REQ) ||
		(GET_MAN_FSM(x) != MAN_FSM_IDLE);
}

static inline unsigned int wait_for_man_vldclr(unsigned int x)
{
	return GET_MAN_FSM(x) != MAN_FSM_WFVLDCLR;
}

static inline unsigned int wait_for_cipher_ready(unsigned int x)
{
	return x != 3;
}

static inline unsigned int wait_for_stdupd_idle(unsigned int x)
{
	return GET_STAUPD_FSM(x) != 0x0;
}

/**************used at _pwrap_wacs2_nochk*************************************/
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
static inline unsigned int wait_for_state_ready_init(loop_condition_fp fp,
	unsigned int timeout_us, void *wacs_register, unsigned int *read_reg)
#else
static inline unsigned int wait_for_state_ready_init(loop_condition_fp fp,
	unsigned int timeout_us, unsigned int *wacs_register,
	unsigned int *read_reg)
#endif
{
	unsigned int start_time_ns = 0, timeout_ns = 0;
	unsigned int reg_rdata = 0x0;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);

	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("ready_init timeout\n");
			pwrap_dump_ap_register();
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
		reg_rdata = WRAP_RD32(wacs_register);
	} while (fp(reg_rdata));

	if (read_reg)
		*read_reg = reg_rdata;

	return 0;
}

#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
static inline unsigned int wait_for_state_idle(loop_condition_fp fp,
	unsigned int timeout_us, void *wacs_register,
	void *wacs_vldclr_register, unsigned int *read_reg, unsigned int pmifid)
#else
static inline unsigned int wait_for_state_idle(loop_condition_fp fp,
	unsigned int timeout_us, unsigned int *wacs_register,
	unsigned int *wacs_vldclr_register, unsigned int *read_reg,
	unsigned int pmifid)
#endif
{
	unsigned int start_time_ns = 0, timeout_ns = 0;
	unsigned int reg_rdata;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);

	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("state_idle timeout\n");
			pwrap_dump_ap_register();
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
		reg_rdata = WRAP_RD32(wacs_register);
		if (GET_SWINF_2_INIT_DONE(reg_rdata) != WACS_INIT_DONE) {
			PWRAPERR("init isn't finished\n");
			return E_PWR_NOT_INIT_DONE;
		}
		switch (GET_SWINF_2_FSM(reg_rdata)) {
			case WACS_FSM_WFVLDCLR:
				WRAP_WR32(wacs_vldclr_register, 1);
				PWRAPERR("WACS_FSM = VLDCLR\n");
				break;
			case WACS_FSM_WFDLE:
				PWRAPERR("WACS_FSM = WFDLE\n");
				break;
			case WACS_FSM_REQ:
				PWRAPERR("WACS_FSM = REQ\n");
				break;
			default:
				break;
		}
	} while (fp(reg_rdata));
	if (read_reg)
		*read_reg = reg_rdata;

	return 0;
}

/**************used at pwrap_wacs2********************************************/
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
static inline unsigned int wait_for_state_ready(loop_condition_fp fp,
	unsigned int timeout_us, void *wacs_register, unsigned int *read_reg,
	unsigned int pmifid)
#else
static inline unsigned int wait_for_state_ready(loop_condition_fp fp,
	unsigned int timeout_us, unsigned int *wacs_register,
	unsigned int *read_reg, unsigned int pmifid)
#endif
{
	unsigned int start_time_ns = 0, timeout_ns = 0;
	unsigned int reg_rdata;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);

	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("state_ready timeout\n");
			pwrap_dump_ap_register();
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
		reg_rdata = WRAP_RD32(wacs_register);
		if (GET_SWINF_2_INIT_DONE(reg_rdata) != WACS_INIT_DONE) {
			PWRAPERR("init isn't finished\n");
			return E_PWR_NOT_INIT_DONE;
		}
	} while (fp(reg_rdata));
	if (read_reg)
		*read_reg = reg_rdata;

	return 0;
}

/*********************internal API for pwrap_init***************************/

signed int pwrap_read_nochk(unsigned int adr, unsigned int *rdata)
{
	return _pwrap_swinf_acc_nochk(PMIF_SPI_AP_SWINF_NO, DEFAULT_CMD, 0, PMIF_SPI_PMIFID,
			DEFAULT_SLVID, adr, DEFAULT_BYTECNT, 0x0, rdata);
}

signed int pwrap_write_nochk(unsigned int adr, unsigned int wdata)
{
	return _pwrap_swinf_acc_nochk(PMIF_SPI_AP_SWINF_NO, DEFAULT_CMD, 1, PMIF_SPI_PMIFID,
			DEFAULT_SLVID, adr, DEFAULT_BYTECNT, wdata, 0x0);
}

signed int _pwrap_swinf_acc(unsigned int swinf_no, unsigned int cmd,
	unsigned int write, unsigned int pmifid, unsigned int slvid, unsigned int addr, unsigned int bytecnt,
	unsigned int wdata, unsigned int *rdata)
{
	unsigned int reg_rdata = 0x0;
	unsigned int wacs_write = 0x0;
	unsigned int wacs_adr = 0x0;
	unsigned int wacs_cmd = 0x0;
	unsigned int return_value = 0x0;

	/* Check argument validation */
	if ((swinf_no & ~(0x3)) != 0)
		return E_PWR_INVALID_SWINF;
	if ((cmd & ~(0x3)) != 0)
		return E_PWR_INVALID_CMD;
	if ((write & ~(0x1)) != 0)
		return E_PWR_INVALID_RW;
	if ((pmifid & ~(0x3)) != 0)
		return E_PWR_INVALID_PMIFID;
	if ((slvid & ~(0xf)) != 0)
		return E_PWR_INVALID_SLVID;
	if ((addr & ~(0xffff)) != 0)
		return E_PWR_INVALID_ADDR;
	if ((bytecnt & ~(0x1)) != 0)
		return E_PWR_INVALID_BYTECNT;
	if ((wdata & ~(0xffff)) != 0)
		return E_PWR_INVALID_WDAT;

	/* Check whether INIT_DONE is set */
	if (pmifid == 0)
		reg_rdata = WRAP_RD32(PMIF_SPI_PMIF_SWINF_0_STA + 0x40 * swinf_no);

	if (GET_SWINF_2_INIT_DONE(reg_rdata) != 0x1)
		return E_PWR_NOT_INIT_DONE;

	/* Wait for Software Interface FSM state to be IDLE */
	return_value = wait_for_state_idle(wait_for_fsm_idle, TIMEOUT_WAIT_IDLE,
			(unsigned int *)(PMIF_SPI_PMIF_SWINF_0_STA + 0x40 * swinf_no),
			(unsigned int *)(PMIF_SPI_PMIF_SWINF_0_VLD_CLR + 0x40 * swinf_no), 0, pmifid);
	if (return_value != 0) {
		PWRAPERR("fsm_idle fail, ret=%d\n", return_value);
		goto FAIL;
	}

	/* Set the write data */
	if (write == 1) {
		if(pmifid == 0)
			WRAP_WR32(PMIF_SPI_PMIF_SWINF_0_WDATA_31_0 + 0x40 * swinf_no, wdata);
	}

	/* Send the command */
	if (pmifid == 0)
		WRAP_WR32(PMIF_SPI_PMIF_SWINF_0_ACC + 0x40 * swinf_no,
			  (cmd << 30) | (write << 29) | (slvid << 24) | (bytecnt << 16) | addr);

	if (write == 0) {
		/* Wait for Software Interface FSM to be WFVLDCLR, read the data and clear the valid flag */
		return_value =
			wait_for_state_ready(wait_for_fsm_vldclr, TIMEOUT_READ,
					(unsigned int *)(PMIF_SPI_PMIF_SWINF_0_STA + 0x40 * swinf_no), &reg_rdata, pmifid);
		if (return_value != 0) {
			PWRAPERR("fsm_vldclr fail, ret=%d\n", return_value);
			return_value += 1;
			goto FAIL;
		}

		if (pmifid == 0)
			*rdata = WRAP_RD32(PMIF_SPI_PMIF_SWINF_0_RDATA_31_0 + 0x40 * swinf_no);

		if (pmifid == 0)
			WRAP_WR32(PMIF_SPI_PMIF_SWINF_0_VLD_CLR + 0x40 * swinf_no, 0x1);
	}

FAIL:
	if (return_value != 0) {
		PWRAPCRI("_pwrap_swinf_acc fail, ret=%d\n", return_value);
	}

	return 0;
}

static signed int _pwrap_swinf_acc_nochk(unsigned int swinf_no, unsigned int cmd,
	unsigned int write, unsigned int pmifid, unsigned int slvid, unsigned int addr, unsigned int bytecnt,
	unsigned int wdata, unsigned int *rdata)
{
	unsigned int reg_rdata = 0x0;
	unsigned int wacs_write = 0x0;
	unsigned int wacs_adr = 0x0;
	unsigned int wacs_cmd = 0x0;
	unsigned int return_value = 0x0;

	/* Check argument validation */
	if ((swinf_no & ~(0x3)) != 0)
		return E_PWR_INVALID_SWINF;
	if ((cmd & ~(0x3)) != 0)
		return E_PWR_INVALID_CMD;
	if ((write & ~(0x1)) != 0)
		return E_PWR_INVALID_RW;
	if ((pmifid & ~(0x3)) != 0)
		return E_PWR_INVALID_PMIFID;
	if ((slvid & ~(0xf)) != 0)
		return E_PWR_INVALID_SLVID;
	if ((addr & ~(0xffff)) != 0)
		return E_PWR_INVALID_ADDR;
	if ((bytecnt & ~(0x1)) != 0)
		return E_PWR_INVALID_BYTECNT;
	if ((wdata & ~(0xffff)) != 0)
		return E_PWR_INVALID_WDAT;

	/* Wait for Software Interface FSM state to be IDLE */
	do {
		if (pmifid == 0)
			reg_rdata = WRAP_RD32(PMIF_SPI_PMIF_SWINF_0_STA + 0x40 * swinf_no);
	} while (GET_SWINF_2_FSM(reg_rdata) != 0x0);

	/* Set the write data */
	if (write == 1) {
		if(pmifid == 0)
			WRAP_WR32(PMIF_SPI_PMIF_SWINF_0_WDATA_31_0 + 0x40 * swinf_no, wdata);
	}

	/* Send the command */
	if (pmifid == 0)
		WRAP_WR32(PMIF_SPI_PMIF_SWINF_0_ACC + 0x40 * swinf_no,
			  (cmd << 30) | (write << 29) | (slvid << 24) | (bytecnt << 16) | addr);

	if (write == 0) {
		/* Wait for Software Interface FSM to be WFVLDCLR, read the data and clear the valid flag */
		do {
			if (pmifid == 0)
				reg_rdata = WRAP_RD32(PMIF_SPI_PMIF_SWINF_0_STA + 0x40 * swinf_no);
		} while (GET_SWINF_2_FSM(reg_rdata) != 0x6);

		if (pmifid == 0)
			*rdata = WRAP_RD32(PMIF_SPI_PMIF_SWINF_0_RDATA_31_0 + 0x40 * swinf_no);

		if (pmifid == 0)
			WRAP_WR32(PMIF_SPI_PMIF_SWINF_0_VLD_CLR + 0x40 * swinf_no, 0x1);
	}

	return 0;
}

/***************************************************
 * Function : _pwrap_init_cipher()
 * Description :
 * Parameter :
 * Return :
 ****************************************************/
static signed int _pwrap_init_cipher(void)
{
	unsigned int rdata = 0;
	unsigned int return_value = 0;
	unsigned int start_time_ns = 0, timeout_ns = 0;

	/* Reset Cipher */
	WRAP_WR32(PMICSPI_MST_SPICIPHER_CTRL, WRAP_RD32(PMICSPI_MST_SPICIPHER_CTRL) | 0x1);
	WRAP_WR32(PMICSPI_MST_SPICIPHER_CTRL, WRAP_RD32(PMICSPI_MST_SPICIPHER_CTRL) & ~0x1);

	/* Configure Cipher for PMIC 0 in PMIC SPI Master */
	WRAP_WR32(PMICSPI_MST_SPICIPHER_CTRL, (0x2 << 4) | (0x1 << 2) | (0x1 << 1));

	/* Configure Cipher of PMIC 0 */
	pwrap_write_nochk(PMIC_DEW_CIPHER_SWRST_ADDR, 0x1);
	pwrap_write_nochk(PMIC_DEW_CIPHER_SWRST_ADDR, 0x0);
	pwrap_write_nochk(PMIC_DEW_CIPHER_KEY_SEL_ADDR, 0x1);
	pwrap_write_nochk(PMIC_DEW_CIPHER_IV_SEL_ADDR, 0x2);
	pwrap_write_nochk(PMIC_DEW_CIPHER_EN_ADDR, 0x1);

	/* Wait for Cipher for PMIC 0 in PMIC SPI Master to be ready */
	do {
		rdata = WRAP_RD32(PMICSPI_MST_SPICIPHER_STA);
	} while (GET_PMIC_0_SPICIPHER_RDY(rdata) != 0x1);

	/* Wait for Cipher of PMIC 0 to be ready */
	do {
		pwrap_read_nochk(PMIC_DEW_CIPHER_RDY_ADDR, &rdata);
	} while (rdata != 0x1);

	/* Enable Cipher of PMIC 0 */
	pwrap_write_nochk(PMIC_DEW_CIPHER_MODE_ADDR, 0x1);

	/* Wait for completion of sending the commands */
	do {
		rdata = WRAP_RD32(PMIF_SPI_PMIF_INF_BUSY_STA);
	} while ((rdata & (0x1 << PMIF_SPI_AP_SWINF_CHAN_NO)) != 0x0);

	do {
		rdata = WRAP_RD32(PMIF_SPI_PMIF_OTHER_BUSY_STA_0);
	} while (GET_CMDISSUE_BUSY(rdata) != 0x0);

	do {
		rdata = WRAP_RD32(PMICSPI_MST_OTHER_BUSY_STA_0);
	} while (GET_PMICSPI_BUSY(rdata) != 0x0);

	/* Enalbe Cipher for PMIC 0 in PMIC SPI Master */
	WRAP_WR32(PMICSPI_MST_SPICIPHER_CTRL, WRAP_RD32(PMICSPI_MST_SPICIPHER_CTRL) | 0x1 << 6);

	/* Read Test */
	pwrap_read_nochk(PMIC_DEW_READ_TEST_ADDR, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		PWRAPERR("cipher read test fail, err=%x, rdata=%x\n", 1, rdata);
		return E_PWR_READ_TEST_FAIL;
	}

	return 0;
}

/************************************************
 * Function : _pwrap_lock_SPISPVReg()
 * Description : protect spi slv register to be write
 * Parameter :
 * Return :
 ************************************************/
static int _pwrap_lock_SPISLVReg(void)
{
	pwrap_write_nochk(PMIC_SPISLV_KEY_ADDR, 0x0);
	return 0;
}

static int _pwrap_unlock_SPISLVReg(void)
{
	pwrap_write_nochk(PMIC_SPISLV_KEY_ADDR, 0xbade);
	return 0;
}

static int _pwrap_InitStaUpd(void)
{
	unsigned int rdata = 0x0;
	unsigned int sub_return = 0;

#ifdef PMIC_WRAP_CRC_SUPPORT
	/* CRC mode */
	/* Unlock SPI Slave registers */
	sub_return = _pwrap_unlock_SPISLVReg();
	if (sub_return != 0) {
		PWRAPERR("Unlock SPI SLV Reg fail, ret=%x\n", sub_return);
		return -1;
	}

	/* Enable CRC of PMIC 0 */
	pwrap_write_nochk(PMIC_DEW_CRC_EN_ADDR, 0x1);

	/* Wait for completion of sending the commands */
	do {
		rdata = WRAP_RD32(PMIF_SPI_PMIF_INF_BUSY_STA);
	} while ((rdata & (0x1 << PMIF_SPI_AP_SWINF_CHAN_NO)) != 0x0);

	do {
		rdata = WRAP_RD32(PMIF_SPI_PMIF_OTHER_BUSY_STA_0);
	} while (GET_CMDISSUE_BUSY(rdata) != 0x0);

	do {
		rdata = WRAP_RD32(PMICSPI_MST_OTHER_BUSY_STA_0);
	} while (GET_PMICSPI_BUSY(rdata) != 0x0);

	/* Configure CRC of PMIC Interface */
	WRAP_WR32(PMIF_SPI_PMIF_CRC_CTRL, 0x1);
	WRAP_WR32(PMIF_SPI_PMIF_SIG_MODE, 0x0);

	/* Lock SPI Slave registers */
	sub_return = _pwrap_lock_SPISLVReg();
	if (sub_return != 0) {
		PWRAPERR("Lock SPI SLV Reg fail, ret=%x\n", sub_return);
		return -1;
	}
	PWRAPLOG("Enable CRC ok\n");
#else
	/* Signature mode */
	WRAP_WR32(PMIF_SPI_PMIF_SIG_MODE, 0x1);
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_SIG_VAL, 0x83);
#endif
	/* Setup PMIC Siganature */
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_SIG_ADDR, PMIC_DEW_CRC_VAL_ADDR);

	/* Setup PMIC EINT */
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_EINT_STA_ADDR, PMIC_CPU_INT_STA_ADDR);

#ifndef TIA_SUPPORT
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_LATEST_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_WP_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_0_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_1_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_2_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_3_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_4_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_5_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_6_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_7_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_8_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_9_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_10_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_11_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_12_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_13_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_14_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_15_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_16_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_17_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_18_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_19_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_20_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_21_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_22_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_23_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_24_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_25_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_26_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_27_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_28_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_29_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_30_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_MD_AUXADC_RDATA_31_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
#endif

#ifdef TIA_GPS_AUXADC_WORKAROUND
	/* Internal GPS AUXADC Interface */
	WRAP_WR32(PMIF_SPI_PMIF_INT_GPS_AUXADC_CMD_ADDR, (PMIC_AUXADC_RQST_CH10_ADDR << 16) + PMIC_AUXADC_RQST_CH7_ADDR);
	WRAP_WR32(PMIF_SPI_PMIF_INT_GPS_AUXADC_CMD, (0x400 << 16) + 0x80);
	WRAP_WR32(PMIF_SPI_PMIF_INT_GPS_AUXADC_RDATA_ADDR, (PMIC_AUXADC_ADC_OUT_CH10_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_ADDR);
#endif

	/* Enable Status update for PMIC and Setup period to 100us */
	WRAP_WR32(PMIF_SPI_PMIF_STAUPD_CTRL, (0x5  << 4) | 0x5);

#ifdef TIA_SUPPORT
#ifdef TIA_GPS_AUXADC_WORKAROUND
	/* Enable TSX/DCXO for GPS AUXADC */
	WRAP_WR32(PMIF_SPI_PMIF_STAUPD_CTRL, WRAP_RD32(PMIF_SPI_PMIF_STAUPD_CTRL) | (0xc << 8));
#endif
#else
	/* Enable TSX/DCXO for GPS AUXADC and TSX for MD AUXADC */
	WRAP_WR32(PMIF_SPI_PMIF_STAUPD_CTRL, WRAP_RD32(PMIF_SPI_PMIF_STAUPD_CTRL) | (0xd << 8));
#endif
	return 0;
}

/************************************************
 * Function : _pwrap_init_sistrobe()
 * scription : Initialize SI_CK_CON and SIDLY
 * Parameter :
 * Return :
 ************************************************/
static signed int _pwrap_init_sistrobe(int dual_si_sample_settings)
{
	unsigned int rdata = 0;
	int si_sample_ck_dly, si_sample_ck_pol, si_dly, si_sample_ctrl, clk_edge_no;
	int found, i;
	unsigned int test_data[30] = {0x6996, 0x9669, 0x6996, 0x9669, 0x6996,
					0x9669, 0x6996, 0x9669, 0x6996, 0x9669,
					0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x5AA5,
					0xA55A, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A,
					0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27,
					0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27
	};

	/* Calibrate SPI read for 26M clock */
	found = 0;
	for (si_sample_ck_dly = 0; si_sample_ck_dly < 8; si_sample_ck_dly++) {
		for (si_sample_ck_pol = 0; si_sample_ck_pol < 2; si_sample_ck_pol++) {
			si_sample_ctrl = (si_sample_ck_dly << 6) | (si_sample_ck_pol << 5);
			WRAP_WR32(PMICSPI_MST_SI_SAMPLING_CTRL, si_sample_ctrl);

			pwrap_read_nochk(PMIC_DEW_READ_TEST_ADDR, &rdata);
			if (rdata == DEFAULT_VALUE_READ_TEST) {
				PWRAPCRI("First Valid Sampling Clock Found!!!\n");
				PWRAPCRI("si_sampling_ck_dly = %x, si_sampling_ck_pol = %x\n",
					si_sample_ck_dly, si_sample_ck_pol);
				PWRAPCRI("si_sample_ctrl = %x, rdata = %x\n",
					si_sample_ctrl, rdata);
				found = 1;
				break;
			}
			PWRAPLOG("si_sampling_ck_dly = %x, si_sampling_ck_pol = %x\n",
				si_sample_ck_dly, si_sample_ck_pol);
			PWRAPLOG("si_sample_ctrl = %x, rdata = %x\n",
				si_sample_ctrl, rdata);
		}
		if (found == 1)
			break;
	}
	if (found == 0) {
		PWRAPCRI("_pwrap_init_sistrobe (not found)\n");
		return E_PWR_INIT_SIDLY_FAIL;
	}
	if((si_sample_ck_dly == 7) && (si_sample_ck_pol == 1)) {
		PWRAPCRI("_pwrap_init_sistrobe (last setting)\n");
		return E_PWR_INIT_SIDLY_FAIL;
	}

	for (si_dly = 0; si_dly < 10; si_dly++) {
		pwrap_write_nochk(PMIC_RG_SPI_DLY_SEL_ADDR, si_dly);

		found = 0;
#ifndef SPEED_UP_PWRAP_INIT
		for (i = 0; i < 30; i++)
#else
		for (i = 0; i < 1; i++)
#endif
		{
			pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, test_data[i]);
			pwrap_read_nochk(PMIC_DEW_WRITE_TEST_ADDR, &rdata);
			if ((rdata & 0x7fff) != (test_data[i] & 0x7fff)) {
				PWRAPCRI("Data Boundary is Found !!!\n");
				PWRAPCRI("si_dly = %x, rdata = %x\n", si_dly, rdata);
				found = 1;
				break;
			}
		}
		if (found == 1)
			break;
		PWRAPLOG("si_dly = %x, *RG_SPI_CON2 = %x, rdata = %x\n",
		         si_dly, si_dly, rdata);
	}

	PWRAPCRI("si_sample_ctrl=0x%x(before)\n", si_sample_ctrl);
#if (CFG_FPGA_PLATFORM)
	si_sample_ctrl = si_sample_ctrl + 0x0;
	PWRAPCRI("si_sample_ctrl=0x%x(after)(FPGA)\n", si_sample_ctrl);
#else
	si_sample_ctrl = si_sample_ctrl + 0x20;
	PWRAPCRI("si_sample_ctrl=0x%x(after)(non-FPGA)\n", si_sample_ctrl);
#endif

	WRAP_WR32(PMICSPI_MST_SI_SAMPLING_CTRL, si_sample_ctrl);
	if (si_dly == 10) {
		PWRAPLOG("SI Strobe Calibration For PMIC 0 Done\n");
		PWRAPLOG("si_sample_ctrl = %x, si_dly = %x\n",
			  si_sample_ctrl, si_dly);
		si_dly--;
	}
	PWRAPCRI("SI Strobe Calibration For PMIC 0 Done\n");
	PWRAPCRI("si_sample_ctrl = %x, si_dly = %x\n",
		  si_sample_ctrl, si_dly);

#ifdef PMIC_WRAP_ULPOSC_CAL
	/* TINFO = "SI Strobe Calibration For ULPOSC Clock For PMIC 0 Start" */
	si_sample_ck_dly = (WRAP_RD32(PMICSPI_MST_SI_SAMPLING_CTRL) << 23) >> 29;
	si_sample_ck_pol = (WRAP_RD32(PMICSPI_MST_SI_SAMPLING_CTRL) << 26) >> 31;
	clk_edge_no = (((si_sample_ck_dly * 2 + si_sample_ck_pol) * 100) *
			CLK_26M_PRD / CLK_ULPOSC_PRD + 50)/100;
	/* TINFO = "ULPOSC Clock For PMIC 0 Is Chosen As Clock Edge" */
	si_sample_ck_dly = clk_edge_no / 2;
	si_sample_ck_pol = clk_edge_no % 2;
	si_sample_ctrl = (1 << 18) | (si_sample_ck_dly << 6) | (si_sample_ck_pol << 5);
	WRAP_WR32(PMICSPI_MST_SI_SAMPLING_CTRL_ULPOSC, si_sample_ctrl);
	/* TINFO = "SI Strobe Calibration For ULPOSC Clock For PMIC 0 Done" */
#endif

	/* Read Test */
	pwrap_read_nochk(PMIC_DEW_READ_TEST_ADDR, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		PWRAPCRI("_pwrap_init_sistrobe Read Test Failed\n");
		PWRAPCRI("Failed, rdata = %x, exp = 0x5aa5\n", rdata);
		return E_PWR_READ_TEST_FAIL;
	}
	PWRAPLOG("_pwrap_init_sistrobe Read Test ok\n");

	return 0;
}

static int __pwrap_InitSPISLV(void)
{
	unsigned int rdata;

	/* Turn on SPI IO filter function */
	pwrap_write_nochk(PMIC_RG_SRCLKEN_IN0_FILTER_EN_ADDR, 0xf0);

	/* Turn on SPI IO SMT function to improve noise immunity */
	pwrap_write_nochk(PMIC_RG_SMT_SPI_CLK_ADDR, 0xf);

	/* Turn off SPI IO pull function for power saving */
	pwrap_write_nochk(PMIC_GPIO_PULLEN0_CLR_ADDR, 0xf0);

	/* Enable SPI access in SODI-3.0 and Suspend modes */
	pwrap_write_nochk(PMIC_RG_SPI_MISO_MODE_SEL_ADDR, 0x2);

	/* Set SPI IO driving strength to 4 mA */
	pwrap_write_nochk(PMIC_RG_OCTL_SPI_CLK_ADDR, 0x8888);

	return 0;
}

/******************************************************
 * Function : _pwrap_reset_spislv()
 * Description :
 * Parameter :
 * Return :
 ******************************************************/
static signed int _pwrap_reset_spislv(void)
{
	unsigned int ret = 0;
	unsigned int rdata = 0;
	unsigned int return_value = 0;
	unsigned int pmicspi_mst_dio_en_backup = 0;

	WRAP_WR32(PMICSPI_MST_SPIWRAP_EN, 0x0);
	WRAP_WR32(PMICSPI_MST_SPIMUX_SEL, 0x1);
	WRAP_WR32(PMICSPI_MST_SPIMAN_EN, 0x1);
	pmicspi_mst_dio_en_backup = WRAP_RD32(PMICSPI_MST_DIO_EN);
	WRAP_WR32(PMICSPI_MST_DIO_EN, 0x0);

	WRAP_WR32(PMICSPI_MST_SPIMAN_ACC, (0x1 << 13) | (OP_CSL  << 8));
	WRAP_WR32(PMICSPI_MST_SPIMAN_ACC, (0x1 << 13) | (OP_OUTS << 8)); //Reset the counter.
	WRAP_WR32(PMICSPI_MST_SPIMAN_ACC, (0x1 << 13) | (OP_CSH  << 8));
	WRAP_WR32(PMICSPI_MST_SPIMAN_ACC, (0x1 << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMICSPI_MST_SPIMAN_ACC, (0x1 << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMICSPI_MST_SPIMAN_ACC, (0x1 << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMICSPI_MST_SPIMAN_ACC, (0x1 << 13) | (OP_OUTS << 8));

	/* Wait for PMIC SPI Master to be idle */
	do {
		rdata = WRAP_RD32(PMICSPI_MST_OTHER_BUSY_STA_0);
	} while (GET_PMICSPI_BUSY(rdata) != 0x0);

	WRAP_WR32(PMICSPI_MST_SPIMAN_EN, 0x0);
	WRAP_WR32(PMICSPI_MST_SPIMUX_SEL, 0x0);
	WRAP_WR32(PMICSPI_MST_SPIWRAP_EN, 0x1);
	WRAP_WR32(PMICSPI_MST_DIO_EN, pmicspi_mst_dio_en_backup);

	return 0;
}

static signed int _pwrap_init_reg_clock(unsigned int regck_sel)
{
	unsigned int rdata;

	//Set SoC SPI IO driving strength to 4 mA.
	WRAP_WR32(IOCFG_LT_DRV_CFG2_CLR, 0x7 << 9);
	WRAP_WR32(IOCFG_LT_DRV_CFG2_SET, 0x2 << 9);

	/* Configure SPI protocol */
	WRAP_WR32(PMICSPI_MST_EXT_CK_WRITE, 0x1);
	WRAP_WR32(PMICSPI_MST_EXT_CK_READ, 0x0);
	WRAP_WR32(PMICSPI_MST_CSHEXT_WRITE, 0x0);
	WRAP_WR32(PMICSPI_MST_CSHEXT_READ, 0x0);
	WRAP_WR32(PMICSPI_MST_CSLEXT_WRITE, 0x0);
	WRAP_WR32(PMICSPI_MST_CSLEXT_READ, 0x0100);

	/* Set Read Dummy Cycle Number (Slave Clock is 18MHz) */
	pwrap_write_nochk(PMIC_DEW_RDDMY_NO_ADDR, 0x8);
	WRAP_WR32(PMICSPI_MST_RDDMY, 0x8);
	PWRAPLOG("Set Read Dummy Cycle ok\n");

	/* Enable DIO mode */
	pwrap_write_nochk(PMIC_DEW_DIO_EN_ADDR, 0x1);

	/* Wait for completion of sending the commands */
	do {
		rdata = WRAP_RD32(PMIF_SPI_PMIF_INF_BUSY_STA);
	} while ((rdata & (0x1 << PMIF_SPI_AP_SWINF_CHAN_NO)) != 0x0);

	do {
		rdata = WRAP_RD32(PMIF_SPI_PMIF_OTHER_BUSY_STA_0);
	} while (GET_CMDISSUE_BUSY(rdata) != 0x0);

	do {
		rdata = WRAP_RD32(PMICSPI_MST_OTHER_BUSY_STA_0);
	} while (GET_PMICSPI_BUSY(rdata) != 0x0);

	WRAP_WR32(PMICSPI_MST_DIO_EN, 0x1);
	PWRAPLOG("_pwrap_init_dio ok\n");

	return 0;
}

static void _pwrap_config_mpu(void)
{
	/*
	 * config pmic wrap mpu
	 * D0(AP) D1(MD) D2(CONN2AP) D3(SCP) D7(MD_HW) D8(SSPM)
	 * D9(SPM) D10 (ADSP) D11(GZ) D14(MCUPM)
	 */
	WRAP_WR32(PMIF_SPI_PMIF_MPU_CTRL, 0x34); // pdomain_scp[5:2] = 0xd for scp
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_0_START, 0x600);  //Start address of Region 0
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_0_END, 0x67E);  //End address of Region 0
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_0_PER, 0xff3ffffb);  //Permission of Region 0 for Domain 15~0

	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_1_START, 0x800);  //Start address of Region 1
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_1_END, 0x13fe);  //End address of Region 1
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_1_PER, 0xfffcfff8);  //Permission of Region 1 for Domain 15~0

	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_2_START, 0x1400);  //Start address of Region 2
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_2_END, 0x207e);  //End address of Region 2
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_2_PER, 0xfffcfff0);  //Permission of Region 2 for Domain 15~0

	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_3_START, 0x2080);  //Start address of Region 3
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_3_END, 0x26fe);  //End address of Region 3
	WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_3_PER, 0xfffcff38);  //Permission of Region 3 for Domain 15~0

	PWRAPLOG("PMIF_SPI_PMIF_MPU_CTRL = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MPU_CTRL));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_0_START = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_0_START));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_0_END = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_0_END));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_0_PER = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_0_PER));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_1_START = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_1_START));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_1_END = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_1_END));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_1_PER = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_1_PER));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_2_START = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_2_START));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_2_END = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_2_END));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_2_PER = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_2_PER));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_3_START = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_3_START));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_3_END = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_3_END));
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_3_PER = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_3_PER));
}

signed int pwrap_enable_mpu(bool en)
{
	if (en == TRUE)
		WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_EN, 0x0000000f);  //Enable Protect Region 0~3
	else
		WRAP_WR32(PMIF_SPI_PMIF_PMIC_RGN_EN, 0x00000000);  //Disable Protect Region 0~3

	WRAP_WR32(PMIF_SPI_PMIF_MPU_CTRL, 0x1);  //Enable Security write control
	PWRAPLOG("PMIF_SPI_PMIF_PMIC_RGN_EN = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_PMIC_RGN_EN));
	PWRAPLOG("PMIF_SPI_PMIF_MPU_CTRL = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MPU_CTRL));

	return 0;
}

static void _pwrap_config_hw_monitor(void)
{
	/* disable monitor STAUPD HW channel (CH14) */
	unsigned int monitor_channel = 0x1fbfff;

	/* Config HW Monitor for TARGET_0 to TARGE_7 */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_0, monitor_channel);
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_0, 0xffff0D0E); /* mask + addr */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_0, 0x00010001); /* mask + wdata */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE, 0x0003); /* only write command for TARGET_0 */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_CTRL, 0x800); /* clear log */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_CTRL, 0x405); /* Match and Stop record, Enable TARGET_0 */
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_0 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_0));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_0 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_0));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_0 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_0));

#if PMIC_WRAP_HW_MONITOR_SET > 1
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_1, monitor_channel);
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_1, 0xffff0D10); /* 0x0D10 = monitor addr */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_1, 0x0); /* 0x0 = monitor all wdata */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE) | 0x3 << 2); /* only write command for TARGET_1 */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_CTRL, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_CTRL) | 0x1 << 3); /* Enable TARGET_1 */
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_1 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_1));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_1 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_1));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_1 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_1));
#endif

#if PMIC_WRAP_HW_MONITOR_SET > 2
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_2, monitor_channel);
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_2, 0xffff0D46); /* 0x0D46 = monitor addr */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_2, 0x0); /* 0x0 = monitor all wdata */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE) | 0x3 << 4); /* only write command for TARGET_2 */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_CTRL, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_CTRL) | 0x1 << 4); /* Enable TARGET_2 */
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_2 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_2));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_2 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_2));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_2 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_2));
#endif

#if PMIC_WRAP_HW_MONITOR_SET > 3
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_3, monitor_channel);
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_3, 0xffff0D48); /* 0x0D48 = monitor addr */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_3, 0x0); /* 0x0 = monitor all wdata */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE) | 0x3 << 6); /* only write command for TARGET_3 */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_CTRL, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_CTRL) | 0x1 << 5); /* Enable TARGET_3 */
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_3 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_3));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_3 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_3));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_3 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_3));
#endif

#if PMIC_WRAP_HW_MONITOR_SET > 4
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_4, monitor_channel);
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_4, 0xffff0D4A); /* 0x0D4A = monitor addr */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_4, 0x0); /* 0x0 = monitor all wdata */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE) | 0x3 << 8); /* only write command for TARGET_4 */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_CTRL, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_CTRL) | 0x1 << 6); /* Enable TARGET_4 */
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_4 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_4));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_4 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_4));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_4 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_4));
#endif

#if PMIC_WRAP_HW_MONITOR_SET > 5
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_5, monitor_channel);
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_5, 0xffff0D4C); /* 0x0D4C = monitor addr */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_5, 0x0); /* 0x0 = monitor all wdata */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE) | 0x3 << 10); /* only write command for TARGET_5 */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_CTRL, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_CTRL) | 0x1 << 7); /* Enable TARGET_5 */
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_5 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_5));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_5 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_5));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_5 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_5));
#endif

#if PMIC_WRAP_HW_MONITOR_SET > 6
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_6, monitor_channel);
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_6, 0xffff0D50); /* 0x0D50 = monitor addr */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_6, 0x0); /* 0x0 = monitor all wdata */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE) | 0x3 << 12); /* only write command for TARGET_6 */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_CTRL, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_CTRL) | 0x1 << 8); /* Enable TARGET_6 */
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_6 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_6));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_6 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_6));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_6 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_6));
#endif

#if PMIC_WRAP_HW_MONITOR_SET > 7
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_7, monitor_channel);
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_7, 0xffff0D98); /* 0x0D98 = monitor addr */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_7, 0x0); /* 0x0 = monitor all wdata */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE) | 0x3 << 14); /* only write command for TARGET_7 */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_CTRL, WRAP_RD32(PMIF_SPI_PMIF_MONITOR_CTRL) | 0x1 << 9); /* Enable TARGET_7 */
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_7 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_7));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_7 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_7));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_7 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_7));
#endif
}

static int _pwrap_wacs2_write_test(int pmic_no)
{
	unsigned int rdata = 0;

	if (pmic_no == 0) {
		pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, 0xa55a);
		pwrap_read_nochk(PMIC_DEW_WRITE_TEST_ADDR, &rdata);
		if (rdata != 0xa55a) {
			PWRAPERR("Error: rdata = 0x%x, exp = 0xa55a\n", rdata);
			return E_PWR_WRITE_TEST_FAIL;
		}
	}

	return 0;
}

static unsigned int pwrap_read_test(void)
{
	unsigned int rdata = 0;
	unsigned int return_value = 0;

	/* Read Test */
	PWRAPLOG("start pwrap_read_test\n");
	return_value = pwrap_wacs2_read(PMIC_DEW_READ_TEST_ADDR, &rdata);
	PWRAPLOG("rdata=0x%x\n", rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		PWRAPERR("Error: r_rdata = 0x%x, exp = 0x5aa5\n", rdata);
		PWRAPERR("Error: return_value = 0x%x\n", return_value);
		return E_PWR_READ_TEST_FAIL;
	}
	else
		PWRAPLOG("Read Test pass, return_value = 0x%d\n", return_value);

	return 0;
}
static unsigned int pwrap_write_test(void)
{
	unsigned int rdata = 0;
	unsigned int sub_return = 0;
	unsigned int sub_return1 = 0;

	/* Write test using WACS2 */
	PWRAPLOG("start pwrap_write_test\n");
	sub_return = pwrap_wacs2_write(PMIC_DEW_WRITE_TEST_ADDR, DEFAULT_VALUE_WRITE_TEST);
	PWRAPLOG("after pwrap_write_test\n");
	sub_return1 = pwrap_wacs2_read(PMIC_DEW_WRITE_TEST_ADDR, &rdata);
	PWRAPLOG("rdata=0x%x (read back)\n", rdata);
	if ((rdata != DEFAULT_VALUE_WRITE_TEST) ||
	    (sub_return != 0) || (sub_return1 != 0)) {
		PWRAPERR("Error: w_rdata = 0x%x, exp = 0xa55a\n", rdata);
		PWRAPERR("Error: sub_return = 0x%x\n", sub_return);
		PWRAPERR("Error: sub_return1 = 0x%x\n", sub_return1);
		return E_PWR_INIT_WRITE_TEST;
	}
	else
		PWRAPLOG("Write Test pass\n");

	return 0;
}
static void pwrap_ut(unsigned int ut_test)
{
	unsigned int sub_return = 0;
	switch (ut_test) {
		case 1:
			pwrap_write_test();
			break;
		case 2:
			pwrap_read_test();
			break;
		case 3:
#ifdef CONFIG_MTK_TINYSYS_SSPM_SUPPORT
			pwrap_wacs2_ipi(0x10010000 + 0xD8, 0xffffffff, (WRITE_CMD | WRITE_PMIC_WRAP));
			break;
#endif
		case 4:
			sub_return = pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, 0x1234);
			sub_return = pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, 0x4321);
			sub_return = pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, 0xF0F0);
			break;
		default:
			PWRAPLOG("default test\n");
			break;
	}
}

signed int PMICSPIMSTDrv_Init(void)
{
	signed int sub_return = 0;
	unsigned int rdata = 0x0;

	/* Reset SPI Slave */
	sub_return = _pwrap_reset_spislv();
	if (sub_return != 0) {
		PWRAPERR("reset_spislv fail, ret=%x\n", sub_return);
		return E_PWR_INIT_RESET_SPI;
	}
	PWRAPLOG("Reset SPISLV ok\n");

	/* Enable SPI Wrapper */
	WRAP_WR32(PMICSPI_MST_SPIWRAP_EN, 0x1);

	/* SPI Waveform Configuration. 0: Safe Mode, 1: SPISLV Clock is 18MHz */
	sub_return = _pwrap_init_reg_clock(1);
	if (sub_return != 0) {
		PWRAPERR("init_reg_clock fail, ret=%x\n", sub_return);
		return E_PWR_INIT_REG_CLOCK;
	}
	PWRAPLOG("_pwrap_init_reg_clock ok\n");

	/* SPI Slave Configuration */
	sub_return = __pwrap_InitSPISLV();
	if (sub_return != 0) {
		PWRAPERR("InitSPISLV Failed, ret = %x\n", sub_return);
		return -1;
	}
	PWRAPLOG("__pwrap_InitSPISLV ok\n");

	/* Input data calibration flow; */
	sub_return = _pwrap_init_sistrobe(0);
	if (sub_return != 0) {
		PWRAPERR("InitSiStrobe fail, ret=%x\n", sub_return);
		return E_PWR_INIT_SIDLY;
	}
	PWRAPLOG("_pwrap_init_sistrobe ok\n");

#if 0
	/* Enable Encryption */
	sub_return = _pwrap_init_cipher();
	if (sub_return != 0) {
		PWRAPERR("Encryption fail, ret=%x\n", sub_return);
		return E_PWR_INIT_CIPHER;
	}
	PWRAPLOG("_pwrap_init_cipher ok\n");
#endif

	/* Lock SPI Slave Registers */
	sub_return = _pwrap_lock_SPISLVReg();
	if (sub_return != 0) {
		PWRAPERR("Lock SPI SLV Reg fail, ret=%x\n", sub_return);
		return -1;
	}
	PWRAPLOG("_pwrap_lock_SPISLVReg ok\n");

	/* Write Test */
	sub_return = _pwrap_wacs2_write_test(0);
	if (sub_return != 0) {
		PWRAPERR("write test fail\n");
		return E_PWR_INIT_WRITE_TEST;
	}
	PWRAPLOG("_pwrap_wacs2_write_test ok\n");

	return 0;
}

signed int pwrap_init(void)
{
	signed int sub_return = 0;
	unsigned int rdata = 0x0;

	PWRAPLOG("pwrap_init start!!!!!!!!!!!!!\n");

	/* Set srclk_en always valid regardless of ulposc_sel_for_scp */
	WRAP_WR32(PMIF_SPI_PMIF_SPI_MODE_CTRL, WRAP_RD32(PMIF_SPI_PMIF_SPI_MODE_CTRL) & ~(0x1 << 5));

#if (CFG_FPGA_PLATFORM)
	/* Force SPI in normal mode in FPGA */
	WRAP_WR32(PMIF_SPI_PMIF_SPI_MODE_CTRL, WRAP_RD32(PMIF_SPI_PMIF_SPI_MODE_CTRL) | 0x1 << 7);
#else
	/* Set SPI mode controlled by srclk_en instead of pmif_rdy */
	/* MT8188/SCP don't use PMIF in suspend mode, no need set srvol_en */
	WRAP_WR32(PMIF_SPI_PMIF_SPI_MODE_CTRL, WRAP_RD32(PMIF_SPI_PMIF_SPI_MODE_CTRL) |  0x1 << 10);
	WRAP_WR32(PMIF_SPI_PMIF_SPI_MODE_CTRL, WRAP_RD32(PMIF_SPI_PMIF_SPI_MODE_CTRL) & ~(0x1 << 9));

	/* Set spm_sleep_req input from the outside */
	WRAP_WR32(PMIF_SPI_PMIF_SLEEP_PROTECTION_CTRL, WRAP_RD32(PMIF_SPI_PMIF_SLEEP_PROTECTION_CTRL) & ~0x3);

	/* Set scp_sleep_req input from the outside */
	WRAP_WR32(PMIF_SPI_PMIF_SLEEP_PROTECTION_CTRL, WRAP_RD32(PMIF_SPI_PMIF_SLEEP_PROTECTION_CTRL) & ~(0x3 << 9));
#endif

	/* Enable SWINF for AP */
	WRAP_WR32(PMIF_SPI_PMIF_INF_EN, 0x1 << PMIF_SPI_AP_SWINF_CHAN_NO);

	/* Enable arbitration for SWINF for AP */
	WRAP_WR32(PMIF_SPI_PMIF_ARB_EN, 0x1 << PMIF_SPI_AP_SWINF_CHAN_NO);

	/* Enable PMIF_SPI Command Issue */
	WRAP_WR32(PMIF_SPI_PMIF_CMDISSUE_EN, 0x1);

	/* Initialize PMIC SPI Master */
	sub_return = PMICSPIMSTDrv_Init();
	if (sub_return != 0) {
		PWRAPERR("PMICSPIMSTDrv_Init Failed, sub_return = %x, exp = 0\n", sub_return);
		return sub_return;
	}

	/* Status update function initialization
	* 1. Signature Checking using CRC (CRC 0 only)
	* 2. EINT update
	* 3. Read back Auxadc thermal data for GPS
	*/
	sub_return = _pwrap_InitStaUpd();
	if (sub_return != 0) {
		PWRAPERR("_pwrap_InitStaUpd Failed, sub_return = %x, exp = 0\n", sub_return);
		return sub_return;
	}
	PWRAPLOG("_pwrap_InitStaUpd ok\n");

	/* Configure PMIF Timer */
	WRAP_WR32(PMIF_SPI_PMIF_TIMER_CTRL, 0x3);

	/* Enable interfaces and arbitration */
#ifdef TIA_SUPPORT
	WRAP_WR32(PMIF_SPI_PMIF_INF_EN, 0x307f | (0x1 << PMIF_SPI_MD_SWINF_CHAN_NO) |
					(0x1 << PMIF_SPI_AP_SECURE_SWINF_CHAN_NO) |
					(0x1 << PMIF_SPI_AP_SWINF_CHAN_NO));

	WRAP_WR32(PMIF_SPI_PMIF_ARB_EN, 0x707f | (0x1 << PMIF_SPI_MD_SWINF_CHAN_NO) |
					(0x1 << PMIF_SPI_AP_SECURE_SWINF_CHAN_NO) |
					(0x1 << PMIF_SPI_AP_SWINF_CHAN_NO));
#ifdef TIA_GPS_AUXADC_WORKAROUND
	//Enable GPS AUXADC HW 0 and 1
	WRAP_WR32(PMIF_SPI_PMIF_ARB_EN, WRAP_RD32(PMIF_SPI_PMIF_ARB_EN) | (0x3 << 19));
	WRAP_WR32(PMIF_SPI_PMIF_OTHER_INF_EN, WRAP_RD32(PMIF_SPI_PMIF_OTHER_INF_EN) | (0x3 << 4));
#endif

#else
	/* Enable arbitration for TSX for MD AUXADC and TSX/DCXO for GPS AUXADC */
	WRAP_WR32(PMIF_SPI_PMIF_INF_EN, 0x305f | (0x1 << PMIF_SPI_MD_SWINF_CHAN_NO) |
					(0x1 << PMIF_SPI_AP_SECURE_SWINF_CHAN_NO) |
					(0x1 << PMIF_SPI_AP_SWINF_CHAN_NO));

	WRAP_WR32(PMIF_SPI_PMIF_OTHER_INF_EN, 0x34);  //Enable GPS AUXADC HW 0 and 1, MD AUXADC HW 0

	WRAP_WR32(PMIF_SPI_PMIF_ARB_EN, 0x1a705f | (0x1 << PMIF_SPI_MD_SWINF_CHAN_NO) |
					(0x1 << PMIF_SPI_AP_SECURE_SWINF_CHAN_NO) |
					(0x1 << PMIF_SPI_AP_SWINF_CHAN_NO));
#endif

	/* Set INIT_DONE */
	WRAP_WR32(PMIF_SPI_PMIF_INIT_DONE, 0x1);

#if !(CFG_FPGA_PLATFORM)
	/* Configure MD ADC Interface */
	udelay(100);

	PWRAPLOG("write MODEM_TEMP_SHARE_CTRL start\n");
	WRAP_WR32(MODEM_TEMP_SHARE_CTRL, 0xf0);
	PWRAPLOG("write MODEM_TEMP_SHARE_CTRL ok\n");
	PWRAPLOG("MODEM_TEMP_SHARE_CTRL:%x\n", WRAP_RD32(MODEM_TEMP_SHARE_CTRL));
#endif

	/* config mpu setting */
	_pwrap_config_mpu();
	PWRAPLOG("_pwrap_config_mpu ok\n");

#ifdef PMIC_WRAP_DEBUG
	/* WACS2 UT */
	pwrap_ut(2);
	pwrap_ut(1);
#endif
	PWRAPLOG("channel pass\n");

	/* Read Last six command */
	pwrap_read_nochk(PMIC_RECORD_CMD0_ADDR, &rdata);
	PWRAPCRI("REC_CMD0:0x%x (The last one adr)\n", (rdata & 0x3fff) << 1);
	pwrap_read_nochk(PMIC_RECORD_WDATA0_ADDR, &rdata);
	PWRAPCRI("REC_WDATA0:0x%x (The last one wdata)\n", rdata);
	pwrap_read_nochk(PMIC_RECORD_CMD1_ADDR, &rdata);
	PWRAPCRI("REC_CMD1:0x%x (The second-last adr)\n", (rdata & 0x3fff) << 1);
	pwrap_read_nochk(PMIC_RECORD_WDATA1_ADDR, &rdata);
	PWRAPCRI("REC_WDATA1:0x%x (The second-last wdata)\n", rdata);
	pwrap_read_nochk(PMIC_RECORD_CMD2_ADDR, &rdata);
	PWRAPCRI("REC_CMD2:0x%x (The third-last adr)\n", (rdata & 0x3fff) << 1);
	pwrap_read_nochk(PMIC_RECORD_WDATA2_ADDR, &rdata);
	PWRAPCRI("REC_WDATA2:0x%x (The third-last wdata)\n", rdata);

	pwrap_read_nochk(PMIC_RECORD_CMD3_ADDR, &rdata);
	PWRAPCRI("REC_CMD3:0x%x (The fourth-last adr)\n", (rdata & 0x3fff) << 1);
	pwrap_read_nochk(PMIC_RECORD_WDATA3_ADDR, &rdata);
	PWRAPCRI("REC_WDATA3:0x%x (The fourth-last wdata)\n", rdata);
	pwrap_read_nochk(PMIC_RECORD_CMD4_ADDR, &rdata);
	PWRAPCRI("REC_CMD4:0x%x (The fifth-last adr)\n", (rdata & 0x3fff) << 1);
	pwrap_read_nochk(PMIC_RECORD_WDATA4_ADDR, &rdata);
	PWRAPCRI("REC_WDATA4:0x%x (The fifth-last wdata)\n", rdata);
	pwrap_read_nochk(PMIC_RECORD_CMD5_ADDR, &rdata);
	PWRAPCRI("REC_CMD5:0x%x (The sixth-last adr)\n", (rdata & 0x3fff) << 1);
	pwrap_read_nochk(PMIC_RECORD_WDATA5_ADDR, &rdata);
	PWRAPCRI("REC_WDATA5:0x%x (The sixth-last wdata)\n", rdata);

	/* Enable Command Recording */
	sub_return = pwrap_write_nochk(PMIC_RG_EN_RECORD_ADDR, 0x1);
	if (sub_return != 0)
		PWRAPCRI("enable spi debug fail, ret=%x\n", sub_return);
	PWRAPLOG("enable spi debug ok\n");

	/* Clear Last six command */
	sub_return = pwrap_write_nochk(PMIC_RG_SPI_RECORD_CLR_ADDR, 0x1);
	if (sub_return != 0)
		PWRAPCRI("clear record command fail, ret=%x\n", sub_return);
	sub_return = pwrap_write_nochk(PMIC_RG_SPI_RECORD_CLR_ADDR, 0x0);
	if (sub_return != 0)
		PWRAPCRI("clear record command fail, ret=%x\n", sub_return);
	PWRAPLOG("clear record command ok\n\r");

#ifdef PMIC_WRAP_MATCH_SUPPORT
	/* enable matching mode */
	PWRAPCRI("PMIC_WRAP_MONITOR_MODE = Matching Mode\n");

	/* enable Matching interrupt for IRQ3 bit 27 */
	WRAP_WR32(PMIF_SPI_PMIF_IRQ_EVENT_EN_3, WRAP_RD32(PMIF_SPI_PMIF_IRQ_EVENT_EN_3) | 0x8000000);
	PWRAPCRI("PMIF_SPI_PMIF_IRQ_EVENT_EN_3 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_IRQ_EVENT_EN_3));

	/* config 8 hw monitor */
	_pwrap_config_hw_monitor();
	PWRAPCRI("_pwrap_config_hw_monitor ok\n");

	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_CTRL = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_CTRL));
	PWRAPCRI("PMIF_SPI_PMIF_MONITOR_TARGET_WRITE = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE));
#else
	/* enable logging mode */
	PWRAPLOG("PMIC_WRAP_MONITOR_MODE = Logging Mode\n");

	/* disable monitor STAUPD HW channel (CH14) */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_0, 0x1fbfff);
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_0, 0x0); /* record all addr */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_0, 0x0); /* record all wdata */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_TARGET_WRITE, 0x0001); /* record all read and write command */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_CTRL, 0x800); /* clear log */
	WRAP_WR32(PMIF_SPI_PMIF_MONITOR_CTRL, 0x5); /* Match and Continue record */
	PWRAPLOG("PMIF_SPI_PMIF_MONITOR_CTRL = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_CTRL));
	PWRAPLOG("PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_0 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_CHAN_0));
	PWRAPLOG("PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_0 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_ADDR_0));
	PWRAPLOG("PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_0 = 0x%x\n", WRAP_RD32(PMIF_SPI_PMIF_MONITOR_TARGET_WDATA_0));
#endif

	return 0;
}

/*-------------------pwrap debug---------------------*/
static inline void pwrap_dump_ap_register(void)
{
	unsigned int i = 0, offset = 0;
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
	unsigned int *reg_addr;
#else
	unsigned int reg_addr;
#endif
	unsigned int reg_value = 0;

	PWRAPCRI("dump pmif reg\n");

	for (i = 0; i <= (PMIF_REG_RANGE/4); i++) {
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
		reg_addr = (unsigned int *) (PMIF_SPI_BASE + i * 4);
		reg_value = WRAP_RD32(reg_addr);
		PWRAPCRI("addr:0x%p = 0x%x\n", reg_addr, reg_value);
#else
		reg_addr = (PMIF_SPI_BASE + i * 4);
		reg_value = WRAP_RD32(reg_addr);
		PWRAPCRI("addr:0x%x = 0x%x\n", reg_addr, reg_value);
#endif
	}

	for (i = 0; i <= 10; i++) {
		offset = 0xc00 + i * 4;
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
		reg_addr = (unsigned int *) (PMIF_SPI_BASE + offset);
		reg_value = WRAP_RD32(reg_addr);
		PWRAPCRI("addr:0x%p = 0x%x\n", reg_addr, reg_value);
#else
		reg_addr = (PMIF_SPI_BASE + offset);
		reg_value = WRAP_RD32(reg_addr);
		PWRAPCRI("addr:0x%x = 0x%x\n", reg_addr, reg_value);
#endif
	}

	for (i = 0; i <= 10; i++) {
		offset = 0xc40 + i * 4;
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
		reg_addr = (unsigned int *) (PMIF_SPI_BASE + offset);
		reg_value = WRAP_RD32(reg_addr);
		PWRAPCRI("addr:0x%p = 0x%x\n", reg_addr, reg_value);
#else
		reg_addr = (PMIF_SPI_BASE + offset);
		reg_value = WRAP_RD32(reg_addr);
		PWRAPCRI("addr:0x%x = 0x%x\n", reg_addr, reg_value);
#endif
	}

	for (i = 0; i <= 10; i++) {
		offset = 0xc80 + i * 4;
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
		reg_addr = (unsigned int *) (PMIF_SPI_BASE + offset);
		reg_value = WRAP_RD32(reg_addr);
		PWRAPCRI("addr:0x%p = 0x%x\n", reg_addr, reg_value);
#else
		reg_addr = (PMIF_SPI_BASE + offset);
		reg_value = WRAP_RD32(reg_addr);
		PWRAPCRI("addr:0x%x = 0x%x\n", reg_addr, reg_value);
#endif
	}

	PWRAPCRI("dump pmicspi_mst reg\n");

	for (i = 0; i <= (PMICSPI_MST_REG_RANGE/4); i++) {
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
		reg_addr = (unsigned int *) (PMICSPI_MST_BASE + i * 4);
		reg_value = WRAP_RD32(reg_addr);
		PWRAPCRI("addr:0x%p = 0x%x\n", reg_addr, reg_value);
#else
		reg_addr = (PMICSPI_MST_BASE + i * 4);
		reg_value = WRAP_RD32(reg_addr);
		PWRAPCRI("addr:0x%x = 0x%x\n", reg_addr, reg_value);
#endif
	}
}

void pwrap_dump_all_register(void)
{
	pwrap_dump_ap_register();
}

static int is_pwrap_init_done(void)
{
	int ret = 0;

	ret = WRAP_RD32(PMIF_SPI_PMIF_INIT_DONE);
	PWRAPLOG("is_pwrap_init_done %d\n", ret);
	if ((ret & 0x1) == 1)
		return 0;
	return E_PWR_NOT_INIT_DONE;
}

signed int pwrap_init_preloader(void)
{
	unsigned int pwrap_ret = 0, i = 0;

	PWRAPFUC();
	if (0 == is_pwrap_init_done()) {
		PWRAPLOG("[PMIC_WRAP]wrap_init already init, do nothing\n");
		return 0;
	}
	for (i = 0; i < 3; i++) {
		pwrap_ret = pwrap_init();
		if (pwrap_ret != 0) {
			PWRAPERR("init fail, ret=%x.\n",pwrap_ret);
			if (i >= 2)
				ASSERT(0);
		} else {
			PWRAPLOG("init pass, ret=%x.\n",pwrap_ret);
			break;
		}
	}

	return 0;
}

#endif /*endif PMIC_WRAP_NO_PMIC */
