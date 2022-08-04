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

/**
 * Create a single persistent task which periodically dynamically creates another
 * two tasks. The original task is called the creator task, the two tasks it
 * creates are called suicidal tasks.
 *
 * One of the created suicidal tasks kill the other suicidal task before killing
 * itself - leaving just the original task remaining.
 *
 * The creator task must be spawned after all of the other demo application tasks
 * as it keeps a check on the number of tasks under the scheduler control. The
 * number of tasks it expects to see running should never be greater than the
 * number of tasks that were in existence when the creator task was spawned, plus
 * one set of four suicidal tasks. If this number is exceeded an error is flagged.
 */

/* PortSpecifics.h includes some 'declare once' declarations. */
#define SUICIDE_TASK_C

#include <stdlib.h>
#include <limits.h>

/* Scheduler include files. */
#include "SafeRTOS_API.h"


/* Demo program include files. */
#include "death.h"
#include "PortSpecifics.h"

/* The task originally created which is responsible for periodically dynamically
 * creating another four tasks. */
static void vCreateTask( void *pvParameters );

/* The task function of the dynamically created tasks. */
static void vSuicidalTask( void *pvParameters );

/* A variable which is incremented every time the dynamic tasks are created.
 * This is used to check that the task is still running. */
portspecCREATE_DELETE_DATA_SECTION static volatile portUnsignedBaseType uxCreationCount = 0;

/* A variable that is incremented within the task delete hook so the create task
 * can ensure the same number of 'suicide' tasks are deleted as created. */
portspecCREATE_DELETE_DATA_SECTION volatile portUnsignedBaseType uxTaskDeleteCallCount = 0;

/* Used to log the detection of any error. */
portspecCREATE_DELETE_DATA_SECTION static volatile portBaseType xErrorOccurred = pdFALSE;

/* Task TCBs. */
portspecTCB_DATA_SECTION static xTCB xCreateTaskTCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xSuicidalTask1TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xSuicidalTask2TCB = { 0 };

/*---------------------------------------------------------------------------*/
/* Static as its address is passed as the task parameter. */
portspecCREATE_DELETE_DATA_SECTION static portUnsignedBaseType uxPriorityParameter = 0U;

