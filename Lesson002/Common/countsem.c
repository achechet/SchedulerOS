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

/*
 * Simple demonstration of the usage of counting semaphore.
 */

/* PortSpecifics.h includes some 'declare once' declarations. */
#define COUNTING_SEMAPHORE_TEST_C

/* Library include files. */
#include <stdlib.h>

/* Scheduler include files. */
#include "SafeRTOS_API.h"

/* Demo Program include files. */
#include "countsemTimer.h"
#include "countsem.h"
#include "PortSpecifics.h"

#if ( configQUEUE_REGISTRY_SIZE > 0 )
    #include "queue_register.h"
#endif

/* The maximum count value that the semaphore used for the demo can hold. */
#define countMAX_COUNT_VALUE            ( 200 )

/* Constants used to indicate whether or not the semaphore should have been
 * created with its maximum count value, or its minimum count value. These
 * numbers are used to ensure that the pointers passed in as the task
 * parameters are valid. */
#define countSTART_AT_MAX_COUNT         ( 0xAA )
#define countSTART_AT_ZERO              ( 0x55 )

/* Three tasks are created for the test.
 * - Task1 uses 1 semaphore created with its count value set to maximum.
 * - Task2 uses 1 semaphore created with the count value set to zero.
 * - Task3 uses 1 semaphore created with its count value set to 1.
 */
#define countNUM_TEST_TASKS             ( 3 )
#define countDONT_BLOCK                 ( ( portTickType ) 0 )

/* Additional Semaphore ISR operational parameters. */
#define countSEM_BLOCK_600_MS           ( ( portTickType ) 600 / configTICK_RATE_MS )

#define countSEM_ISR_STATE_NULL         ( 0x1000 )
#define countSEM_ISR_STATE_GIVE         ( 0x1010 )
#define countSEM_ISR_STATE_TAKE         ( 0x1020 )

/*---------------------------------------------------------------------------*/

/* Error Flag that will be latched to pdTRUE
 * should any unexpected behaviour be detected in any of the tasks. */
portspecCOUNTSEM_TASK_ZERO_DATA_SECTION static volatile portBaseType xCountsemErrorDetected = pdFALSE;

/* Task3 - Semaphore ISR Operating Parameters */
portspecCOUNTSEM_TASK_ZERO_DATA_SECTION static volatile portUnsignedBaseType uxSemaphoreISRCount = 0;
portspecCOUNTSEM_TASK_DATA_SECTION static volatile portBaseType xSemaphoreISRState = countSEM_ISR_STATE_NULL;

/* Task3 - Semaphore ISR Task Handle */
portspecCOUNTSEM_TASK_ZERO_DATA_SECTION static portTaskHandleType xTestTask3Handle = NULL;

/*---------------------------------------------------------------------------*/

/*
 * The first 2 demo tasks use the same function body which simply counts the
 * semaphore up to its maximum value, then counts it back down again.
 * The result of each semaphore 'give' and 'take' is inspected, with an error
 * being flagged if it is found not to be the expected result.
 */
static void prvCountingSemaphoreTask( void *pvParameters );

/*
 * The task body used by the third demo task introduces a semaphore
 * 'give from ISR' and 'take from ISR' performed within an independent
 * Timer Counter interrupt event handler.
 */
static void prvInterruptSemaphoreTask( void *pvParameters );

/*
 * Utility function to increment the semaphore count value up from zero to
 * countMAX_COUNT_VALUE.
 */
static void prvIncrementSemaphoreCount( xSemaphoreHandle xSemaphore,
                                        portUnsignedBaseType *puxLoopCounter );

/*
 * Utility function to decrement the semaphore count value up from
 * countMAX_COUNT_VALUE to zero.
 */
static void prvDecrementSemaphoreCount( xSemaphoreHandle xSemaphore,
                                        portUnsignedBaseType *puxLoopCounter );

/*---------------------------------------------------------------------------*/

/* The structure that is passed into the task as the task parameter. */
typedef struct COUNT_SEM_STRUCT
{
    /* The semaphore to be used for the demo. */
    xSemaphoreHandle xSemaphore;

    /* Set to countSTART_AT_MAX_COUNT if the semaphore should be created with
     * its count value set to its max count value, or countSTART_AT_ZERO if
     * it should have been created with its count value set to 0. */
    portUnsignedBaseType uxExpectedStartCount;

    /* Incremented on each cycle of the demo task.
     * Used to detect a stalled task. */
    portUnsignedBaseType uxLoopCounter;

} xCountSemStruct;

