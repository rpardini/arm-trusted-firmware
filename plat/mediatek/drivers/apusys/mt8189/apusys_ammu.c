/*
 * Copyright (c) 2025, MediaTek Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>
#include <lib/mmio.h>
#include <common/debug.h>
#include <mtk_mmap_pool.h>
#include <platform_def.h>
#include <apusys_security_ctrl_perm.h>
#include "apusys_ammu.h"

#define TBL_SRAM_SIZE			(0x3C00)
#define TBL_SIZE			(0x118)
#define TBL_NUM				(TBL_SRAM_SIZE / TBL_SIZE)

enum rsv_vsid {
	RSV_VSID_UPRV = 0,
	RSV_VSID_LOGGER,
	RSV_VSID_APMCU,
	RSV_VSID_GPU,
	RSV_VSID_MDLA,
	RSV_VSID_MVPU0,
	RSV_VSID_CE,
	RSV_VSID_MAX
};

#define VSID_RSV_LAST			(TBL_NUM - 1)
#define VSID_RSV_CE			(VSID_RSV_LAST - RSV_VSID_CE)
#define VSID_RSV_MVPU0			(VSID_RSV_LAST - RSV_VSID_MVPU0)
#define VSID_RSV_MDLA			(VSID_RSV_LAST - RSV_VSID_MDLA)
#define VSID_RSV_GPU			(VSID_RSV_LAST - RSV_VSID_GPU)
#define VSID_RSV_APMCU			(VSID_RSV_LAST - RSV_VSID_APMCU)
#define VSID_RSV_LOGGER			(VSID_RSV_LAST - RSV_VSID_LOGGER)
#define VSID_RSV_UPRV			(VSID_RSV_LAST - RSV_VSID_UPRV)

#define VSID_RSV_START			(VSID_RSV_CE)
#define VSID_RSV_END			(VSID_RSV_UPRV + 1)
#define VSID_RSV_NUM			(VSID_RSV_END - VSID_RSV_START)

#define err_p(x, ...)			ERROR("[ammu][%s] " x "%s", __func__, __VA_ARGS__)
#define err_printf(...)			err_p(__VA_ARGS__, "")

#define inf_p(x, ...)			INFO("[ammu] " x "%s", __VA_ARGS__)
#define inf_printf(...)			inf_p(__VA_ARGS__, "")

enum {
	APUMMU_THD_ID_APMCU_NORMAL = 1,
	APUMMU_THD_ID_TEE = 1,
	APUMMU_THD_ID_MAX
};

static uint32_t tbl_offset(uint32_t vsid)
{
#define DESC_SRAM	0x1400

	uint32_t val;

	/**
	 * 0x1400
	 * VSID desc sram
	 *   vsid table for index 0
	 *   vsid table for index 1
	 *   ...
	 */
	val  = DESC_SRAM;
	val += vsid * TBL_SIZE;

	return val;
}

static uint32_t seg_offset(uint32_t vsid, uint32_t seg_idx)
{
	uint32_t val;

	/**
	 * 0x0000
	 *   segment 0
	 * 0x0010
	 *   segment 1
	 * ...
	 * 0x0090
	 *   segment 9
	 */
	val  = tbl_offset(vsid) + 0x0000;
	val += seg_idx * 0x10;

	return val;
}

static uint32_t tbl_address(uint32_t vsid)
{
#define DESC_SRAM_START	0x0400

	uint32_t val;

	/**
	 * 0x0400
	 * VSID desc sram
	 *   vsid table for index 0
	 *   vsid table for index 1
	 *   ...
	 */
	val  = DESC_SRAM_START;
	val += vsid * TBL_SIZE;

	return val;
}

static uint32_t page_offset(uint32_t vsid, uint32_t page_sel)
{
	uint32_t val;

	/**
	 * 0x00A0
	 *   page array 0
	 * 0x00B8
	 *   page array 1
	 * ...
	 */
	val  = tbl_offset(vsid) + 0x00A0;
	val += (page_sel - 3) * 0x18;

	return val;
}

static int sram_config(void)
{
#define DESC_INDEX	0x1000

	uint32_t i;

	/**
	 * config reserved vsid: START~END
	 */
	for (i = VSID_RSV_START; i < VSID_RSV_END; i++) {
		uint32_t ofs, val, j;

		/**
		 * 0x1000
		 * VSID desc index
		 *   0x0000 -> VSID   0 descriptor address
		 *   0x0004 -> VSID   1 descriptor address
		 *   ...
		 *   0x03FC -> VSID 255 descriptor address
		 */
		ofs = DESC_INDEX + i * 4;
		val = tbl_address(i);
		mmio_write_32(APU_CMU_TOP + ofs, val);

		for (j = 0; j < 10; j++) {
			/**
			 * 0x000C
			 *   [31:31]: segment vld
			 */
			ofs = seg_offset(i, j) + 0x000C;
			val = 0x0;
			mmio_write_32(APU_CMU_TOP + ofs, val);
		}

		for (j = 3; j < 8; j++) {
			/**
			 * 0x0014
			 *   [05:00]: Page enable num
			 */
			ofs = page_offset(i, j) + 0x0014;
			val = 0x0;
			mmio_write_32(APU_CMU_TOP + ofs, val);
		}
	}

	return 0;
}

