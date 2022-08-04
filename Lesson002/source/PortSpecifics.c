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

/* Scheduler include files. */
#include "SafeRTOS_API.h"

/* Demo program include files. */
#include "PortSpecifics.h"


/*-----------------------------------------------------------------------------
 * Maths Test Task Parameters
 *---------------------------------------------------------------------------*/
/* Maths Test Task Stacks */
#pragma data_alignment=portspecMATHS_TEST_STACK_SIZE
portInt8Type acUnprivilegedTask1Stack[ portspecMATHS_TEST_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecMATHS_TEST_STACK_SIZE
portInt8Type acUnprivilegedTask2Stack[ portspecMATHS_TEST_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecMATHS_TEST_STACK_SIZE
portInt8Type acUnprivilegedTask3Stack[ portspecMATHS_TEST_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecMATHS_TEST_STACK_SIZE
portInt8Type acUnprivilegedTask4Stack[ portspecMATHS_TEST_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecMATHS_TEST_STACK_SIZE
portInt8Type acPrivilegedTask1Stack[ portspecMATHS_TEST_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecMATHS_TEST_STACK_SIZE
portInt8Type acPrivilegedTask2Stack[ portspecMATHS_TEST_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecMATHS_TEST_STACK_SIZE
portInt8Type acPrivilegedTask3Stack[ portspecMATHS_TEST_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecMATHS_TEST_STACK_SIZE
portInt8Type acPrivilegedTask4Stack[ portspecMATHS_TEST_STACK_SIZE ] = { 0 };


/*-----------------------------------------------------------------------------
 * Queue Blocking Task Parameters
 *---------------------------------------------------------------------------*/
/* Blocking Queue Task Stacks */
#pragma data_alignment=portspecBLOCK_Q_STACK_SIZE
portInt8Type acQueueTestTask1Stack[ portspecBLOCK_Q_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecBLOCK_Q_STACK_SIZE
portInt8Type acQueueTestTask2Stack[ portspecBLOCK_Q_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecBLOCK_Q_STACK_SIZE
portInt8Type acQueueTestTask3Stack[ portspecBLOCK_Q_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecBLOCK_Q_STACK_SIZE
portInt8Type acQueueTestTask4Stack[ portspecBLOCK_Q_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecBLOCK_Q_STACK_SIZE
portInt8Type acQueueTestTask5Stack[ portspecBLOCK_Q_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecBLOCK_Q_STACK_SIZE
portInt8Type acQueueTestTask6Stack[ portspecBLOCK_Q_STACK_SIZE ] = { 0 };

/* Queue Buffers */
#pragma data_alignment=4
portInt8Type acQueue1Buffer[ portspecBLOCK_Q_BUFFER_LENGTH_1 ] = { 0 };

#pragma data_alignment=4
portInt8Type acQueue2Buffer[ portspecBLOCK_Q_BUFFER_LENGTH_1 ] = { 0 };

#pragma data_alignment=4
portInt8Type acQueue3Buffer[ portspecBLOCK_Q_BUFFER_LENGTH_5 ] = { 0 };


/*-----------------------------------------------------------------------------
 * Block Time Test Task Parameters
 *---------------------------------------------------------------------------*/
/* Task Stacks */
#pragma data_alignment=portspecBLOCK_TIME_STACK_SIZE
portInt8Type acBlockTimeTestTask1Stack[ portspecBLOCK_TIME_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecBLOCK_TIME_STACK_SIZE
portInt8Type acBlockTimeTestTask2Stack[ portspecBLOCK_TIME_STACK_SIZE ] = { 0 };

/* Queue Buffer */
#pragma data_alignment=4
portInt8Type acQueueBuffer[ portspecBLOCK_TIME_BUFFER_LENGTH ] = { 0 };


/*-----------------------------------------------------------------------------
 * Counting Semaphore Test Task Parameters
 *---------------------------------------------------------------------------*/
/* Task Stacks */
#pragma data_alignment=portspecCOUNTSEM_TASK_STACK_SIZE
portInt8Type acCountSemTestTask1Stack[ portspecCOUNTSEM_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecCOUNTSEM_TASK_STACK_SIZE
portInt8Type acCountSemTestTask2Stack[ portspecCOUNTSEM_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecCOUNTSEM_TASK3_STACK_SIZE
portInt8Type acCountSemTestTask3Stack[ portspecCOUNTSEM_TASK3_STACK_SIZE ] = { 0 };

/* Counting Semaphore Buffers.
 * No actual data is stored into these buffers so the buffer
 * need only be large enough to hold the queue structure itself. */
#pragma data_alignment=4
portInt8Type acCountSem1[ portQUEUE_OVERHEAD_BYTES ] = { 0 };

#pragma data_alignment=4
portInt8Type acCountSem2[ portQUEUE_OVERHEAD_BYTES ] = { 0 };

#pragma data_alignment=4
portInt8Type acCountSem3[ portQUEUE_OVERHEAD_BYTES ] = { 0 };


/*-----------------------------------------------------------------------------
 * Create Delete Test Task Parameters
 *---------------------------------------------------------------------------*/
/* Task Stacks */
#pragma data_alignment=portspecCREATE_TASK_STACK_SIZE
portInt8Type acCreateTaskStack[ portspecCREATE_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecSUICIDAL_TASK_STACK_SIZE
portInt8Type acSuicidalTask1Stack[ portspecSUICIDAL_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecSUICIDAL_TASK_STACK_SIZE
portInt8Type acSuicidalTask2Stack[ portspecSUICIDAL_TASK_STACK_SIZE ] = { 0 };


/*-----------------------------------------------------------------------------
 * Dynamic Task Parameters
 *---------------------------------------------------------------------------*/
/* Dynamic Task Stacks */
#pragma data_alignment=portspecDYNAMIC_TASK_STACK_SIZE
portInt8Type acContinuousIncrementTaskStack[ portspecDYNAMIC_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecDYNAMIC_TASK_STACK_SIZE
portInt8Type acLimitedIncrementTaskStack[ portspecDYNAMIC_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecDYNAMIC_TASK_STACK_SIZE
portInt8Type acCounterControlTaskStack[ portspecDYNAMIC_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecDYNAMIC_TASK_STACK_SIZE
portInt8Type acQueueSendWhenSuspendedTaskStack[ portspecDYNAMIC_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecDYNAMIC_TASK_STACK_SIZE
portInt8Type acQueueReceiveWhenSuspendedTaskStack[ portspecDYNAMIC_TASK_STACK_SIZE ] = { 0 };

/* Dynamic Task Queue Buffer */
#pragma data_alignment=4
portInt8Type acSuspendTestQueueBuffer[ portspecDYNAMIC_TASK_SUSPEND_QUEUE_BUFFER_LENGTH ] = { 0 };


/*-----------------------------------------------------------------------------
 * LED Flash Task Parameters
 *---------------------------------------------------------------------------*/
/* Task Stack */
#pragma data_alignment=portspecLED_TASK_STACK_SIZE
portInt8Type acLedTaskStack[ portspecNUMBER_OF_LEDS ][ portspecLED_TASK_STACK_SIZE ] = { 0 };

/* Task pvParameters */
portspecLED_TASK_DATA_SECTION xLedFlashTaskParameters xLedTaskParameters[ portspecNUMBER_OF_LEDS ] =
{
    { 3, 500 },
    { 1, 125 },
    { 2, 250 }
};


/*-----------------------------------------------------------------------------
 * Poll Q Task Parameters
 *---------------------------------------------------------------------------*/
/* Poll Queue Task Stacks */
#pragma data_alignment=portspecPOLL_Q_STACK_SIZE
portInt8Type acPollQueueTestTask1Stack[ portspecPOLL_Q_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecPOLL_Q_STACK_SIZE
portInt8Type acPollQueueTestTask2Stack[ portspecPOLL_Q_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecPOLL_Q_STACK_SIZE
portInt8Type acPollQueueTestTask3Stack[ portspecPOLL_Q_STACK_SIZE ] = { 0 };

/* Queue buffers. */
#pragma data_alignment=4
portInt8Type acPollQueue1Buffer[ portspecPOLL_Q_BUFFER_LENGTH ] = { 0 };

#pragma data_alignment=4
portInt8Type acPollQueue2Buffer[ portspecPOLL_Q_BUFFER_LENGTH ] = { 0 };


/*-----------------------------------------------------------------------------
 * Binary Semaphore Test Task Parameters
 *---------------------------------------------------------------------------*/
/* Binary Semaphore Test Task Stacks */
#pragma data_alignment=portspecSEMAPHORE_TASK_STACK_SIZE
portInt8Type acSemaphoreTestTask1Stack[ portspecSEMAPHORE_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecSEMAPHORE_TASK_STACK_SIZE
portInt8Type acSemaphoreTestTask2Stack[ portspecSEMAPHORE_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecSEMAPHORE_TASK_STACK_SIZE
portInt8Type acSemaphoreTestTask3Stack[ portspecSEMAPHORE_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecSEMAPHORE_TASK_STACK_SIZE
portInt8Type acSemaphoreTestTask4Stack[ portspecSEMAPHORE_TASK_STACK_SIZE ] = { 0 };

/* Binary Semaphore Buffers. No actual data is stored into these buffers, so
 * the buffer need only be large enough to hold the queue structure itself. */
#pragma data_alignment=4
portInt8Type acSemaphore1[ portQUEUE_OVERHEAD_BYTES ] = { 0 };

#pragma data_alignment=4
portInt8Type acSemaphore2[ portQUEUE_OVERHEAD_BYTES ] = { 0 };

/*-----------------------------------------------------------------------------
 * Timer Demo Task Parameters
 *---------------------------------------------------------------------------*/
/* Task Stacks */
#pragma data_alignment=portspecTIMER_TASK_STACK_SIZE
portInt8Type acTimerTestTaskStack[ portspecTIMER_TASK_STACK_SIZE ] = { 0 };


/*-----------------------------------------------------------------------------
 * Task Notify Parameters
 *---------------------------------------------------------------------------*/
/* Task Notify Task Stack */
#pragma data_alignment=portspecNOTIFIED_TASK_STACK_SIZE
portInt8Type acNotifiedTaskStack[ portspecNOTIFIED_TASK_STACK_SIZE ] = { 0 };


/*-----------------------------------------------------------------------------
 * Com Test Task Parameters
 *---------------------------------------------------------------------------*/
/* Task Stacks */
#pragma data_alignment=portspecCOM_TEST_TASK_STACK_SIZE
portInt8Type acTxStack[ portspecCOM_TEST_TASK_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecCOM_TEST_TASK_STACK_SIZE
portInt8Type acRxStack[ portspecCOM_TEST_TASK_STACK_SIZE ] = { 0 };
/*-----------------------------------------------------------------------------
 * Recursive Mutex Task Parameters
 *---------------------------------------------------------------------------*/
/* Task Stacks */
#pragma data_alignment=portspecMUTEX_STACK_SIZE
portInt8Type acRecursiveMutexControllingTaskStack[ portspecMUTEX_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecMUTEX_STACK_SIZE
portInt8Type acRecursiveMutexBlockingTaskStack[ portspecMUTEX_STACK_SIZE ] = { 0 };

#pragma data_alignment=portspecMUTEX_STACK_SIZE
portInt8Type acRecursiveMutexPollingTaskStack[ portspecMUTEX_STACK_SIZE ] = { 0 };

/* Queue Buffer */
#pragma data_alignment=4
portInt8Type acMutexBuffer[ portQUEUE_OVERHEAD_BYTES ] = { 0 };


/*---------------------------------------------------------------------------*/
