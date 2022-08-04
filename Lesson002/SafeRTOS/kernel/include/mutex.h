/*
    Copyright (C)2006 onward WITTENSTEIN aerospace & simulation limited. All rights reserved.

    This file is part of the SafeRTOS product, see projdefs.h for version number information.

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

#ifndef RECURSIVE_MUTEX_H
#define RECURSIVE_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
 * MACROS AND DEFINITIONS
 *---------------------------------------------------------------------------*/

typedef void   *xMutexHandleType;

/*-----------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

/* Macros to assist in turning the result from xQueueMessagesWaiting() into
 * something meaningful to mutexes. */
#define mutexTAKEN                                  ( 0 )   /* the underlying queue is empty */
#define mutexAVAILABLE                              ( 1 )   /* the underlying queue is full */

/* The following macro's are part of the Mutex API however they can be
 * mapped directly to the equivalent queue functions and hence are implemented
 * as macros for efficiency purposes. */

#define xMutexTake( xMutex, xBlockTime )            xQueueTakeMutex( ( xQueueHandle )( xMutex ), ( xBlockTime ) )
#define xMutexGive( xMutex )                        xQueueGiveMutex( ( xQueueHandle )( xMutex ) )

#define xMutexGetState( xMutex, puxMutexState )     xQueueMessagesWaiting( ( xQueueHandle )( xMutex ), ( puxMutexState ) )

KERNEL_FUNCTION portBaseType xMutexCreate( portInt8Type *pcMutexBuffer, xMutexHandleType *pxMutex );

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* RECURSIVE_MUTEX_H */
