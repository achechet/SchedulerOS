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
 * Creates six tasks that operate on three queues as follows:
 *
 * The first two tasks send and receive an incrementing number to/from a queue.
 * One task acts as a producer and the other as the consumer.  The consumer is a
 * higher priority than the producer and is set to block on queue reads.  The queue
 * only has space for one item - as soon as the producer posts a message on the
 * queue the consumer will unblock, pre-empt the producer, and remove the item.
 *
 * The second two tasks work the other way around.  Again the queue used only has
 * enough space for one item.  This time the consumer has a lower priority than the
 * producer.  The producer will try to post on the queue blocking when the queue is
 * full.  When the consumer wakes it will remove the item from the queue, causing
 * the producer to unblock, pre-empt the consumer, and immediately re-fill the
 * queue.
 *
 * The last two tasks use the same queue producer and consumer functions.  This time the queue has
 * enough space for lots of items and the tasks operate at the same priority.  The
 * producer will execute, placing items into the queue.  The consumer will start
 * executing when either the queue becomes full (causing the producer to block) or
 * a context switch occurs (tasks of the same priority will time slice).
 */

/* PortSpecifics.h includes some 'declare once' declarations. */
#define BLOCK_Q_C

#include <stdlib.h>

/* Scheduler include files. */
#include "SafeRTOS_API.h"


/* Demo program include files. */
#include "BlockQ.h"
#include "PortSpecifics.h"

#if ( configQUEUE_REGISTRY_SIZE > 0 )
    #include "queue_register.h"
#endif

#define blckqNUM_TASK_SETS      ( 3 )

/* Structure used to pass parameters to the blocking queue tasks. */
typedef struct BLOCKING_QUEUE_PARAMETERS
{
    xQueueHandle xQueue;                        /* The queue to be used by the task. */
    portTickType xBlockTime;                    /* The block time to use on queue reads/writes. */
    volatile portInt16Type *psCheckVariable;    /* Incremented on each successful cycle to check the task is still running. */
} xBlockingQueueParameters;

/* Queue Parameters. */
portspecBLOCK_Q_DATA_SECTION static xBlockingQueueParameters xQueueParameters1 = { NULL, 0, NULL };
portspecBLOCK_Q_DATA_SECTION static xBlockingQueueParameters xQueueParameters2 = { NULL, 0, NULL };
portspecBLOCK_Q_DATA_SECTION static xBlockingQueueParameters xQueueParameters3 = { NULL, 0, NULL };
portspecBLOCK_Q_DATA_SECTION static xBlockingQueueParameters xQueueParameters4 = { NULL, 0, NULL };
portspecBLOCK_Q_DATA_SECTION static xBlockingQueueParameters xQueueParameters5 = { NULL, 0, NULL };
portspecBLOCK_Q_DATA_SECTION static xBlockingQueueParameters xQueueParameters6 = { NULL, 0, NULL };

/* Task function that creates an incrementing number and posts it on a queue. */
static void vBlockingQueueProducer( void *pvParameters );

/* Task function that removes the incrementing number from a queue and checks
 * that it is the expected number. */
static void vBlockingQueueConsumer( void *pvParameters );

/* Variables which are incremented each time an item is removed from a queue,
 * and found to be the expected value.
 * These are used to check that the tasks are still running. */
portspecBLOCK_Q_DATA_SECTION static volatile portInt16Type asBlockingConsumerCount[ blckqNUM_TASK_SETS ] = { ( portInt16Type ) 0, ( portInt16Type ) 0, ( portInt16Type ) 0 };

/* Variable which are incremented each time an item is posted on a queue.
 * These are used to check that the tasks are still running. */
portspecBLOCK_Q_DATA_SECTION static volatile portInt16Type asBlockingProducerCount[ blckqNUM_TASK_SETS ] = { ( portInt16Type ) 0, ( portInt16Type ) 0, ( portInt16Type ) 0 };

/* Blocking Queue Task TCBs. */
portspecTCB_DATA_SECTION static xTCB xBlockingQueueConsumerTask1TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xBlockingQueueProducerTask1TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xBlockingQueueConsumerTask2TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xBlockingQueueProducerTask2TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xBlockingQueueProducerTask3TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xBlockingQueueConsumerTask3TCB = { 0 };