/* Three structures are defined with one being passed to each test task. */
portspecCOUNTSEM_TASK_ZERO_DATA_SECTION static volatile xCountSemStruct xParameters[ countNUM_TEST_TASKS ] =
{
    { NULL, 0U, 0U },
    { NULL, 0U, 0U },
    { NULL, 0U, 0U }
};

/* Semaphore Test Task TCBs. */
portspecTCB_DATA_SECTION static xTCB xCountSemaphoreTestTask1TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xCountSemaphoreTestTask2TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xCountSemaphoreTestTask3TCB = { 0 };

/*---------------------------------------------------------------------------*/

/* Counting Semaphore Timer Handler */
portBaseType xCountingSemaphoreTimerHandler( void )
{
    portUnsignedBaseType uxSavedInterruptStatus;

    portBaseType xHigherPriorityTaskWoken = pdFALSE;
    portBaseType xStatus;

    xSemaphoreHandle xSemaTimerHandle = xParameters[ 2 ].xSemaphore;

    if( countSEM_ISR_STATE_NULL != xSemaphoreISRState )
    {
        if( countSEM_ISR_STATE_GIVE == xSemaphoreISRState )
        {
            xSemaphoreISRState = countSEM_ISR_STATE_TAKE;

            /* Release the task blocked on this semaphore. */
            xStatus = xSemaphoreGiveFromISR( xSemaTimerHandle,
                                             &xHigherPriorityTaskWoken );
            if( pdPASS != xStatus )
            {
                /* Log the error. */
                xCountsemErrorDetected = pdTRUE;
            }
        }
        else
        {
            xSemaphoreISRState = countSEM_ISR_STATE_GIVE;

            /* Release the task blocked on this semaphore. */
            xStatus = xSemaphoreTakeFromISR( xSemaTimerHandle,
                                             &xHigherPriorityTaskWoken );
            if( pdPASS != xStatus )
            {
                /* Log the error. */
                xCountsemErrorDetected = pdTRUE;
            }
        }
    }

    uxSavedInterruptStatus = taskSET_INTERRUPT_MASK_FROM_ISR();
    {
        /* Increment loop counter. */
        uxSemaphoreISRCount++;
    }
    taskCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

    return xHigherPriorityTaskWoken;
}
/*--------------------------------------------------------------------------*/

