/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <mtk_sip_svc.h>
#include <lib/xlat_tables/xlat_tables_v2.h>

#include "apusys_rv.h"
#include "apusys_rv_coredump.h"
#include "apusys_rv_load_img.h"

#if ENABLE_APUSYS_EMI_PROTECTION
#ifdef CONFIG_MTK_APUSYS_SMPU
extern void emi_mpu_set_protection(u_register_t start, u_register_t end, u_register_t region);
#else
#include <emi.h>
#endif
#endif

#define NEED_VERIFY_IMG	(1)

#if NEED_VERIFY_IMG
#include "apusys_secure_boot.h"
#endif

#define APUSYS_IMG_VALID_DATA_OFFSET (0x200)

static bool apusys_rv_load_image_called;

static int apusys_rv_parse_apu_img(uint64_t apu_secure_info_pa, uint64_t apu_img_base_pa,
	uint64_t sec_buf_pa, uint64_t sec_buf_size)
{
	uint32_t apusys_pmsize = 0, apusys_xsize = 0;
	void *apusys_pmimg = NULL, *apusys_ximg = NULL;
	void *dst_addr = NULL, *src_addr = NULL;
	void *img = NULL;
	struct ptimg_hdr_t *hdr = NULL;
	unsigned int *img_size = NULL;
	uint32_t calibration_size = 0;
	struct apusys_secure_info_t *sec_info = (struct apusys_secure_info_t *)apu_secure_info_pa;

	INFO("%s, up_code_buf_ofs is 0x%x, up_code_buf_sz is 0x%x\n",
		__func__, sec_info->up_code_buf_ofs, sec_info->up_code_buf_sz);

	src_addr = (void *)apu_img_base_pa + APUSYS_IMG_VALID_DATA_OFFSET;
	dst_addr = (void *)sec_buf_pa +
				sec_info->up_code_buf_ofs +
				sec_info->up_code_buf_sz +
				ROUNDUP(sizeof(*sec_info), APUSYS_FW_ALIGN);
	/*img_size + APUSYS_IMG_VALID_DATA_OFFSET == "apusys.img" size*/
	img_size = (void *)apu_img_base_pa + (0x4);

	calibration_size = (*img_size) + ((uint64_t)dst_addr - sec_buf_pa);
	if (calibration_size > sec_info->total_sz) {
		ERROR("%s: calibration_size(0x%x) > sec_mem_size:0x%x\n", __func__,
				calibration_size, sec_info->total_sz);
		return -ENOENT;
	}

	memcpy(dst_addr, src_addr, (*img_size));
	hdr = dst_addr;
	INFO("%s: fw copy:dst:0x%lx, src:0x%lx, img_size:0x%x, hdr->magic is 0x%x\n", __func__,
			(uint64_t)dst_addr, (uint64_t)src_addr, *img_size, hdr->magic);

	while (hdr->magic == PT_MAGIC) {
		img = ((void *) hdr) + hdr->hdr_size;
		INFO("Rhdr->hdr_size= 0x%x\n", hdr->hdr_size);
		INFO("img address is 0x%lx\n", (uint64_t)img);

		switch (hdr->id) {
		case PT_ID_APUSYS_FW:
			INFO("PT_ID_APUSYS_FW\n");
			apusys_pmimg = img;
			apusys_pmsize = hdr->img_size;
			sec_info->up_fw_ofs = (uint64_t)img - (uint64_t)sec_buf_pa;
			sec_info->up_fw_sz = apusys_pmsize;
			INFO("up_fw_ofs = 0x%x, up_fw_sz = 0x%x\n",
				sec_info->up_fw_ofs, sec_info->up_fw_sz);
			break;
		case PT_ID_APUSYS_XFILE:
			INFO("PT_ID_APUSYS_XFILE\n");
			apusys_ximg = img;
			apusys_xsize = hdr->img_size;
			sec_info->up_xfile_ofs = (uint64_t)hdr - (uint64_t)sec_buf_pa;
			sec_info->up_xfile_sz = hdr->hdr_size + apusys_xsize;
			INFO("up_xfile_ofs = 0x%x, up_xfile_sz = 0x%x\n",
				sec_info->up_xfile_ofs, sec_info->up_xfile_sz);
			break;
		case PT_ID_MDLA_FW_BOOT:
			INFO("PT_ID_MDLA_FW_BOOT\n");
			sec_info->mdla_fw_boot_ofs = (uint64_t)img - (uint64_t)sec_buf_pa;
			sec_info->mdla_fw_boot_sz = hdr->img_size;
			INFO("mdla_fw_boot_ofs = 0x%x, mdla_fw_boot_sz = 0x%x\n",
				sec_info->mdla_fw_boot_ofs, sec_info->mdla_fw_boot_sz);
			break;
		case PT_ID_MDLA_FW_MAIN:
			INFO("PT_ID_MDLA_FW_MAIN\n");
			sec_info->mdla_fw_main_ofs = (uint64_t)img - (uint64_t)sec_buf_pa;
			sec_info->mdla_fw_main_sz = hdr->img_size;
			INFO("mdla_fw_main_ofs = 0x%x, mdla_fw_main_sz = 0x%x\n",
				sec_info->mdla_fw_main_ofs, sec_info->mdla_fw_main_sz);
			break;
		case PT_ID_MDLA_XFILE:
			INFO("PT_ID_MDLA_XFILE\n");
			sec_info->mdla_xfile_ofs = (uint64_t)hdr - (uint64_t)sec_buf_pa;
			sec_info->mdla_xfile_sz = hdr->hdr_size + hdr->img_size;
			INFO("mdla_xfile_ofs = 0x%x, mdla_xfile_sz = 0x%x\n",
				sec_info->mdla_xfile_ofs, sec_info->mdla_xfile_sz);
			break;
		case PT_ID_MVPU_FW:
			INFO("PT_ID_MVPU_FW\n");
			sec_info->mvpu_fw_ofs = (uint64_t)img - (uint64_t)sec_buf_pa;
			sec_info->mvpu_fw_sz = hdr->img_size;
			INFO("mvpu_fw_ofs = 0x%x, mvpu_fw_sz = 0x%x\n",
				sec_info->mvpu_fw_ofs, sec_info->mvpu_fw_sz);
			break;
		case PT_ID_MVPU_XFILE:
			INFO("PT_ID_MVPU_XFILE\n");
			sec_info->mvpu_xfile_ofs = (uint64_t)hdr - (uint64_t)sec_buf_pa;
			sec_info->mvpu_xfile_sz = hdr->hdr_size + hdr->img_size;
			INFO("mvpu_xfile_ofs = 0x%x, mvpu_xfile_sz = 0x%x\n",
				sec_info->mvpu_xfile_ofs, sec_info->mvpu_xfile_sz);
			break;
		case PT_ID_MVPU_SEC_FW:
			INFO("PT_ID_MVPU_SEC_FW\n");
			sec_info->mvpu_sec_fw_ofs = (uint64_t)img - (uint64_t)sec_buf_pa;
			sec_info->mvpu_sec_fw_sz = hdr->img_size;
			INFO("mvpu_sec_fw_ofs = 0x%x, mvpu_sec_fw_sz = 0x%x\n",
				sec_info->mvpu_sec_fw_ofs, sec_info->mvpu_sec_fw_sz);
			break;
		case PT_ID_MVPU_SEC_XFILE:
			INFO("PT_ID_MVPU_SEC_XFILE\n");
			sec_info->mvpu_sec_xfile_ofs = (uint64_t)hdr - (uint64_t)sec_buf_pa;
			sec_info->mvpu_sec_xfile_sz = hdr->hdr_size + hdr->img_size;
			INFO("mvpu_sec_xfile_ofs = 0x%x, mvpu_sec_xfile_sz = 0x%x\n",
				sec_info->mvpu_sec_xfile_ofs, sec_info->mvpu_sec_xfile_sz);
			break;
		case PT_ID_CE_BIN:
			INFO("PT_ID_CE_BIN\n");
			sec_info->ce_bin_ofs = (uint64_t)img - (uint64_t)sec_buf_pa;
			sec_info->ce_bin_sz = hdr->hdr_size + hdr->img_size;
			INFO("ce_bin_ofs = 0x%x, ce_bin_sz = 0x%x\n",
				sec_info->ce_bin_ofs, sec_info->ce_bin_sz);
			break;
		case PT_ID_CE_BIN_DUMMY:
			/* dummy image do nothing */
			break;
		default:
			ERROR("Warning: Ignore unknown APUSYS image_%d\n", hdr->id);
			break;
		}

		INFO("hdr->img_size = 0x%x, ROUNDUP(hdr->img_size, hdr->align) = 0x%x\n",
			hdr->img_size, ROUNDUP(hdr->img_size, hdr->align));
		img += ROUNDUP(hdr->img_size, hdr->align);
		hdr = (struct ptimg_hdr_t *) img;
	}

	if (!apusys_pmimg || !apusys_ximg) {
		ERROR("APUSYS partition missing - PM:0x%lx, XM:0x%lx (@0x%lx)\n",
			(uint64_t)apusys_pmimg, (uint64_t)apusys_ximg, (uint64_t)dst_addr);
		return -ENOENT;
	}

	INFO("%s APUSYS load finished: PM:0x%lx, XM:0x%lx\n",
		__func__, (uint64_t)apusys_pmimg, (uint64_t)apusys_ximg);

	/* copy uP firmware to code buffer */
	memcpy((void *)(sec_buf_pa + sec_info->up_code_buf_ofs),
		(void *)(sec_buf_pa + sec_info->up_fw_ofs),
		sec_info->up_fw_sz);

	/* For ATF's apu_rv.c [static struct apusys_secure_info_t *apusys_secure_info]*/
	memcpy((void *)(sec_buf_pa + sec_info->up_code_buf_ofs +
			sec_info->up_code_buf_sz),
			(void *)sec_info, sizeof(*sec_info));

	INFO("apusys_kernel_apusys_rv_load_apu_img end\n");
	flush_dcache_range((uintptr_t)apu_secure_info_pa, sizeof(*sec_info));
	flush_dcache_range((uintptr_t)sec_buf_pa, sec_buf_size);
	return 0;
}

