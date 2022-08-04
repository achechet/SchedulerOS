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

/* PortSpecifics.h may include some 'declare once' declarations. */
#define TIMERTEST_C

/******************************************************************************
 * Tests the behaviour of timers. Some timers are created before the scheduler
 * is started, and some after.
 *****************************************************************************/

/* Standard includes. */
#include <string.h>

/* Scheduler include files. */
#include "SafeRTOS_API.h"

/* Demo program include files. */
#include "PortSpecifics.h"
#include "TimerDemo.h"

/* This defines the number of auto reload timers that will be tested
 * simultaneously. Note that the number of auto reload timers that can actually
 * run is equal to configTIMER_CMD_QUEUE_LEN, any extra timers that are
 * requested here are used to test the queue full conditions.
 * IMPORTANT: If a timer is used for the 'check' functionality then that
 * timer must either be started after the scheduler has started or
 * tmrdemoNUM_AUTO_TEST_TIMERS must be less than configTIMER_CMD_QUEUE_LEN. */
#define tmrdemoNUM_AUTO_TEST_TIMERS     ( 14 )

#if ( tmrdemoNUM_AUTO_TEST_TIMERS < 2 )
    #error "tmrdemoNUM_AUTO_TEST_TIMERS must be at least 2."
#endif

#define tmrdemoDONT_BLOCK               ( ( portTickType ) 0 )
#define tmrdemoONE_SHOT_TIMER_PERIOD    ( xBasePeriod * ( ( portTickType ) 3 ) )
#define trmdemoNUM_TIMER_RESETS         ( ( portUInt8Type ) 10 )

#ifndef MIN
    #define MIN( a, b )                     ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif

/*---------------------------------------------------------------------------*/

/* The callback functions used by the timers. These each increment a counter to
 * indicate which timer has expired. The auto-reload timers that are used by
 * the test task (as opposed to being used from an ISR) all share the same
 * prvPeriodicTimerCallback() callback function, and use the ID of the
 * pxExpiredTimer parameter passed into that function to know which counter to
 * increment. The other timers all have their own unique callback function and
 * simply increment their counters without using the callback function
 * parameter. */
static void prvPeriodicTimerCallback( timerHandleType pxExpiredTimer );
static void prvOneShotTimerCallback( timerHandleType pxExpiredTimer );
static void prvTimerTestTask( void *pvParameters );
static void prvISRPeriodicTimerCallback( timerHandleType pxExpiredTimer );
static void prvISROneShotTimerCallback( timerHandleType pxExpiredTimer );

/* The test functions used by the timer test task. These manipulate the auto
 * reload and one shot timers in various ways, then delay, then inspect the
 * timers to ensure they have behaved as expected. */
static void prvTest1_CreateTimersWithoutSchedulerRunning( portUnsignedBaseType uxCmdQueueLen );
static void prvTest2_CheckTaskAndTimersInitialState( portUnsignedBaseType uxCmdQueueLen );
static void prvTest3_CheckPeriodicExpireRates( portUnsignedBaseType uxCmdQueueLen );
static void prvTest4_CheckPeriodicTimersCanBeStopped( portUnsignedBaseType uxCmdQueueLen );
static void prvTest5_CheckBasicOneShotTimerBehaviour( void );
static void prvTest6_CheckPeriodicResetBehaviour( portUnsignedBaseType uxCmdQueueLen );
static void prvResetStartConditionsForNextIteration( portUnsignedBaseType uxCmdQueueLen );

/*---------------------------------------------------------------------------*/

/* Flag that will be latched to pdFAIL should any unexpected behaviour be
 * detected in any of the demo tests. */
portspecTIMER_TEST_DATA_SECTION static volatile portBaseType xTimerDemoTestStatus = pdPASS;
portspecTIMER_TEST_DATA_SECTION static volatile portBaseType xISRTestStatus = pdPASS;

/* Counter that is incremented on each cycle of a test. This is used to
 * detect a stalled task - a test that is no longer running. */
portspecTIMER_TEST_ZERO_DATA_SECTION static volatile portUInt32Type ulLoopCounter = 0;

/* A set of auto reload timers - each of which use the same callback function.
 * The callback function uses the timer ID to index into, and then increment, a
 * counter in the aucPeriodicTimerCounters[] array. The auto reload timers
 * referenced from axPeriodicTimers[] are used by the prvTimerTestTask task. */
portspecTIMER_TEST_ZERO_DATA_SECTION static timerHandleType axPeriodicTimers[ tmrdemoNUM_AUTO_TEST_TIMERS ] = { 0 };
portspecTIMER_TEST_ZERO_DATA_SECTION static volatile portUInt8Type aucPeriodicTimerCounters[ tmrdemoNUM_AUTO_TEST_TIMERS ] = { 0 };
portspecTIMER_TEST_ZERO_DATA_SECTION static timerControlBlockType axPeriodicTimersBuffer[ tmrdemoNUM_AUTO_TEST_TIMERS ] = { { 0 } };