static int ammu_enable(void)
{
#define CMU_CON		0x0000

	uint32_t ofs, val;

	/**
	 * 0x0000
	 * cmu_con
	 *   [00:00]: APU_MMU_enable
	 *   [01:01]: tcu_apb_dbg_en
	 *   [02:02]: tcu_secure_chk_en
	 *   [03:03]: CMU_DCM_en
	 *   [04:04]: sw_slp_prot_en_override
	 */
	ofs  = CMU_CON;
	val  = mmio_read_32(APU_CMU_TOP + ofs);
	val |= 0x1;
	mmio_write_32(APU_CMU_TOP + ofs, val);

	return 0;
}

static int boot_init(void)
{
	int ret;

	ret = sram_config();
	if (ret)
		goto exit;

	ret = ammu_enable();
	if (ret)
		goto exit;
exit:
	return ret;
}

static int get_dns(enum apusys_dev_type engine_type,
	enum apusys_sec_level sec_level, uint8_t *domain, uint8_t *ns)
{
	int ret;

	ret = sec_get_dns(engine_type, sec_level, domain, ns);
	if (ret) {
		err_printf("sec_get_dns fail: %d\n", ret);
		goto exit;
	}
exit:
	return ret;
}

static int add_seg(uint32_t vsid, uint32_t seg_idx, uint32_t in_adr,
	uint32_t map_adr, uint32_t page_len, uint8_t domain, uint8_t ns)
{
	uint32_t seg, val;

	// init
	seg = seg_offset(vsid, seg_idx);

	/**
	 * 0x0000
	 *   [02:00]: Page length
	 *   [05:03]: Page select
	 *   [31:10]: Input address [33:12]
	 */
	val  = (page_len & 0x7) <<  0;
	val |= (0x0)            <<  3;
	val |= (in_adr >> 12)   << 10;
	mmio_write_32(APU_CMU_TOP + seg + 0x0000, val);

	/**
	 * 0x0004
	 *   [00:00]: axmmusecsid
	 *   [01:01]: IOMMU enable
	 *   [09:02]: smmu_sid
	 *   [31:10]: Mapped address base [33:12]
	 */
	val  = (0x0)           <<  0;
	val |= (0x1)           <<  1;
	val |= (0x0)           <<  2;
	val |= (map_adr >> 12) << 10;
	mmio_write_32(APU_CMU_TOP + seg + 0x0004, val);

	/**
	 * 0x0008
	 *   [00:00]: NS
	 *   [16:13]: Domain
	 */
	val  = (ns ? 1 : 0)   <<  0;
	val |= (domain & 0xf) << 13;
	mmio_write_32(APU_CMU_TOP + seg + 0x0008, val);

	/**
	 * 0x000C
	 *   [31:31]: segment vld
	 */
	val  = 0x1u << 31;
	mmio_write_32(APU_CMU_TOP + seg + 0x000C, val);

	return 0;
}

static int enable_vsid(uint32_t vsid)
{
#define VSID_ENABLE	0x0050
#define VSID_VALID	0x00B0

	uint32_t ofs, val;

	/**
	 * 0x0050
	 * VSID_enable0_set
	 *   [31:00]: the_VSID31_0_enable_set_register  W1S
	 * VSID_enable1_set
	 *   [31:00]: the_VSID63_32_enable_set_register W1S
	 * ...
	 */
	ofs = VSID_ENABLE + (vsid >> 5) * 4;
	val = 0x1u << (vsid & 0x1f);
	mmio_write_32(APU_CMU_TOP + ofs, val);

	/**
	 * 0x00B0
	 * VSID_valid0_set
	 *   [31:00]: the_VSID31_0_valid_set_register  W1S
	 * VSID_valid1_set
	 *   [31:00]: the_VSID63_31_valid_set_register W1S
	 * ...
	 */
	ofs = VSID_VALID + (vsid >> 5) * 4;
	val = 0x1u << (vsid & 0x1f);
	mmio_write_32(APU_CMU_TOP + ofs, val);

	return 0;
}

