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

#ifndef PORT_SPECIFICS_H
#define PORT_SPECIFICS_H



/*-----------------------------------------------------------------------------
 * Constants common to all demo tasks
 *---------------------------------------------------------------------------*/

/* This constant is used where it is desirable to locate all task TCBs in a
 * specified area of memory.
 * Not required for this product variant.
 */
#define portspecTCB_DATA_SECTION

/* This constant is used where it is desirable to locate all common privileged
 * demo data in a specified area of memory.
 * Not required for this product variant.
 */
#define portspecCOMMON_PRIV_DATA_SECTION

/*-----------------------------------------------------------------------------
 * Maths Test task parameters
 *---------------------------------------------------------------------------*/

/* Stack size */
#define portspecMATHS_TEST_STACK_SIZE   ( configMINIMAL_STACK_SIZE_WITH_FPU )

/* Task stacks. */
extern portInt8Type acUnprivilegedTask1Stack[];
extern portInt8Type acUnprivilegedTask2Stack[];
extern portInt8Type acUnprivilegedTask3Stack[];
extern portInt8Type acUnprivilegedTask4Stack[];

extern portInt8Type acPrivilegedTask1Stack[];
extern portInt8Type acPrivilegedTask2Stack[];
extern portInt8Type acPrivilegedTask3Stack[];
extern portInt8Type acPrivilegedTask4Stack[];

#ifdef MATHS_TEST_C

/* MPU parameters */
/* Define portspecMATHS_TEST_DATA_SECTION
 * so that all Maths Test static data is in the same section. */
#define portspecMATHS_TEST_DATA_SECTION     _Pragma("location=\"__maths_test_data__\"")

#define portspecMATHS_TEST_DATA_SIZE        ( 0x40 )

/* Linker Defined Symbol for Reserved Data Section Start Address */
extern portUInt32Type __maths_test_data_block__$$Base[];

/* This definition corresponds to the xMPUParameters structure that is passed
 * to xTaskCreate() as part of the xTaskParameters structure when the Maths
 * Test tasks are created. It defines the privilege level and region
 * definitions for the Maths test tasks. All the Blocking Queue tasks run
 * in Unprivileged (User) mode with 1 additional region that corresponds to the
 * linker section that holds the Blocking Queue static data. */
#define portspecMATHS_PRIV_NO_REGION_PARMS          \
pdTRUE,                                             \
{                                                   \
    mpuPRIVILEGED_TASK,                             \
    {                                               \
        { NULL, 0U, 0U, 0U },                       \
        { NULL, 0U, 0U, 0U },                       \
        { NULL, 0U, 0U, 0U }                        \
    }                                               \
}

#define portspecMATHS_UNPRIV_REGION_PARMS                                           \
pdTRUE,                                                                             \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __maths_test_data_block__$$Base,                                        \
            portspecMATHS_TEST_DATA_SIZE,                                           \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xMathsPrivTask1PortParameters       portspecMATHS_PRIV_NO_REGION_PARMS
#define xMathsPrivTask2PortParameters       portspecMATHS_PRIV_NO_REGION_PARMS
#define xMathsPrivTask3PortParameters       portspecMATHS_PRIV_NO_REGION_PARMS
#define xMathsPrivTask4PortParameters       portspecMATHS_PRIV_NO_REGION_PARMS

#define xMathsUnprivTask1PortParameters     portspecMATHS_UNPRIV_REGION_PARMS
#define xMathsUnprivTask2PortParameters     portspecMATHS_UNPRIV_REGION_PARMS
#define xMathsUnprivTask3PortParameters     portspecMATHS_UNPRIV_REGION_PARMS
#define xMathsUnprivTask4PortParameters     portspecMATHS_UNPRIV_REGION_PARMS

#endif /* MATHS_TEST_C */

/*-----------------------------------------------------------------------------
 * Queue Blocking task parameters
 *---------------------------------------------------------------------------*/

