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
 * The first test creates three tasks - two counter tasks (one continuous count
 * and one limited count) and one controller. A "count" variable is shared
 * between all three tasks. The two counter tasks should never be in a "ready"
 * state at the same time. The controller task runs at the same priority as
 * the continuous count task, and at a lower priority than the limited count
 * task.
 *
 * One counter task loops indefinitely, incrementing the shared count variable
 * on each iteration. To ensure it has exclusive access to the variable it
 * raises its priority above that of the controller task before each
 * increment, lowering it again to its original priority before starting the
 * next iteration.
 *
 * The other counter task increments the shared count variable on each
 * iteration of its loop until the count has reached a limit of 0xff - at
 * which point it suspends itself. It will not start a new loop until the
 * controller task has made it "ready" again by calling xTaskResume().
 * This second counter task operates at a higher priority than controller
 * task so does not need to worry about mutual exclusion of the counter
 * variable.
 *
 * The controller task is in two sections. The first section controls and
 * monitors the continuous count task. When this section is operational the
 * limited count task is suspended. Likewise, the second section controls
 * and monitors the limited count task. When this section is operational the
 * continuous count task is suspended.
 *
 * In the first section the controller task first takes a copy of the shared
 * count variable. To ensure mutual exclusion on the count variable it
 * suspends the continuous count task, resuming it again when the copy has been
 * taken. The controller task then sleeps for a fixed period - during which
 * the continuous count task will execute and increment the shared variable.
 * When the controller task wakes it checks that the continuous count task
 * has executed by comparing the copy of the shared variable with its current
 * value. This time, to ensure mutual exclusion, the scheduler itself is
 * suspended with a call to xTaskSuspendAll(). This is for demonstration
 * purposes only and is not a recommended technique due to its inefficiency.
 *
 * After a fixed number of iterations the controller task suspends the
 * continuous count task, and moves on to its second section.
 *
 * At the start of the second section the shared variable is cleared to zero.
 * The limited count task is then woken from its suspension by a call to
 * xTaskResume(). As this counter task operates at a higher priority than
 * the controller task the controller task should not run again until the
 * shared variable has been counted up to the limited value causing the counter
 * task to suspend itself. The next line after xTaskResume() is therefore
 * a check on the shared variable to ensure everything is as expected.
 *
 * The second test consists of a couple of very simple tasks that post onto a
 * queue while the scheduler is suspended. This test was added to test parts
 * of the scheduler not exercised by the first test.
 */

/* PortSpecifics.h includes some 'declare once' declarations. */
#define DYNAMIC_MANIPULATION_C

#include <stdlib.h>

/* Scheduler include files. */
#include "SafeRTOS_API.h"

/* Demo app include files. */
#include "dynamic.h"
#include "PortSpecifics.h"

#if ( configQUEUE_REGISTRY_SIZE > 0 )
    #include "queue_register.h"
#endif

/* Function that implements the "limited count" task as described above. */
static void vLimitedIncrementTask( void *pvParameters );

/* Function that implements the "continuous count" task as described above. */
static void vContinuousIncrementTask( void *pvParameters );

/* Function that implements the controller task as described above. */
static void vCounterControlTask( void *pvParameters );

static void vQueueReceiveWhenSuspendedTask( void *pvParameters );
static void vQueueSendWhenSuspendedTask( void *pvParameters );

/* Demo task specific constants. */
#define priSLEEP_TIME                   ( ( portTickType ) 128 / configTICK_RATE_MS )
#define priLOOPS                        ( 5 )
#define priMAX_COUNT                    ( ( portUInt32Type ) 0xFF )
#define priNO_BLOCK                     ( ( portTickType ) 0 )

/* Used to log the detection of any error. */
portspecDYNAMIC_TASK_DATA_SECTION static volatile portBaseType xErrorOccurred = pdFALSE;

/* Task TCBs. */
portspecTCB_DATA_SECTION static xTCB xContinuousIncrementTaskTCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xLimitedIncrementTaskTCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xCounterControlTaskTCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xQueueSendWhenSuspendedTaskTCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xQueueReceiveWhenSuspendedTaskTCB = { 0 };

/*---------------------------------------------------------------------------*/

/* Handles to the two counter tasks. These could be passed in as parameters
 * to the controller task to prevent them having to be file scope. */