static int add_rv_map(uint32_t map_adr)
{
	uint8_t domain = 5, ns = 0;
	int ret;

	/* secure */

	ret = get_dns(APUSYS_DEVICE_UP, SEC_LEVEL_SECURE, &domain, &ns);
	if (ret)
		goto exit;

	ret = add_seg(VSID_RSV_UPRV, 0, 0, map_adr, APUMMU_PAGE_LEN_1MB, domain, ns);
	if (ret)
		goto exit;

	ret = add_seg(VSID_RSV_UPRV, 1, 0, 0, APUMMU_PAGE_LEN_512MB, domain, ns);
	if (ret)
		goto exit;

	/* normal */

	ret = get_dns(APUSYS_DEVICE_UP, SEC_LEVEL_NORMAL, &domain, &ns);
	if (ret)
		goto exit;

	ret = add_seg(VSID_RSV_UPRV, 2, 0, 0, APUMMU_PAGE_LEN_4GB, domain, ns);
	if (ret)
		goto exit;

	/* enable */

	ret = enable_vsid(VSID_RSV_UPRV);
	if (ret)
		goto exit;
exit:
	return ret;
}

static int bind_vsid(uint32_t base, uint32_t vsid, uint32_t thread)
{
	uint32_t ofs, val;

	/**
	 * 0x0000
	 * thread_to_vsid_cid_mapping_table0
	 *   [00:00]: vsid_vld
	 *   [01:01]: corid_vld
	 *   [02:02]: vsid_prefetch_trigger
	 *   [10:03]: vsid
	 *   [17:11]: cor_id
	 * thread_to_vsid_cid_mapping_table1
	 * ...
	 */
	ofs  = thread * 4;
	val  = (0x1)         <<  0;
	val |= (0x0)         <<  1;
	val |= (vsid & 0xff) <<  3;
	val |= (0x0)         << 11;
	mmio_write_32(base + ofs, val);

	return 0;
}

static int bind_rv_vsid(uint32_t thread)
{
	int ret;

	if (thread > 7) {
		err_printf("invalid thread: %d\n", thread);
		ret = -EINVAL;
		goto exit;
	}

	ret = bind_vsid(APU_RCX_UPRV_TCU, VSID_RSV_UPRV, thread);
	if (ret)
		goto exit;
exit:
	return ret;
}

static int virtual_engine_thread(void)
{
#define D2T_MAP_TBL0	0x0040

	uint32_t ofs, val;

	/**
	 * TFA (domain, ns) = (0, 0) = 0000 0 = 0
	 * 0x0040
	 * apu_mmu_d2t_mapping_table0
	 *   [02:00]: domain_ns0_thread_info
	 *   [05:03]: domain_ns1_thread_info
	 *   ...
	 *   [29:27]: domain_ns9_thread_info
	 * ...
	 * apu_mmu_d2t_mapping_table3
	 *   [02:00]: domain_ns30_thread_info
	 *   [05:03]: domain_ns31_thread_info
	 */
	ofs = D2T_MAP_TBL0;
	val = APUMMU_THD_ID_TEE;
	mmio_write_32(APU_RCX_EXTM_TCU + ofs, val);

	return 0;
}

static int add_apmcu_map(uint32_t in_adr, uint32_t map_adr,
	enum apummu_page_size page_size)
{
	uint8_t domain = 7, ns = 1;
	int ret;

	ret = add_seg(VSID_RSV_APMCU, 0, in_adr, map_adr, page_size, domain, ns);
	if (ret)
		goto exit;

	ret = enable_vsid(VSID_RSV_APMCU);
	if (ret)
		goto exit;
exit:
	return ret;
}

static int bind_apmcu_vsid(uint32_t thread)
{
	int ret;

	if (thread > 7) {
		err_printf("invalid thread: %d\n", thread);
		ret = -EINVAL;
		goto exit;
	}

	ret = bind_vsid(APU_RCX_EXTM_TCU, VSID_RSV_APMCU, thread);
	if (ret)
		goto exit;
exit:
	return ret;
}

int rv_boot(uint32_t uP_seg_output, uint32_t uP_hw_thread,
	uint32_t logger_seg_output, enum apummu_page_size logger_page_size,
	uint32_t XPU_seg_output, enum apummu_page_size XPU_page_size)
{
	int ret;

	ret = boot_init();
	if (ret)
		goto exit;

	ret = add_rv_map(uP_seg_output);
	if (ret)
		goto exit;

	/* thread: 0 for normal */
	ret = bind_rv_vsid(uP_hw_thread);
	if (ret)
		goto exit;

	/* thread: 1 for secure */
	ret = bind_rv_vsid(uP_hw_thread + 1);
	if (ret)
		goto exit;

	ret = virtual_engine_thread();
	if (ret)
		goto exit;

	ret = add_apmcu_map(XPU_seg_output, XPU_seg_output, XPU_page_size);
	if (ret)
		goto exit;

	ret = bind_apmcu_vsid(APUMMU_THD_ID_TEE);
	if (ret)
		goto exit;

exit:
	return ret;
}
