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

#ifndef SUICIDE_TASK_H
#define SUICIDE_TASK_H

/*---------------------------------------------------------------------------*/

/* Incremented within the task delete hook so the create task can ensure the
 * same number of 'suicide' tasks are deleted as created. */
extern volatile portUnsignedBaseType uxTaskDeleteCallCount;

/*---------------------------------------------------------------------------*/

portBaseType xCreateSuicidalTasks( portUnsignedBaseType uxPriority );
portBaseType xIsCreateTaskStillRunning( void );

/*---------------------------------------------------------------------------*/

#endif /* SUICIDE_TASK_H */


