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
 * This file contains some test scenarios that ensure tasks do not exit queue
 * send or receive functions prematurely.  A description of the tests is
 * included within the code.
 */

/* PortSpecifics.h includes some 'declare once' declarations. */
#define BLOCK_TIME_TEST_C

/* Kernel includes. */
#include "SafeRTOS_API.h"


/* Demo includes. */
#include "blocktim.h"
#include "PortSpecifics.h"

#if ( configQUEUE_REGISTRY_SIZE > 0 )
    #include "queue_register.h"
#endif

/* Task priorities. */
#define bktPRIMARY_PRIORITY         ( 3 )
#define bktSECONDARY_PRIORITY       ( 2 )

/* Task behaviour. */
#define bktSHORT_WAIT               ( ( ( portTickType ) 20 ) / configTICK_RATE_MS )
#define bktPRIMARY_BLOCK_TIME		( ( portTickType ) 10 )
#define bktALLOWABLE_MARGIN         ( 15 )
#define bktTIME_TO_BLOCK            ( 175 )
#define bktDONT_BLOCK               ( ( portTickType ) 0 )
#define bktRUN_INDICATOR            ( ( portUnsignedBaseType ) 0x55 )


/* The queue on which the tasks block. */
portspecBLOCK_TIME_TEST_DATA_SECTION static xQueueHandle xTestQueue = NULL;

/* Handle to the secondary task is required by the primary task for calls
to vTaskSuspend/Resume(). */
portspecBLOCK_TIME_TEST_DATA_SECTION static portTaskHandleType xSecondary = NULL;

/* Used to ensure that tasks are still executing without error. */
portspecBLOCK_TIME_TEST_DATA_SECTION static volatile portBaseType xPrimaryCycles = 0;
portspecBLOCK_TIME_TEST_DATA_SECTION static volatile portBaseType xSecondaryCycles = 0;
portspecBLOCK_TIME_TEST_DATA_SECTION static volatile portBaseType xErrorOccurred = pdFALSE;

/* Provides a simple mechanism for the primary task to know when the
secondary task has executed. */
portspecBLOCK_TIME_TEST_DATA_SECTION static volatile portUnsignedBaseType uxRunIndicator = 0U;

/* The two test tasks.  Their behaviour is commented within the files. */
static void vPrimaryBlockTimeTestTask( void *pvParameters );
static void vSecondaryBlockTimeTestTask( void *pvParameters );

/* Task TCBs. */
portspecTCB_DATA_SECTION static xTCB xPrimaryBlockTimeTestTaskTCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xSecondaryBlockTimeTestTaskTCB = { 0 };

/*---------------------------------------------------------------------------*/

portBaseType xCreateBlockTimeTasks( void )
{
    portBaseType xStatus = pdPASS;
    xTaskParameters xPrimaryBlockTimeTestTaskParameters =
    {
        vPrimaryBlockTimeTestTask,                  /* Task code */
        "BTest1",                                   /* Task name */
        &xPrimaryBlockTimeTestTaskTCB,              /* TCB */
        acBlockTimeTestTask1Stack,                  /* Stack buffer */
        portspecBLOCK_TIME_STACK_SIZE,              /* Stack depth bytes */
        NULL,                                       /* Parameters */
        bktPRIMARY_PRIORITY,                        /* Priority */
        NULL,                                       /* TLS object */
        xPrimaryBlockTimeTestTaskPortParameters     /* Port-specific task parameters */
    };
    xTaskParameters xSecondaryBlockTimeTestTaskParameters =
    {
        vSecondaryBlockTimeTestTask,                /* Task code */
        "BTest2",                                   /* Task name */
        &xSecondaryBlockTimeTestTaskTCB,            /* TCB */
        acBlockTimeTestTask2Stack,                  /* Stack buffer */
        portspecBLOCK_TIME_STACK_SIZE,              /* Stack depth bytes */
        NULL,                                       /* Parameters */
        bktSECONDARY_PRIORITY,                      /* Priority */
        NULL,                                       /* TLS object */
        xSecondaryBlockTimeTestTaskPortParameters   /* Port-specific task parameters */
    };

    /* Create the queue on which the two tasks block. */
    if( xQueueCreate( acQueueBuffer, portspecBLOCK_TIME_BUFFER_LENGTH, portspecBLOCK_TIME_QUEUE_LENGTH, portspecBLOCK_TIME_QUEUE_ITEM_SIZE, &xTestQueue ) == pdPASS )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        vQueueAddToRegistry( xTestQueue, "BlockTimeQ" );
#endif
    }
    else
    {
        xStatus = pdFAIL;
    }

    /* Create the two test tasks. */
    if( xTaskCreate( &xPrimaryBlockTimeTestTaskParameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xSecondaryBlockTimeTestTaskParameters, &xSecondary ) != pdPASS )
    {
        xStatus = pdFAIL;
    }

    return xStatus;
}
/*---------------------------------------------------------------------------*/