/* The one shot timer is configured to use a callback function that increments
 * ucOneShotTimerCounter each time it gets called. */
portspecTIMER_TEST_ZERO_DATA_SECTION static timerHandleType xOneShotTimer = NULL;
portspecTIMER_TEST_ZERO_DATA_SECTION static volatile portUInt8Type ucOneShotTimerCounter = ( portUInt8Type ) 0;
portspecTIMER_TEST_ZERO_DATA_SECTION static timerControlBlockType xOneShotTimerBuffer = { 0 };

/* The ISR reload timer is controlled from the tick hook to exercise the timer
 * API functions that can be used from an ISR. It is configured to increment
 * ucISRReloadTimerCounter each time its callback function is executed. */
portspecTIMER_TEST_ZERO_DATA_SECTION static timerHandleType xISRPeriodicTimer = NULL;
portspecTIMER_TEST_ZERO_DATA_SECTION static volatile portUnsignedBaseType uxISRPeriodicTimerCounter = ( portUInt8Type ) 0;
portspecTIMER_TEST_ZERO_DATA_SECTION static timerControlBlockType xISRPeriodicTimerBuffer = { 0 };

/* The ISR one shot timer is controlled from the tick hook to exercise the
 * timer API functions that can be used from an ISR. It is configured to
 * increment ucISRReloadTimerCounter each time its callback function is
 * executed. */
portspecTIMER_TEST_ZERO_DATA_SECTION static timerHandleType xISROneShotTimer = NULL;
portspecTIMER_TEST_ZERO_DATA_SECTION static volatile portUnsignedBaseType uxISROneShotTimerCounter = ( portUInt8Type ) 0U;
portspecTIMER_TEST_ZERO_DATA_SECTION static timerControlBlockType xISROneShotTimerBuffer = { 0 };

/* The period of all the timers are a multiple of the base period. The base
 * period is configured by the parameter to vStartTimerDemoTask(). */
portspecTIMER_TEST_ZERO_DATA_SECTION static portTickType xBasePeriod = ( portTickType ) 0U;

/* Timer Test Task TCB. */
portspecTCB_DATA_SECTION static xTCB xTimerTestTaskTCB = { 0 };

/*---------------------------------------------------------------------------*/


portBaseType xStartTimerDemoTask( portTickType xBasePeriodIn, portUnsignedBaseType uxCmdQueueLen, portUnsignedBaseType uxTimerTaskPriority )
{
    portBaseType xReturn = pdPASS;
    xTaskParameters xTimerTestTaskParameters =
    {
        prvTimerTestTask,                   /* Task code */
        "TimerTest",                        /* Task name */
        &xTimerTestTaskTCB,                 /* TCB */
        acTimerTestTaskStack,               /* Stack buffer */
        portspecTIMER_TASK_STACK_SIZE,      /* Stack depth bytes */
        NULL,                               /* Parameters */
        0,                                  /* Priority */
        NULL,                               /* TLS object */
        xTimerTestTaskPortParameters        /* Port-specific task parameters */
    };

    /* Start with the timer and counter arrays clear - this is only necessary
     * where the compiler does not clear them automatically on start up. */
    memset( ( void * ) aucPeriodicTimerCounters, 0x00, tmrdemoNUM_AUTO_TEST_TIMERS * sizeof( portUInt8Type ) );
    memset( ( void * ) axPeriodicTimers, 0x00, tmrdemoNUM_AUTO_TEST_TIMERS * sizeof( timerHandleType ) );

    /* Store the period from which all the timer periods will be generated from
     * (multiples of). */
    xBasePeriod = xBasePeriodIn;

    /* Create a set of timers for use by this demo/test. */
    prvTest1_CreateTimersWithoutSchedulerRunning( uxCmdQueueLen );

    /* Create the task that will control and monitor the timers. This is
     * created at a lower priority than the timer service task to ensure, as
     * far as it is concerned, commands on timers are actioned immediately
     * (sending a command to the timer service task will unblock the timer
     * service task, which will then preempt this task). */

    /* The demo cannot run if the timer task priority is 0 */
    if( 0 == uxTimerTaskPriority )
    {
        xReturn = pdFAIL;
    }
    else
    {
        xTimerTestTaskParameters.uxPriority = uxTimerTaskPriority - 1U;
        xTimerTestTaskParameters.pvParameters = ( void * ) uxCmdQueueLen;
        xReturn = xTaskCreate( &xTimerTestTaskParameters, ( portTaskHandleType * ) NULL );
    }

    return xReturn;
}
/*---------------------------------------------------------------------------*/

