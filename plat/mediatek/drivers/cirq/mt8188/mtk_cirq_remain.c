// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2022, MediaTek Inc. All rights reserved.
 */

#include <stdint.h>
#include <stdio.h>
#include <platform_def.h>

#include <mt_cirq.h>
#include <lib/mtk_init/mtk_init.h>
#include <lib/pm/mtk_pm.h>
#include <lpm/mt_lp_rm.h>

/*GIC interrupt ID*/
#define UART0_IRQ_ID		173
#define CAMSV1_IRQ_ID		349
#define CAMSV2_IRQ_ID		350
#define CAMSV3_IRQ_ID		351
#define CAMSV4_IRQ_ID		352
#define CAMSV5_IRQ_ID		361
#define CAMSV6_IRQ_ID		362
#define CAMSV7_IRQ_ID		363
#define CAMSV8_IRQ_ID		364

static struct mt_irqremain remain_irqs;

/*
 * mt_cirq_irqremain_submit: pass the logged information to cirq.
 */
static int mt_cirq_irqremain_submit(void)
{
	if (remain_irqs.count == 0)
		return -1;

	set_wakeup_sources(remain_irqs.irqs, remain_irqs.count);

	return 0;
}

/*
 * add_remain_irq: add irq num that requires the use of cirq.
 */
static void add_remain_irq(unsigned int irq)
{
	unsigned int i = remain_irqs.count;

	if (i >= MT_IRQ_REMAIN_MAX) {
		INFO("%s: Remain.irq.count limit exceeded\n", __func__);
		return;
	}

	remain_irqs.irqs[i] = irq;
	remain_irqs.wakeupsrc_cat[i] = 0;
	remain_irqs.wakeupsrc[i] = 0;
	remain_irqs.count++;
}

/*
 * mt_cirq_irqremain_init：cirq irqremain init.
 */
int mt_cirq_irqremain_init(void)
{
	int remain_sta;

	remain_irqs.count = 0;
	add_remain_irq(UART0_IRQ_ID);
	add_remain_irq(CAMSV1_IRQ_ID);
	add_remain_irq(CAMSV2_IRQ_ID);
	add_remain_irq(CAMSV3_IRQ_ID);
	add_remain_irq(CAMSV4_IRQ_ID);
	add_remain_irq(CAMSV5_IRQ_ID);
	add_remain_irq(CAMSV6_IRQ_ID);
	add_remain_irq(CAMSV7_IRQ_ID);
	add_remain_irq(CAMSV8_IRQ_ID);
	remain_sta = mt_cirq_irqremain_submit();
	if (remain_sta < 0)
		INFO("%s: no irq uses cirq\n", __func__);

	return 0;
}
MTK_ARCH_INIT(mt_cirq_irqremain_init);

/*
 * mt_cirq_mcusys_off: mcusys off callback
 */
void *mt_cirq_mcusys_off(const void *arg)
{
	struct mt_cpupm_event_data *nb =
		(struct mt_cpupm_event_data *) arg;

	if (remain_irqs.count == 0) {
		INFO("%s: no irq uses cirq\n", __func__);
		return (void *)arg;
	}

	if (nb && nb->pwr_domain & MT_CPUPM_PWR_DOMAIN_MCUSYS) {
		mt_cirq_sw_reset();
		mt_cirq_clone_gic();
		mt_cirq_enable();
	}

	return (void *)arg;
}
MT_CPUPM_SUBCRIBE_MCUSYS_PWR_OFF(mt_cirq_mcusys_off);

/*
 * mt_cirq_mcusys_on: mcusys on callback
 */
void *mt_cirq_mcusys_on(const void *arg)
{
	struct mt_cpupm_event_data *nb =
		(struct mt_cpupm_event_data *) arg;

	if (remain_irqs.count == 0)
		return (void *)arg;

	if (nb && nb->pwr_domain & MT_CPUPM_PWR_DOMAIN_MCUSYS) {
		mt_cirq_flush();
		mt_cirq_disable();
	}

	return (void *)arg;
}
MT_CPUPM_SUBCRIBE_MCUSYS_PWR_ON(mt_cirq_mcusys_on);
