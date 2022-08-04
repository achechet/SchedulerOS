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

/* SafeRTOS Includes */
#include "SafeRTOS_API.h"

/* Program Includes */
#include "FullDemo.h"   /* Required for Hook Function Prototypes */

/* Firmware includes. */
#include "stm32l4xx_hal.h"

/* Linker Provided Memory Area Addresses */
extern portUInt32Type *__ICFEDIT_size_cstack__;


/* A linker defined symbol that gives the start address of the Idle task
 * data section. */
extern portUInt32Type __idle_hook_data_block__$$Base[];


/*-- Local CONSTANT Definitions -------------------------------------------*/

/* Scheduler Initialisation Definitions */
/* They indicate the location of both vector table and system stack
 * The system stack location is the first entry in the vector table
 * on Cortex-M4 devices) as well as size of the system stack in bytes. */

#define configSYSTEM_STACK_SIZE             ( ( portUInt32Type ) &__ICFEDIT_size_cstack__ )

/* The user configuration for the timer module. */
#define configTIMER_CMD_QUEUE_SIZ           ( configTIMER_CMD_QUEUE_LEN * sizeof( timerQueueMessageType ) )
#define configTIMER_CMD_QUEUE_BUFFER_SIZE   ( configTIMER_CMD_QUEUE_SIZ + portQUEUE_OVERHEAD_BYTES )

/* The size of the Idle task data section. */
#define configIDLE_TASK_DATA_SIZE           ( 0x20U )

/* MPU General Peripherals Hardware address range */
#define configPERIPHERALS_START_ADDRESS     ( 0x40000000U )
#define configPERIPHERALS_END_ADDRESS       ( 0x60000000U )

/*-- Local ENUM-TYPE Definitions ------------------------------------------*/

/*-- Local VARIABLE Declarations ------------------------------------------*/

#pragma data_alignment=configIDLE_TASK_STACK_SIZE
static portInt8Type acIdleTaskStack[ configIDLE_TASK_STACK_SIZE ] = { 0 };

/* Declare the stack for the timer task, it cannot be done in the timer
module as the syntax for alignment is port specific. Also the callback
functions are executed in the timer task and their complexity/stack
requirements are application specific. */
#pragma data_alignment=configTIMER_TASK_STACK_SIZE
static portInt8Type acTimerTaskStack[ configTIMER_TASK_STACK_SIZE ] = { 0 };

/* The buffer for the timer command queue. */
#pragma data_alignment=4
static portInt8Type acTimerCommandQueueBuffer[ configTIMER_CMD_QUEUE_BUFFER_SIZE ] = { 0 };

/* The structure passed to xTaskInitializeScheduler() to configure the kernel
 * with the application defined constants and call back functions. */
static xPORT_INIT_PARAMETERS xPortInit =
{
    configCPU_CLOCK_HZ,                 /* ulCPUClockHz */
    configTICK_RATE_HZ,                 /* ulTickRateHz */

    /* Hook Functions */
    NULL,                               /* pxSetupTickInterruptFunction */
    vDemoApplicationTaskDeleteHook,
    vDemoApplicationErrorHook,
    vDemoApplicationIdleHook,
    vDemoApplicationTickHook,
    vDemoApplicationSvcHook,            /* pxSvcHookFunction */
    xDemoApplicationInhibitSleepHook,   /* pxInhibitSleepHookFunction */
    vDemoApplicationPreSleepHook,       /* pxPreSleepHookFunction */
    vDemoApplicationPostSleepHook,      /* pxPostSleepHookFunction */

    /* System Stack parameters */
    0,                                  /* uxSystemStackSizeBytes */
    configSTACK_CHECK_MARGIN,           /* uxAdditionalStackCheckMarginBytes */

    /* Idle Task parameters */
    acIdleTaskStack,                    /* pcIdleTaskStackBuffer */
    configIDLE_TASK_STACK_SIZE,         /* uxIdleTaskStackSizeBytes */
    pdFALSE,                            /* The idle task will not use the FPU. */

    /* MPU Operating parameters */
    {                                 /* xIdleTaskMPUParameters */
        mpuUNPRIVILEGED_TASK,           /* The idle hook will be executed in unprivileged mode. */
        {
            {
                __idle_hook_data_block__$$Base,
                configIDLE_TASK_DATA_SIZE,
                ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |
                  portmpuREGION_EXECUTE_NEVER |
                  portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),
                0U
            },
            { NULL, 0U, 0U, 0U },
            { NULL, 0U, 0U, 0U }
        }
    },
    NULL,                                   /* No Thread Local Storage for the Idle Task. */

    /* Timer feature initialisation. */
    configTIMER_TASK_PRIORITY,              /* uxTimerTaskPriority */
    configTIMER_TASK_STACK_SIZE,            /* uxTimerTaskStackSize */
    acTimerTaskStack,                       /* pcTimerTaskStackBuffer */
    configTIMER_CMD_QUEUE_LEN,              /* uxTimerCommandQueueLength */
    configTIMER_CMD_QUEUE_BUFFER_SIZE,      /* uxTimerCommandQueueBufferSize */
    acTimerCommandQueueBuffer,              /* pcTimerCommandQueueBuffer */

    /* Tickless mode configuration. */
    configEXPECTED_IDLE_TIME_BEFORE_SLEEP,  /* xMaxIdleTime */
    portSTOP_ENTRY_WFI                      /* xStopModeEntry */
};

/*--------------------------------------------------------------------------*/

portBaseType  xInitializeScheduler( void )
{
    portBaseType xInitSchedResult;

    /* Set the system stack size in the xPortInit structure during run time
     * as the compiler complains about type conversion otherwise. */
    xPortInit.uxSystemStackSizeBytes = configSYSTEM_STACK_SIZE;

    /* Initialise the kernel by passing in a pointer to the xPortInit structure
     * and return the resulting error code. */
    xInitSchedResult = xTaskInitializeScheduler( &xPortInit );

    if( pdPASS == xInitSchedResult )
    {
        /* Configure a global region mapped onto the peripherals */
        xInitSchedResult = xMPUConfigureGlobalRegion( portmpuGLOBAL_CONFIGURABLE_REGION_FIRST,
                                                      configPERIPHERALS_START_ADDRESS,
                                                      ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |
                                                        portmpuREGION_EXECUTE_NEVER ),
                                                      ( configPERIPHERALS_END_ADDRESS - configPERIPHERALS_START_ADDRESS ),
                                                      portmpuREGION_ALL_SUB_REGIONS_ENABLED );
    }

    return xInitSchedResult;
}
/*---------------------------------------------------------------------------*/