static void prvTimerTestTask( void *pvParameters )
{
    portBaseType xStatus = pdFAIL;
    portUnsignedBaseType uxCmdQueueLen;

    timerInitParametersType xTimerParams =
    {
        "Oneshot Timer",                /* Text name to facilitate debugging. The kernel does not use this itself. */
        0,                              /* The period for the timer. */
        pdFALSE,                        /* Don't auto-reload - hence a one shot timer. */
        0,                              /* No ID required as this is a single instance only. */
        &xOneShotTimerBuffer,           /* The timer structure buffer */
        prvOneShotTimerCallback,        /* The callback to be called when the timer expires. */
        NULL,                           /* Use the default timer instance. */
        NULL                            /* No Timer Local Storage required */
    };

    /*Need to be initialised at run-time for some compilers */
    xTimerParams.xTimerPeriodInTicks = tmrdemoONE_SHOT_TIMER_PERIOD;

    /* get the command queue length */
    uxCmdQueueLen = ( portUnsignedBaseType ) pvParameters;

    /* Create a one-shot timer for use later on in this test. */
    xStatus = xTimerCreate( &xTimerParams, &xOneShotTimer );

    if( pdPASS != xStatus )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    /* Ensure all the timers are in their expected initial state. This depends
     * on the timer service task having a higher priority than this task. */
    prvTest2_CheckTaskAndTimersInitialState( uxCmdQueueLen );

    for( ;; )
    {
        /* Check the auto reload timers expire at the expected/correct rates. */
        prvTest3_CheckPeriodicExpireRates( uxCmdQueueLen );

        /* Check the auto reload timers can be stopped correctly, and correctly
         * report their state. */
        prvTest4_CheckPeriodicTimersCanBeStopped( uxCmdQueueLen );

        /* Check the one shot timer only calls its callback once after it has
         * been started, and that it reports its state correctly. */
        prvTest5_CheckBasicOneShotTimerBehaviour();

        /* Check timer reset behaviour. */
        prvTest6_CheckPeriodicResetBehaviour( uxCmdQueueLen );

        if( pdPASS != xTimerDemoTestStatus )
        {
            xStatus++;
        }

        /* Start the timers again to restart all the tests over again. */
        prvResetStartConditionsForNextIteration( uxCmdQueueLen );
    }
}
/*---------------------------------------------------------------------------*/

/* This is called to check that the created task is still running and has not
 * detected any errors. */
portBaseType xAreTimerDemoTasksStillRunning( portTickType xCycleFrequency, portUnsignedBaseType uxCmdQueueLen )
{
    portspecCOMMON_PRIV_DATA_SECTION static portUInt32Type ulLastLoopCounter = 0UL;
    portTickType xMaxBlockTimeUsedByTheseTests, xLoopCounterIncrementTimeMax;
    portspecCOMMON_PRIV_DATA_SECTION static portTickType xIterationsWithoutCounterIncrement = ( portTickType ) 0, xLastCycleFrequency = ( portTickType ) 0;

    if( xLastCycleFrequency != xCycleFrequency )
    {
        /* The cycle frequency has probably become much faster due to an error
         * elsewhere. Start counting Iterations again. */
        xIterationsWithoutCounterIncrement = ( portTickType ) 0;
        xLastCycleFrequency = xCycleFrequency;
    }

    /* Calculate the maximum number of times that it is permissible for this
     * function to be called without ulLoopCounter being incremented. This is
     * necessary because the tests in this file block for extended periods, and
     * the block period might be longer than the time between calls to this
     * function. */
    xMaxBlockTimeUsedByTheseTests = ( ( portTickType ) ( uxCmdQueueLen / 2U ) ) * xBasePeriod;
    xLoopCounterIncrementTimeMax = xMaxBlockTimeUsedByTheseTests / xCycleFrequency;

    /* If the demo task is still running then we expect the loopcounter to have
     * incremented every xLoopCounterIncrementTimeMax calls. */
    if( ulLastLoopCounter == ulLoopCounter )
    {
        xIterationsWithoutCounterIncrement++;
        if( xIterationsWithoutCounterIncrement > xLoopCounterIncrementTimeMax )
        {
            /* The tests appear to be no longer running (stalled). */
            xTimerDemoTestStatus = pdFAIL;
        }
    }
    else
    {
        /* ulLoopCounter changed, so the count of times this function was
         * called without a change can be reset to zero. */
        xIterationsWithoutCounterIncrement = ( portTickType ) 0;
    }

    ulLastLoopCounter = ulLoopCounter;

    /* See if the ISR tests have failed. */
    if( pdFAIL == xISRTestStatus )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    /* Errors detected in the task itself will have latched
     * xTimerDemoTestStatus to pdFAIL. */

    return xTimerDemoTestStatus;
}
/*---------------------------------------------------------------------------*/