/* Blocking Queue task constants */
#define portspecBLOCK_Q_STACK_SIZE          ( configMINIMAL_STACK_SIZE_NO_FPU )
#define portspecBLOCK_Q_QUEUE_LENGTH_1      ( 1U )
#define portspecBLOCK_Q_QUEUE_LENGTH_5      ( 5U )
#define portspecBLOCK_Q_QUEUE_ITEM_SIZE     ( sizeof( portUInt16Type ) )
#define portspecBLOCK_Q_BUFFER_LENGTH_1     ( ( portspecBLOCK_Q_QUEUE_LENGTH_1 * portspecBLOCK_Q_QUEUE_ITEM_SIZE ) + portQUEUE_OVERHEAD_BYTES )
#define portspecBLOCK_Q_BUFFER_LENGTH_5     ( ( portspecBLOCK_Q_QUEUE_LENGTH_5 * portspecBLOCK_Q_QUEUE_ITEM_SIZE ) + portQUEUE_OVERHEAD_BYTES )

/* Blocking Queue task stacks */
extern portInt8Type acQueueTestTask1Stack[];
extern portInt8Type acQueueTestTask2Stack[];
extern portInt8Type acQueueTestTask3Stack[];
extern portInt8Type acQueueTestTask4Stack[];
extern portInt8Type acQueueTestTask5Stack[];
extern portInt8Type acQueueTestTask6Stack[];

/* Queue buffers */
extern portInt8Type acQueue1Buffer[];
extern portInt8Type acQueue2Buffer[];
extern portInt8Type acQueue3Buffer[];

#ifdef BLOCK_Q_C

/* MPU parameters */
/* Define portspecBLOCK_Q_DATA_SECTION
 * so that all Blocking Queue static data is in the same section. */
#define portspecBLOCK_Q_DATA_SECTION    _Pragma("location=\"__block_q_data__\"")

/* The size of the Blocking Queue data section. */
#define portspecBLOCK_Q_DATA_SIZE       ( 0x80 )

/* A linker defined symbol that gives the start address of the Blocking Queue
data section. */
extern portUInt32Type __block_q_data_block__$$Base[];

/* This definition corresponds to the xMPUParameters structure that is passed
 * to xTaskCreate() as part of the xTaskParameters structure when the Blocking
 * Queue tasks are created. It defines the privilege level and region
 * definitions for the Blocking Queue tasks. All the Blocking Queue tasks run
 * in Unprivileged (User) mode with 1 additional region that corresponds to the
 * linker section that holds the Blocking Queue static data. */
#define portspecBLOCK_Q_TASK_PARAMETERS                                             \
pdFALSE,                                                                            \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __block_q_data_block__$$Base,                                           \
            portspecBLOCK_Q_DATA_SIZE,                                              \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xBlockingQueueConsumerTask1PortParameters   portspecBLOCK_Q_TASK_PARAMETERS
#define xBlockingQueueProducerTask1PortParameters   portspecBLOCK_Q_TASK_PARAMETERS
#define xBlockingQueueConsumerTask2PortParameters   portspecBLOCK_Q_TASK_PARAMETERS
#define xBlockingQueueProducerTask2PortParameters   portspecBLOCK_Q_TASK_PARAMETERS
#define xBlockingQueueProducerTask3PortParameters   portspecBLOCK_Q_TASK_PARAMETERS
#define xBlockingQueueConsumerTask3PortParameters   portspecBLOCK_Q_TASK_PARAMETERS

#endif /* BLOCK_Q_C */

/*-----------------------------------------------------------------------------
 * Block Time Test task parameters
 *---------------------------------------------------------------------------*/

/* Block Time Test task constants */
#define portspecBLOCK_TIME_STACK_SIZE       ( 512U )
#define portspecBLOCK_TIME_QUEUE_LENGTH     ( 5U )
#define portspecBLOCK_TIME_QUEUE_ITEM_SIZE  ( sizeof( portBaseType ) )
#define portspecBLOCK_TIME_BUFFER_LENGTH    ( ( portspecBLOCK_TIME_QUEUE_LENGTH * portspecBLOCK_TIME_QUEUE_ITEM_SIZE ) + portQUEUE_OVERHEAD_BYTES )

/* Block Time Test task stacks */
extern portInt8Type acBlockTimeTestTask1Stack[];
extern portInt8Type acBlockTimeTestTask2Stack[];

/* The buffer for the queue to use. */
extern portInt8Type acQueueBuffer[];

#ifdef BLOCK_TIME_TEST_C

/* MPU parameters */
/* Define portspecBLOCK_TIME_TEST_DATA_SECTION
 * so that all Block Time Test static data is in the same section. */
#define portspecBLOCK_TIME_TEST_DATA_SECTION    _Pragma("location=\"__block_tim_data__\"")