portspecDYNAMIC_TASK_DATA_SECTION static portTaskHandleType xContinousIncrementHandle = NULL;
portspecDYNAMIC_TASK_DATA_SECTION static portTaskHandleType xLimitedIncrementHandle = NULL;

/* The shared counter variable. This is passed in as a parameter to the two
 * counter variables for demonstration purposes. */
portspecDYNAMIC_TASK_DATA_SECTION static portUInt32Type ulCounter = 0;

/* Variables used to check that the tasks are still operating without error.
 * Each complete iteration of the controller task increments this variable
 * provided no errors have been found. The variable maintaining the same value
 * is therefore indication of an error. */
portspecDYNAMIC_TASK_DATA_SECTION static volatile portUInt16Type usCheckVariable = ( portUInt16Type ) 0;
portspecDYNAMIC_TASK_DATA_SECTION static volatile portBaseType xSuspendedQueueSendError = pdFALSE;
portspecDYNAMIC_TASK_DATA_SECTION static volatile portBaseType xSuspendedQueueReceiveError = pdFALSE;

/* Queue used by the second test. */
portspecDYNAMIC_TASK_DATA_SECTION static xQueueHandle xSuspendedTestQueue = NULL;

/*---------------------------------------------------------------------------*/
/*
 * Start the three tasks as described at the top of the file.
 * Note that the limited count task is given a higher priority.
 */