static void prvTest1_CreateTimersWithoutSchedulerRunning( portUnsignedBaseType uxCmdQueueLen )
{
    portUnsignedBaseType uxTimer;
    portBaseType xStatus = pdFAIL;

    timerInitParametersType xTimerParams =
    {
        "FR Timer",                     /* Text name to facilitate debugging. The kernel does not use this itself. */
        0,                              /* The period for the timer. Setup in loop below */
        pdTRUE,                         /* Auto-reload is set to true. */
        0,                              /* Timer ID. Setup in loop below */
        NULL,                           /* Timer Control Block. Setup in loop below */
        prvPeriodicTimerCallback,       /* The callback to be called when the timer expires. */
        NULL,                           /* Use the default timer instance. */
        NULL                            /* No Timer Local Storage required */
    };

    timerInitParametersType xPeriodicISRTimerParams =
    {
        "ISR AR",                       /* The text name given to the timer. */
        0xffff,                         /* The timer is not given a period yet - this will be done from the tick hook, but a period of 0 is invalid. */
        pdTRUE,                         /* This is an auto reload timer. */
        0L,                             /* The identifier is not required. */
        &xISRPeriodicTimerBuffer,       /* The timer buffer. */
        prvISRPeriodicTimerCallback,    /* The callback that is executed when the timer expires. */
        NULL,                           /* Use the default timer instance. */
        NULL                            /* No Timer Local Storage required */
    };

    timerInitParametersType xOneShotISRTimerParams =
    {
        "ISR OS",                       /* The text name given to the timer. */
        0xffff,                         /* The timer is not given a period yet - this will be done from the tick hook, but a period of 0 is invalid. */
        pdFALSE,                        /* This is a one shot timer. */
        0L,                             /* The identifier is not required. */
        &xISROneShotTimerBuffer,        /* The timer buffer. */
        prvISROneShotTimerCallback,     /* The callback that is executed when the timer expires. */
        NULL,                           /* Use the default timer instance. */
        NULL                            /* No Timer Local Storage required */
    };

    /* Initialise the Timer buffers, to prevent xTimerCreate() from reporting
     * that the timer is already in use.
     */
    memset( axPeriodicTimersBuffer, 0, sizeof( axPeriodicTimersBuffer ) );

    /* Note that this demo is different to the FreeRTOS demo and does not
     * attempt to exercise the queue full condition. This is to allow the use
     * of other timers such as a check task timer. */
    for( uxTimer = 0; uxTimer < MIN( tmrdemoNUM_AUTO_TEST_TIMERS, uxCmdQueueLen ); uxTimer++ )
    {
        /* The period for the timer. The plus 1 ensures a period of zero is not specified. */
        xTimerParams.xTimerPeriodInTicks = ( ( uxTimer + ( portTickType ) 1 ) * xBasePeriod );

        /* Use the loop index as the ID. */
        xTimerParams.xTimerID = ( portBaseType ) uxTimer;

        /* Timer Control Block */
        xTimerParams.pxNewTimer = &( axPeriodicTimersBuffer[ uxTimer ] );

        /* As the timer queue is not yet full, it should be possible to both
         * create and start a timer. These timers are being started before the
         * scheduler has been started, so their block times should get set to
         * zero within the timer API itself. */
        xStatus = xTimerCreate( &xTimerParams, &( axPeriodicTimers[ uxTimer ] ));

        if( pdPASS != xStatus )
        {
            xTimerDemoTestStatus = pdFAIL;
        }
        else
        {
            /* The scheduler has not yet started, so the block period of
             * portMAX_DELAY should just get set to zero in xTimerStart().
             * Also, the timer queue is not yet full so xTimerStart() should
             * return pdPASS. */
            if( xTimerStart( axPeriodicTimers[ uxTimer ], portMAX_DELAY ) != pdPASS )
            {
                xTimerDemoTestStatus = pdFAIL;
            }
        }
    }

    /* The timer command queue is now full, we can create but not start any
     * extra timers. */
    for( uxTimer = uxCmdQueueLen; uxTimer < tmrdemoNUM_AUTO_TEST_TIMERS; uxTimer++ )
    {
        /* The period for the timer. The plus 1 ensures a period of zero is not specified. */
        xTimerParams.xTimerPeriodInTicks = ( ( uxTimer + ( portTickType ) 1 ) * xBasePeriod );

        /* Use the loop index as the ID. */
        xTimerParams.xTimerID = ( portBaseType ) uxTimer;

        /* Timer Control Block */
        xTimerParams.pxNewTimer = &( axPeriodicTimersBuffer[ uxTimer ] );

        /* As the timer command queue is now full, the timers can be created but not started. */
        xStatus = xTimerCreate( &xTimerParams, &( axPeriodicTimers[ uxTimer ] ));                                               /* No Timer Local Storage required */

        if( pdPASS != xStatus )
        {
            xTimerDemoTestStatus = pdFAIL;
        }
        else
        {
            /* The scheduler has not yet started, so the block period of
             * portMAX_DELAY should just get set to zero in xTimerStart().
             * Also, the timer queue is full so xTimerStart() should
             * return pdFAIL as the timers cannot be started. */
            if( xTimerStart( axPeriodicTimers[ uxTimer ], portMAX_DELAY ) == pdPASS )
            {
                xTimerDemoTestStatus = pdFAIL;
            }
        }
    }

    /* Create the timers that are used from the tick interrupt to test the
     * timer API functions that can be called from an ISR. */
    xStatus = xTimerCreate( &xPeriodicISRTimerParams, &xISRPeriodicTimer);

    if( pdPASS != xStatus )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    xStatus = xTimerCreate( &xOneShotISRTimerParams, &xISROneShotTimer );

    if( pdPASS != xStatus )
    {
        xTimerDemoTestStatus = pdFAIL;
    }
}
/*---------------------------------------------------------------------------*/

