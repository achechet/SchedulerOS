/*
    Copyright (C)2006 onward WITTENSTEIN aerospace & simulation limited. All rights reserved.

    This file is part of the SafeRTOS product, see projdefs.h for version number
    information.

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
 * Tests the behaviour of direct task notifications.
 */

/* PortSpecifics.h may include some 'declare once' declarations. */
#define TASKNOTIFYDEMO_C

/* Library includes. */
#include <limits.h>

/* Scheduler include files. */
#include "SafeRTOS_API.h"

/* Demo program include files. */
#include "PortSpecifics.h"
#include "TaskNotify.h"

/*---------------------------------------------------------------------------*/

#define tmrdemoDONT_BLOCK       ( ( portTickType ) 0 )

#define notifyMAX_VALUE         ( ~( ( portUnsignedBaseType ) 0U ) )

#define notifyBIT_0             ( ( portUnsignedBaseType ) 0x01 )
#define notifyBIT_1             ( ( portUnsignedBaseType ) 0x02 )

#define notifyTIMER_ID          ( 71 )

#define notifyMIN_PERIOD        ( 10 )

/*---------------------------------------------------------------------------*/

/*
 * Implementation of the task that gets notified.
 */
static void prvNotifiedTask( void *pvParameters );

/*
 * Performs a few initial tests that can be done prior to creating the second
 * task.
 */
static void prvSingleTaskTests( void );

/*
 * Software timer callback function from which xTaskNotifySend() is called.
 */
static void prvNotifyingTimerCb( timerHandleType xTimerHandle );

/*
 * Utility function to create pseudo random numbers.
 */
static portUInt32Type prvRand( void );

/*---------------------------------------------------------------------------*/

/* Used to latch errors during the test's execution. */
portspecTASK_NOTIFY_DATA_SECTION static volatile portBaseType xNotifyErrorStatus = pdPASS;

/* Used to ensure the task has not stalled. */
portspecTASK_NOTIFY_ZERO_DATA_SECTION static volatile portUnsignedBaseType uxNotifyCycleCount = 0U;

/* The handle of the task that receives the notifications. */
portspecTASK_NOTIFY_ZERO_DATA_SECTION static portTaskHandleType xTaskToNotify = NULL;

/* Used to count the notifications sent to the task from a software timer and
 * the number of notifications received by the task from the software timer.
 * The two should stay synchronised. */
portspecTASK_NOTIFY_ZERO_DATA_SECTION static volatile portUnsignedBaseType uxNotificationsReceived = 0U;
portspecCOMMON_PRIV_DATA_SECTION static volatile portUnsignedBaseType uxNotificationsSent = 0U;

/* The timer used to notify the task. */
portspecTASK_NOTIFY_ZERO_DATA_SECTION static timerHandleType xNotifyingTimerHandle = NULL;
portspecCOMMON_PRIV_DATA_SECTION static timerControlBlockType xNotifyingTimer = { 0 };

/* Used by the pseudo random number generating function. */
portspecTASK_NOTIFY_ZERO_DATA_SECTION static portUInt32Type ulNextRand = 0UL;

/* Average notify period. */
portspecTASK_NOTIFY_ZERO_DATA_SECTION static portTickType xAverageNotifyPeriod = ( portTickType ) 0;

/* Notified task TCB. */
portspecTCB_DATA_SECTION static xTCB xNotifiedTaskTCB = { 0 };

/*---------------------------------------------------------------------------*/

portBaseType xStartTaskNotifyTask( portTickType xAveragePeriod, portUnsignedBaseType uxTimerTaskPriority )
{
    portBaseType xReturn;
    xTaskParameters xNotifiedTaskParameters =
    {
        prvNotifiedTask,                    /* Task code */
        "Notified",                         /* Task name */
        &xNotifiedTaskTCB,                  /* TCB */
        acNotifiedTaskStack,                /* Stack buffer */
        portspecNOTIFIED_TASK_STACK_SIZE,   /* Stack depth bytes */
        NULL,                               /* Parameters */
        0,                                  /* Priority - to be updated by the function */
        NULL,                               /* TLS object */
        xNotifiedTaskPortParameters         /* Port-specific task parameters */
    };

    /* Pseudo seed the random number generator. */
    ulNextRand = prvRand();

    /* Average notify period. */
    xAverageNotifyPeriod = xAveragePeriod;

    /* The demo cannot run if the timer task priority is 0. */
    if( 0U == uxTimerTaskPriority )
    {
        xReturn = pdFAIL;
    }
    else
    {
        xNotifiedTaskParameters.uxPriority = uxTimerTaskPriority - 1U;

        /* Create the task that performs some tests by itself, then loops around
         * being notified by both a software timer and an interrupt. */
        xReturn = xTaskCreate( &xNotifiedTaskParameters, &xTaskToNotify );
    }

    return xReturn;
}
/*---------------------------------------------------------------------------*/

