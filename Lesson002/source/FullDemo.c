/*
    Copyright (C)2006 onward WITTENSTEIN high integrity systems Limited. All rights reserved.

    This file is part of the SafeRTOS product, see projdefs.h for version number
    information.

    SafeRTOS is distributed exclusively by WITTENSTEIN high integrity systems,
    and is subject to the terms of the License granted to your organization,
    including its warranties and limitations on use and distribution. It cannot be
    copied or reproduced in any way except as permitted by the License.

    Licenses authorise use by processor, compiler, business unit, and product.

    WITTENSTEIN high integrity systems Limited, Registered Office: Brown's Court, Long Ashton
    Business Park, Yanley Lane, Long Ashton, Bristol, BS41 9LB, UK.
    Tel: +44 1275 395 600
    Email: info@highintegritysystems.com
    www.highintegritysystems.com
*/

/* SafeRTOS includes */
#include "SafeRTOS_API.h"

/* Demo application include files. These files are common to all demo applications. */
#include "death.h"
#include "flash.h"
#include "PollQ.h"
#include "BlockQ.h"
#include "dynamic.h"
#include "semtest.h"
#include "blocktim.h"
#include "countsem.h"
#include "TimerDemo.h"
#include "TaskNotify.h"
#include "MathsTest.h"
#include "comtest.h"
#include "TaskMutex.h"

/* Program includes */
#include "PortSpecifics.h"
#if ( INCLUDE_DEMO_DISPLAY == 1 )
    #include "DemoDisplay.h"
#endif
#include "ParTest.h"
#include "FullDemo.h"

#include "stm32l4xx_hal.h"

#if defined( USE_STM32L4XX_NUCLEO )
    #include "stm32l4xx_nucleo.h"
#endif


/*-----------------------------------------------------------------------------
 * Local constant definitions
 *---------------------------------------------------------------------------*/

/* Task priorities.
 * The lowest priority is always Zero ( 0 ).
 * The highest priority is defined by configMAX_PRIORITIES.
 * Any number of tasks can share the same priority.
 * Note, the timer task priority is set in "FullDemo.h". */
#define fulldemoCHECK_TASK_PRIORITY             ( 3U )
#define fulldemoBLOCKING_QUEUE_TASKS_PRIORITY   ( 1U )
#define fulldemoSUICIDAL_TASKS_PRIORITY         ( 1U )
#define fulldemoFLASH_LED_TASKS_PRIORITY        ( 1U )
#define fulldemoPOLLED_QUEUE_TASKS_PRIORITY     ( 0U )
#define fulldemoSEMAPHORE_TASKS_PRIORITY        ( 1U )
#define fulldemoCOM_TEST_TASK_PRIORITY          ( 2U )

/* LED indicators to be used by the Check task. */
#define fulldemoLED_CHECK_TASK                  ( 0U )

/* If many demo tasks are disabled, set this to zero to prevent the check
 * task from displaying an error due to low CPU time. */
#define fulldemoCHECK_IDLE_TIMES                ( 0 )

/* LED indicators to be used by the COM Test tasks.
 * Note that this is higher than the number of available LED
 * and hence this is effectively disabled. */
#define fulldemoLED_COM_TEST_TASK               ( 2U )

/* Check that the tick hook is being called at every tick.
 * NOTE: this won't work with tickless mode, as the tick hook is not called
 * while the system is sleeping. */
#define fulldemoCHECK_TICK_COUNT                ( 0 )

/* Demonstration task executable configuration.
 * Set to '1' to Enable or '0' to Disable */
#define fulldemoINCLUDE_FLASH_DEMO              ( 0 )
#define fulldemoINCLUDE_POLL_Q_DEMO             ( 0 )
#define fulldemoINCLUDE_BLOCK_Q_DEMO            ( 0 )
#define fulldemoINCLUDE_DYNAMIC_DEMO            ( 0 )
#define fulldemoINCLUDE_SEMA_COUNT_DEMO         ( 0 )
#define fulldemoINCLUDE_SEMA_BINARY_DEMO        ( 0 )
#define fulldemoINCLUDE_TIMER_DEMO              ( 0 )
#define fulldemoINCLUDE_SUICIDE_DEMO            ( 0 )
#define fulldemoINCLUDE_TASK_NOTIFY_DEMO        ( 0 )
#define fulldemoINCLUDE_SVC_TEST_DEMO           ( 0 )
#define fulldemoINCLUDE_REC_MUTEX_DEMO          ( 0 )

