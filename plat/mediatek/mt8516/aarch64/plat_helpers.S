/*
 * Copyright (c) 2013-2015, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <arch.h>
#include <asm_macros.S>
#include <mt8516_def.h>

	.globl	plat_secondary_cold_boot_setup
	.globl	plat_report_exception
	.globl	platform_is_primary_cpu
	.globl  plat_my_core_pos
	.globl	read_cpuectlr
	.globl	write_cpuectlr

	/* -----------------------------------------------------
	 * void plat_secondary_cold_boot_setup (void);
	 *
	 * This function performs any platform specific actions
	 * needed for a secondary cpu after a cold reset e.g
	 * mark the cpu's presence, mechanism to place it in a
	 * holding pen etc.
	 * -----------------------------------------------------
	 */
func plat_secondary_cold_boot_setup
	/* MT8173 Oak does not do cold boot for secondary CPU */
cb_panic:
	b	cb_panic
endfunc plat_secondary_cold_boot_setup

func read_cpuectlr
	MRS	x0, S3_1_C15_C2_1
	ret
endfunc	read_cpuectlr

func write_cpuectlr
	MSR	S3_1_C15_C2_1, x0
	ret
endfunc	write_cpuectlr

func platform_is_primary_cpu
	and	x0, x0, #(MPIDR_CLUSTER_MASK | MPIDR_CPU_MASK)
	cmp	x0, #MT8516_PRIMARY_CPU
	cset	x0, eq
	ret
endfunc platform_is_primary_cpu

	/* -----------------------------------------------------
	 * unsigned int plat_my_core_pos(void);
	 *
	 * result: CorePos = CoreId + (ClusterId << 2)
	 * -----------------------------------------------------
	 */
func plat_my_core_pos
	mrs     x0, mpidr_el1
	and     x1, x0, #MPIDR_CPU_MASK
	and     x0, x0, #MPIDR_CLUSTER_MASK
	add     x0, x1, x0, LSR #6
	ret
endfunc plat_my_core_pos