/*
 * apusys_rv_initialize_aee_coredump_buf() -  initialize_aee_coredump memory
 */
static int apusys_rv_initialize_aee_coredump_buf(struct apusys_secure_info_t *sec_info,
	uint64_t aee_coredump_buf_pa,
	uint64_t aee_coredump_buf_sz,
	uint64_t sec_buf_pa)
{
	struct apusys_aee_coredump_info_t *aee_coredump_info =
					(struct apusys_aee_coredump_info_t *)aee_coredump_buf_pa;
	aee_coredump_info->up_coredump_ofs = sizeof(struct apusys_aee_coredump_info_t);
	aee_coredump_info->up_coredump_sz = sec_info->up_coredump_sz;

	aee_coredump_info->regdump_ofs = aee_coredump_info->up_coredump_ofs +
		aee_coredump_info->up_coredump_sz;
	aee_coredump_info->regdump_sz = REGDUMP_BUF_SZ;

	aee_coredump_info->mdla_coredump_ofs = aee_coredump_info->regdump_ofs +
		aee_coredump_info->regdump_sz;
	aee_coredump_info->mdla_coredump_sz = sec_info->mdla_coredump_sz;

	aee_coredump_info->mvpu_coredump_ofs = aee_coredump_info->mdla_coredump_ofs +
		aee_coredump_info->mdla_coredump_sz;
	aee_coredump_info->mvpu_coredump_sz = sec_info->mvpu_coredump_sz;

	aee_coredump_info->mvpu_sec_coredump_ofs = aee_coredump_info->mvpu_coredump_ofs +
		aee_coredump_info->mvpu_coredump_sz;
	aee_coredump_info->mvpu_sec_coredump_sz = sec_info->mvpu_sec_coredump_sz;

	aee_coredump_info->up_xfile_ofs = aee_coredump_info->mvpu_sec_coredump_ofs +
		aee_coredump_info->mvpu_sec_coredump_sz;
	aee_coredump_info->up_xfile_sz = sec_info->up_xfile_sz;

	aee_coredump_info->mdla_xfile_ofs = aee_coredump_info->up_xfile_ofs +
		aee_coredump_info->up_xfile_sz;
	aee_coredump_info->mdla_xfile_sz = sec_info->mdla_xfile_sz;

	aee_coredump_info->mvpu_xfile_ofs = aee_coredump_info->mdla_xfile_ofs +
		aee_coredump_info->mdla_xfile_sz;
	aee_coredump_info->mvpu_xfile_sz = sec_info->mvpu_xfile_sz;

	aee_coredump_info->mvpu_sec_xfile_ofs = aee_coredump_info->mvpu_xfile_ofs +
		aee_coredump_info->mvpu_xfile_sz;
	aee_coredump_info->mvpu_sec_xfile_sz = sec_info->mvpu_sec_xfile_sz;

	aee_coredump_info->ce_bin_ofs = aee_coredump_info->mvpu_sec_xfile_ofs +
		aee_coredump_info->mvpu_sec_xfile_sz;
	aee_coredump_info->ce_bin_sz = sec_info->ce_bin_sz;

	aee_coredump_info->are_sram_ofs = aee_coredump_info->ce_bin_ofs +
		aee_coredump_info->ce_bin_sz;
	aee_coredump_info->are_sram_sz = CE_COREDUMP_BUF_SZ;

	/* copy uP xfile to aee_coredump buffer */
	INFO("%s: copy uP xfile to aee_coredump buffer, offset:0x%x\n",
		__func__, aee_coredump_info->up_xfile_ofs);
	memcpy((void *)(aee_coredump_buf_pa + aee_coredump_info->up_xfile_ofs),
			(void *)(sec_buf_pa + sec_info->up_xfile_ofs),
			sec_info->up_xfile_sz);

	/* copy mdla xfile to aee_coredump buffer */
	INFO("%s: copy mdla xfile to aee_coredump buffer, offset:0x%x\n",
		__func__, aee_coredump_info->mdla_xfile_ofs);
	memcpy((void *)(aee_coredump_buf_pa + aee_coredump_info->mdla_xfile_ofs),
			(void *)(sec_buf_pa + sec_info->mdla_xfile_ofs),
			sec_info->mdla_xfile_sz);

	/* copy mvpu xfile to aee_coredump buffer */
	INFO("%s: copy mvpu xfile to aee_coredump buffer, offset:0x%x\n",
		__func__, aee_coredump_info->mvpu_xfile_ofs);
	memcpy((void *)(aee_coredump_buf_pa + aee_coredump_info->mvpu_xfile_ofs),
			(void *)(sec_buf_pa + sec_info->mvpu_xfile_ofs),
			sec_info->mvpu_xfile_sz);

	/* copy mvpu_sec xfile to aee_coredump buffer */
	INFO("%s: copy mvpu_sec xfile to aee_coredump buffer, offset:0x%x\n",
		__func__, aee_coredump_info->mvpu_sec_xfile_ofs);
	memcpy((void *)(aee_coredump_buf_pa + aee_coredump_info->mvpu_sec_xfile_ofs),
			(void *)(sec_buf_pa + sec_info->mvpu_sec_xfile_ofs),
			sec_info->mvpu_sec_xfile_sz);

	/* copy ce bin to aee_coredump buffer */
	memcpy((void *) (aee_coredump_buf_pa + aee_coredump_info->ce_bin_ofs),
			(void *) (sec_buf_pa + sec_info->ce_bin_ofs),
			sec_info->ce_bin_sz);

	INFO("%s: aee_coredump_info = 0x%lx, up_coredump_ofs = 0x%x\n",
		__func__, (uint64_t)aee_coredump_info, aee_coredump_info->up_coredump_ofs);
	INFO("%s: up_xfile_ofs = 0x%x, up_xfile_sz = 0x%x\n",
		__func__, aee_coredump_info->up_xfile_ofs, aee_coredump_info->up_xfile_sz);

	return 0;
}