static void vPrimaryBlockTimeTestTask( void *pvParameters )
{
    portBaseType xItem, xData;
    portTickType xTimeWhenBlocking;
    portTickType xTimeToBlock, xBlockedTime;

    ( void ) pvParameters;

    for( ;; )
    {
        /**********************************************************************
         * Test 1
         * Simple block time wakeup test on queue receives.
         *********************************************************************/
        for( xItem = 0; xItem < portspecBLOCK_TIME_QUEUE_LENGTH; xItem++ )
        {
            /* The queue is empty. Attempt to read from the queue using a block
             * time. When we wake, ensure the delta in time is as expected. */
            xTimeToBlock = bktPRIMARY_BLOCK_TIME << xItem;

            xTimeWhenBlocking = xTaskGetTickCount();

            /* We should unblock after xTimeToBlock having not received
             * anything on the queue. */
            if( xQueueReceive( xTestQueue, &xData, xTimeToBlock ) != errQUEUE_EMPTY )
            {
                xErrorOccurred = pdTRUE;
            }

            /* How long were we blocked for? */
            xBlockedTime = xTaskGetTickCount() - xTimeWhenBlocking;

            if( xBlockedTime < xTimeToBlock )
            {
                /* Should not have blocked for less than we requested. */
                xErrorOccurred = pdTRUE;
            }

            if( xBlockedTime > ( xTimeToBlock + bktALLOWABLE_MARGIN ) )
            {
                /* Should not have blocked for longer than we requested,
                 * although we would not necessarily run as soon as we were
                 * unblocked so a margin is allowed. */
                xErrorOccurred = pdTRUE;
            }
        }

        /**********************************************************************
         * Test 2
         * Simple block time wakeup test on queue sends.
         * First fill the queue. It should be empty so all sends should pass.
         *********************************************************************/
        for( xItem = 0; xItem < portspecBLOCK_TIME_QUEUE_LENGTH; xItem++ )
        {
            if( xQueueSend( xTestQueue, &xItem, bktDONT_BLOCK ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            taskYIELD();
        }

        for( xItem = 0; xItem < portspecBLOCK_TIME_QUEUE_LENGTH; xItem++ )
        {
            /* The queue is full. Attempt to write to the queue using a block
             * time. When we wake, ensure the delta in time is as expected. */
            xTimeToBlock = bktPRIMARY_BLOCK_TIME << xItem;

            xTimeWhenBlocking = xTaskGetTickCount();

            /* We should unblock after xTimeToBlock having not received
             * anything on the queue. */
            if( xQueueSend( xTestQueue, &xItem, xTimeToBlock ) != errQUEUE_FULL )
            {
                xErrorOccurred = pdTRUE;
            }

            /* How long were we blocked for? */
            xBlockedTime = xTaskGetTickCount() - xTimeWhenBlocking;

            if( xBlockedTime < xTimeToBlock )
            {
                /* Should not have blocked for less than we requested. */
                xErrorOccurred = pdTRUE;
            }

            if( xBlockedTime > ( xTimeToBlock + bktALLOWABLE_MARGIN ) )
            {
                /* Should not have blocked for longer than we requested,
                 * although we would not necessarily run as soon as we were
                 * unblocked so a margin is allowed. */
                xErrorOccurred = pdTRUE;
            }
        }

        /**********************************************************************
         * Test 3
         * Wake the other task, it will block attempting to post to the queue.
         * When we read from the queue the other task will wake, but before it
         * can run we will post to the queue again. When the other task runs it
         * will find the queue still full, even though it was woken. It should
         * recognise that its block time has not expired and return to block
         * for the remains of its block time.
         * Wake the other task so it blocks attempting to post to the already
         * full queue.
         *********************************************************************/
        uxRunIndicator = 0;
        if( xTaskResume( xSecondary ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        /* We need to wait a little to ensure the other task executes. */
        while( uxRunIndicator != bktRUN_INDICATOR )
        {
            /* The other task has not yet executed. */
            if( xTaskDelay( bktSHORT_WAIT ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }
        }
        /* Make sure the other task is blocked on the queue. */
        if( xTaskDelay( bktSHORT_WAIT ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }
        uxRunIndicator = 0;

        for( xItem = 0; xItem < portspecBLOCK_TIME_QUEUE_LENGTH; xItem++ )
        {
            /* Now when we make space on the queue the other task should wake
             * but not execute as this task has higher priority. */
            if( xQueueReceive( xTestQueue, &xData, bktDONT_BLOCK ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            /* Now fill the queue again before the other task gets a chance to
             * execute. If the other task had executed we would find the queue
             * full ourselves, and the other task have set uxRunIndicator. */
            if( xQueueSend( xTestQueue, &xItem, bktDONT_BLOCK ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            if( bktRUN_INDICATOR == uxRunIndicator )
            {
                /* The other task should not have executed. */
                xErrorOccurred = pdTRUE;
            }

            /* Raise the priority of the other task so it executes and blocks
             * on the queue again. */
            if( xTaskPrioritySet( xSecondary, bktPRIMARY_PRIORITY + 2 ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            /* The other task should now have re-blocked without exiting the
             * queue function. */
            if( bktRUN_INDICATOR == uxRunIndicator )
            {
                /* The other task should not have executed outside of the
                 * queue function. */
                xErrorOccurred = pdTRUE;
            }

            /* Set the priority back down. */
            if( xTaskPrioritySet( xSecondary, bktSECONDARY_PRIORITY ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }
        }

        /* Let the other task timeout. When it unblockes it will check that it
         * unblocked at the correct time, then suspend itself. */
        while( uxRunIndicator != bktRUN_INDICATOR )
        {
            if( xTaskDelay( bktSHORT_WAIT ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }
        }
        if( xTaskDelay( bktSHORT_WAIT ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }
        uxRunIndicator = 0;

        /**********************************************************************
         * Test 4
         * As per test 3 - but with the send and receive the other way around.
         * The other task blocks attempting to read from the queue.
         * Empty the queue. We should find that it is full.
         *********************************************************************/
        for( xItem = 0; xItem < portspecBLOCK_TIME_QUEUE_LENGTH; xItem++ )
        {
            if( xQueueReceive( xTestQueue, &xData, bktDONT_BLOCK ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }
        }

        /* Wake the other task so it blocks attempting to read from the already
         * empty queue. */
        if( xTaskResume( xSecondary ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }


        /* We need to wait a little to ensure the other task executes. */
        while( uxRunIndicator != bktRUN_INDICATOR )
        {
            if( xTaskDelay( bktSHORT_WAIT ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }
        }
        if( xTaskDelay( bktSHORT_WAIT ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }
        uxRunIndicator = 0;

        for( xItem = 0; xItem < portspecBLOCK_TIME_QUEUE_LENGTH; xItem++ )
        {
            /* Now when we place an item on the queue the other task should
             * wake but not execute as this task has higher priority. */
            if( xQueueSend( xTestQueue, &xItem, bktDONT_BLOCK ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            /* Now empty the queue again before the other task gets a chance to
             * execute. If the other task had executed we would find the queue
             * empty ourselves, and the other task would be suspended. */
            if( xQueueReceive( xTestQueue, &xData, bktDONT_BLOCK ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            if( bktRUN_INDICATOR == uxRunIndicator )
            {
                /* The other task should not have executed. */
                xErrorOccurred = pdTRUE;
            }

            /* Raise the priority of the other task so it executes and blocks
             * on the queue again. */
            if( xTaskPrioritySet( xSecondary, bktPRIMARY_PRIORITY + 2 ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            /* The other task should now have re-blocked without exiting the
             * queue function. */
            if( bktRUN_INDICATOR == uxRunIndicator )
            {
                /* The other task should not have executed outside of the queue
                 * function. */
                xErrorOccurred = pdTRUE;
            }
            if( xTaskPrioritySet( xSecondary, bktSECONDARY_PRIORITY ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }
        }

        /* Let the other task timeout. When it unblockes it will check that it
         * unblocked at the correct time, then suspend itself. */
        while( uxRunIndicator != bktRUN_INDICATOR )
        {
            if( xTaskDelay( bktSHORT_WAIT ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }
        }
        if( xTaskDelay( bktSHORT_WAIT ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        xPrimaryCycles++;
    }
}
/*---------------------------------------------------------------------------*/

static void vSecondaryBlockTimeTestTask( void *pvParameters )
{
    portTickType xTimeWhenBlocking, xBlockedTime;
    portBaseType xData;

    ( void ) pvParameters;

    for( ;; )
    {
        /**********************************************************************
         * Test 1 and 2
         * This task does does not participate in these tests.
         *********************************************************************/
        if( xTaskSuspend( NULL ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        /**********************************************************************
         * Test 3
         * The first thing we do is attempt to read from the queue. It should be
         * full so we block. Note the time before we block so we can check the
         * wake time is as per that expected.
         *********************************************************************/
        xTimeWhenBlocking = xTaskGetTickCount();

        /* We should unblock after bktTIME_TO_BLOCK having not received
         * anything on the queue. */
        xData = 0;
        uxRunIndicator = bktRUN_INDICATOR;
        if( xQueueSend( xTestQueue, &xData, bktTIME_TO_BLOCK ) != errQUEUE_FULL )
        {
            xErrorOccurred = pdTRUE;
        }

        /* How long were we inside the send function? */
        xBlockedTime = xTaskGetTickCount() - xTimeWhenBlocking;

        /* We should not have blocked for less time than bktTIME_TO_BLOCK. */
        if( xBlockedTime < bktTIME_TO_BLOCK )
        {
            xErrorOccurred = pdTRUE;
        }

        /* We should of not blocked for much longer than bktALLOWABLE_MARGIN
         * either. A margin is permitted as we would not necessarily run as
         * soon as we unblocked. */
        if( xBlockedTime > ( bktTIME_TO_BLOCK + bktALLOWABLE_MARGIN ) )
        {
            xErrorOccurred = pdTRUE;
        }

        /* Suspend ready for test 3. */
        uxRunIndicator = bktRUN_INDICATOR;
        if( xTaskSuspend( NULL ) != pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        /**********************************************************************
         * Test 4
         * As per test three, but with the send and receive reversed.
         *********************************************************************/
        xTimeWhenBlocking = xTaskGetTickCount();

        /* We should unblock after bktTIME_TO_BLOCK having not received
         * anything on the queue. */
        uxRunIndicator = bktRUN_INDICATOR;
        if( xQueueReceive( xTestQueue, &xData, bktTIME_TO_BLOCK ) != errQUEUE_EMPTY )
        {
            xErrorOccurred = pdTRUE;
        }

        xBlockedTime = xTaskGetTickCount() - xTimeWhenBlocking;

        /* We should not have blocked for less time than bktTIME_TO_BLOCK. */
        if( xBlockedTime < bktTIME_TO_BLOCK )
        {
            xErrorOccurred = pdTRUE;
        }

        /* We should of not blocked for much longer than bktALLOWABLE_MARGIN
         * either. A margin is permitted as we would not necessarily run as soon
         * as we unblocked. */
        if( xBlockedTime > ( bktTIME_TO_BLOCK + bktALLOWABLE_MARGIN ) )
        {
            xErrorOccurred = pdTRUE;
        }

        uxRunIndicator = bktRUN_INDICATOR;

        xSecondaryCycles++;
    }
}
/*---------------------------------------------------------------------------*/

portBaseType xAreBlockTimeTestTasksStillRunning( void )
{
    portspecCOMMON_PRIV_DATA_SECTION static portBaseType xLastPrimaryCycleCount = 0, xLastSecondaryCycleCount = 0;
    portBaseType xReturn = pdPASS;

    /* Have both tasks performed at least one cycle since this function was
     * last called? */
    if( xPrimaryCycles == xLastPrimaryCycleCount )
    {
        xReturn = pdFAIL;
    }

    if( xSecondaryCycles == xLastSecondaryCycleCount )
    {
        xReturn = pdFAIL;
    }

    if( pdFALSE != xErrorOccurred )
    {
        xReturn = pdFAIL;
    }

    xLastSecondaryCycleCount = xSecondaryCycles;
    xLastPrimaryCycleCount = xPrimaryCycles;

    return xReturn;
}
/*---------------------------------------------------------------------------*/