/*---------------------------------------------------------------------------*/

portBaseType xStartBlockingQueueTasks( portUnsignedBaseType uxPriority )
{
    const portTickType xBlockTime = ( portTickType ) 1000 / configTICK_RATE_MS;
    const portTickType xDontBlock = ( portTickType ) 0;
    portBaseType xStatus = pdPASS;
    xTaskParameters xBlockingQueueConsumerTask1Parameters =
    {
        vBlockingQueueConsumer,                     /* Task code */
        "QConsB1",                                  /* Task name */
        &xBlockingQueueConsumerTask1TCB,            /* TCB */
        acQueueTestTask1Stack,                      /* Stack buffer */
        portspecBLOCK_Q_STACK_SIZE,                 /* Stack depth bytes */
        ( void * ) &xQueueParameters1,              /* Parameters */
        taskIDLE_PRIORITY,                          /* Priority */
        NULL,                                       /* TLS object */
        xBlockingQueueConsumerTask1PortParameters   /* Port-specific task parameters */
    };
    xTaskParameters xBlockingQueueProducerTask1Parameters =
    {
        vBlockingQueueProducer,                     /* Task code */
        "QProdB2",                                  /* Task name */
        &xBlockingQueueProducerTask1TCB,            /* TCB */
        acQueueTestTask2Stack,                      /* Stack buffer */
        portspecBLOCK_Q_STACK_SIZE,                 /* Stack depth bytes */
        ( void * ) &xQueueParameters2,              /* Parameters */
        taskIDLE_PRIORITY,                          /* Priority */
        NULL,                                       /* TLS object */
        xBlockingQueueProducerTask1PortParameters   /* Port-specific task parameters */
    };
    xTaskParameters xBlockingQueueConsumerTask2Parameters =
    {
        vBlockingQueueConsumer,                     /* Task code */
        "QConsB3",                                  /* Task name */
        &xBlockingQueueConsumerTask2TCB,            /* TCB */
        acQueueTestTask3Stack,                      /* Stack buffer */
        portspecBLOCK_Q_STACK_SIZE,                 /* Stack depth bytes */
        ( void * ) &xQueueParameters3,              /* Parameters */
        taskIDLE_PRIORITY,                          /* Priority */
        NULL,                                       /* TLS object */
        xBlockingQueueConsumerTask2PortParameters   /* Port-specific task parameters */
    };
    xTaskParameters xBlockingQueueProducerTask2Parameters =
    {
        vBlockingQueueProducer,                     /* Task code */
        "QProdB4",                                  /* Task name */
        &xBlockingQueueProducerTask2TCB,            /* TCB */
        acQueueTestTask4Stack,                      /* Stack buffer */
        portspecBLOCK_Q_STACK_SIZE,                 /* Stack depth bytes */
        ( void * ) &xQueueParameters4,              /* Parameters */
        taskIDLE_PRIORITY,                          /* Priority */
        NULL,                                       /* TLS object */
        xBlockingQueueProducerTask2PortParameters   /* Port-specific task parameters */
    };
    xTaskParameters xBlockingQueueProducerTask3Parameters =
    {
        vBlockingQueueProducer,                     /* Task code */
        "QConsB5",                                  /* Task name */
        &xBlockingQueueProducerTask3TCB,            /* TCB */
        acQueueTestTask5Stack,                      /* Stack buffer */
        portspecBLOCK_Q_STACK_SIZE,                 /* Stack depth bytes */
        ( void * ) &xQueueParameters5,              /* Parameters */
        taskIDLE_PRIORITY,                          /* Priority */
        NULL,                                       /* TLS object */
        xBlockingQueueProducerTask3PortParameters   /* Port-specific task parameters */
    };
    xTaskParameters xBlockingQueueConsumerTask3Parameters =
    {
        vBlockingQueueConsumer,                     /* Task code */
        "QProdB6",                                  /* Task name */
        &xBlockingQueueConsumerTask3TCB,            /* TCB */
        acQueueTestTask6Stack,                      /* Stack buffer */
        portspecBLOCK_Q_STACK_SIZE,                 /* Stack depth bytes */
        ( void * ) &xQueueParameters6,              /* Parameters */
        taskIDLE_PRIORITY,                          /* Priority */
        NULL,                                       /* TLS object */
        xBlockingQueueConsumerTask3PortParameters   /* Port-specific task parameters */
    };


    /* Create the queue used by the first two tasks to pass the incrementing number.
     * Pass a pointer to the queue in the parameters to be used to create the consumer tasks. */
    if( xQueueCreate( acQueue1Buffer, portspecBLOCK_Q_BUFFER_LENGTH_1, portspecBLOCK_Q_QUEUE_LENGTH_1, portspecBLOCK_Q_QUEUE_ITEM_SIZE, &( xQueueParameters1.xQueue ) ) == pdPASS )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        vQueueAddToRegistry( xQueueParameters1.xQueue, "BlockQ #1" );
#endif
    }
    else
    {
        xStatus = pdFAIL;
    }

    /* The consumer is created first so gets a block time as described above. */
    xQueueParameters1.xBlockTime = xBlockTime;

    /* Pass in the variable that this task is going to increment so we can check it
     * is still running. */
    xQueueParameters1.psCheckVariable = &( asBlockingConsumerCount[ 0 ] );

    /* Pass the queue to the structure used to pass parameters to the producer task. */
    xQueueParameters2.xQueue = xQueueParameters1.xQueue;

    /* The producer is not going to block - as soon as it posts the consumer will
     * wake and remove the item so the producer should always have room to post. */
    xQueueParameters2.xBlockTime = xDontBlock;

    /* Pass in the variable that this task is going to increment so we can check
     * it is still running. */
    xQueueParameters2.psCheckVariable = &( asBlockingProducerCount[ 0 ] );

    /* Create the first two tasks as described at the top of the file. Note the
     * producer has a lower priority than the consumer when the tasks are spawned. */
    xBlockingQueueConsumerTask1Parameters.uxPriority = uxPriority;
    if( xTaskCreate( &xBlockingQueueConsumerTask1Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xBlockingQueueProducerTask1Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }

    /* Create the second two tasks as described at the top of the file.
     * This uses the same mechanism but reverses the task priorities. */

    if( xQueueCreate( acQueue2Buffer, portspecBLOCK_Q_BUFFER_LENGTH_1, portspecBLOCK_Q_QUEUE_LENGTH_1, portspecBLOCK_Q_QUEUE_ITEM_SIZE, &( xQueueParameters3.xQueue ) ) == pdPASS )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        vQueueAddToRegistry( xQueueParameters3.xQueue, "BlockQ #2" );
#endif
    }
    else
    {
        xStatus = pdFAIL;
    }

    xQueueParameters3.xBlockTime = xDontBlock;
    xQueueParameters3.psCheckVariable = &( asBlockingProducerCount[ 1 ] );

    xQueueParameters4.xQueue = xQueueParameters3.xQueue;
    xQueueParameters4.xBlockTime = xBlockTime;
    xQueueParameters4.psCheckVariable = &( asBlockingConsumerCount[ 1 ] );

    if( xTaskCreate( &xBlockingQueueConsumerTask2Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    xBlockingQueueProducerTask2Parameters.uxPriority = uxPriority;
    if( xTaskCreate( &xBlockingQueueProducerTask2Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }

    /* Create the last two tasks as described above. The mechanism is again just
     * the same. This time both parameter structures are given a block time. */
    if( xQueueCreate( acQueue3Buffer, portspecBLOCK_Q_BUFFER_LENGTH_5, portspecBLOCK_Q_QUEUE_LENGTH_5, portspecBLOCK_Q_QUEUE_ITEM_SIZE, &( xQueueParameters5.xQueue ) ) == pdPASS )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        vQueueAddToRegistry( xQueueParameters5.xQueue, "BlockQ #3" );
#endif
    }
    else
    {
        xStatus = pdFAIL;
    }

    xQueueParameters5.xBlockTime = xBlockTime;
    xQueueParameters5.psCheckVariable = &( asBlockingProducerCount[ 2 ] );

    xQueueParameters6.xQueue = xQueueParameters5.xQueue;
    xQueueParameters6.xBlockTime = xBlockTime;
    xQueueParameters6.psCheckVariable = &( asBlockingConsumerCount[ 2 ] );

    if( xTaskCreate( &xBlockingQueueProducerTask3Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xBlockingQueueConsumerTask3Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }

    return xStatus;
}
/*---------------------------------------------------------------------------*/

static void vBlockingQueueProducer( void *pvParameters )
{
    portUInt16Type usValue = 0;
    xBlockingQueueParameters *pxQueueParameters;
    portBaseType xErrorEverOccurred = pdFALSE;
    pxQueueParameters = ( xBlockingQueueParameters * ) pvParameters;

    for( ;; )
    {
        if( xQueueSend( pxQueueParameters->xQueue, ( void * ) &usValue, pxQueueParameters->xBlockTime ) != pdPASS )
        {
            xErrorEverOccurred = pdTRUE;
        }
        else
        {
            /* We have successfully posted a message, so increment the variable
             * used to check we are still running. */
            if( pdFALSE == xErrorEverOccurred )
            {
                ( *pxQueueParameters->psCheckVariable )++;
            }

            /* Increment the variable we are going to post next time round.
             * The consumer will expect the numbers to follow in numerical order. */
            ++usValue;
        }
    }
}
/*---------------------------------------------------------------------------*/

static void vBlockingQueueConsumer( void *pvParameters )
{
    portUInt16Type usData, usExpectedValue = 0;
    xBlockingQueueParameters *pxQueueParameters;
    portBaseType xErrorEverOccurred = pdFALSE;
    pxQueueParameters = ( xBlockingQueueParameters * ) pvParameters;

    for( ;; )
    {
        if( xQueueReceive( pxQueueParameters->xQueue, &usData, pxQueueParameters->xBlockTime ) == pdPASS )
        {
            if( usData != usExpectedValue )
            {
                /* Catch-up. */
                usExpectedValue = usData;

                xErrorEverOccurred = pdTRUE;
            }
            else
            {
                /* We have successfully received a message, so increment the
                 * variable used to check we are still running. */
                if( pdFALSE == xErrorEverOccurred )
                {
                    ( *pxQueueParameters->psCheckVariable )++;
                }

                /* Increment the value we expect to remove from the queue next
                 * time round. */
                ++usExpectedValue;
            }
        }
    }
}
/*---------------------------------------------------------------------------*/

/* This is called to check that all the created tasks are still running. */
portBaseType xAreBlockingQueuesStillRunning( void )
{
    portspecCOMMON_PRIV_DATA_SECTION static portInt16Type asLastBlockingConsumerCount[ blckqNUM_TASK_SETS ] = { ( portInt16Type ) 0, ( portInt16Type ) 0, ( portInt16Type ) 0 };
    portspecCOMMON_PRIV_DATA_SECTION static portInt16Type asLastBlockingProducerCount[ blckqNUM_TASK_SETS ] = { ( portInt16Type ) 0, ( portInt16Type ) 0, ( portInt16Type ) 0 };
    portBaseType xReturn = pdPASS, xTasks;

    /* Not too worried about mutual exclusion on these variables as they are 16
     * bits and we are only reading them. We also only care to see if they have
     * changed or not.
     *
     * Loop through each check variable to and return pdFALSE if any are found
     * not to have changed since the last call. */

    for( xTasks = 0; xTasks < blckqNUM_TASK_SETS; xTasks++ )
    {
        if( asBlockingConsumerCount[ xTasks ] == asLastBlockingConsumerCount[ xTasks ] )
        {
            xReturn = pdFALSE;
        }
        asLastBlockingConsumerCount[ xTasks ] = asBlockingConsumerCount[ xTasks ];


        if( asBlockingProducerCount[ xTasks ] == asLastBlockingProducerCount[ xTasks ] )
        {
            xReturn = pdFALSE;
        }
        asLastBlockingProducerCount[ xTasks ] = asBlockingProducerCount[ xTasks ];
    }

    return xReturn;
}
/*---------------------------------------------------------------------------*/