/* The size of the Block Time Test data section. */
#define portspecBLOCK_TIME_TEST_DATA_SIZE       ( 0x20 )

/* A linker defined symbol that gives the start address of the Block Time Test
data section. */
extern portUInt32Type __block_tim_data_block__$$Base[];

/* This definition corresponds to the xMPUParameters structure that is passed
 * to xTaskCreate() as part of the xTaskParameters structure when the Block
 * Time Test tasks are created. It defines the privilege level and region
 * definitions for the Block Time Test tasks. All the Block Time Test tasks run
 * in Unprivileged (User) mode with 1 additional region that corresponds to the
 * linker section that holds the Block Time Test static data. */
#define portspecBLOCK_TIME_TEST_PARAMETERS                                          \
pdFALSE,                                                                            \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __block_tim_data_block__$$Base,                                         \
            portspecBLOCK_TIME_TEST_DATA_SIZE,                                      \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xPrimaryBlockTimeTestTaskPortParameters     portspecBLOCK_TIME_TEST_PARAMETERS
#define xSecondaryBlockTimeTestTaskPortParameters   portspecBLOCK_TIME_TEST_PARAMETERS

#endif /* BLOCK_TIME_TEST_C */

/******************************************************************************
 * Com Test task parameters
 *****************************************************************************/

/* Com Test task constants */
#define portspecCOM_TEST_TASK_STACK_SIZE    ( 512UL )

/* Task stacks */
extern portInt8Type acTxStack[];
extern portInt8Type acRxStack[];

#ifdef COMTEST_C

/* MPU parameters */
/* Define portspecCOM_TEST_DATA_SECTION and portspecCOM_TEST_ZERO_DATA_SECTION
 * so that all Com Test static data is in the same section. */
#define portspecCOM_TEST_DATA_SECTION       _Pragma("location=\"__comtest_task_data__\"")
#define portspecCOM_TEST_ZERO_DATA_SECTION  _Pragma("location=\"__comtest_task_zero_data__\"")

/* The size of the Com Test data section. */
#define portspecCOM_TEST_DATA_SIZE                          ( 0x80 )

/* A linker defined symbol that gives the start address
 * of the ComTest data section. */
extern portUInt32Type __comtest_task_data_block__$$Base[];

/* This definition corresponds to the xUsingFPU and xMPUParameters structure
 * members that are passed to xTaskCreate() as part of the xTaskParameters
 * structure when the Com Test tasks are created. It defines the privilege
 * level and region definitions for the Com Test tasks as well as informing the
 * scheduler that the FPU will not be used by these tasks. All the Com Test
 * tasks run in Unprivileged (User) mode with 1 additional region which
 * corresponds to the linker section that holds the Com Test tasks static
 * data. */
#define portspecCOM_TEST_PARAMETERS                                     \
pdFALSE,                                                                \
{                                                                       \
    mpuUNPRIVILEGED_TASK,                                               \
    {                                                                   \
        {                                                               \
            __comtest_task_data_block__$$Base,                          \
            portspecCOM_TEST_DATA_SIZE,                                 \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0UL                                                         \
        },                                                              \
        { NULL, 0UL, 0UL, 0UL },                                        \
        { NULL, 0UL, 0UL, 0UL }                                         \
    }                                                                   \
}

#define xComTxTaskPortParameters    portspecCOM_TEST_PARAMETERS
#define xComRxTaskPortParameters    portspecCOM_TEST_PARAMETERS

#endif /* COMTEST_C */

/******************************************************************************
 * Counting Semaphore Test task parameters
 *---------------------------------------------------------------------------*/

/* Counting Semaphore Test task constants. */
#define portspecCOUNTSEM_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE_NO_FPU )
#define portspecCOUNTSEM_TASK3_STACK_SIZE       ( 512U )

/* Counting Semaphore Test Task Stacks */
extern portInt8Type acCountSemTestTask1Stack[];
extern portInt8Type acCountSemTestTask2Stack[];
extern portInt8Type acCountSemTestTask3Stack[];

/* Counting Semaphore Test Semaphore buffers.
 * No actual data is stored into these buffers so the buffer
 * need only to be large enough to hold the queue structure itself. */
extern portInt8Type acCountSem1[];
extern portInt8Type acCountSem2[];
extern portInt8Type acCountSem3[];

#ifdef COUNTING_SEMAPHORE_TEST_C