portBaseType xStartDynamicPriorityTasks( void )
{
    portBaseType xStatus = pdPASS;
    xTaskParameters xContinuousIncrementTaskParameters =
    {
        vContinuousIncrementTask,                       /* Task code */
        "CNT_INC",                                      /* Task name */
        &xContinuousIncrementTaskTCB,                   /* TCB */
        acContinuousIncrementTaskStack,                 /* Stack buffer */
        portspecDYNAMIC_TASK_STACK_SIZE,                /* Stack depth bytes */
        ( void * )( &ulCounter ),                       /* Parameters */
        taskIDLE_PRIORITY,                              /* Priority */
        NULL,                                           /* TLS object */
        xContinuousIncrementTaskPortParameters          /* Port-specific task parameters */
    };
    xTaskParameters xLimitedIncrementTaskParameters =
    {
        vLimitedIncrementTask,                          /* Task code */
        "LIM_INC",                                      /* Task name */
        &xLimitedIncrementTaskTCB,                      /* TCB */
        acLimitedIncrementTaskStack,                    /* Stack buffer */
        portspecDYNAMIC_TASK_STACK_SIZE,                /* Stack depth bytes */
        ( void * )( &ulCounter ),                       /* Parameters */
        taskIDLE_PRIORITY,                              /* Priority */
        NULL,                                           /* TLS object */
        xLimitedIncrementTaskPortParameters             /* Port-specific task parameters */
    };
    xTaskParameters xCounterControlTaskParameters =
    {
        vCounterControlTask,                            /* Task code */
        "C_CTRL",                                       /* Task name */
        &xCounterControlTaskTCB,                        /* TCB */
        acCounterControlTaskStack,                      /* Stack buffer */
        portspecDYNAMIC_TASK_STACK_SIZE,                /* Stack depth bytes */
        NULL,                                           /* Parameters */
        taskIDLE_PRIORITY,                              /* Priority */
        NULL,                                           /* TLS object */
        xCounterControlTaskPortParameters               /* Port-specific task parameters */
    };
    xTaskParameters xQueueSendWhenSuspendedTaskParameters =
    {
        vQueueSendWhenSuspendedTask,                    /* Task code */
        "SUSP_TX",                                      /* Task name */
        &xQueueSendWhenSuspendedTaskTCB,                /* TCB */
        acQueueSendWhenSuspendedTaskStack,              /* Stack buffer */
        portspecDYNAMIC_TASK_STACK_SIZE,                /* Stack depth bytes */
        NULL,                                           /* Parameters */
        taskIDLE_PRIORITY,                              /* Priority */
        NULL,                                           /* TLS object */
        xQueueSendWhenSuspendedTaskPortParameters       /* Port-specific task parameters */
    };
    xTaskParameters xQueueReceiveWhenSuspendedTaskParameters =
    {
        vQueueReceiveWhenSuspendedTask,                 /* Task code */
        "SUSP_RX",                                      /* Task name */
        &xQueueReceiveWhenSuspendedTaskTCB,             /* TCB */
        acQueueReceiveWhenSuspendedTaskStack,           /* Stack buffer */
        portspecDYNAMIC_TASK_STACK_SIZE,                /* Stack depth bytes */
        NULL,                                           /* Parameters */
        taskIDLE_PRIORITY,                              /* Priority */
        NULL,                                           /* TLS object */
        xQueueReceiveWhenSuspendedTaskPortParameters    /* Port-specific task parameters */
    };


    /* Create the Suspend Test Queue. */
    if( xQueueCreate( acSuspendTestQueueBuffer, portspecDYNAMIC_TASK_SUSPEND_QUEUE_BUFFER_LENGTH, portspecDYNAMIC_TASK_SUSPENDED_QUEUE_LENGTH, portspecDYNAMIC_TASK_QUEUE_ITEM_SIZE, &xSuspendedTestQueue) == pdPASS )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        vQueueAddToRegistry( xSuspendedTestQueue, "SuspendTestQ" );
#endif
    }
    else
    {
        xStatus = pdFAIL;
    }

    /* Create the required tasks. */
    if( xTaskCreate( &xContinuousIncrementTaskParameters, &xContinousIncrementHandle ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    xLimitedIncrementTaskParameters.uxPriority += 1;
    if( xTaskCreate( &xLimitedIncrementTaskParameters, &xLimitedIncrementHandle ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xCounterControlTaskParameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xQueueSendWhenSuspendedTaskParameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xQueueReceiveWhenSuspendedTaskParameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }

    return xStatus;
}
/*---------------------------------------------------------------------------*/

/*
 * Just loops around incrementing the shared variable until the limit has been
 * reached. Once the limit has been reached it suspends itself.
 */
static void vLimitedIncrementTask( void *pvParameters )
{
    portUInt32Type *pulCounter;

    /* Take a pointer to the shared variable from the parameters passed into
     * the task. */
    pulCounter = ( portUInt32Type * ) pvParameters;

    /* This will run before the control task, so the first thing it does is
     * suspend - the control task will resume it when ready. */
    if( xTaskSuspend( NULL ) != pdPASS )
    {
        xErrorOccurred = pdTRUE;
    }

    for( ;; )
    {
        /* Just count up to a value then suspend. */
        ( *pulCounter )++;

        if( *pulCounter >= priMAX_COUNT )
        {
            if( xTaskSuspend( NULL ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }
        }
    }
}
/*---------------------------------------------------------------------------*/

/*
 * Just keep counting the shared variable up. The control task will suspend
 * this task when it wants.
 */
static void vContinuousIncrementTask( void *pvParameters )
{
    portUInt32Type *pulCounter;
    portUnsignedBaseType uxOurPriority;

    /* Take a pointer to the shared variable from the parameters passed into
     * the task. */
    pulCounter = ( portUInt32Type * ) pvParameters;

    /* Query our priority so we can raise it when exclusive access to the
     * shared variable is required. */
    if( xTaskPriorityGet( NULL, &uxOurPriority ) != pdPASS )
    {
        xErrorOccurred = pdTRUE;
    }

    for( ;; )
    {
        /* Raise our priority above the controller task to ensure a context
         * switch does not occur while we are accessing this variable. */
        if( pdPASS == xTaskPrioritySet( NULL, uxOurPriority + 1 ) )
        {
            ( *pulCounter )++;
            if( xTaskPrioritySet( NULL, uxOurPriority ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }
        }
        else
        {
            xErrorOccurred = pdTRUE;
        }
    }
}
/*---------------------------------------------------------------------------*/

/*
 * Controller task as described above.
 */
static void vCounterControlTask( void *pvParameters )
{
    portUInt32Type ulLastCounter = 0UL;
    portInt16Type sLoops;
    portBaseType xError = pdFALSE;

    /* Just to stop warning messages. */
    ( void ) pvParameters;

    for( ;; )
    {
        /* Start with the counter at zero. */
        ulCounter = ( portUInt32Type ) 0;

        /* First section: */

        /* Check the continuous count task is running. */
        for( sLoops = 0; sLoops < priLOOPS; sLoops++ )
        {
            /* Suspend the continuous count task so we can take a mirror of the
             * shared variable without risk of corruption. */
            if( pdPASS == xTaskSuspend( xContinousIncrementHandle ) )
            {
                ulLastCounter = ulCounter;
                if( xTaskResume( xContinousIncrementHandle ) != pdPASS )
                {
                    xErrorOccurred = pdTRUE;
                }
            }
            else
            {
                xErrorOccurred = pdTRUE;
            }

            /* Now delay to ensure the other task has processor time. */
            if( xTaskDelay( priSLEEP_TIME ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            /* Check the shared variable again. This time to ensure mutual
             * exclusion the whole scheduler will be locked. This is just for
             * demo purposes! */
            vTaskSuspendScheduler();
            {
                if( ulLastCounter == ulCounter )
                {
                    /* The shared variable has not changed. There is a problem
                     * with the continuous count task so flag an error. */
                    xError = pdTRUE;
                }
            }
            ( void )xTaskResumeScheduler();
        }


        /* Second section: */

        /* Suspend the continuous counter task so it stops accessing the shared variable. */
        if( xTaskSuspend( xContinousIncrementHandle ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        /* Reset the variable. */
        ulCounter = ( portUInt32Type ) 0;

        /* Resume the limited count task which has a higher priority than us.
         * We should therefore not return from this call until the limited count
         * task has suspended itself with a known value in the counter variable. */
        if( xTaskResume( xLimitedIncrementHandle ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        /* Does the counter variable have the expected value? */
        if( ulCounter != priMAX_COUNT )
        {
            xError = pdTRUE;
        }

        if( pdFALSE == xError )
        {
            /* If no errors have occurred then increment the check variable. */
            taskENTER_CRITICAL();
            {
                usCheckVariable++;
            }
            taskEXIT_CRITICAL();
        }

        /* Resume the continuous count task and do it all again. */
        if( xTaskResume( xContinousIncrementHandle ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }
    }
}
/*---------------------------------------------------------------------------*/

portspecDYNAMIC_TASK_DATA_SECTION static portUInt32Type ulValueToSend = ( portUInt32Type ) 0;

static void vQueueSendWhenSuspendedTask( void *pvParameters )
{
    /* Just to stop warning messages. */
    ( void ) pvParameters;

    for( ;; )
    {
        if( xQueueSend( xSuspendedTestQueue, ( void * ) &ulValueToSend, priNO_BLOCK ) != pdTRUE )
        {
            xSuspendedQueueSendError = pdTRUE;
        }

        if( xTaskDelay( priSLEEP_TIME ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        ++ulValueToSend;
    }
}
/*---------------------------------------------------------------------------*/

portspecDYNAMIC_TASK_DATA_SECTION static portUInt32Type ulExpectedValue = ( portUInt32Type ) 0;

static void vQueueReceiveWhenSuspendedTask( void *pvParameters )
{
    portUInt32Type ulReceivedValue;
    portBaseType xGotValue;

    /* Just to stop warning messages. */
    ( void ) pvParameters;

    for( ;; )
    {
        do
        {
            xGotValue = xQueueReceive( xSuspendedTestQueue, ( void * ) &ulReceivedValue, priNO_BLOCK );

        } while( xGotValue != pdPASS );

        if( ulReceivedValue != ulExpectedValue )
        {
            xSuspendedQueueReceiveError = pdTRUE;
        }

        ++ulExpectedValue;
    }
}
/*---------------------------------------------------------------------------*/

/* Called to check that all the created tasks are still running without error. */
portBaseType xAreDynamicPriorityTasksStillRunning( void )
{
    /* Keep a history of the check variables so we know if it has been incremented
     * since the last call. */
    portspecCOMMON_PRIV_DATA_SECTION static portUInt16Type usLastTaskCheck = ( portUInt16Type ) 0;
    portBaseType xReturn = pdTRUE;

    /* Check the tasks are still running by ensuring the check variable
     * is still incrementing. */

    if( usCheckVariable == usLastTaskCheck )
    {
        /* The check has not incremented so an error exists. */
        xReturn = pdFALSE;
    }

    if( pdTRUE == xSuspendedQueueSendError )
    {
        xReturn = pdFALSE;
    }

    if( pdTRUE == xSuspendedQueueReceiveError )
    {
        xReturn = pdFALSE;
    }

    if( pdTRUE == xErrorOccurred )
    {
        xReturn = pdFALSE;
    }

    usLastTaskCheck = usCheckVariable;
    return xReturn;
}
/*---------------------------------------------------------------------------*/