static void prvTest2_CheckTaskAndTimersInitialState( portUnsignedBaseType uxCmdQueueLen )
{
    portUnsignedBaseType uxTimer;

    /* Ensure all the timers are in their expected initial state. This depends
     * on the timer service task having a higher priority than this task.
     * Auto reload timers 0 to ( configTIMER_CMD_QUEUE_LEN - 1 ) should now be
     * active, and other auto reload timers should not yet be active (they
     * could not be started prior to the scheduler being started when it was
     * created). */
    for( uxTimer = 0; uxTimer < MIN( tmrdemoNUM_AUTO_TEST_TIMERS, uxCmdQueueLen ); uxTimer++ )
    {
        if( xTimerIsTimerActive( axPeriodicTimers[ uxTimer ] ) != pdTRUE )
        {
            xTimerDemoTestStatus = pdFAIL;
        }
    }
    for( uxTimer = uxCmdQueueLen; uxTimer < tmrdemoNUM_AUTO_TEST_TIMERS; uxTimer++ )
    {
        if( xTimerIsTimerActive( axPeriodicTimers[ uxTimer ] ) == pdTRUE )
        {
            xTimerDemoTestStatus = pdFAIL;
        }
    }
}
/*---------------------------------------------------------------------------*/

static void prvTest3_CheckPeriodicExpireRates( portUnsignedBaseType uxCmdQueueLen )
{
    portUInt8Type ucMaxAllowableValue, ucMinAllowableValue, ucTimer;
    portTickType xBlockPeriod, xTimerPeriod, xExpectedNumber;

    /* Check the auto reload timers expire at the expected rates. */

    /* Delaying for tmrdemoNUM_AUTO_TEST_TIMERS * xBasePeriod ticks should allow
     * all the auto reload timers to expire at least once. */
    xBlockPeriod = ( ( portTickType ) tmrdemoNUM_AUTO_TEST_TIMERS ) * xBasePeriod;
    xTaskDelay( xBlockPeriod );

    /* Check that all the auto reload timers have called their callback
     * function the expected number of times. */
    for( ucTimer = 0; ucTimer < ( portUInt8Type ) MIN( tmrdemoNUM_AUTO_TEST_TIMERS, uxCmdQueueLen ); ucTimer++ )
    {
        /* The expected number of expiries is equal to the block period divided
         * by the timer period. */
        xTimerPeriod = ( ( ( portTickType ) ucTimer + ( portTickType ) 1 ) * xBasePeriod );
        xExpectedNumber = xBlockPeriod / xTimerPeriod;

        ucMaxAllowableValue = ( ( portUInt8Type ) xExpectedNumber ) ;
        ucMinAllowableValue = ( ( portUInt8Type ) xExpectedNumber - ( portUInt8Type ) 1 );

        if( ( aucPeriodicTimerCounters[ ucTimer ] < ucMinAllowableValue ) ||
                ( aucPeriodicTimerCounters[ ucTimer ] > ucMaxAllowableValue )
          )
        {
            xTimerDemoTestStatus = pdFAIL;
        }
    }

    if( pdPASS == xTimerDemoTestStatus )
    {
        /* No errors have been reported so increment the loop counter so the
         * check task knows this task is still running. */
        ulLoopCounter++;
    }
}
/*---------------------------------------------------------------------------*/