/* MPU parameters */
/* Define portspecCOUNT_SEM_TASK_DATA_SECTION and portspecCOUNTSEM_TASK_ZERO_DATA_SECTION
 * so that all Counting Semaphore static data is in the same section. */
#define portspecCOUNTSEM_TASK_DATA_SECTION      _Pragma("location=\"__counting_semaphore_task_data__\"")
#define portspecCOUNTSEM_TASK_ZERO_DATA_SECTION _Pragma("location=\"__counting_semaphore_task_zero_data__\"")

/* The size of the Counting Semaphore Test data section. */
#define portspecCOUNTSEM_TASK_DATA_SIZE         ( 0x40 )

/* A linker defined symbol that gives the start address of the Counting
Semaphore Test data section. */
extern portUInt32Type __counting_semaphore_task_data_block__$$Base[];

/* This definition corresponds to the xMPUParameters structure that is passed to
 * xTaskCreate() as part of the xTaskParameters structure when the Counting
 * Semaphore tasks are created. It defines the privilege level and region
 * definitions for the Counting Semaphore tasks. All the Counting Semaphore tasks
 * run in Unprivileged (User) mode with 1 additional region that corresponds to the
 * linker section that holds the Counting Semaphore static data. */
#define portspecCOUNTING_SEMAPHORE_PARAMETERS                                       \
pdFALSE,                                                                            \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __counting_semaphore_task_data_block__$$Base,                           \
            portspecCOUNTSEM_TASK_DATA_SIZE,                                        \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define portspecINTERRUPT_SEMAPHORE_PARAMETERS  \
pdFALSE,                                        \
{                                               \
    mpuPRIVILEGED_TASK,                         \
    {                                           \
        { NULL, 0U, 0U, 0U },                   \
        { NULL, 0U, 0U, 0U },                   \
        { NULL, 0U, 0U, 0U }                    \
    }                                           \
}

#define portspecINTERRUPT_MPU_UPDATE_PARAMETERS                                     \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __counting_semaphore_task_data_block__$$Base,                           \
            portspecCOUNTSEM_TASK_DATA_SIZE,                                        \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xCountSemaphoreTestTask1PortParameters      portspecCOUNTING_SEMAPHORE_PARAMETERS
#define xCountSemaphoreTestTask2PortParameters      portspecCOUNTING_SEMAPHORE_PARAMETERS
#define xCountSemaphoreTestTask3PortParameters      portspecINTERRUPT_SEMAPHORE_PARAMETERS
#define xCountSemaphoreTestTask3MpuUpdParameters    portspecINTERRUPT_MPU_UPDATE_PARAMETERS

#endif /* COUNTING_SEMAPHORE_TEST_C */

/*-----------------------------------------------------------------------------
 * Create Delete Test task parameters
 *---------------------------------------------------------------------------*/

/* Create Delete Test task constants */
#define portspecCREATE_TASK_STACK_SIZE      ( 512U )
#define portspecSUICIDAL_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE_NO_FPU )

/* Task stacks */
extern portInt8Type acCreateTaskStack[];
extern portInt8Type acSuicidalTask1Stack[];
extern portInt8Type acSuicidalTask2Stack[];

#ifdef SUICIDE_TASK_C

/* MPU parameters */
/* Define portspecCREATE_DELETE_DATA_SECTION
 * so that all Create Delete Test static data is in the same section. */
#define portspecCREATE_DELETE_DATA_SECTION  _Pragma("location=\"__create_delete_data__\"")

/* The size of the Create Delete Test data section. */
#define portspecCREATE_DELETE_DATA_SIZE     ( 0x20 )

/* A linker defined symbol that gives the start address of the Create Delete
Test data section. */
extern portUInt32Type __create_delete_data_block__$$Base[];

/* This definition corresponds to the xMPUParameters structure that is passed
 * to xTaskCreate() as part of the xTaskParameters structure when the Create
 * Delete Test tasks are created. It defines the privilege level and region
 * definitions for the Create Delete Test tasks. All the Create Delete Test
 * tasks run in Unprivileged (User) mode with 1 additional region that
 * corresponds to the linker section that holds the Create Delete Test static
 * data. */
#define portspecCREATE_DELETE_TASK_PARAMETERS                                       \
pdFALSE,                                                                            \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __create_delete_data_block__$$Base,                                     \
            portspecCREATE_DELETE_DATA_SIZE,                                        \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xCreateTaskPortParameters       portspecCREATE_DELETE_TASK_PARAMETERS