/* The block time demo is very processor intensive and can be disabled in
 * demo applications that use processors with lower clock speeds. */
#define fulldemoINCLUDE_BLOCK_TIME_DEMO         ( 0 )

/* The maths task demo is intended for processors that implement a floating
 * point unit (FPU). */
#define fulldemoINCLUDE_MATHS_DEMO              ( 0 )

/* The com test demo requires an external loopback. */
#define fulldemoINCLUDE_COM_TEST_DEMO           ( 0 )

/* Base periods of the timers used in the timer and task notify demo. */
#define fulldemoTIMER_TEST_PERIOD               ( 50U )
#define fulldemoTASK_NOTIFY_PERIOD              ( 50U )

/* Check task operational definitions. */
#define fulldemoCHECK_TASK_STACK_SIZE           ( 512UL )

#define fulldemoCHECK_TASK_NORMAL_CYCLE_RATE    ( ( portTickType ) 5000U / configTICK_RATE_MS )
#define fulldemoCHECK_TASK_ERROR_CYCLE_RATE     ( ( portTickType ) 1000U / configTICK_RATE_MS )

/* Check task error identifiers.
 * Used to keep track of errors that may have been detected. */
#define fulldemoCHECK_TASK_ERROR_MATHS_TASKS            ( 0x00000001UL )
#define fulldemoCHECK_TASK_ERROR_BLOCKING_QUEUES        ( 0x00000002UL )
#define fulldemoCHECK_TASK_ERROR_BLOCK_TIME_TEST        ( 0x00000004UL )
#define fulldemoCHECK_TASK_ERROR_COM_TEST_TASKS         ( 0x00000008UL )
#define fulldemoCHECK_TASK_ERROR_COUNTING_SEMAPHORES    ( 0x00000010UL )
#define fulldemoCHECK_TASK_ERROR_CREATE_TASKS           ( 0x00000020UL )
#define fulldemoCHECK_TASK_ERROR_DYNAMIC_PRIORITIES     ( 0x00000040UL )
#define fulldemoCHECK_TASK_ERROR_POLLING_QUEUES         ( 0x00000080UL )
#define fulldemoCHECK_TASK_ERROR_BINARY_SEMAPHORES      ( 0x00000100UL )
#define fulldemoCHECK_TASK_ERROR_TIMER_TASKS            ( 0x00000200UL )
#define fulldemoCHECK_TASK_ERROR_TASK_NOTIFY            ( 0x00000400UL )
#define fulldemoCHECK_TASK_ERROR_MUTEX                  ( 0x00000800UL )

#define fulldemoCHECK_TASK_ERROR_TICK_HOOK              ( 0x00001000UL )
#define fulldemoCHECK_TASK_ERROR_TICK_HOOK_COUNT        ( 0x00002000UL )
#define fulldemoCHECK_TASK_ERROR_IDLE_TASK              ( 0x00004000UL )
#define fulldemoCHECK_TASK_ERROR_SVC_COUNT              ( 0x00008000UL )

#define fulldemoCHECK_TASK_ERROR_INSUFFICIENT_IDLE      ( 0x00010000UL )
#define fulldemoCHECK_TASK_ERROR_EXCESSIVE_IDLE         ( 0x00020000UL )
#define fulldemoCHECK_TASK_ERROR_DELAY                  ( 0x00040000UL )

/* Idle task CPU Usage limits. */
#define fulldemoIDLE_TASK_MINIMUM_CPU_PERCENTAGE        ( 5UL )     /* 0.05% */
#define fulldemoIDLE_TASK_MAXIMUM_CPU_PERCENTAGE        ( 500UL )   /* 5.00% */