static void prvSingleTaskTests( void )
{
    const portTickType xTicksToWait = 100 / configTICK_RATE_MS;
    portBaseType xReturnValue;
    portUnsignedBaseType uxNotifiedValue, uxNotifyingValue;
    portUnsignedBaseType uxLoop;
    portTickType xTimeOnEntering;
    const portUnsignedBaseType uxFirstNotifiedValueConst = ( portUnsignedBaseType ) 0x3C3C3C3C;
    const portUnsignedBaseType uxSecondNotifiedValueConst = ( portUnsignedBaseType ) 0x55555555;
    const portUnsignedBaseType uxMaxLoops = 5U;

    /**************************************************************************
     * Check blocking when there are no notifications.
     */
    uxNotifiedValue = 0;
    xTimeOnEntering = xTaskGetTickCount();
    xReturnValue = xTaskNotifyWait( notifyMAX_VALUE, 0, &uxNotifiedValue, xTicksToWait );
    if( errNOTIFICATION_NOT_RECEIVED != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }
    else if( ( xTaskGetTickCount() - xTimeOnEntering ) < xTicksToWait )
    {
        /* Should have blocked for the entire block time. */
        xNotifyErrorStatus = pdFAIL;
    }


    /**************************************************************************
     * Check no blocking when notifications are pending.
     * First notify itself - this would not be a normal thing to do and is done
     * here for test purposes only.
     */
    xReturnValue = xTaskNotifySend( xTaskToNotify, taskNOTIFICATION_SET_VALUE_WITHOUT_OVERWRITE, uxFirstNotifiedValueConst );
    if( pdPASS != xReturnValue )
    {
        /* Even through the 'without overwrite' action was used the update
         * should have been successful. */
        xNotifyErrorStatus = pdFAIL;
    }

    /* The task should now have a notification pending, and so not time out. */
    xTimeOnEntering = xTaskGetTickCount();
    xReturnValue = xTaskNotifyWait( notifyMAX_VALUE, 0, &uxNotifiedValue, xTicksToWait );
    if( pdPASS != xReturnValue )
    {
        /* The task should have been notified. */
        xNotifyErrorStatus = pdFAIL;
    }
    else if( uxNotifiedValue != uxFirstNotifiedValueConst )
    {
        /* The notified value should be equal to uxFirstNotifiedValueConst. */
        xNotifyErrorStatus = pdFAIL;
    }
    else if( ( xTaskGetTickCount() - xTimeOnEntering ) >= xTicksToWait )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    /* Incremented to show the task is still running. */
    uxNotifyCycleCount++;


    /**************************************************************************
     * Check the non-overwriting functionality.
     * The notification is done twice using two different notification values.
     * The action says don't overwrite so only the first notification should
     * pass and the value read back should also be that used with the first
     * notification.
     */
    xReturnValue = xTaskNotifySend( xTaskToNotify, taskNOTIFICATION_SET_VALUE_WITHOUT_OVERWRITE, uxFirstNotifiedValueConst );
    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    xReturnValue = xTaskNotifySend( xTaskToNotify, taskNOTIFICATION_SET_VALUE_WITHOUT_OVERWRITE, uxSecondNotifiedValueConst );
    if( errNOTIFICATION_ALREADY_PENDING != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    /* Waiting for the notification should now return immediately so a block
     * time of zero is used. */
    xReturnValue = xTaskNotifyWait( notifyMAX_VALUE, 0x00, &uxNotifiedValue, tmrdemoDONT_BLOCK );
    if( pdPASS != xReturnValue )
    {
        /* The task should have been notified. */
        xNotifyErrorStatus = pdFAIL;
    }
    else if( uxNotifiedValue != uxFirstNotifiedValueConst )
    {
        /* The task should have been notified. */
        xNotifyErrorStatus = pdFAIL;
    }


    /**************************************************************************
     * Do the same again, only this time use the overwriting version.
     * This time both notifications should pass, and the value written the
     * second time should overwrite the value written the first time, and so be
     * the value that is read back.
     */
    xReturnValue = xTaskNotifySend( xTaskToNotify, taskNOTIFICATION_SET_VALUE_WITH_OVERWRITE, uxFirstNotifiedValueConst );
    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    xReturnValue = xTaskNotifySend( xTaskToNotify, taskNOTIFICATION_SET_VALUE_WITH_OVERWRITE, uxSecondNotifiedValueConst );
    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    xReturnValue = xTaskNotifyWait( notifyMAX_VALUE, 0, &uxNotifiedValue, tmrdemoDONT_BLOCK );
    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }
    else if( uxNotifiedValue != uxSecondNotifiedValueConst )
    {
        xNotifyErrorStatus = pdFAIL;
    }


    /**************************************************************************
     * Check notifications with no action pass without updating the value.
     * Even though uxFirstNotifiedValueConst is used as the value the value read
     * back should remain at ulSecondNotifiedConst.
     */
    xReturnValue = xTaskNotifySend( xTaskToNotify, taskNOTIFICATION_NO_ACTION, uxFirstNotifiedValueConst );
    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    xReturnValue = xTaskNotifyWait( notifyMAX_VALUE, 0x00, &uxNotifiedValue, tmrdemoDONT_BLOCK );
    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }
    else if( uxNotifiedValue != uxSecondNotifiedValueConst )
    {
        xNotifyErrorStatus = pdFAIL;
    }


    /**************************************************************************
     * Check incrementing values.
     * Send ulMaxLoop increment notifications, then ensure the received value
     * is as expected - which should be ulSecondNotificationValueConst plus how
     * ever many times to loop iterated.
     */
    for( uxLoop = 0; uxLoop < uxMaxLoops; uxLoop++ )
    {
        xReturnValue = xTaskNotifySend( xTaskToNotify, taskNOTIFICATION_INCREMENT, 0 );
        if( pdPASS != xReturnValue )
        {
            xNotifyErrorStatus = pdFAIL;
        }
    }

    xReturnValue = xTaskNotifyWait( notifyMAX_VALUE, 0, &uxNotifiedValue, tmrdemoDONT_BLOCK );
    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }
    else if( uxNotifiedValue != ( uxSecondNotifiedValueConst + uxMaxLoops ) )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    /* Should not be any notifications pending now. */
    xReturnValue = xTaskNotifyWait( 0x00, 0x00, &uxNotifiedValue, tmrdemoDONT_BLOCK );
    if( errNOTIFICATION_NOT_RECEIVED != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }


    /**************************************************************************
     * Check all bits can be set by notifying the task with one additional bit
     * set on each notification, and exiting the loop when all the bits are
     * found to be set.
     * As there are 32-bits the loop should execute 32 times before all the
     * bits are found to be set.
     */
    uxNotifyingValue = 0x01;
    uxLoop = 0;

    /* Start with all bits clear. */
    xTaskNotifyWait( notifyMAX_VALUE, 0x00, &uxNotifiedValue, tmrdemoDONT_BLOCK );

    do
    {
        /* Set the next bit in the task's notified value. */
        xReturnValue = xTaskNotifySend( xTaskToNotify, taskNOTIFICATION_SET_BITS, uxNotifyingValue );
        if( pdPASS != xReturnValue )
        {
            xNotifyErrorStatus = pdFAIL;
        }

        /* Wait for the notified value - which of course will already be
         * available. Don't clear the bits on entry or exit as this loop is
         * exited when all the bits are set. */
        xReturnValue = xTaskNotifyWait( 0, 0, &uxNotifiedValue, 0 );
        if( pdPASS != xReturnValue )
        {
            xNotifyErrorStatus = pdFAIL;
        }

        uxLoop++;

        /* Use the next bit on the next iteration around this loop. */
        uxNotifyingValue <<= 1;

    } while( uxNotifiedValue != notifyMAX_VALUE );

    /* As a native-size value was used the loop should have executed the
     * corresponding number of times before all the bits were set. */
    if( ( CHAR_BIT * sizeof( portUnsignedBaseType ) ) != uxLoop )
    {
        xNotifyErrorStatus = pdFAIL;
    }


    /**************************************************************************
     * Check bits are cleared on entry but not on exit when a notification
     * fails to arrive before timing out - both with and without a timeout value.
     * Wait for the notification again - but this time it is not given by
     * anything and should return pdFAIL.
     * The parameters are set to clear bit zero on entry and bit one on exit.
     * As no notification was received only the bit cleared on entry should
     * actually get cleared.
     */
    xReturnValue = xTaskNotifyWait( notifyBIT_0, notifyBIT_1, &uxNotifiedValue, xTicksToWait );
    if( errNOTIFICATION_NOT_RECEIVED != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    /* Notify the task with no action so as not to update the bits even though
     * notifyMAX_VALUE is used as the notification value. */
    xReturnValue = xTaskNotifySend( xTaskToNotify, taskNOTIFICATION_NO_ACTION, notifyMAX_VALUE );
    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    /* Reading back the value should should find bit 0 is clear, as this was
     * cleared on entry, but bit 1 is not clear as it will not have been
     * cleared on exit as no notification was received. */
    xReturnValue = xTaskNotifyWait( 0x00, 0x00, &uxNotifiedValue, tmrdemoDONT_BLOCK );
    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }
    else if( uxNotifiedValue != ( notifyMAX_VALUE & ~notifyBIT_0 ) )
    {
        xNotifyErrorStatus = pdFAIL;
    }


    /**************************************************************************
     * Now try clearing the bit on exit.
     * For that to happen a notification must be received, so the task is
     * notified first.
     */
    xReturnValue = xTaskNotifySend( xTaskToNotify, taskNOTIFICATION_NO_ACTION, 0 );
    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    xReturnValue = xTaskNotifyWait( 0x00, notifyBIT_1, &uxNotifiedValue, tmrdemoDONT_BLOCK );
    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    /* However as the bit is cleared on exit, after the returned notification
     * value is set, the returned notification value should not have the bit
     * cleared... */
    if( uxNotifiedValue != ( notifyMAX_VALUE & ~notifyBIT_0 ) )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    /* ...but reading the value back again should find that the bit was indeed
     * cleared internally.
     * The returned value should be errNOTIFICATION_NOT_RECEIVED however as
     * nothing has notified the task in the mean time. */
    xReturnValue = xTaskNotifyWait( 0x00, 0x00, &uxNotifiedValue, tmrdemoDONT_BLOCK );
    if( errNOTIFICATION_NOT_RECEIVED != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }
    else if( uxNotifiedValue != ( notifyMAX_VALUE & ~( notifyBIT_0 | notifyBIT_1 ) ) )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    /* Incremented to show the task is still running. */
    uxNotifyCycleCount++;

    /* Leave all bits cleared. */
    ( void ) xTaskNotifyWait( notifyMAX_VALUE, 0, NULL, tmrdemoDONT_BLOCK );
}
/*---------------------------------------------------------------------------*/