#define xSuicidalTask1PortParameters    portspecCREATE_DELETE_TASK_PARAMETERS
#define xSuicidalTask2PortParameters    portspecCREATE_DELETE_TASK_PARAMETERS

#endif /* SUICIDE_TASK_C */

/*-----------------------------------------------------------------------------
 * Dynamic task parameters
 *---------------------------------------------------------------------------*/

/* Dynamic task constants */
#define portspecDYNAMIC_TASK_STACK_SIZE                     ( configMINIMAL_STACK_SIZE_NO_FPU )
#define portspecDYNAMIC_TASK_SUSPENDED_QUEUE_LENGTH         ( 1U )
#define portspecDYNAMIC_TASK_QUEUE_ITEM_SIZE                ( sizeof( portUInt32Type ) )
#define portspecDYNAMIC_TASK_SUSPEND_QUEUE_BUFFER_LENGTH    ( ( portspecDYNAMIC_TASK_SUSPENDED_QUEUE_LENGTH * portspecDYNAMIC_TASK_QUEUE_ITEM_SIZE ) + portQUEUE_OVERHEAD_BYTES )

/* Dynamic task stacks */
extern portInt8Type acContinuousIncrementTaskStack[];
extern portInt8Type acLimitedIncrementTaskStack[];
extern portInt8Type acCounterControlTaskStack[];
extern portInt8Type acQueueSendWhenSuspendedTaskStack[];
extern portInt8Type acQueueReceiveWhenSuspendedTaskStack[];

/* Dynamic Task Queue buffer */
extern portInt8Type acSuspendTestQueueBuffer[];

#ifdef DYNAMIC_MANIPULATION_C

/* MPU parameters */
/* Define portspecDYNAMIC_TASK_DATA_SECTION
 * so that all Dynamic Task static data is in the same section. */
#define portspecDYNAMIC_TASK_DATA_SECTION   _Pragma("location=\"__dynamic_task_data__\"")

/* The size of the Dynamic Task data section. */
#define portspecDYNAMIC_TASK_DATA_SIZE      ( 0x40 )

/* A linker defined symbol that gives the start address of the Dynamic Task data
section. */
extern portUInt32Type __dynamic_task_data_block__$$Base[];

/* This definition corresponds to the xMPUParameters structure that is passed
 * to xTaskCreate() as part of the xTaskParameters structure when the Dynamic
 * Tasks are created. It defines the privilege level and region definitions for
 * the Dynamic Tasks. All the Dynamic Tasks run in Unprivileged (User) mode
 * with 1 additional region that corresponds to the linker section that holds
 * the Dynamic Task static data. */
#define portspecDYNAMIC_TASK_PARAMETERS                                             \
pdFALSE,                                                                            \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __dynamic_task_data_block__$$Base,                                      \
            portspecDYNAMIC_TASK_DATA_SIZE,                                         \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xContinuousIncrementTaskPortParameters          portspecDYNAMIC_TASK_PARAMETERS
#define xLimitedIncrementTaskPortParameters             portspecDYNAMIC_TASK_PARAMETERS
#define xCounterControlTaskPortParameters               portspecDYNAMIC_TASK_PARAMETERS
#define xQueueSendWhenSuspendedTaskPortParameters       portspecDYNAMIC_TASK_PARAMETERS
#define xQueueReceiveWhenSuspendedTaskPortParameters    portspecDYNAMIC_TASK_PARAMETERS

#endif /* DYNAMIC_MANIPULATION_C */

/*-----------------------------------------------------------------------------
 * LED Flash task parameters
 *---------------------------------------------------------------------------*/


/* Structure used to pass parameters to the LED Flash tasks. */
typedef struct LED_FLASH_TASK_PARAMETERS
{
    portUnsignedBaseType uxLEDNumber;   /* The LED to be flashed. */
    portTickType xFlashRate_ms;         /* The flash rate in milliseconds. */
} xLedFlashTaskParameters;

/* LED Flash Task Constants */
#define portspecNUMBER_OF_LEDS          ( 3UL )
#define portspecLED_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE_NO_FPU )

/* LED Flash stacks */
extern portInt8Type acLedTaskStack[ portspecNUMBER_OF_LEDS ][ portspecLED_TASK_STACK_SIZE ];