portBaseType xStartCountingSemaphoreTasks( void )
{
    portBaseType xStatus = pdPASS;

    xTaskParameters xTestTask1Parameters =
    {
        prvCountingSemaphoreTask,               /* Task code */
        "CNT1",                                 /* Task name */
        &xCountSemaphoreTestTask1TCB,           /* TCB */
        acCountSemTestTask1Stack,               /* Stack buffer */
        portspecCOUNTSEM_TASK_STACK_SIZE,       /* Stack depth bytes */
        ( void * ) &( xParameters[ 0 ] ),       /* Parameters */
        taskIDLE_PRIORITY,                      /* Priority */
        NULL,                                       /* TLS object */
        xCountSemaphoreTestTask1PortParameters  /* Port specific task parameters */
    };

    xTaskParameters xTestTask2Parameters =
    {
        prvCountingSemaphoreTask,               /* Task code */
        "CNT2",                                 /* Task name */
        &xCountSemaphoreTestTask2TCB,           /* TCB */
        acCountSemTestTask2Stack,               /* Stack buffer */
        portspecCOUNTSEM_TASK_STACK_SIZE,       /* Stack depth bytes */
        ( void * ) &( xParameters[ 1 ] ),       /* Parameters */
        taskIDLE_PRIORITY,                      /* Priority */
        NULL,                                   /* TLS object */
        xCountSemaphoreTestTask2PortParameters  /* Port specific task parameters */
    };

    xTaskParameters xTestTask3Parameters =
    {
        prvInterruptSemaphoreTask,              /* Task code */
        "CNT3",                                 /* Task name */
        &xCountSemaphoreTestTask3TCB,           /* TCB */
        acCountSemTestTask3Stack,               /* Stack buffer */
        portspecCOUNTSEM_TASK3_STACK_SIZE,      /* Stack depth bytes */
        ( void * ) &( xParameters[ 2 ] ),       /* Parameters */
        taskIDLE_PRIORITY,                      /* Priority */
        NULL,                                   /* TLS object */
        xCountSemaphoreTestTask3PortParameters  /* Port specific task parameters */
    };

    /* Check scheduler is NOT running before creating semaphores & tasks. */
    if( pdFALSE != xTaskIsSchedulerStarted() )
    {
        xStatus = pdFAIL;
    }

    /* Create the semaphores that we are going to use for the test demo.
     * - 1st is created such that it starts at its maximum count value
     * - 2nd is created such that it starts with a count value of zero.
     * - 3rd is created such that it starts with a count value of one.
     */
    if( pdPASS == xStatus )
    {
        xStatus = xSemaphoreCreateCounting( countMAX_COUNT_VALUE,
                                            countMAX_COUNT_VALUE,
                                            acCountSem1,
                                            ( xSemaphoreHandle * ) &( xParameters[ 0 ].xSemaphore ) );
    }

    if( ( xStatus == pdPASS ) && ( xParameters[ 0 ].xSemaphore != NULL ) )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        /* Add to Queue Registry */
        vQueueAddToRegistry( xParameters[ 0 ].xSemaphore, "CountingSemaphore #1" );
#endif

        xParameters[ 0 ].uxExpectedStartCount = countSTART_AT_MAX_COUNT;
        xParameters[ 0 ].uxLoopCounter = 0;

        /* Create 1st demo task, passing in semaphore to use as parameter. */
        xStatus = xTaskCreate( &xTestTask1Parameters, ( portTaskHandleType * ) NULL );
    }

    if( pdPASS == xStatus )
    {
        xStatus = xSemaphoreCreateCounting( countMAX_COUNT_VALUE,
                                            0,
                                            acCountSem2,
                                            ( xSemaphoreHandle * ) &( xParameters[ 1 ].xSemaphore ) );
    }

    if( ( xStatus == pdPASS ) && ( xParameters[ 1 ].xSemaphore != NULL ) )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        /* Add to Queue Registry. */
        vQueueAddToRegistry( xParameters[ 1 ].xSemaphore, "CountingSemaphore #2" );
#endif

        xParameters[ 1 ].uxExpectedStartCount = 0;
        xParameters[ 1 ].uxLoopCounter = 0;

        /* Create 2nd demo task, passing in semaphore to use as parameter. */
        xStatus = xTaskCreate( &xTestTask2Parameters, ( portTaskHandleType * ) NULL );
    }

    if( pdPASS == xStatus )
    {
        xStatus = xSemaphoreCreateCounting( 1, 1, acCountSem3,
                                            ( xSemaphoreHandle * ) &( xParameters[ 2 ].xSemaphore ) );
    }

    if( ( xStatus == pdPASS ) && ( xParameters[ 2 ].xSemaphore != NULL ) )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        /* Add to Queue Registry. */
        vQueueAddToRegistry( xParameters[ 2 ].xSemaphore, "CountingSemaphore #3" );
#endif

        xParameters[ 2 ].uxExpectedStartCount = 0;
        xParameters[ 2 ].uxLoopCounter = 0;

        /* Create 3rd demo task, passing in semaphore to use as parameter. */
        xStatus = xTaskCreate( &xTestTask3Parameters, &xTestTask3Handle );

        if( pdPASS == xStatus )
        {
            if( NULL == xTestTask3Handle )
            {
                xStatus = pdFAIL;
            }
        }
    }

    return xStatus;
}
/*---------------------------------------------------------------------------*/