portBaseType xCreateSuicidalTasks( portUnsignedBaseType uxPriority )
{
    portBaseType xStatus = pdPASS;
    xTaskParameters xCreateTaskParameters =
    {
        vCreateTask,                       /* Task code */
        "CREATOR",                          /* Task name */
        &xCreateTaskTCB,                    /* TCB */
        acCreateTaskStack,                  /* Stack buffer */
        portspecCREATE_TASK_STACK_SIZE,     /* Stack depth bytes */
        ( void * )( &uxPriorityParameter ), /* Parameters */
        taskIDLE_PRIORITY,                  /* Priority */
        NULL,                               /* TLS object */
        xCreateTaskPortParameters           /* Port-specific task parameters */
    };

    /* Create the Creator tasks - passing in as a parameter the priority at
     * which the suicidal tasks should be created. */
    uxPriorityParameter = uxPriority;
    xCreateTaskParameters.uxPriority = uxPriority;
    if( xTaskCreate( &xCreateTaskParameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }

    return xStatus;
}
/*---------------------------------------------------------------------------*/

static void vSuicidalTask( void *pvParameters )
{
    volatile portInt32Type lFirstDummyVariable, lSecondDummyVariable;
    portInt32Type lResult;
    portTaskHandleType xTaskToKill;
    const portTickType xDelay = ( portTickType ) 200 / configTICK_RATE_MS;

    if( pvParameters != NULL )
    {
        /* This task is periodically created four times. Two created tasks are
         * passed a handle to the other task so it can kill it before killing itself.
         * The other task is passed in NULL. */
        xTaskToKill = *( portTaskHandleType * )pvParameters;
    }
    else
    {
        xTaskToKill = NULL;
    }

    for( ;; )
    {
        /* Do something random just to use some stack and registers. */
        lFirstDummyVariable = 2;
        lSecondDummyVariable = 89;
        lResult = lFirstDummyVariable;
        lResult *= lSecondDummyVariable;
        lSecondDummyVariable = lResult;
        if( xTaskDelay( xDelay ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        if( xTaskToKill != NULL )
        {
            /* Make sure the other task has a go before we delete it. */
            if( xTaskDelay( ( portTickType ) 0 ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            /* Kill the other task that was created by vCreateTask(). */
            if( xTaskDelete( xTaskToKill ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            /* Kill ourselves. */
            if( xTaskDelete( NULL ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }
        }
    }
}
/*---------------------------------------------------------------------------*/

portspecCREATE_DELETE_DATA_SECTION static portTaskHandleType xCreatedTask = NULL;

static void vCreateTask( void *pvParameters )
{
    /* Used to store a handle to the task that should be killed by a suicidal
     * task, before it kills itself. */
    const portTickType xDelay = ( portTickType ) 1000 / configTICK_RATE_MS;
    portUnsignedBaseType uxPriority;
    portUnsignedBaseType uxExpectedDeleteCount = 0;
    xTaskParameters xSuicidalTask1Parameters =
    {
        vSuicidalTask,                     /* Task code */
        "SUICID1",                          /* Task name */
        &xSuicidalTask1TCB,                 /* TCB */
        acSuicidalTask1Stack,               /* Stack buffer */
        portspecSUICIDAL_TASK_STACK_SIZE,   /* Stack depth bytes */
        NULL,                               /* Parameters */
        taskIDLE_PRIORITY,                  /* Priority */
        NULL,                               /* TLS object */
        xSuicidalTask1PortParameters        /* Port-specific task parameters */
    };
    xTaskParameters xSuicidalTask2Parameters =
    {
        vSuicidalTask,                     /* Task code */
        "SUICID2",                          /* Task name */
        &xSuicidalTask2TCB,                 /* TCB */
        acSuicidalTask2Stack,               /* Stack buffer */
        portspecSUICIDAL_TASK_STACK_SIZE,   /* Stack depth bytes */
        &xCreatedTask,                      /* Parameters */
        taskIDLE_PRIORITY,                  /* Priority */
        NULL,                               /* TLS object */
        xSuicidalTask2PortParameters        /* Port-specific task parameters */
    };


    uxPriority = *( portUnsignedBaseType * ) pvParameters;
    xSuicidalTask1Parameters.uxPriority = uxPriority;
    xSuicidalTask2Parameters.uxPriority = uxPriority;

    for( ;; )
    {
        /* Just loop round, delaying then creating the four suicidal tasks. */
        if( xTaskDelay( xDelay ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        /* Check the task delete function has been called the expected number
         * of times. */
        if( uxExpectedDeleteCount != uxTaskDeleteCallCount )
        {
            xErrorOccurred = pdTRUE;
        }

        xCreatedTask = NULL;

        if( xTaskCreate( &xSuicidalTask1Parameters, &xCreatedTask ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }
        if( xTaskCreate( &xSuicidalTask2Parameters, NULL ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        ++uxCreationCount;

        /* When we run again we expect the two tasks just created to have
         * been deleted. */
        uxExpectedDeleteCount += 2;
    }
}
/*---------------------------------------------------------------------------*/

/* This is called to check that the creator task is still running and that there
 * are not any more than four extra tasks. */
portBaseType xIsCreateTaskStillRunning( void )
{
    portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType uxLastCreationCount = ( portUnsignedBaseType ) ULONG_MAX;
    portBaseType xReturn = pdTRUE;

    if( uxLastCreationCount == uxCreationCount )
    {
        xReturn = pdFALSE;
    }
    else
    {
        uxLastCreationCount = uxCreationCount;
    }

    if( pdTRUE == xErrorOccurred )
    {
        xReturn = pdFALSE;
    }
    else
    {
        /* Everything is okay. */
    }

    return xReturn;
}
/*---------------------------------------------------------------------------*/