/******************************************************************************
 * Public Variable Declaration
 */



/*-----------------------------------------------------------------------------
 * Local Prototypes
 *---------------------------------------------------------------------------*/

static void prvCheckTask( void *pvCheckTaskParams );

#if ( fulldemoINCLUDE_SVC_TEST_DEMO == 1 )
    static void prvDemoApplicationSvcIncrValue( void );
#endif

/*-----------------------------------------------------------------------------
 * Local Variables
 *---------------------------------------------------------------------------*/

/* Incremented in the idle task hook so the check task
 * can ensure the idle task hook function is being called as expected. */
_Pragma("location=\"__idle_hook_zero_data__\"") static volatile portUInt32Type ulIdleHookCallCount = 0UL;

/* Incremented within the tick hook so the check task
 * can ensure the tick hook is being called. */
static volatile portUInt32Type ulTickHookCallCount = 0UL;

/* The check task needs to know the Idle task's handle,
 * so it can query its runtime statistics. */
_Pragma("location=\"__idle_hook_zero_data__\"") static portTaskHandleType xIdleTaskHandle = NULL;

_Pragma("location=\"__idle_hook_data__\"") static portBaseType xIdleTaskFirstExecution = pdTRUE;

/* A counter to test the SVC hook. */
static volatile portUnsignedBaseType uxSvcCounter = 0U;

#if ( INCLUDE_DEMO_DISPLAY == 1 )
    static const portCharType *pcStatusMessage = "PASS";
#endif

/* Declare task TCB:
 * Due to the use of the MPU background region, by default, all RAM can only
 * be accessed in privileged mode unless a specific MPU region has been setup
 * allowing unprivileged access. */
static xTCB xCheckTaskTCB = { 0 };

/* Declare task stacks :
 * These ARE protected by MPU regions so the alignment must follow the
 * MPU alignment rules, and basically be aligned to the same power of two
 * value as their length in bytes. */
#pragma data_alignment=fulldemoCHECK_TASK_STACK_SIZE
static portInt8Type acCheckTaskStack[ fulldemoCHECK_TASK_STACK_SIZE ] = { 0 };

/*-----------------------------------------------------------------------------
 * Public Function Definitions
 *---------------------------------------------------------------------------*/

portBaseType xFullDemoCreate( void )
{
    portBaseType xCreateResult;

    /* The structure passed to xTaskCreate() to create the check task. */
    xTaskParameters xCheckTaskParams =
    {
        prvCheckTask,                   /* The function that implements the task being created. */
        "CHECK TASK",                   /* The name of the task being created. The kernel does not use this itself, its just to assist debugging. */
        &xCheckTaskTCB,                 /* The TCB for the check task. */
        acCheckTaskStack,               /* The buffer allocated for use as the task stack. */
        fulldemoCHECK_TASK_STACK_SIZE,  /* The size of the buffer allocated for use as the task stack - note this is in BYTES! */
        NULL,                           /* The task parameter, not used in this case. */
        fulldemoCHECK_TASK_PRIORITY,    /* The priority to assigned to the task being created. */
        NULL,                           /* Thread Local Storage not used. */
        pdTRUE,                         /* Check task does use the FPU. */
        {                                /* MPU task parameters. */
            mpuPRIVILEGED_TASK,         /* Check task is privileged. */
            {
                { NULL, 0U, 0U, 0U },   /* No additional region definitions are required. */
                { NULL, 0U, 0U, 0U },
                { NULL, 0U, 0U, 0U }
            }
        }
    };

    /* Create the check task. */
    xCreateResult = xTaskCreate( &xCheckTaskParams, /* The structure containing the task parameters created at the start of this function. */
                                 NULL );            /* This parameter can be used to receive a handle to the created task, but is not used in this case. */

    /**************************************************************************
     * The first block of standard demo tasks are dependent on the hardware
     * capability of either the processor or target hardware and hence
     * inclusion is decided by project level settings.
     *************************************************************************/

#if ( fulldemoINCLUDE_COM_TEST_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xAltStartComTestTasks( fulldemoCOM_TEST_TASK_PRIORITY,
                                               fulldemoLED_COM_TEST_TASK );
    }