#ifndef CONFIG_MTK_APUSYS_SMPU
static int apusys_rv_sec_mem_emi_protect_en(uint64_t pa, uint64_t size)
{
#if ENABLE_APUSYS_EMI_PROTECTION
	/*
	 * setup EMI MPU
	 * domain 0: APMCU
	 * domain 5: APUSYS
	 */
	struct emi_region_info_t region_info;

	region_info.start = (unsigned long long) pa;
	region_info.end = (unsigned long long) (pa + size);
	region_info.region = APUSYS_SEC_FW_EMI_REGION;

	SET_ACCESS_PERMISSION(region_info.apc, UNLOCK,
					 SEC_RW, SEC_RW, FORBIDDEN, FORBIDDEN,
					 FORBIDDEN, FORBIDDEN, FORBIDDEN, FORBIDDEN,
					 FORBIDDEN, FORBIDDEN, SEC_RW, FORBIDDEN,
					 FORBIDDEN, FORBIDDEN, FORBIDDEN, SEC_RW);

	return emi_mpu_set_protection(&region_info);
#else
	return 0;
#endif
}
#endif

int32_t apusys_kernel_apusys_rv_load_image(uint64_t unused, uint64_t res_mem_size, uint64_t apusys_img_size)
{
	int ret = 0;
	struct apusys_secure_info_t *sec_info = NULL;
	struct apusys_aee_coredump_info_t *aee_coredump_info = NULL;
	uint64_t sec_info_pa = 0, sec_info_mem_sz = 0;
	uint64_t res_mem_start = 0, sec_mem_addr_pa = 0, sec_mem_sz = 0, sec_mem_total_size = 0,
		sec_mem_addr_ofs = 0, sec_info_ofs = 0;
	uint64_t xfile_buf_sz = 0, coredump_buf_sz = 0, regdump_buf_sz = 0;
	uint64_t aee_coredump_mem_addr_pa = 0, aee_coredump_mem_size = 0;

	if (apusys_rv_load_image_called) {
		ERROR("%s: only permitted called once\n", __func__);
		return -EPERM;
	}

	if ((res_mem_size != APUSYS_RESERVED_MEM_SZ) ||
		(apusys_img_size == 0) ||
		(apusys_img_size >= res_mem_size)) {
		ERROR("%s: Input parameter error\n", __func__);
		return -EPERM;
	}

	apusys_rv_load_image_called = true;

	res_mem_start = APUSYS_RESERVED_MEM_PA;
	sec_info_ofs = ROUNDUP(apusys_img_size, APUSYS_MEM_ALIGN);
	sec_info_mem_sz = ROUNDUP(sizeof(struct apusys_secure_info_t),
								APUSYS_MEM_ALIGN);
	sec_mem_addr_ofs = ROUNDUP((sec_info_ofs + sec_info_mem_sz), APUSYS_MEM_IOVA_ALIGN);


	INFO("%s: reserved addr pa :0x%lx, size:%lx, img_size:%lx\n",
		__func__, res_mem_start, res_mem_size, apusys_img_size);

	sec_info_pa = res_mem_start + sec_info_ofs;
	INFO("%s: secure_info addr:pa is 0x%lx, size is 0x%lx\n",
		__func__, sec_info_pa, sec_info_mem_sz);

	sec_mem_addr_pa = ROUNDUP((res_mem_start + sec_mem_addr_ofs), APUSYS_MEM_IOVA_ALIGN);
	sec_info = (struct apusys_secure_info_t *)sec_info_pa;
	sec_mem_sz = sec_info->total_sz;
	INFO("%s: secure mem buffer addr:pa 0x%lx, sec_mem_size = 0x%lx\n",
		__func__, sec_mem_addr_pa, sec_mem_sz);

	emi_mpu_set_protection(res_mem_start,
		((res_mem_start + sec_mem_addr_ofs) - 1), APUSYS_NS_FW_EMI_REGION);

#if NEED_VERIFY_IMG
	char *sig = NULL;
	/* verify img */
	sig = (char *)(res_mem_start + apusys_img_size - 256);
	ret = apusys_image_verify((uint64_t *)res_mem_start, (uint32_t)apusys_img_size, sig);
	if (ret != 0) {
		ERROR("%s: apusys_image_verify failed, ret(%d)\n",
			__func__, (int)ret);
		return ret;
	}
#endif

	if (apusys_rv_ops.apusys_rv_ops_setup_secure_mem != NULL) {
		ret = apusys_rv_ops.apusys_rv_ops_setup_secure_mem(sec_mem_addr_pa, sec_mem_sz);
		if (ret) {
			ERROR("%s: apusys_rv_setup_secure_mem failed, ret(%d)\n", __func__, ret);
			return ret;
		}
	} else {
		return -EOPNOTSUPP;
	}

	/* load apu img */
	ret = apusys_rv_parse_apu_img(sec_info_pa, res_mem_start, sec_mem_addr_pa, sec_mem_sz);
	if (ret) {
		ERROR("%s: apusys_rv_parse_apu_img failed, ret(%d)\n", __func__, ret);
		return ret;
	}

	if (sec_info->ce_bin_sz > 0) {
		if (apusys_rv_ops.apusys_rv_ops_setup_ce_bin != NULL) {
			ret = apusys_rv_ops.apusys_rv_ops_setup_ce_bin();
			if (ret) {
				ERROR("%s: setup_ce_bin failed, ret(%d)\n", __func__, ret);
				return ret;
			}
		} else {
			ret = -EOPNOTSUPP;
		}
	}

	/* prepare & map aee coredump mem region */
	xfile_buf_sz = sec_info->up_xfile_sz + sec_info->mdla_xfile_sz +
					sec_info->mvpu_xfile_sz + sec_info->mvpu_sec_xfile_sz;
	coredump_buf_sz = sec_info->up_coredump_sz +
					sec_info->mdla_coredump_sz +
					sec_info->mvpu_coredump_sz +
					sec_info->mvpu_sec_coredump_sz +
					CE_COREDUMP_BUF_SZ;

	regdump_buf_sz = REGDUMP_BUF_SZ;

	INFO("%s: xfile_buf_sz = 0x%lx, sizeof(coredump_info) = 0x%lx, coredump_buf_sz = 0x%lx\n",
		__func__, xfile_buf_sz, (uint64_t)sizeof(*aee_coredump_info), coredump_buf_sz);

	INFO("%s: regdump_buf_sz is 0x%lx\n", __func__, regdump_buf_sz);

#ifdef CONFIG_MTK_APUSYS_APPEND_IOMMU_PAGE_TBL
	/* append iommu page tbl buffer behind the apu sec buffer */
	INFO("%s: [ATF] ENABLE_APPEND_IOMMU_PAGE_TBL\n", __func__);
	sec_mem_total_size = ROUNDUP((sec_mem_sz + APUSYS_IOVA_PAGE), APUSYS_MEM_ALIGN);
#else
	INFO("%s: [ATF] NOT - ENABLE_APPEND_IOMMU_PAGE_TBL\n", __func__);
	sec_mem_total_size = ROUNDUP(sec_mem_sz, APUSYS_MEM_ALIGN);
#endif
	aee_coredump_mem_addr_pa = ROUNDUP((sec_mem_addr_pa + sec_mem_total_size),
		 APUSYS_MEM_ALIGN);
	aee_coredump_mem_size = sizeof(struct apusys_aee_coredump_info_t) + coredump_buf_sz +
		xfile_buf_sz + regdump_buf_sz + sec_info->ce_bin_sz;
	aee_coredump_mem_size = ROUNDUP(aee_coredump_mem_size, APUSYS_AEE_COREDUMP_MEM_ALIGN);
	INFO("%s:aee_coredump buffer addr:pa:0x%lx, size:0x%lx\n",
			__func__, aee_coredump_mem_addr_pa, aee_coredump_mem_size);

	if (apusys_rv_ops.apusys_rv_ops_setup_aee_coredump_mem != NULL) {
		ret = apusys_rv_ops.apusys_rv_ops_setup_aee_coredump_mem(aee_coredump_mem_addr_pa,
				aee_coredump_mem_size);
		if (ret != 0) {
			ERROR("%s: apusys_rv_setup_aee_coredump_mem failed, ret(%d)\n",
				__func__, (int)ret);
			return ret;
		}
	} else {
		return -EOPNOTSUPP;
	}

	/* init aee coredump buf info */
	ret = apusys_rv_initialize_aee_coredump_buf(sec_info, aee_coredump_mem_addr_pa,
				aee_coredump_mem_size, sec_mem_addr_pa);
	if (ret != 0) {
		ERROR("%s: apusys_rv_initialize_aee_coredump_buf failed, ret(%d)\n",
				__func__, ret);
		return ret;
	}

#ifdef CONFIG_MTK_APUSYS_SMPU
	emi_mpu_set_protection(sec_mem_addr_pa,
		((sec_mem_addr_pa + sec_mem_total_size) - 1), APUSYS_SEC_FW_EMI_REGION);
#else
	ret = apusys_rv_sec_mem_emi_protect_en(sec_mem_addr_pa, (sec_mem_total_size - 1));
#endif
	if (ret != 0) {
		ERROR("%s: sip_emi_set_protection failed, ret(%d), addr:0x%lx\n",
			__func__, ret, sec_mem_addr_pa);
		return ret;
	}

	return 0;
}
