/*
	Copyright (C)2006 onward WITTENSTEIN aerospace & simulation limited. All rights reserved.

	This file is part of the SafeRTOS product, see projdefs.h for version number
	information.

	SafeRTOS is distributed exclusively by WITTENSTEIN high integrity systems,
	and is subject to the terms of the License granted to your organization,
	including its warranties and limitations on use and distribution. It cannot be
	copied or reproduced in any way except as permitted by the License.

	Licenses authorize use by processor, compiler, business unit, and product.

	WITTENSTEIN high integrity systems is a trading name of WITTENSTEIN
	aerospace & simulation ltd, Registered Office: Brown's Court, Long Ashton
	Business Park, Yanley Lane, Long Ashton, Bristol, BS41 9LB, UK.
	Tel: +44 (0) 1275 395 600, fax: +44 (0) 1275 393 630.
	E-mail: info@HighIntegritySystems.com

	http://www.HighIntegritySystems.com
*/

#ifndef SAFERTOS_CONFIG_H
#define SAFERTOS_CONFIG_H

#define configMAX_PRIORITIES				( 6U )

#define configMINIMAL_STACK_SIZE_WITH_FPU	( 512U )	/* Needs to be a power of two value. */
#define configMINIMAL_STACK_SIZE_NO_FPU		( 256U )	/* Needs to be a power of two value. */

#define configTICK_RATE_HZ					( ( portTickType ) 1000U )

/* This macro calculates the number of ticks per millisecond.
 * NOTE: This integer calculation is only correct for values
 * of configTICK_RATE_HZ less than or equal to 1000 that are also divisors
 * of 1000. */
#define configTICK_RATE_MS					( ( portTickType ) 1000U / configTICK_RATE_HZ )

#define configCPU_CLOCK_HZ					( ( portUInt32Type ) 32768 )

/* This defines the priority of the kernel tick interrupt handler. It is set to
 * the lowest possible priority - priority 15, or ( 15 << 4 ) as only four
 * priority bits are implemented. */
#define configKERNEL_INTERRUPT_PRIORITY		( 15U << 4U )

/* This defines the maximum interrupt priority level that is permitted to use
 * SafeRTOS API calls. Note that on the Cortex-M4 higher numbers mean low
 * priority therefore only interrupt routines with a priority equal to or
 * greater than configSYSTEM_INTERRUPT_PRIORITY may utilise API calls.
 * Here it is defined as priority 10, or ( 10 << 4 ) as only four priority bits
 * are implemented. */
#define configSYSTEM_INTERRUPT_PRIORITY		( 10U << 4U )

/* This defines the priority of the SVC call handler - it must be one higher
 * than configSYSTEM_INTERRUPT_PRIORITY. Here it is defined as priority 4, or
 * ( 9 << 4 ) as only four priority bits are implemented. */
#define configSYSCALL_INTERRUPT_PRIORITY	( 9U << 4U )

/* Size of the Queue registry for use with the StateViewer plugin.
 * Set it to 0 to disable. */
#define configQUEUE_REGISTRY_SIZE			( 0UL )

/* Select the LPTIM module. */
#define vSafeRTOSTickHandler           LPTIM1_IRQHandler

/* Macros used for run-time statistics. This is the interface with a
 * counter/timer, usually the SysTick timer, that has an accessible counter
 * that increments or decrements by a reasonable number of counts per tick
 * interrupt. Two macros are required, as follows:
 *
 * configRTS_COUNTER_VALUE :
 * A macro expected to return a DESCENDING value.
 *
 * configRTS_COUNTER_MAX :
 * A macro that is either a constant or an expression involving a dereferenced
 * register pointer (e.g. a reload register or match register), that gives the
 * maximum numeric value that configRTS_COUNTER_VALUE can reach.

 * configMIN_CONTEXT_SWITCH_TIME_COUNTS :
 * A macro containing the minimum duration of a context switch, expressed as
 * number of timer counter decrements.
 */
#define configST_REGISTER_MASK					( ( portUInt32Type ) 0x00FFFFFF )
#define configRTS_COUNTER_MAX_REGISTER			( ( ( volatile portUInt32Type * ) 0xE000E014UL ) )
#define configRTS_COUNTER_VALUE_REGISTER		( ( ( volatile portUInt32Type * ) 0xE000E018UL ) )

#define configRTS_COUNTER_MAX					( *configRTS_COUNTER_MAX_REGISTER )
#define configRTS_COUNTER_VALUE					( *configRTS_COUNTER_VALUE_REGISTER )
#define configMIN_CONTEXT_SWITCH_TIME_COUNTS	( 200L )

/* The user configuration for the software timer module. */
#define configTIMER_TASK_PRIORITY				( 2U )
#define	configTIMER_TASK_STACK_SIZE				( 512U )
#define configTIMER_CMD_QUEUE_LEN				( 10U )
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP	( 2U )
/* The user configuration for the idle task. */
#define	configIDLE_TASK_STACK_SIZE				( configMINIMAL_STACK_SIZE_NO_FPU )

/* Additional stack margin */
#define configSTACK_CHECK_MARGIN				( 0U )

#endif /* SAFERTOS_CONFIG_H */
