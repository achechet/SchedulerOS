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

#ifndef COUNT_SEMAPHORE_TEST_H
#define COUNT_SEMAPHORE_TEST_H

/*---------------------------------------------------------------------------*/

portBaseType xStartCountingSemaphoreTasks( void );
portBaseType xAreCountingSemaphoreTasksStillRunning( void );

/* xCountingSemaphoreTimerHandler() returns a boolean telling whether a context switch
 * is required. The taskYIELD_FROM_ISR() must be performed by the caller function. */
portBaseType xCountingSemaphoreTimerHandler( void );

/*---------------------------------------------------------------------------*/

#endif /* COUNT_SEMAPHORE_TEST_H */
