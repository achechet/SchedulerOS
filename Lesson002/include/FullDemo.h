/*
    Copyright (C)2006 onward WITTENSTEIN high integrity systems Limited. All rights reserved.

    This file is part of the SafeRTOS product, see projdefs.h for version number
    information.

    SafeRTOS is distributed exclusively by WITTENSTEIN high integrity systems,
    and is subject to the terms of the License granted to your organization,
    including its warranties and limitations on use and distribution. It cannot be
    copied or reproduced in any way except as permitted by the License.

    Licenses authorise use by processor, compiler, business unit, and product.

    WITTENSTEIN high integrity systems Limited, Registered Office: Brown's Court, Long Ashton
    Business Park, Yanley Lane, Long Ashton, Bristol, BS41 9LB, UK.
    Tel: +44 1275 395 600
    Email: info@highintegritysystems.com
    www.highintegritysystems.com
*/

#ifndef FULL_DEMO_H
#define FULL_DEMO_H

/* SafeRTOS Includes */
#include "SafeRTOS_API.h"


/*-----------------------------------------------------------------------------
 * Public constant definitions
 *---------------------------------------------------------------------------*/

/* SVC numbers for demo. */
#define fulldemoSVC_INCREMENT_VALUE     ( 10U )


/*-----------------------------------------------------------------------------
 * Public Prototypes
 *---------------------------------------------------------------------------*/

/* Prototype for the function that creates the demo application tasks. */
portBaseType xFullDemoCreate( void );

/* Following four prototypes are the SafeRTOS hook (or callback) functions.
 * These are passed to the kernel during initialisation.  */
void vDemoApplicationIdleHook( void );
void vDemoApplicationTickHook( void );

void vDemoApplicationTaskDeleteHook( portTaskHandleType xTaskBeingDeleted );

void vDemoApplicationErrorHook( portTaskHandleType xHandleOfTaskWithError,
                                const portCharType *pcErrorString,
                                portBaseType xErrorCode );

void vDemoApplicationSvcHook( portUnsignedBaseType uxSvcNumber );
portBaseType xDemoApplicationInhibitSleepHook( void );
void vDemoApplicationPreSleepHook( void );
void vDemoApplicationPostSleepHook( void );

/*---------------------------------------------------------------------------*/


#endif /* FULL_DEMO_H */
