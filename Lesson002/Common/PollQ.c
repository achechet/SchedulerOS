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
 * Creates two tasks that communicate over a single queue.  One task acts as a
 * producer, the other a consumer.
 *
 * The producer loops for three iteration, posting an incrementing number onto the
 * queue each cycle.  It then delays for a fixed period before doing exactly the
 * same again.
 *
 * The consumer loops emptying the queue.  Each item removed from the queue is
 * checked to ensure it contains the expected value.  When the queue is empty it
 * blocks for a fixed period, then does the same again.
 *
 * All queue access is performed without blocking.  The consumer completely empties
 * the queue each time it runs so the producer should never find the queue full.
 *
 * An error is flagged if the consumer obtains an unexpected value or the producer
 * find the queue is full.
 */

/* PortSpecifics.h includes some 'declare once' declarations. */
#define POLLED_Q_C

#include <stdlib.h>

/* Scheduler include files. */
#include "SafeRTOS_API.h"


/* Demo program include files. */
#include "PollQ.h"
#include "PortSpecifics.h"

#if ( configQUEUE_REGISTRY_SIZE > 0 )
    #include "queue_register.h"
#endif

#define pollqPRODUCER_DELAY         ( ( portTickType ) 200U / configTICK_RATE_MS )
#define pollqCONSUMER_DELAY         ( pollqPRODUCER_DELAY - ( portTickType ) ( 20U / configTICK_RATE_MS ) )
#define pollqNO_DELAY               ( ( portTickType ) 0U )
#define pollqVALUES_TO_PRODUCE      ( 3U )
#define pollqNUM_SEND_TO_FRONT      ( portspecPOLL_Q_QUEUE_LENGTH )
#define pollqINITIAL_VALUE          ( 0U )
#define pollqNO_MESSAGES_WAITING    ( 0U )

/* The task that posts the incrementing number onto the queue. */
static void vPolledQueueProducer( void *pvParameters );

/* The task that empties the queue. */
static void vPolledQueueConsumer( void *pvParameters );

/* The task that sends to the front of the queue and reads back. */
static void vPolledQueueSendFront( void *pvParameters );

/* Variables that are used to check that the tasks are still running with no
 * errors. */
portspecPOLL_Q_DATA_SECTION static volatile portUnsignedBaseType uxPollingConsumerCount = pollqINITIAL_VALUE;
portspecPOLL_Q_DATA_SECTION static volatile portUnsignedBaseType uxPollingProducerCount = pollqINITIAL_VALUE;
portspecPOLL_Q_DATA_SECTION static volatile portUnsignedBaseType uxPollingSendFrontCount = pollqINITIAL_VALUE;

/* Poll Queue Task TCBs. */
portspecTCB_DATA_SECTION static xTCB xPolledQueueConsumerTaskTCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xPolledQueueProducerTaskTCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xPolledQueueSendFrontTaskTCB = { 0 };

portspecPOLL_Q_DATA_SECTION static xQueueHandle xPolledQueue1 = NULL;
portspecPOLL_Q_DATA_SECTION static xQueueHandle xPolledQueue2 = NULL;

/*---------------------------------------------------------------------------*/

portBaseType xStartPolledQueueTasks( portUnsignedBaseType uxPriority )
{
    portBaseType xStatus = pdPASS;
    xTaskParameters xPolledQueueConsumerTaskParameters =
    {
        vPolledQueueConsumer,                   /* Task code */
        "QConsNB",                              /* Task name */
        &xPolledQueueConsumerTaskTCB,           /* TCB */
        acPollQueueTestTask1Stack,              /* Stack buffer */
        portspecPOLL_Q_STACK_SIZE,              /* Stack depth bytes */
        ( void * ) &xPolledQueue1,              /* Parameters */
        taskIDLE_PRIORITY,                      /* Priority */
        NULL,                                   /* TLS object */
        xPolledQueueConsumerTaskPortParameters  /* Port-specific task parameters */
    };
    xTaskParameters xPolledQueueProducerTaskParameters =
    {
        vPolledQueueProducer,                   /* Task code */
        "QProdNB",                              /* Task name */
        &xPolledQueueProducerTaskTCB,           /* TCB */
        acPollQueueTestTask2Stack,              /* Stack buffer */
        portspecPOLL_Q_STACK_SIZE,              /* Stack depth bytes */
        ( void * ) &xPolledQueue1,              /* Parameters */
        taskIDLE_PRIORITY,                      /* Priority */
        NULL,                                   /* TLS object */
        xPolledQueueProducerTaskPortParameters  /* Port-specific task parameters */
    };
    xTaskParameters xPolledQueueSendFrontTaskParameters =
    {
        vPolledQueueSendFront,                  /* Task code */
        "QSendFront",                           /* Task name */
        &xPolledQueueSendFrontTaskTCB,          /* TCB */
        acPollQueueTestTask3Stack,              /* Stack buffer */
        portspecPOLL_Q_STACK_SIZE,              /* Stack depth bytes */
        ( void * ) &xPolledQueue2,              /* Parameters */
        taskIDLE_PRIORITY,                      /* Priority */
        NULL,                                   /* TLS object */
        xPolledQueueSendFrontTaskPortParameters /* Port-specific task parameters */
    };


    /* Create the queue used by the producer and consumer. */
    if( xQueueCreate( acPollQueue1Buffer, portspecPOLL_Q_BUFFER_LENGTH, portspecPOLL_Q_QUEUE_LENGTH, portspecPOLL_Q_QUEUE_ITEM_SIZE, &xPolledQueue1 ) == pdPASS )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        vQueueAddToRegistry( xPolledQueue1, "PollQ" );
#endif
    }
    else
    {
        xStatus = pdFAIL;
    }

    /* Create the queue used by the producer and consumer. */
    if( xQueueCreate( acPollQueue2Buffer, portspecPOLL_Q_BUFFER_LENGTH, portspecPOLL_Q_QUEUE_LENGTH, portspecPOLL_Q_QUEUE_ITEM_SIZE, &xPolledQueue2 ) == pdPASS )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        vQueueAddToRegistry( xPolledQueue2, "SendFrontQ" );