static void prvNotifyingTimerCb( timerHandleType xTimerHandle )
{
    ( void ) xTimerHandle;

    ( void ) xTaskNotifySend( xTaskToNotify, taskNOTIFICATION_INCREMENT, 0 );

    /* This value is also incremented from an interrupt. */
    taskENTER_CRITICAL();
    {
        uxNotificationsSent++;
    }
    taskEXIT_CRITICAL();
}
/*---------------------------------------------------------------------------*/

static void prvNotifiedTask( void *pvParameters )
{
    portTickType xMinPeriod = notifyMIN_PERIOD;
    portTickType xMaxPeriod = ( xAverageNotifyPeriod << 1 ) - xMinPeriod;
    portTickType xPeriod;
    portBaseType xReturnValue;
    portUnsignedBaseType uxNotificationValue;

    timerInitParametersType xNotifierTimerParams =
    {
        "Notifier",                 /* Timer Name */
        0,                          /* Timer Period in Ticks */
        pdFALSE,                    /* Is Periodic */
        notifyTIMER_ID,             /* Timer ID */
        &xNotifyingTimer,           /* Timer Control Block */
        prvNotifyingTimerCb,        /* Timer Callback Function */
        NULL,                       /* Timer Instance */
        NULL                        /* Timer Local Storage */
    };

    /*Need to be initialised at run-time for some compilers */
    xNotifierTimerParams.xTimerPeriodInTicks = xMaxPeriod;

    /* Remove compiler warnings about unused parameters. */
    ( void ) pvParameters;

    /* Run a few tests that can be done from a single task before entering the
     * main loop. */
    prvSingleTaskTests();

    /* Create the software timer that is used to send notifications to this task.
     * Notifications are also received from an interrupt. */
    xReturnValue = xTimerCreate( &xNotifierTimerParams, &xNotifyingTimerHandle );

    if( pdPASS != xReturnValue )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    for( ;; )
    {
        /* Start the timer again with a different period.
         * Sometimes the period will be higher than the tasks block time,
         * sometimes it will be lower than the tasks block time. */
        xPeriod = prvRand() % xMaxPeriod;
        if( xPeriod < xMinPeriod )
        {
            xPeriod = xMinPeriod;
        }

        xReturnValue = xTimerChangePeriod( xNotifyingTimerHandle, xPeriod, portMAX_DELAY );
        if( pdPASS != xReturnValue )
        {
            xNotifyErrorStatus = pdFAIL;
        }

        xReturnValue = xTimerStart( xNotifyingTimerHandle, portMAX_DELAY );
        if( pdPASS != xReturnValue )
        {
            xNotifyErrorStatus = pdFAIL;
        }

        /* Block waiting for the notification again with a different period.
         * Sometimes the period will be higher than the tasks block time,
         * sometimes it will be lower than the tasks block time. */
        xPeriod = prvRand() % xMaxPeriod;
        if( xPeriod < xMinPeriod )
        {
            xPeriod = xMinPeriod;
        }

        /* Block to wait for a notification but without clearing the
         * notification value. */
        ( void ) xTaskNotifyWait( 0x00, 0x00, NULL, xPeriod );

        /* Take a notification and clear before exit, this time without a
         * block time specified. */
        if( xTaskNotifyWait( 0x00, notifyMAX_VALUE, &uxNotificationValue, tmrdemoDONT_BLOCK ) == pdPASS )
        {
            uxNotificationsReceived += uxNotificationValue;
        }

        /* Wait for a notification and clear before exit, for the max block time. */
        if( xTaskNotifyWait( 0x00, notifyMAX_VALUE, &uxNotificationValue, portMAX_DELAY ) == pdPASS )
        {
            uxNotificationsReceived += uxNotificationValue;
        }

        /* Incremented to show the task is still running. */
        uxNotifyCycleCount++;
    }
}
/*---------------------------------------------------------------------------*/