/* Task pvParameters */
extern xLedFlashTaskParameters xLedTaskParameters[ portspecNUMBER_OF_LEDS ];

/* MPU parameters */
/* Define portspecLED_TASK_DATA_SECTION
 * so that all LED Task static data is in the same section. */
#define portspecLED_TASK_DATA_SECTION   _Pragma("location=\"__led_task_data__\"")

/* The size of the LED Task data section. */
#define portspecLED_TASK_DATA_SIZE      ( 0x20 )

#ifdef FLASH_LED_C

/* A linker defined symbol that gives the start address of the LED Task data
 * section. */
extern portUInt32Type __led_task_data_block__$$Base[];

/* This definition corresponds to the xMPUParameters structure that is passed to
 * xTaskCreate() as part of the xTaskParameters structure when the LED Tasks are
 * created. It defines the privilege level and region definitions for the LED
 * Tasks. All the LED Tasks run in Unprivileged (User) mode with 1 additional
 * region that corresponds to the linker section that holds the LED Task static
 * data. */
#define portspecLED_FLASH_PARAMETERS                                                \
pdFALSE,                                                                            \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __led_task_data_block__$$Base,                                          \
            portspecLED_TASK_DATA_SIZE,                                             \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xLEDFlashTaskPortParameters     portspecLED_FLASH_PARAMETERS

#endif /* FLASH_LED_C */

/*-----------------------------------------------------------------------------
 * Poll Queue task parameters
 *---------------------------------------------------------------------------*/

/* Poll Q task constants */
#define portspecPOLL_Q_STACK_SIZE       ( configMINIMAL_STACK_SIZE_NO_FPU )
#define portspecPOLL_Q_QUEUE_LENGTH     ( 10U )
#define portspecPOLL_Q_QUEUE_ITEM_SIZE  ( sizeof( portUInt16Type ) )
#define portspecPOLL_Q_BUFFER_LENGTH    ( ( portspecPOLL_Q_QUEUE_LENGTH * portspecPOLL_Q_QUEUE_ITEM_SIZE ) + portQUEUE_OVERHEAD_BYTES )

/* Poll Queue task stacks */
extern portInt8Type acPollQueueTestTask1Stack[];
extern portInt8Type acPollQueueTestTask2Stack[];
extern portInt8Type acPollQueueTestTask3Stack[];

/* Queue buffer */
extern portInt8Type acPollQueue1Buffer[];
extern portInt8Type acPollQueue2Buffer[];

/* MPU parameters */
#ifdef POLLED_Q_C

/* Define portspecPOLL_Q_DATA_SECTION
 * so that all Poll Queue static data is in the same section. */
#define portspecPOLL_Q_DATA_SECTION                         _Pragma("location=\"__poll_q_data__\"")

/* The size of the Poll Queue data section. */
#define portspecPOLL_Q_DATA_SIZE                            ( 0x20 )

/* A linker defined symbol that gives the start address of the Poll Queue data
section. */
extern portUInt32Type __poll_q_data_block__$$Base[];

/* This definition corresponds to the xMPUParameters structure that is passed to
xTaskCreate() as part of the xTaskParameters structure when the Poll Queue tasks
are created. It defines the privilege level and region definitions for the Poll
Queue tasks. All the Poll Queue tasks run in Unprivileged (User) mode with 1
additional region that corresponds to the linker section that holds the Poll
Queue static data. */
#define portspecPOLL_Q_PARAMETERS                                                   \
pdFALSE,                                                                            \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __poll_q_data_block__$$Base,                                            \
            portspecPOLL_Q_DATA_SIZE,                                               \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xPolledQueueConsumerTaskPortParameters  portspecPOLL_Q_PARAMETERS
#define xPolledQueueProducerTaskPortParameters  portspecPOLL_Q_PARAMETERS
#define xPolledQueueSendFrontTaskPortParameters portspecPOLL_Q_PARAMETERS

#endif  /* POLLED_Q_C */

/*-----------------------------------------------------------------------------
 * Binary Semaphore Test task parameters
 *---------------------------------------------------------------------------*/
/* Binary Semaphore Test Task Constants */
#define portspecSEMAPHORE_TASK_STACK_SIZE       ( configMINIMAL_STACK_SIZE_NO_FPU )