static void prvDecrementSemaphoreCount( xSemaphoreHandle xSemaphore,
                                        portUnsignedBaseType *puxLoopCounter )
{
    portUnsignedBaseType uxCount;

    /* If the semaphore count is at its maximum then we should not be able to
     * 'give' the semaphore. */
    if( xSemaphoreGive( xSemaphore ) == pdPASS )
    {
        xCountsemErrorDetected = pdTRUE;
    }

    /* We should be able to 'take' the semaphore countMAX_COUNT_VALUE times. */
    for( uxCount = 0; uxCount < countMAX_COUNT_VALUE; uxCount++ )
    {
        if( xSemaphoreTake( xSemaphore, countDONT_BLOCK ) != pdPASS )
        {
            /* We expected to be able to take the semaphore. */
            xCountsemErrorDetected = pdTRUE;
        }

        ( *puxLoopCounter )++;
    }

    taskYIELD();

    /* If the semaphore count is zero then we should not be able to 'take'
     * the semaphore. */
    if( xSemaphoreTake( xSemaphore, countDONT_BLOCK ) == pdPASS )
    {
        xCountsemErrorDetected = pdTRUE;
    }
}
/*---------------------------------------------------------------------------*/

static void prvIncrementSemaphoreCount( xSemaphoreHandle xSemaphore,
                                        portUnsignedBaseType *puxLoopCounter )
{
    portUnsignedBaseType uxCount;
    portUnsignedBaseType uxReportedCount = 0UL;

    /* If the semaphore count is zero then we should not be able to 'take'
     * the semaphore. */
    if( xSemaphoreTake( xSemaphore, countDONT_BLOCK ) == pdPASS )
    {
        xCountsemErrorDetected = pdTRUE;
    }

    /* We should be able to 'give' the semaphore countMAX_COUNT_VALUE times. */
    for( uxCount = 0; uxCount < countMAX_COUNT_VALUE; uxCount++ )
    {
        if( xSemaphoreGive( xSemaphore ) != pdPASS )
        {
            /* We expected to be able to take the semaphore. */
            xCountsemErrorDetected = pdTRUE;
        }

        ( *puxLoopCounter )++;


        if( pdPASS != xSemaphoreGetCountDepth( xSemaphore, &uxReportedCount ) )
        {
            /* We expected to be able to take the semaphore. */
            xCountsemErrorDetected = pdTRUE;
        }

        if( ( uxCount + 1 ) != uxReportedCount )
        {
            /* We expected the count to be correct. */
            xCountsemErrorDetected = pdTRUE;
        }
    }

    taskYIELD();

    /* If the semaphore count is at its maximum then we should not be able to
     * 'give' the semaphore. */
    if( xSemaphoreGive( xSemaphore ) == pdPASS )
    {
        xCountsemErrorDetected = pdTRUE;
    }
}
/*---------------------------------------------------------------------------*/

static void prvCountingSemaphoreTask( void *pvParameters )
{
    xCountSemStruct *pxParameter;

    /* The semaphore to be used was passed as the parameter. */
    pxParameter = ( xCountSemStruct * ) pvParameters;

    /* Did we expect to find the semaphore already
     * at its max count value, or at zero? */
    if( pxParameter->uxExpectedStartCount == countSTART_AT_MAX_COUNT )
    {
        prvDecrementSemaphoreCount( pxParameter->xSemaphore,
                                    &( pxParameter->uxLoopCounter ) );
    }

    /* Now we expect the semaphore count to be 0,
     * so this time there is an error if we can take the semaphore. */
    if( xSemaphoreTake( pxParameter->xSemaphore, 0 ) == pdPASS )
    {
        xCountsemErrorDetected = pdTRUE;
    }

    for( ;; )
    {
        prvIncrementSemaphoreCount( pxParameter->xSemaphore,
                                    &( pxParameter->uxLoopCounter ) );

        prvDecrementSemaphoreCount( pxParameter->xSemaphore,
                                    &( pxParameter->uxLoopCounter ) );
    }
}
/*---------------------------------------------------------------------------*/

