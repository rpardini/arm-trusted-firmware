/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2014. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include <arch.h>
#include <arch_helpers.h>
#include <console.h>
#include <ep_info.h>
#include <context_mgmt.h>
#include <debug.h>
#include <mt_spm.h>
#include <mtk_plat_common.h>
#include <plat_debug.h>
#include <plat_sip_svc.h>
#include <platform.h>
#include <platform_def.h>
#include <runtime_svc.h>
#include <mtk_sip_svc.h>
#include <plat_pm.h>

#include <mt_spm.h>
#include <mt_spm_vcorefs.h>
#include <mt_spm_dpidle.h>
#include <mt_spm_sodi.h>
#include <mt_spm_idle.h>
#include <plat_dcm.h>
#include <plat_sip_calls.h>

/*******************************************************************************
 * SIP top level handler for servicing SMCs.
 ******************************************************************************/
uint64_t mediatek_plat_sip_handler(uint32_t smc_fid,
			  uint64_t x1,
			  uint64_t x2,
			  uint64_t x3,
			  uint64_t x4,
			  void *cookie,
			  void *handle,
			  uint64_t flags)
{
	uint64_t rc;
	uint32_t ns;
	rc = 0;

	cpu_context_t *ns_cpu_context;
	uint32_t mpidr = 0;
	uint32_t linear_id = 0;
	uint32_t res = 0;
	uint64_t spsr = 0;

	mpidr = read_mpidr();
	linear_id = plat_core_pos_by_mpidr(mpidr);
	ns_cpu_context = (cpu_context_t *) cm_get_context_by_index(linear_id, NON_SECURE);

	/* Determine which security state this SMC originated from */
	ns = is_caller_non_secure(flags);
	if (!ns) {
		/***************************************/
		/* SiP SMC service secure world's call */
		/***************************************/
		switch (smc_fid) {

		default:
			rc = -1;//SMC_UNK;
			ERROR("%s: unhandled Sec SMC (0x%x)\n", __func__, smc_fid);
			goto smc_ret;
		}
	}

	/***************************************/
	/* SiP SMC service normal world's call */
	/***************************************/
	spsr = SMC_GET_EL3(ns_cpu_context, CTX_SPSR_EL3);
	if (GET_RW(spsr) == MODE_RW_64) {
		/* INFO("originated from kernel 64\n"); */
		switch (smc_fid) {
		case MTK_SIP_KERNEL_SPM_PWR_CTRL_ARGS_AARCH64:
			spm_pwr_ctrl_args(x1, x2, x3);
			break;
		case MTK_SIP_KERNEL_SPM_GET_PWR_CTRL_ARGS_AARCH64:
			rc = spm_get_pwr_ctrl_args(x1, x2, x3);
			break;
		case MTK_SIP_PARTNAME_ID_AARCH64:
			rc = mtk_plat_get_partid(&res);
			SMC_RET2(handle, rc, res);
			break;
		default:
			rc = -2;/*SMC_UNK;*/
			ERROR("%s: unknown 64B kernel SMC(0x%x)\n", __func__, smc_fid);
		}
	} else if ((GET_RW(spsr) == MODE_RW_32) &&
		((spsr & MODE32_MASK) == MODE32_svc)) {
		/* INFO("originated from kernel 32\n"); */
		switch (smc_fid) {
		case MTK_SIP_KERNEL_SPM_PWR_CTRL_ARGS_AARCH32:
			spm_pwr_ctrl_args(x1, x2, x3);
			break;
		case MTK_SIP_KERNEL_SPM_GET_PWR_CTRL_ARGS_AARCH32:
			rc = spm_get_pwr_ctrl_args(x1, x2, x3);
			break;
		case MTK_SIP_PARTNAME_ID_AARCH32:
			rc = mtk_plat_get_partid(&res);
			SMC_RET2(handle, rc, res);
			break;
		default:
			rc = -3;/*SMC_UNK;*/
			ERROR("%s: unknown 32B kernel SMC(0x%x)\n", __func__, smc_fid);
		}
	} else {
		rc = -4;/*SMC_UNK;*/
		ERROR("%s: unknown kernel stage SMC(0x%x)\n", __func__, smc_fid);
	}
smc_ret:
	SMC_RET1(handle, rc);
}