/* Binary Semaphore Test Task Stacks */
extern portInt8Type acSemaphoreTestTask1Stack[];
extern portInt8Type acSemaphoreTestTask2Stack[];
extern portInt8Type acSemaphoreTestTask3Stack[];
extern portInt8Type acSemaphoreTestTask4Stack[];

/* Binary Semaphore Test Semaphore buffers.
 * No actual data is stored into these buffers so the buffer need only be large
 * enough to hold the queue structure itself. */
extern portInt8Type acSemaphore1[];
extern portInt8Type acSemaphore2[];

#ifdef SEMAPHORE_TEST_C

/* MPU parameters */
/* Define portspecSEMAPHORE_TASK_DATA_SECTION
 * so that all Binary Semaphore static data is in the same section. */
#define portspecSEMAPHORE_TASK_DATA_SECTION     _Pragma("location=\"__binary_semaphore_task_data__\"")

/* The size of the Binary Semaphore Task data section. */
#define portspecBINARY_SEMAPHORE_TASK_DATA_SIZE     ( 0x40 )

/* A linker defined symbol that gives the start address of the Binary Semaphore
 * Task data section. */
extern portUInt32Type __binary_semaphore_task_data_block__$$Base[];

/* This definition corresponds to the xMPUParameters structure that is passed
 * to xTaskCreate() as part of the xTaskParameters structure when the Binary
 * Semaphore tasks are created. It defines the privilege level and region
 * definitions for the Binary Semaphore tasks. All the Binary Semaphore tasks
 * run in Unprivileged (User) mode with 1 additional region that corresponds to
 * the linker section that holds the Binary Semaphore static data. */
#define portspecBINARY_SEMAPHORE_PARAMETERS                                         \
pdFALSE,                                                                            \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __binary_semaphore_task_data_block__$$Base,                             \
            portspecBINARY_SEMAPHORE_TASK_DATA_SIZE,                                \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xSemaphoreTestTask1PortParameters   portspecBINARY_SEMAPHORE_PARAMETERS
#define xSemaphoreTestTask2PortParameters   portspecBINARY_SEMAPHORE_PARAMETERS
#define xSemaphoreTestTask3PortParameters   portspecBINARY_SEMAPHORE_PARAMETERS
#define xSemaphoreTestTask4PortParameters   portspecBINARY_SEMAPHORE_PARAMETERS

#endif /* SEMAPHORE_TEST_C */

/*-----------------------------------------------------------------------------
 * Timer demo task parameters
 *---------------------------------------------------------------------------*/

/* Task constants */
#define portspecTIMER_TASK_STACK_SIZE       ( 512U )

#ifdef TIMERTEST_C

/* Stacks for the task */
extern portInt8Type acTimerTestTaskStack[ portspecTIMER_TASK_STACK_SIZE ];

/* MPU parameters */
/* Define portspecTIMER_TEST_DATA_SECTION
 * so that all timer demo static data is in the same section. */
#define portspecTIMER_TEST_DATA_SECTION         _Pragma("location=\"__timer_demo_data__\"")
#define portspecTIMER_TEST_ZERO_DATA_SECTION    _Pragma("location=\"__timer_demo_zero_data__\"")

/* The size of the Com Test data section. */
#define portspecTIMER_TEST_DATA_SIZE            ( 0x800 )

/* A linker defined symbol that gives the start address of the Timer Test data
section. */
extern portUInt32Type __timer_demo_data_block__$$Base[];

/* This definition corresponds to the xMPUParameters structure that is passed to
xTaskCreate() as part of the xTaskParameters structure when the Timer Test tasks
are created. It defines the privilege level and region definitions for the Timer
Test tasks. All the Timer Test tasks run in Unprivileged (User) mode with 1
additional region that corresponds to the linker section that holds the Timer
Test static data. */
#define portspecTIMER_TEST_TASK_PARAMETERS                                          \
pdFALSE,                                                                            \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __timer_demo_data_block__$$Base,                                        \
            portspecTIMER_TEST_DATA_SIZE,                                           \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xTimerTestTaskPortParameters        portspecTIMER_TEST_TASK_PARAMETERS

#endif /* TIMERTEST_C */

/*-----------------------------------------------------------------------------
 * Notify demo task parameters
 *---------------------------------------------------------------------------*/

/* Task constants */
#define portspecNOTIFIED_TASK_STACK_SIZE        ( 512U )

/* Stacks for the task */
extern portInt8Type acNotifiedTaskStack[ portspecNOTIFIED_TASK_STACK_SIZE ];