#endif

    /**************************************************************************
     * The second block of standard demo tasks are not dependent on the
     * hardware capability of either the processor or target hardware and
     * therefore their inclusion is a matter of preference and controlled
     * by settings in this file.
     *************************************************************************/

#if ( fulldemoINCLUDE_MATHS_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xStartMathsTasks();
    }
#endif

#if ( fulldemoINCLUDE_BLOCK_Q_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xStartBlockingQueueTasks( fulldemoBLOCKING_QUEUE_TASKS_PRIORITY );
    }
#endif

#if ( fulldemoINCLUDE_BLOCK_TIME_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xCreateBlockTimeTasks();
    }
#endif

#if ( fulldemoINCLUDE_DYNAMIC_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xStartDynamicPriorityTasks();
    }
#endif

#if ( fulldemoINCLUDE_FLASH_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xStartLEDFlashTasks( fulldemoFLASH_LED_TASKS_PRIORITY );
    }
#endif

#if ( fulldemoINCLUDE_POLL_Q_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xStartPolledQueueTasks( fulldemoPOLLED_QUEUE_TASKS_PRIORITY );
    }
#endif

#if ( fulldemoINCLUDE_SEMA_BINARY_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xStartSemaphoreTasks( fulldemoSEMAPHORE_TASKS_PRIORITY );
    }
#endif

#if ( fulldemoINCLUDE_SEMA_COUNT_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xStartCountingSemaphoreTasks();
    }
#endif

#if ( fulldemoINCLUDE_TIMER_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xStartTimerDemoTask( fulldemoTIMER_TEST_PERIOD,
                                             configTIMER_CMD_QUEUE_LEN,
                                             configTIMER_TASK_PRIORITY );
    }
#endif

#if ( fulldemoINCLUDE_TASK_NOTIFY_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xStartTaskNotifyTask( fulldemoTASK_NOTIFY_PERIOD,
                                              configTIMER_TASK_PRIORITY );
    }
#endif

#if ( fulldemoINCLUDE_REC_MUTEX_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xStartRecursiveMutexTasks();
    }
#endif
#if ( fulldemoINCLUDE_SUICIDE_DEMO == 1 )
    if( pdPASS == xCreateResult )
    {
        xCreateResult = xCreateSuicidalTasks( fulldemoSUICIDAL_TASKS_PRIORITY );
    }
#endif

    return xCreateResult;
}
/*---------------------------------------------------------------------------*/

void vDemoApplicationIdleHook( void )
{
    /* Is this the first time that the idle task hook has executed? */
    if( pdFALSE != xIdleTaskFirstExecution )
    {
        /* Yes, clear the flag. */
        xIdleTaskFirstExecution = pdFALSE;

        /* Now store a copy of our task handle so that the check task
         * can examine the runtime statistics of the idle task. */
        xIdleTaskHandle = xTaskGetCurrentTaskHandle();
    }

    /* Increment a counter so the check task can see that the idle task is
     * still running. */
    ulIdleHookCallCount++;
}
/*---------------------------------------------------------------------------*/

