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

    www.HighIntegritySystems.com
*/

/*
 * API documentation is included in the user manual. Do not refer to this
 * file for documentation.
 */

#ifndef QUEUE_H
#define QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif


typedef void *  xQueueHandle;


/*-----------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

KERNEL_FUNCTION portBaseType xQueueCreate( portInt8Type *pcQueueMemory, portUnsignedBaseType uxBufferLength, portUnsignedBaseType uxQueueLength, portUnsignedBaseType uxItemSize, xQueueHandle *pxQueue );
KERNEL_FUNCTION portBaseType xQueueSend( xQueueHandle xQueue, const void * const pvItemToQueue, portTickType xTicksToWait );
KERNEL_FUNCTION portBaseType xQueueSendToFront( xQueueHandle xQueue, const void * const pvItemToQueue, portTickType xTicksToWait );
KERNEL_FUNCTION portBaseType xQueueReceive( xQueueHandle xQueue, void * const pvBuffer, portTickType xTicksToWait );
KERNEL_FUNCTION portBaseType xQueuePeek( xQueueHandle xQueue, void * const pvBuffer, portTickType xTicksToWait );
KERNEL_FUNCTION portBaseType xQueueMessagesWaiting( const xQueueHandle xQueue, portUnsignedBaseType *puxMessagesWaiting );
KERNEL_FUNCTION portBaseType xQueueSendFromISR( xQueueHandle xQueue, const void * const pvItemToQueue, portBaseType *pxHigherPriorityTaskWoken );
KERNEL_FUNCTION portBaseType xQueueSendToFrontFromISR( xQueueHandle xQueue, const void * const pvItemToQueue, portBaseType *pxHigherPriorityTaskWoken );
KERNEL_FUNCTION portBaseType xQueueReceiveFromISR( xQueueHandle xQueue, void * const pvBuffer, portBaseType *pxHigherPriorityTaskWoken );

/* SAFERTOSTRACE QUEUEFUNCTIONS */

/*-----------------------------------------------------------------------------
 * Private API
 *---------------------------------------------------------------------------*/
/* Functions beyond this part are not part of the public API and are intended
 * for use by the kernel only. */

KERNEL_FUNCTION void vQueueSetSemaphoreInitialCount( xQueueHandle xQueue, portUnsignedBaseType uxInitialCount );

KERNEL_FUNCTION void vQueueInitialiseMutex( xQueueHandle xQueue );
KERNEL_FUNCTION portBaseType xQueueGiveMutex( xQueueHandle xMutex );
KERNEL_FUNCTION portBaseType xQueueTakeMutex( xQueueHandle xMutex, portTickType xTicksToWait );
KERNEL_FUNCTION portUnsignedBaseType uxQueueGetBlockedTaskPriority( xQueueHandle xQueue );
KERNEL_FUNCTION void vQueueRemoveListItemAndCheckInheritance( xListItem *pxListItem );
KERNEL_FUNCTION portBaseType xQueueForcedMutexRelease( xQueueHandle xMutex );

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* QUEUE_H */