/* MPU parameters */
#ifdef TASKNOTIFYDEMO_C

/* Define portspecTASK_NOTIFY_DATA_SECTION and portspecTASK_NOTIFY_ZERO_DATA_SECTION
 * so that all task notify demo static data is in the same section. */
#define portspecTASK_NOTIFY_DATA_SECTION        _Pragma("location=\"__task_notify_data__\"")
#define portspecTASK_NOTIFY_ZERO_DATA_SECTION   _Pragma("location=\"__task_notify_zero_data__\"")

/* The size of the TAsk Notify data section. */
#define portspecTASK_NOTIFY_DATA_SIZE           ( 0x20 )

/* A linker defined symbol that gives the start address of the Timer Test data
 * section. */
extern portUInt32Type __task_notify_data_block__$$Base[];

/* This definition corresponds to the xMPUParameters structure that is passed
 * to xTaskCreate() as part of the xTaskParameters structure when the Notify
 * demo tasks are created. It defines the privilege level and region
 * definitions for the Notify demo tasks. All the Notify demo tasks run in
 * Unprivileged (User) mode with 1 additional region that corresponds to the
 * linker section that holds the Notify demo static data. */
#define portspecNOTIFIED_TASK_PARAMETERS                                            \
pdFALSE,                                                                            \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __task_notify_data_block__$$Base,                                       \
            portspecTASK_NOTIFY_DATA_SIZE,                                          \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xNotifiedTaskPortParameters     portspecNOTIFIED_TASK_PARAMETERS

#endif /* TASKNOTIFYDEMO_C */


/*-----------------------------------------------------------------------------
 * Recursive mutex task parameters
 *---------------------------------------------------------------------------*/

#define portspecMUTEX_STACK_SIZE        ( 512U )

/* Mutex demo task stacks */
extern portInt8Type acRecursiveMutexControllingTaskStack[ portspecMUTEX_STACK_SIZE ];
extern portInt8Type acRecursiveMutexBlockingTaskStack[ portspecMUTEX_STACK_SIZE ];
extern portInt8Type acRecursiveMutexPollingTaskStack[ portspecMUTEX_STACK_SIZE ];

/* Queue Buffer */
extern portInt8Type acMutexBuffer[ portQUEUE_OVERHEAD_BYTES ];

#ifdef TASK_MUTEX_C

/* MPU parameters */
/* Define portspecREC_MUTEX_DATA_SECTION
 * so that all static data is in the same section. */
#define portspecMUTEX_TASK_DATA_SECTION     _Pragma("location=\"__rec_mutex_data__\"")

/* The size of the TAsk Notify data section. */
#define portspecREC_MUTEX_DATA_SIZE         ( 0x100 )

/* A linker defined symbol that gives the start address of the Timer Test data
 * section. */
extern portUInt32Type __rec_mutex_data_block__$$Base[];



/* This definition corresponds to the xMPUParameters structure that is passed
 * to xTaskCreate() as part of the xTaskParameters structure when the
 * Recursive Mutex tasks are created. It defines the privilege level and region
 * definitions for the Poll Queue tasks. All the Poll Queue tasks run in
 * Unprivileged (User) mode with 1 additional region that corresponds to the
 * linker section that holds the Poll Queue static data. */
#define portspecREC_MUTEX_PARAMETERS                                                \
pdFALSE,                                                                            \
{                                                                                   \
    mpuUNPRIVILEGED_TASK,                                                           \
    {                                                                               \
        {                                                                           \
            __rec_mutex_data_block__$$Base,                                         \
            portspecREC_MUTEX_DATA_SIZE,                                            \
            ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE |                 \
              portmpuREGION_EXECUTE_NEVER |                                         \
              portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE ),   \
            0U                                                                      \
        },                                                                          \
        { NULL, 0U, 0U, 0U },                                                       \
        { NULL, 0U, 0U, 0U }                                                        \
    }                                                                               \
}

#define xRecursiveMutexControllingTaskPortParameters    portspecREC_MUTEX_PARAMETERS
#define xRecursiveMutexBlockingTaskPortParameters       portspecREC_MUTEX_PARAMETERS
#define xRecursiveMutexPollingTaskPortParameters        portspecREC_MUTEX_PARAMETERS

#endif  /* REC_MUTEX_C */

#endif /* PORT_SPECIFICS_H */