static void prvTest4_CheckPeriodicTimersCanBeStopped( portUnsignedBaseType uxCmdQueueLen )
{
    portUInt8Type ucTimer;

    /* Check the auto reload timers can be stopped correctly, and correctly
     * report their state. */

    /* Stop all the active timers. */
    for( ucTimer = 0; ucTimer < ( portUInt8Type ) MIN( tmrdemoNUM_AUTO_TEST_TIMERS, uxCmdQueueLen ); ucTimer++ )
    {
        /* The timer has not been stopped yet! */
        if( xTimerIsTimerActive( axPeriodicTimers[ ucTimer ] ) == pdFALSE )
        {
            xTimerDemoTestStatus = pdFAIL;
        }

        /* Now stop the timer. This will appear to happen immediately to this
         * task because this task is running at a priority below the timer
         * service task. */
        xTimerStop( axPeriodicTimers[ ucTimer ], tmrdemoDONT_BLOCK );

        /* The timer should now be inactive. */
        if( xTimerIsTimerActive( axPeriodicTimers[ ucTimer ] ) != pdFALSE )
        {
            xTimerDemoTestStatus = pdFAIL;
        }
    }

    taskENTER_CRITICAL();
    {
        /* Clear the timer callback count. */
        memset( ( void * ) aucPeriodicTimerCounters, 0, tmrdemoNUM_AUTO_TEST_TIMERS * sizeof( portUInt8Type ) );
    }
    taskEXIT_CRITICAL();

    /* The timers are now all inactive, so this time, after delaying, none of
     * the callback counters should have incremented. */
    xTaskDelay( ( ( portTickType ) tmrdemoNUM_AUTO_TEST_TIMERS ) * xBasePeriod );
    for( ucTimer = 0; ucTimer < ( portUInt8Type ) MIN( tmrdemoNUM_AUTO_TEST_TIMERS, uxCmdQueueLen ); ucTimer++ )
    {
        if( aucPeriodicTimerCounters[ ucTimer ] != ( portUInt8Type ) 0 )
        {
            xTimerDemoTestStatus = pdFAIL;
        }
    }

    if( pdPASS == xTimerDemoTestStatus )
    {
        /* No errors have been reported so increment the loop counter so the
         * check task knows this task is still running. */
        ulLoopCounter++;
    }
}
/*---------------------------------------------------------------------------*/

static void prvTest5_CheckBasicOneShotTimerBehaviour( void )
{
    /* Check the one shot timer only calls its callback once after it has been
     * started, and that it reports its state correctly. */

    /* The one shot timer should not be active yet. */
    if( xTimerIsTimerActive( xOneShotTimer ) != pdFALSE )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    if( ucOneShotTimerCounter != ( portUInt8Type ) 0 )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    /* Start the one shot timer and check that it reports its state correctly. */
    xTimerStart( xOneShotTimer, tmrdemoDONT_BLOCK );
    if( xTimerIsTimerActive( xOneShotTimer ) == pdFALSE )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    /* Delay for three times as long as the one shot timer period, then check
     * to ensure it has only called its callback once, and is now not in the
     * active state. */
    xTaskDelay( tmrdemoONE_SHOT_TIMER_PERIOD * ( portTickType ) 3 );

    if( xTimerIsTimerActive( xOneShotTimer ) != pdFALSE )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    if( ucOneShotTimerCounter != ( portUInt8Type ) 1 )
    {
        xTimerDemoTestStatus = pdFAIL;
    }
    else
    {
        /* Reset the one shot timer callback count. */
        ucOneShotTimerCounter = ( portUInt8Type ) 0;
    }

    if( pdPASS == xTimerDemoTestStatus )
    {
        /* No errors have been reported so increment the loop counter so the
         * check task knows this task is still running. */
        ulLoopCounter++;
    }
}
/*---------------------------------------------------------------------------*/