void vDemoApplicationTickHook( void )
{
    portBaseType xHigherPriorityTaskWoken = pdFALSE;

#if ( fulldemoINCLUDE_TIMER_DEMO == 1 )
    /* Call the periodic timer test, which tests the timer API functions that
     * can be called from an ISR. */
    if( xTimerPeriodicISRTests() != pdFALSE )
    {
        xHigherPriorityTaskWoken = pdTRUE;
    }
#endif

#if ( fulldemoINCLUDE_TASK_NOTIFY_DEMO == 1 )
    /* Call the periodic notify test, which tests the task notify API functions
     * that can be called from an ISR. */
    if( xNotifyTaskFromISR() != pdFALSE )
    {
        xHigherPriorityTaskWoken = pdTRUE;
    }
#endif

    /* Increment a counter so the check task can see that the tick hook is
     * being called. */
    ulTickHookCallCount++;

    taskYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/*---------------------------------------------------------------------------*/

void vDemoApplicationTaskDeleteHook( portTaskHandleType xTaskBeingDeleted )
{
    /* The parameter is not used, this line prevents a compiler warning. */
    ( void ) xTaskBeingDeleted;

    /* Increment a counter each time a task is deleted.
     * The suicide tasks use this counter to ensure the expected number of
     * tasks have been deleted. */
    uxTaskDeleteCallCount++;
}
/*---------------------------------------------------------------------------*/

void vDemoApplicationErrorHook( portTaskHandleType xHandleOfTaskWithError,
                                const portCharType *pcErrorString,
                                portBaseType xErrorCode )
{
    /* The parameters are not used, these lines prevent compiler warnings. */
    ( void ) xHandleOfTaskWithError;
    ( void ) pcErrorString;
    ( void ) xErrorCode;

    /* Switch All LED Indicators ON! */
    vParTestSetLED( 0, pdTRUE );
    vParTestSetLED( 1, pdTRUE );
    vParTestSetLED( 2, pdTRUE );
    vParTestSetLED( 3, pdTRUE );

    /* Will only get here if an internal kernel error occurs. */
    for( ;; );
}
/*---------------------------------------------------------------------------*/

void vDemoApplicationSvcHook( portUnsignedBaseType uxSvcNumber )
{
    switch( uxSvcNumber )
    {
        case fulldemoSVC_INCREMENT_VALUE:
            uxSvcCounter++;
            break;
        default:
            break;
    }
}
/*---------------------------------------------------------------------------*/

portBaseType xDemoApplicationInhibitSleepHook( void )
{
    portBaseType xReturn;

    /* User can add his own implementation to inhibit the system from entering sleep mode */
    xReturn = pdFALSE;

    return xReturn;
}
/*---------------------------------------------------------------------------*/

void vDemoApplicationPreSleepHook( void )
{
    /* Pre-sleep configuration */


    /* Set Stop mode 2 */
    MODIFY_REG(PWR->CR1, PWR_CR1_LPMS, PWR_CR1_LPMS_STOP2);

    /* Set SLEEPDEEP bit of Cortex System Control Register */
    SET_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
}
/*---------------------------------------------------------------------------*/

void vDemoApplicationPostSleepHook( void )
{
    /* Post-sleep configuration */
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    /* Enable the main PLL. */
    __HAL_RCC_PLL_ENABLE();

    /* Enable PLL System Clock output. */
    __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL_SYSCLK);

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     * clocks dividers. */
    RCC_ClkInitStruct.ClockType = ( RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 );
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if( HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_4 ) != HAL_OK )
    {
        /* Initialisation error. */
        while(1);
    }

    /* Disable the SYSTICK and clear any pending interrupts. The systick is
    automatically started by HAL_RCC_ClockConfig(). */
    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;


    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
}
/*---------------------------------------------------------------------------*/