#endif
    }
    else
    {
        xStatus = pdFAIL;
    }

    /* Spawn the producer and consumer. */
    xPolledQueueConsumerTaskParameters.uxPriority = uxPriority;
    if( xTaskCreate( &xPolledQueueConsumerTaskParameters, ( portTaskHandleType * ) NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    xPolledQueueProducerTaskParameters.uxPriority = uxPriority;
    if( xTaskCreate( &xPolledQueueProducerTaskParameters, ( portTaskHandleType * ) NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    xPolledQueueSendFrontTaskParameters.uxPriority = uxPriority;
    if( xTaskCreate( &xPolledQueueSendFrontTaskParameters, ( portTaskHandleType * ) NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }

    return xStatus;
}
/*---------------------------------------------------------------------------*/

static void vPolledQueueProducer( void *pvParameters )
{
    portUInt16Type usValue = ( portUInt16Type ) 0U;
    portBaseType xError = pdFALSE;
    portUnsignedBaseType uxLoop;

    for( ;; )
    {
        for( uxLoop = 0; uxLoop < pollqVALUES_TO_PRODUCE; uxLoop++ )
        {
            /* Send an incrementing number on the queue without blocking. */
            if( xQueueSend( *( ( xQueueHandle * ) pvParameters ), ( void * ) &usValue, pollqNO_DELAY ) != pdPASS )
            {
                /* We should never find the queue full so if we get here there
                 * has been an error. */
                xError = pdTRUE;
            }
            else
            {
                if( pdFALSE == xError )
                {
                    /* If an error has ever been recorded we stop incrementing
                     * the check variable. */
                    taskENTER_CRITICAL();
                    {
                        uxPollingProducerCount++;
                    }
                    taskEXIT_CRITICAL();
                }

                /* Update the value we are going to post next time around. */
                usValue++;
            }
        }

        /* Wait before we start posting again to ensure the consumer runs and
         * empties the queue. */
        if( xTaskDelay( pollqPRODUCER_DELAY ) != pdPASS )
        {
            xError = pdTRUE;
        }
    }
}
/*---------------------------------------------------------------------------*/

static void vPolledQueueConsumer( void *pvParameters )
{
    portUInt16Type usData, usExpectedValue = ( portUInt16Type ) 0U;
    portBaseType xNumberOfMessagesAvailable;
    portUnsignedBaseType uxNumberOfMessagesWaiting = pollqNO_MESSAGES_WAITING;
    portBaseType xError = pdFALSE;
    portBaseType xState = pdFALSE;

    for( ;; )
    {
        /* Loop until the queue is empty. */
        xNumberOfMessagesAvailable = xQueueMessagesWaiting( *( ( xQueueHandle * ) pvParameters ), &uxNumberOfMessagesWaiting );

        while( ( pdPASS == xNumberOfMessagesAvailable ) && ( uxNumberOfMessagesWaiting > pollqNO_MESSAGES_WAITING ) )
        {
            if( xQueueReceive( *( ( xQueueHandle * ) pvParameters ), &usData, pollqNO_DELAY ) == pdPASS )
            {
                if( usData != usExpectedValue )
                {
                    /* This is not what we expected to receive so an error has
                     * occurred. */
                    xError = pdTRUE;

                    /* Catch-up to the value we received so our next expected
                     * value should again be correct. */
                    usExpectedValue = usData;
                }
                else
                {
                    if( pdFALSE == xError )
                    {
                        /* Only increment the check variable if no errors have
                         * occurred. */
                        taskENTER_CRITICAL();
                        {
                            uxPollingConsumerCount++;
                        }
                        taskEXIT_CRITICAL();
                    }
                }

                /* Next time round we would expect the number to be one higher. */
                usExpectedValue++;
            }

            xNumberOfMessagesAvailable = xQueueMessagesWaiting( *( ( xQueueHandle * ) pvParameters ), &uxNumberOfMessagesWaiting );

            if( uxNumberOfMessagesWaiting > 0 )
            {
                xState = xQueuePeek( *( ( xQueueHandle * ) pvParameters ), &usData, pollqNO_DELAY );

                if( xState != pdPASS )
                {
                    /* We were unable to determine entry in the queue. */
                    xError = pdTRUE;
                }
            }
        }

        if( pdPASS != xNumberOfMessagesAvailable )
        {
            /* We were unable to determine the number of messages in the queue. */
            xError = pdTRUE;
        }

        /* Now the queue is empty we block, allowing the producer to place more
         * items in the queue. */
        if( xTaskDelay( pollqCONSUMER_DELAY ) != pdPASS )
        {
            xError = pdTRUE;
        }
    }
}
/*---------------------------------------------------------------------------*/

static void vPolledQueueSendFront( void *pvParameters )
{
    portUInt16Type usTxValue = ( portUInt16Type ) 0U;
    portUInt16Type usRxValue = ( portUInt16Type ) 0U;
    portBaseType xError = pdFALSE;
    portUnsignedBaseType uxLoop;

    ( void ) pvParameters;

    for( ;; )
    {
        /* Send an incrementing number on the queue without blocking. */
        for( uxLoop = 0; uxLoop < pollqNUM_SEND_TO_FRONT; uxLoop++ )
        {
            /* Update the value we are going to post.
             * NOTE: we increment before sending, so it is easier to compare it
             * with the received value. */
            usTxValue++;

            if( xQueueSendToFront( *( ( xQueueHandle * ) pvParameters ), ( void * ) &usTxValue, pollqNO_DELAY ) != pdPASS )
            {
                /* We should never find the queue full so if we get here there
                 * has been an error. */
                xError = pdTRUE;
            }
            else
            {
                if( pdFALSE == xError )
                {
                    /* If an error has ever been recorded we stop incrementing
                     * the check variable. */
                    taskENTER_CRITICAL();
                    {
                        uxPollingSendFrontCount++;
                    }
                    taskEXIT_CRITICAL();
                }
            }
        }

        /* Receive the values just sent. */
        for( uxLoop = 0U; uxLoop < pollqNUM_SEND_TO_FRONT; uxLoop++ )
        {
            /* Send an incrementing number on the queue without blocking. */
            if( xQueueReceive( *( ( xQueueHandle * ) pvParameters ), ( void * ) &usRxValue, pollqNO_DELAY ) != pdPASS )
            {
                /* We should never find the queue full so if we get here there
                 * has been an error. */
                xError = pdTRUE;
            }
            else if( usRxValue != ( ( portUInt16Type )( usTxValue - uxLoop ) ) )
            {
                /* We expect to get the last pollqNUM_SEND_TO_FRONT values in LIFO order. */
                xError = pdTRUE;
            }
            else
            {
                if( pdFALSE == xError )
                {
                    /* If an error has ever been recorded we stop incrementing
                     * the check variable. */
                    taskENTER_CRITICAL();
                    {
                        uxPollingSendFrontCount++;
                    }
                    taskEXIT_CRITICAL();
                }
            }
        }

        /* Wait before we start posting again to ensure the other two tasks run. */
        if( xTaskDelay( pollqPRODUCER_DELAY ) != pdPASS )
        {
            xError = pdTRUE;
        }
    }
}
/*---------------------------------------------------------------------------*/

/* This is called to check that all the created tasks are still running with no errors. */
portBaseType xArePollingQueuesStillRunning( void )
{
    portBaseType xReturn;

    /* Check both the consumer and producer poll count to check they have both
     * been changed since out last trip round.  We do not need a critical
     * section around the check variables as this is called from a higher
     * priority than the other tasks that access the same variables. */
    if( ( uxPollingConsumerCount == pollqINITIAL_VALUE ) ||
            ( uxPollingProducerCount == pollqINITIAL_VALUE ) ||
            ( uxPollingSendFrontCount == pollqINITIAL_VALUE )
      )
    {
        xReturn = pdFALSE;
    }
    else
    {
        xReturn = pdTRUE;
    }

    /* Set the check variables back down so we know if they have been
     * incremented the next time around. */
    uxPollingConsumerCount = pollqINITIAL_VALUE;
    uxPollingProducerCount = pollqINITIAL_VALUE;
    uxPollingSendFrontCount = pollqINITIAL_VALUE;

    return xReturn;
}