static void prvTest6_CheckPeriodicResetBehaviour( portUnsignedBaseType uxCmdQueueLen )
{
    portUInt8Type ucTimer;
    portUnsignedBaseType uxPeriodicTimerNum = MIN( tmrdemoNUM_AUTO_TEST_TIMERS, uxCmdQueueLen ) - 2U;

    /* Check timer reset behaviour. */

    /* Restart the one shot timer and check it reports its status correctly. */
    xTimerStart( xOneShotTimer, tmrdemoDONT_BLOCK );
    if( xTimerIsTimerActive( xOneShotTimer ) == pdFALSE )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    /* Restart one of the auto reload timers and check that it reports its
     * status correctly. */
    xTimerStart( axPeriodicTimers[ uxPeriodicTimerNum ], tmrdemoDONT_BLOCK );
    if( xTimerIsTimerActive( axPeriodicTimers[ uxPeriodicTimerNum ] ) == pdFALSE )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    for( ucTimer = 0; ucTimer < trmdemoNUM_TIMER_RESETS; ucTimer++ )
    {
        /* Delay for half as long as the one shot timer period, then reset it.
         * It should never expire while this is done, so its callback count
         * should never increment. */
        xTaskDelay( tmrdemoONE_SHOT_TIMER_PERIOD / 2 );

        /* Check both running timers are still active, but have not called
         * their callback functions. */
        if( xTimerIsTimerActive( xOneShotTimer ) == pdFALSE )
        {
            xTimerDemoTestStatus = pdFAIL;
        }

        if( ucOneShotTimerCounter != ( portUInt8Type ) 0 )
        {
            xTimerDemoTestStatus = pdFAIL;
        }

        if( xTimerIsTimerActive( axPeriodicTimers[ uxPeriodicTimerNum ] ) == pdFALSE )
        {
            xTimerDemoTestStatus = pdFAIL;
        }

        if( aucPeriodicTimerCounters[ uxPeriodicTimerNum ] != ( portUInt8Type ) 0 )
        {
            xTimerDemoTestStatus = pdFAIL;
        }

        /* Reset both running timers. */
        xTimerReset( xOneShotTimer, tmrdemoDONT_BLOCK );
        xTimerReset( axPeriodicTimers[ uxPeriodicTimerNum ], tmrdemoDONT_BLOCK );

        if( pdPASS == xTimerDemoTestStatus )
        {
            /* No errors have been reported so increment the loop counter so
             * the check task knows this task is still running. */
            ulLoopCounter++;
        }
    }

    /* Finally delay long enough for both running timers to expire. */
    xTaskDelay( ( ( portTickType ) uxCmdQueueLen ) * xBasePeriod );

    /* The timers were not reset during the above delay period so should now
     * both have called their callback functions. */
    if( ucOneShotTimerCounter != ( portUInt8Type ) 1 )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    if( aucPeriodicTimerCounters[ uxPeriodicTimerNum ] == 0 )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    /* The one shot timer should no longer be active, while the auto reload
     * timer should still be active. */
    if( xTimerIsTimerActive( axPeriodicTimers[ uxPeriodicTimerNum ] ) == pdFALSE )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    if( xTimerIsTimerActive( xOneShotTimer ) == pdTRUE )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    /* Stop the auto reload timer again. */
    xTimerStop( axPeriodicTimers[ uxPeriodicTimerNum ], tmrdemoDONT_BLOCK );

    if( xTimerIsTimerActive( axPeriodicTimers[ uxPeriodicTimerNum ] ) != pdFALSE )
    {
        xTimerDemoTestStatus = pdFAIL;
    }

    /* Clear the timer callback counts, ready for another iteration of these
     * tests. */
    aucPeriodicTimerCounters[ uxPeriodicTimerNum ] = ( portUInt8Type ) 0;
    ucOneShotTimerCounter = ( portUInt8Type ) 0;

    if( pdPASS == xTimerDemoTestStatus )
    {
        /* No errors have been reported so increment the loop counter so the
         * check task knows this task is still running. */
        ulLoopCounter++;
    }
}
/*---------------------------------------------------------------------------*/

static void prvResetStartConditionsForNextIteration( portUnsignedBaseType uxCmdQueueLen )
{
    portUInt8Type ucTimer;

    /* Start the timers again to start all the tests over again. */

    /* Start the timers again. */
    for( ucTimer = 0; ucTimer < ( portUInt8Type ) MIN( tmrdemoNUM_AUTO_TEST_TIMERS, uxCmdQueueLen ); ucTimer++ )
    {
        /* The timer has not been started yet! */
        if( xTimerIsTimerActive( axPeriodicTimers[ ucTimer ] ) != pdFALSE )
        {
            xTimerDemoTestStatus = pdFAIL;
        }

        /* Now start the timer. This will appear to happen immediately to this
         * task because this task is running at a priority below the timer
         * service task. */
        xTimerStart( axPeriodicTimers[ ucTimer ], tmrdemoDONT_BLOCK );

        /* The timer should now be active. */
        if( xTimerIsTimerActive( axPeriodicTimers[ ucTimer ] ) == pdFALSE )
        {
            xTimerDemoTestStatus = pdFAIL;
        }
    }

    if( pdPASS == xTimerDemoTestStatus )
    {
        /* No errors have been reported so increment the loop counter so the
         * check task knows this task is still running. */
        ulLoopCounter++;
    }
}
/*---------------------------------------------------------------------------*/