static void prvCheckTask( void *pvParameters )
{
    portBaseType xCheckStatus;
    portTickType xLastTime;
    portTickType xCheckTaskCycleRate;
    portUInt32Type ulErrorsDetected = 0U;
    portUInt32Type ulIdleHookLastCount = 0U;
    portUInt32Type ulTickHookLastCount = 0U;
    xPERCENTAGES xIdleTaskPercentages = { { 0U, 0U }, { 0U, 0U } };
#if ( fulldemoINCLUDE_SVC_TEST_DEMO == 1 )
    portUnsignedBaseType uxCachedSvcCounter = 0U;
#endif

    /* The parameters are not used in this task. This just prevents a compiler
     * warning. */
    ( void ) pvParameters;

    /* Switch Check LED ON to show we good to go. */
    vParTestSetLED( fulldemoLED_CHECK_TASK, pdTRUE );

    /* No errors have been detected yet. */
    xCheckTaskCycleRate = fulldemoCHECK_TASK_NORMAL_CYCLE_RATE;

    /* Initialise the variable used in calls to xTaskDelayUntil() to control
     * the tasks execution period to the current time. */
    xLastTime = xTaskGetTickCount();

    /* Forever loop... */
    for( ;; )
    {
        /* Delay until the next check time. */
        xCheckStatus = xTaskDelayUntil( &xLastTime, xCheckTaskCycleRate );
        if( pdPASS != xCheckStatus )
        {
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_DELAY;
        }

        /**********************************************************************
         * Check Demo Test operations.
         **********************************************************************
         * Ask each set of demo tasks to report their status.
         **********************************************************************
         * This set of demo tasks reported an error.
         * Send an error message to the display task and latch that an error
         * has occurred during this loop.
         * Note that only a pointer to the error message is actually queued so
         * this assumes the string is in const, non-stack memory.
         * This assumption is fine for a simple demo, but not for a high
         * integrity application!
         *********************************************************************/

        /**********************************************************************
         * The first block of standard demo tasks are dependent on the hardware
         * capability of either the processor or target hardware and hence
         * inclusion is decided by project level settings.
         *********************************************************************/

#if ( fulldemoINCLUDE_COM_TEST_DEMO == 1 )
        /* Check COM Test tasks. */
        xCheckStatus = xAreComTestTasksStillRunning();
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: COM TEST";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_COM_TEST_TASKS;
        }
#endif

#if ( fulldemoINCLUDE_SVC_TEST_DEMO == 1 )
        /* Check SVC demo.*/
        prvDemoApplicationSvcIncrValue();
        if( uxCachedSvcCounter == uxSvcCounter )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: SVC TEST";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_SVC_COUNT;
        }
        else
        {
            uxCachedSvcCounter = uxSvcCounter;
        }
#endif

        /**********************************************************************
         * The second block of standard demo tasks are not dependent on the
         * hardware capability of either the processor or target hardware and
         * therefore their inclusion is a matter of preference and controlled
         * by settings in this file.
         *********************************************************************/

#if ( fulldemoINCLUDE_MATHS_DEMO == 1 )
        /* Check Math Test tasks. */
        xCheckStatus = xAreMathsTasksStillRunning();
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: MATHS TEST";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_MATHS_TASKS;
        }
#endif

#if ( fulldemoINCLUDE_TIMER_DEMO == 1 )
        xCheckStatus = xAreTimerDemoTasksStillRunning( xCheckTaskCycleRate,
                                                       configTIMER_CMD_QUEUE_LEN );
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: TIMERS TEST";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_TIMER_TASKS;
        }
#endif

#if ( fulldemoINCLUDE_TASK_NOTIFY_DEMO == 1 )
        /* Check Task Notify demo. */
        xCheckStatus = xAreTaskNotificationTasksStillRunning();
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: TASK NOTIFY TEST";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_TASK_NOTIFY;
        }
#endif

#if ( fulldemoINCLUDE_BLOCK_Q_DEMO == 1 )
        /* Check Blocking Queue tasks. */
        xCheckStatus = xAreBlockingQueuesStillRunning();
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: BLOCK Q";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_BLOCKING_QUEUES;
        }
#endif

#if ( fulldemoINCLUDE_BLOCK_TIME_DEMO == 1 )
        /* Check Blocking Time tasks. */
        xCheckStatus = xAreBlockTimeTestTasksStillRunning();
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: BLOCK TIME";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_BLOCK_TIME_TEST;
        }
#endif

#if ( fulldemoINCLUDE_SEMA_COUNT_DEMO == 1 )
        /* Check Counting Semaphore tasks. */
        xCheckStatus = xAreCountingSemaphoreTasksStillRunning();
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: COUNTSEM";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_COUNTING_SEMAPHORES;
        }
#endif

#if ( fulldemoINCLUDE_SUICIDE_DEMO == 1 )
        /* Check Suicide tasks. */
        xCheckStatus = xIsCreateTaskStillRunning();
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: CREATE";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_CREATE_TASKS;
        }
#endif

#if ( fulldemoINCLUDE_DYNAMIC_DEMO == 1 )
        /* Check Dynamic tasks. */
        xCheckStatus = xAreDynamicPriorityTasksStillRunning();
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: DYNAMIC";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_DYNAMIC_PRIORITIES;
        }
#endif