static void prvInterruptSemaphoreTask( void *pvParameters )
{
    portBaseType xStatus;
    xCountSemStruct *pxParameter;
#if ( portspecCOUNTSEM_INTERRUPT_TASK_RUNS_UNPRIVILEGED == 1 )
    mpuTaskParamType xMpuParameters = xCountSemaphoreTestTask3MpuUpdParameters;
#endif

    /* The semaphore to be used was passed as the parameter. */
    pxParameter = ( xCountSemStruct * ) pvParameters;

    taskENTER_CRITICAL();
    {
        /* Start Semaphore ISR test counter. */
        vCountSemTimerInit();
    }
    taskEXIT_CRITICAL();

#if ( portspecCOUNTSEM_INTERRUPT_TASK_RUNS_UNPRIVILEGED == 1 )
    /* Update MPU Regions to Access Task Variables & Timer Peripheral */
    xStatus = xMPUSetTaskRegions( xTestTask3Handle, &xMpuParameters );

    if( pdPASS != xStatus )
    {
        /* Log the error. */
        xCountsemErrorDetected = pdTRUE;
    }

    /* Now that the timer is initialised, switch to Unprivileged Mode. */
    vMPUTaskExecuteInUnprivilegedMode();
#endif

    for( ;; )
    {
        /* This attempt to take the semaphore should succeed immediately. */
        xStatus = xSemaphoreTake( pxParameter->xSemaphore, countDONT_BLOCK );

        if( pdPASS != xStatus )
        {
            /* Log the error. */
            xCountsemErrorDetected = pdTRUE;
        }
        else
        {
            /* Enable Semaphore ISR Test Counter Operations */
            if( countSEM_ISR_STATE_NULL == xSemaphoreISRState )
            {
                xSemaphoreISRState = countSEM_ISR_STATE_GIVE;
            }
        }

        /* This attempt to take the semaphore should block until the ISR next fires. */
        xStatus = xSemaphoreTake( pxParameter->xSemaphore, portMAX_DELAY );

        if( pdPASS != xStatus )
        {
            /* Log the error. */
            xCountsemErrorDetected = pdTRUE;
        }

        /* This attempt to give the semaphore should succeed immediately. */
        xStatus = xSemaphoreGive( pxParameter->xSemaphore );

        if( pdPASS != xStatus )
        {
            /* Log the error. */
            xCountsemErrorDetected = pdTRUE;
        }

        /* Since it's not possible to block while waiting to give a Semaphore,
         * delay long enough to allow the ISR to take the semaphore first. */
        if( xTaskDelay( countSEM_BLOCK_600_MS ) != pdPASS )
        {
            /* Log the error. */
            xCountsemErrorDetected = pdTRUE;
        }

        /* This attempt to give the semaphore will only succeed
         * if the Timer ISR occurred while this task was delayed. */
        xStatus = xSemaphoreGive( pxParameter->xSemaphore );

        if( pdPASS != xStatus )
        {
            /* Log the error. */
            xCountsemErrorDetected = pdTRUE;
        }

        taskENTER_CRITICAL();
        {
            /* Increment loop counter. */
            pxParameter->uxLoopCounter++;
        }
        taskEXIT_CRITICAL();
    }
}
/*---------------------------------------------------------------------------*/

portBaseType xAreCountingSemaphoreTasksStillRunning( void )
{
    portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType uxLastCount0 = 0;
    portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType uxLastCount1 = 0;
    portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType uxLastCount2 = 0;
    portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType uxLastSemaphoreISRCount = 0;

    portBaseType xReturn = pdPASS;

    /* Return fail if any 'give' or 'take' did not result in the expected
     * behaviour. */
    if( pdFALSE != xCountsemErrorDetected )
    {
        xReturn = pdFAIL;
    }

    /* Return fail if either task is not still incrementing its loop counter. */
    if( uxLastCount0 == xParameters[ 0 ].uxLoopCounter )
    {
        xReturn = pdFAIL;
    }
    else
    {
        uxLastCount0 = xParameters[ 0 ].uxLoopCounter;
    }

    if( uxLastCount1 == xParameters[ 1 ].uxLoopCounter )
    {
        xReturn = pdFAIL;
    }
    else
    {
        uxLastCount1 = xParameters[ 1 ].uxLoopCounter;
    }

    if( uxLastCount2 == xParameters[ 2 ].uxLoopCounter )
    {
        xReturn = pdFAIL;
    }
    else
    {
        uxLastCount2 = xParameters[ 2 ].uxLoopCounter;
    }

    taskENTER_CRITICAL();
    {
        if( uxLastSemaphoreISRCount == uxSemaphoreISRCount )
        {
            xReturn = pdFAIL;
        }
        else
        {
            uxLastSemaphoreISRCount = uxSemaphoreISRCount;
        }
    }
    taskEXIT_CRITICAL();

    return xReturn;
}
/*---------------------------------------------------------------------------*/