portBaseType xNotifyTaskFromISR( void )
{
    portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType uxCallCount = 0;
    portBaseType xHigherPriorityTaskWoken = pdFALSE;

    /* The task performs some tests before starting the timer that gives the
     * notification from this interrupt.  If the timer has not been created yet
     * then the initial tests have not yet completed and the notification
     * should not be sent. */
    if( NULL != xNotifyingTimerHandle )
    {
        uxCallCount++;

        if( uxCallCount >= xAverageNotifyPeriod )
        {
            /* It is time to send the notification again. */
            uxCallCount = 0U;

            ( void ) xTaskNotifySendFromISR( xTaskToNotify, taskNOTIFICATION_INCREMENT, 0, &xHigherPriorityTaskWoken );
            uxNotificationsSent++;

            /* No need to yield since the tick ISR will do it anyway. */
        }
    }

    return xHigherPriorityTaskWoken;
}
/*---------------------------------------------------------------------------*/

/* This is called to check the created tasks are still running and have not
detected any errors. */
portBaseType xAreTaskNotificationTasksStillRunning( void )
{
    portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType uxLastNotifyCycleCount = 0;
    const portUnsignedBaseType uxMaxSendReceiveDeviation = 5U;
    portUnsignedBaseType uxCurrentSendReceiveDeviation;

    /* Check the cycle count is still incrementing to ensure the task is still
     * actually running. */
    if( uxLastNotifyCycleCount == uxNotifyCycleCount )
    {
        xNotifyErrorStatus = pdFAIL;
    }
    else
    {
        uxLastNotifyCycleCount = uxNotifyCycleCount;
    }

    /* Check the count of 'takes' from the software timer is keeping track with
     * the amount of 'gives'. */
    uxCurrentSendReceiveDeviation = uxNotificationsSent;
    uxCurrentSendReceiveDeviation -= uxNotificationsReceived;
    if( uxCurrentSendReceiveDeviation > uxMaxSendReceiveDeviation )
    {
        xNotifyErrorStatus = pdFAIL;
    }

    return xNotifyErrorStatus;
}
/*---------------------------------------------------------------------------*/

static portUInt32Type prvRand( void )
{
    const portUInt32Type ulMultiplier = 0x015A4E35UL, ulIncrement = 1UL;

    /* Utility function to generate a pseudo random number. */
    ulNextRand = ( ulMultiplier * ulNextRand ) + ulIncrement;
    return ( ( ulNextRand >> 16 ) & 0x7FFFUL );
}
/*---------------------------------------------------------------------------*/