#if ( fulldemoINCLUDE_POLL_Q_DEMO == 1 )
        /* Check Polled Queue tasks. */
        xCheckStatus = xArePollingQueuesStillRunning();
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: POLL Q";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_POLLING_QUEUES;
        }
#endif

#if ( fulldemoINCLUDE_SEMA_BINARY_DEMO == 1 )
        /* Check Binary Semaphore tasks. */
        xCheckStatus = xAreSemaphoreTasksStillRunning();
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: SEMAPHORE";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_BINARY_SEMAPHORES;
        }
#endif

#if ( fulldemoINCLUDE_REC_MUTEX_DEMO == 1 )
        xCheckStatus = xAreRecursiveMutexTasksStillRunning();
        if( pdTRUE != xCheckStatus )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: MUTEX";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_MUTEX;
        }
#endif
        /**********************************************************************
         * Check SafeRTOS operating parameters.
         *********************************************************************/

        /* Check that the Tick Hook is being called. */
        if( ulTickHookCallCount == ulTickHookLastCount )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: TICK HOOK";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_TICK_HOOK;
        }
        ulTickHookLastCount = ulTickHookCallCount;

        /* Finally, check that the idle task is getting called. */
        if( ulIdleHookCallCount == ulIdleHookLastCount )
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "ERR: IDLE HOOK";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_IDLE_TASK;
        }
        ulIdleHookLastCount = ulIdleHookCallCount;

        /* Retrieve the runtime statistics associated with the idle task. */
        if( NULL != xIdleTaskHandle )
        {
            xCheckStatus = xCalculateCPUUsage( xIdleTaskHandle, &xIdleTaskPercentages );
            if( pdPASS == xCheckStatus )
            {
                /* It is now possible to inspect the statistics to ensure that
                 * the expected percentage of CPU time is being spent in the
                 * Idle task - this will be different for each application. */
#if( fulldemoCHECK_IDLE_TIMES == 1 )

                if( xIdleTaskPercentages.xPeriod.ulCurrent < fulldemoIDLE_TASK_MINIMUM_CPU_PERCENTAGE )
                {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
                    pcStatusMessage = "ERR: INSUFFICIENT IDLE TIME";
                    xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
                    ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_INSUFFICIENT_IDLE;
                }

                if( xIdleTaskPercentages.xOverall.ulMax > fulldemoIDLE_TASK_MAXIMUM_CPU_PERCENTAGE )
                {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
                    pcStatusMessage = "ERR: EXCESSIVE IDLE TIME";
                    xQueueSend( xDisplayQueue, &pcStatusMessage, 0 );
#endif
                    ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_EXCESSIVE_IDLE;
                }
#endif /* ( fulldemoCHECK_IDLE_TIMES == 1 ) */
            }
        }

#if( fulldemoCHECK_TICK_COUNT == 1 )
        /* Check that the tick hook has been called at every tick. */
        if( xTaskGetTickCount() != ulTickHookCallCount )
        {
            ulErrorsDetected |= fulldemoCHECK_TASK_ERROR_TICK_HOOK_COUNT;
        }
#endif

        /* If an error has been detected... toggle check LED faster
         * to give a visible indication of the error detection. */
        if( 0UL != ulErrorsDetected )
        {
            xCheckTaskCycleRate = fulldemoCHECK_TASK_ERROR_CYCLE_RATE;
        }
        else
        {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
            pcStatusMessage = "PASS";
            xQueueSend( xDisplayQueue, &pcStatusMessage, 0U );
#endif
        }

        /* Toggle the check task LED. */
        vParTestToggleLED( fulldemoLED_CHECK_TASK );
    }
}
/*---------------------------------------------------------------------------*/

#if ( fulldemoINCLUDE_SVC_TEST_DEMO == 1 )
static void prvDemoApplicationSvcIncrValue( void )
{
    __asm volatile
    (
        "	svc %[NUM]	"
        ::  [NUM] "i" ( fulldemoSVC_INCREMENT_VALUE )
        :   "LR"
    );
}
#endif
/*---------------------------------------------------------------------------*/