portBaseType xTimerPeriodicISRTests( void )
{
    /* The xHigherPriorityTaskWoken parameter is not used in this case as this
     * function is called from the tick hook anyway. However the API required it to
     * be present. */
    portBaseType xHigherPriorityTaskWoken = pdFALSE;
    portTickType xMargin;
    portBaseType xPeriodicStatus;
    portBaseType xOneShotStatus;
    portspecCOMMON_PRIV_DATA_SECTION static portBaseType xTestRunning = pdFALSE;
    portspecCOMMON_PRIV_DATA_SECTION static portTickType xExpectedExpiryTime;
    portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType uxExpectedISRPeriodicTimerCounter;

    /* The timer service task may not be the highest priority task, so it
     * cannot be assumed that timings will be exact. Timers should never call
     * their callback before their expiry time, but a margin is permissible for
     * calling their callback after their expiry time. If exact timing is
     * required, then configTIMER_TASK_PRIORITY must be set to ensure the timer
     * service task is the highest priority task in the system. */
    xMargin = 5;

    /* This test is called from the tick ISR even when the scheduler is
     * suspended. Therefore, it is possible for the xTickCount to be
     * temporarily less than the uxTicks count maintained in this function.
     * That can result in calculated unblock times being too short, as this
     * function is not called as missed ticks (ticks that occur while the
     * scheduler is suspended) are unwound to re-instate the real tick value. */

    if( xTestRunning == pdFALSE )
    {
        /* The timer has been created, but not started. Start it by setting the
         * period. */
        xPeriodicStatus = xTimerChangePeriodFromISR( xISRPeriodicTimer, xBasePeriod, &xHigherPriorityTaskWoken );

        /* Set the one shot timer up with a slightly longer period as we will
         * keep resetting it and it should never expire. */
        xOneShotStatus = xTimerChangePeriodFromISR( xISROneShotTimer, ( xBasePeriod + xMargin + 1 ), &xHigherPriorityTaskWoken );

        if( ( pdPASS == xPeriodicStatus ) && ( pdPASS == xOneShotStatus ) )
        {
            /* Set the initial expected expiry time and reset the expiry count. */
            xExpectedExpiryTime = xTaskGetTickCountFromISR() + xBasePeriod;
            uxISRPeriodicTimerCounter = 0;
            uxISROneShotTimerCounter = 0;
            uxExpectedISRPeriodicTimerCounter = 0;

            /* The test is running now. */
            xTestRunning = pdTRUE;
        }
    }
    else
    {
        /* See if the auto reload timer should have expired. */
        if( xTaskGetTickCountFromISR() >= ( xExpectedExpiryTime + xMargin ) )
        {
            if( ++uxExpectedISRPeriodicTimerCounter != uxISRPeriodicTimerCounter )
            {
                xISRTestStatus = pdFAIL;
            }
            if( 0L != uxISROneShotTimerCounter )
            {
                xISRTestStatus = pdFAIL;
            }

            xExpectedExpiryTime += xBasePeriod;

            /* Reset the one shot timer. */
            xTimerResetFromISR( xISROneShotTimer, &xHigherPriorityTaskWoken );
        }
    }

    return xHigherPriorityTaskWoken;
}
/*---------------------------------------------------------------------------*/

/*** Timer callback functions are defined below here. ***/

static void prvPeriodicTimerCallback( timerHandleType xExpiredTimer )
{
    portBaseType xTimerID;

    if( pdPASS == xTimerGetTimerID( xExpiredTimer, &xTimerID ) )
    {
        if( xTimerID <= ( tmrdemoNUM_AUTO_TEST_TIMERS ) )
        {
            ( aucPeriodicTimerCounters[ xTimerID ] )++;
        }
        else
        {
            /* The timer ID appears to be unexpected (invalid). */
            xTimerDemoTestStatus = pdFAIL;
        }
    }
    else
    {
        /* The timer handle is invalid. */
        xTimerDemoTestStatus = pdFAIL;
    }
}
/*---------------------------------------------------------------------------*/
static void prvOneShotTimerCallback( timerHandleType xExpiredTimer )
{
    /* The parameter is not used in this case as only one timer uses this
     * callback function. */
    ( void ) xExpiredTimer;

    ucOneShotTimerCounter++;
}
/*---------------------------------------------------------------------------*/

static void prvISRPeriodicTimerCallback( timerHandleType xExpiredTimer )
{
    /* The parameter is not used in this case as only one timer uses this
     * callback function. */
    ( void ) xExpiredTimer;

    uxISRPeriodicTimerCounter++;
}
/*---------------------------------------------------------------------------*/

static void prvISROneShotTimerCallback( timerHandleType xExpiredTimer )
{
    /* The parameter is not used in this case as only one timer uses this
     * callback function. */
    ( void ) xExpiredTimer;

    uxISROneShotTimerCounter++;
}
/*---------------------------------------------------------------------------*/
