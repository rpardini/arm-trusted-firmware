/*
 * Copyright (c) 2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MT_GIC_V3_H
#define MT_GIC_V3_H

#include <lib/mmio.h>

#define GIC_INT_MASK (MCUCFG_BASE + 0x5e8)
#define GIC500_ACTIVE_SEL_SHIFT 3
#define GIC500_ACTIVE_SEL_MASK (0x7 << GIC500_ACTIVE_SEL_SHIFT)
#define GIC500_ACTIVE_CPU_SHIFT 16
#define GIC500_ACTIVE_CPU_MASK (0xff << GIC500_ACTIVE_CPU_SHIFT)

void mt_gic_driver_init(void);
void mt_gic_init(void);
void mt_gic_set_pending(uint32_t irq);
uint32_t mt_gic_get_pending(uint32_t irq);
void mt_gic_cpuif_enable(void);
void mt_gic_cpuif_disable(void);
void mt_gic_rdistif_init(void);
void mt_gic_distif_save(void);
void mt_gic_distif_restore(void);
void mt_gic_rdistif_save(void);
void mt_gic_rdistif_restore(void);
void mt_gic_rdistif_restore_all(void);
void mt_gic_sync_dcm_enable(void);
void mt_gic_sync_dcm_disable(void);
void mt_gic_sgi_save_all(void);
void mt_gic_sgi_restore_all(void);
void mt_irq_set_pending(uint32_t irq);

#endif /* MT_GIC_V3_H */
