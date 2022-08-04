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
    The tasks defined on this page demonstrate the use of recursive mutexes.

    For recursive mutex functionality the created mutex should be created using
    xMutexCreate(), then be manipulated using the xMutexTake() and xMutexGive()
    API functions.

    This demo creates three tasks all of which access the same recursive mutex:

    prvRecursiveMutexControllingTask() has the highest priority so executes
    first and grabs the mutex.  It then performs some recursive accesses -
    between each of which it sleeps for a short period to let the lower
    priority tasks execute.  When it has completed its demo functionality
    it gives the mutex back before suspending itself.

    prvRecursiveMutexBlockingTask() attempts to access the mutex by performing
    a blocking 'take'.  The blocking task has a lower priority than the
    controlling task so by the time it executes the mutex has already been
    taken by the controlling task,  causing the blocking task to block.  It
    does not unblock until the controlling task has given the mutex back,
    and it does not actually run until the controlling task has suspended
    itself (due to the relative priorities).  When it eventually does obtain
    the mutex all it does is give the mutex back prior to also suspending
    itself.  At this point both the controlling task and the blocking task are
    suspended.

    prvRecursiveMutexPollingTask() runs at the idle priority.  It spins round
    a tight loop attempting to obtain the mutex with a non-blocking call.  As
    the lowest priority task it will not successfully obtain the mutex until
    both the controlling and blocking tasks are suspended.  Once it eventually
    does obtain the mutex it first unsuspends both the controlling task and
    blocking task prior to giving the mutex back - resulting in the polling
    task temporarily inheriting the controlling tasks priority.
*/

/* PortSpecifics.h includes some 'declare once' declarations. */
#define TASK_MUTEX_C

/* Scheduler include files. */
#include "SafeRTOS_API.h"

/* Demo app include files. */
#include "TaskMutex.h"
#include "PortSpecifics.h"

#if ( configQUEUE_REGISTRY_SIZE > 0 )
    #include "queue_register.h"
#endif

/* Priorities assigned to the three tasks.  taskmutexCONTROLLING_TASK_PRIORITY can
 * be overridden by a definition in FreeRTOSConfig.h. */
#define taskmutexCONTROLLING_TASK_PRIORITY  ( taskIDLE_PRIORITY + 2 )
#define taskmutexBLOCKING_TASK_PRIORITY     ( taskIDLE_PRIORITY + 1 )
#define taskmutexPOLLING_TASK_PRIORITY      ( taskIDLE_PRIORITY + 0 )

/* The recursive call depth. */
#define taskmutexMAX_COUNT                  ( 10 )

/* Misc. */
#define taskmutexSHORT_DELAY                ( 20 )
#define taskmutexNO_DELAY                   ( 0 )
#define taskmutex50ms_DELAY                 ( 50 )

/* The three tasks as described at the top of this file. */
static void prvRecursiveMutexControllingTask( void *pvParameters );
static void prvRecursiveMutexBlockingTask( void *pvParameters );
static void prvRecursiveMutexPollingTask( void *pvParameters );

/* The mutex used by the demo. */
portspecMUTEX_TASK_DATA_SECTION static xMutexHandleType xMutex = NULL;

/* Variables used to detect and latch errors. */
portspecMUTEX_TASK_DATA_SECTION static volatile portBaseType xErrorOccurred = pdFALSE;
portspecMUTEX_TASK_DATA_SECTION static volatile portBaseType xControllingIsSuspended = pdFALSE;
portspecMUTEX_TASK_DATA_SECTION static volatile portBaseType xBlockingIsSuspended = pdFALSE;
portspecMUTEX_TASK_DATA_SECTION static volatile portUnsignedBaseType uxControllingCycles = 0;
portspecMUTEX_TASK_DATA_SECTION static volatile portUnsignedBaseType uxBlockingCycles = 0;
portspecMUTEX_TASK_DATA_SECTION static volatile portUnsignedBaseType uxPollingCycles = 0;

/* Handles of the two higher priority tasks, required so they can be resumed
 * (unsuspended). */
portspecMUTEX_TASK_DATA_SECTION static portTaskHandleType xControllingTaskHandle = NULL;
portspecMUTEX_TASK_DATA_SECTION static portTaskHandleType xBlockingTaskHandle = NULL;

/* Task TCBs. */
portspecTCB_DATA_SECTION static xTCB xRecursiveMutexControllingTaskTCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xRecursiveMutexBlockingTaskTCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xRecursiveMutexPollingTaskTCB = { 0 };

/*---------------------------------------------------------------------------*/

portBaseType xStartRecursiveMutexTasks( void )
{
    portBaseType xStatus;
    xTaskParameters xRecursiveMutexControllingTaskParameters =
    {
        prvRecursiveMutexControllingTask,               /* Task code */
        "Rec Mtx Ctrl",                                 /* Task name */
        &xRecursiveMutexControllingTaskTCB,             /* TCB */
        acRecursiveMutexControllingTaskStack,           /* Stack buffer */
        portspecMUTEX_STACK_SIZE,                       /* Stack depth bytes */
        ( void * ) &xMutex,                             /* Parameters */
        taskmutexCONTROLLING_TASK_PRIORITY,                 /* Priority */
        NULL,                                           /* TLS object */
        xRecursiveMutexControllingTaskPortParameters    /* Port-specific task parameters */
    };
    xTaskParameters xRecursiveMutexBlockingTaskParameters =
    {
        prvRecursiveMutexBlockingTask,                  /* Task code */
        "Rec Mtx Ctrl",                                 /* Task name */
        &xRecursiveMutexBlockingTaskTCB,                /* TCB */
        acRecursiveMutexBlockingTaskStack,              /* Stack buffer */
        portspecMUTEX_STACK_SIZE,                       /* Stack depth bytes */
        ( void * ) &xMutex,                             /* Parameters */
        taskmutexBLOCKING_TASK_PRIORITY,                    /* Priority */
        NULL,                                           /* TLS object */
        xRecursiveMutexBlockingTaskPortParameters       /* Port-specific task parameters */
    };
    xTaskParameters xRecursiveMutexPollingTaskParameters =
    {
        prvRecursiveMutexPollingTask,                   /* Task code */
        "Rec Mtx Ctrl",                                 /* Task name */
        &xRecursiveMutexPollingTaskTCB,                 /* TCB */
        acRecursiveMutexPollingTaskStack,               /* Stack buffer */
        portspecMUTEX_STACK_SIZE,                       /* Stack depth bytes */
        ( void * ) &xMutex,                             /* Parameters */
        taskmutexPOLLING_TASK_PRIORITY,                     /* Priority */
        NULL,                                           /* TLS object */
        xRecursiveMutexPollingTaskPortParameters        /* Port-specific task parameters */
    };

    /* Just create the mutex and the three tasks. */

    xStatus = xMutexCreate( acMutexBuffer, &xMutex );

    if( pdPASS == xStatus )
    {
        /* vQueueAddToRegistry() adds the mutex to the registry, if one is in
         * use. The registry is provided as a means for kernel aware debuggers
         * to locate mutex and has no purpose if a kernel aware debugger is not
         * being used. The call to vQueueAddToRegistry() will be removed by the
         * pre-processor if configQUEUE_REGISTRY_SIZE is not defined or is
         * defined to be less than 1. */
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        vQueueAddToRegistry( xMutex, "Recursive_Mutex" );
#endif

        if( xTaskCreate( &xRecursiveMutexControllingTaskParameters, ( portTaskHandleType * ) &xControllingTaskHandle ) != pdPASS )
        {
            xStatus = pdFAIL;
        }
        if( xTaskCreate( &xRecursiveMutexBlockingTaskParameters, ( portTaskHandleType * ) &xBlockingTaskHandle ) != pdPASS )
        {
            xStatus = pdFAIL;
        }
        if( xTaskCreate( &xRecursiveMutexPollingTaskParameters, NULL ) != pdPASS )
        {
            xStatus = pdFAIL;
        }
    }
    return xStatus;
}
/*---------------------------------------------------------------------------*/

static void prvRecursiveMutexControllingTask( void *pvParameters )
{
    portUnsignedBaseType uxIndex;

    /* Just to remove compiler warning. */
    ( void ) pvParameters;

    for( ;; )
    {
        /* Should not be able to 'give' the mutex, as we have not yet 'taken' it.
         * The first time through, the mutex will not have been used yet,
         * subsequent times through, at this point the mutex will be held by the
         * polling task. */
        if( xMutexGive( xMutex ) == pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        for( uxIndex = 0; uxIndex < taskmutexMAX_COUNT; uxIndex++ )
        {
            /* We should now be able to take the mutex as many times as we like.
             *
             * The first time through the mutex will be immediately available,
             * on subsequent times through the mutex will be held by the polling
             * task at this point and this Take will cause the polling task to
             * inherit the priority of this task. In this case the block time
             * must be long enough to ensure the polling task will execute again
             * before the block time expires. If the block time does expire then
             * the error flag will be set here. */
            if( xMutexTake( xMutex, taskmutex50ms_DELAY ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            /* Ensure the other task attempting to access the mutex (and the
             * other demo tasks) are able to execute to ensure they either block
             * (where a block time is specified) or return an error (where no
             * block time is specified) as the mutex is held by this task. */
            xTaskDelay( taskmutexSHORT_DELAY );
        }

        /* For each time we took the mutex, give it back. */
        for( uxIndex = 0; uxIndex < taskmutexMAX_COUNT; uxIndex++ )
        {
            /* Ensure the other task attempting to access the mutex (and the
             * other demo tasks) are able to execute. */
            xTaskDelay( taskmutexSHORT_DELAY );

            /* We should now be able to give the mutex as many times as we took
             * it. When the mutex is available again the Blocking task should be
             * unblocked but not run because it has a lower priority than this
             * task. The polling task should also not run at this point as it
             * too has a lower priority than this task. */
            if( xMutexGive( xMutex ) != pdPASS )
            {
                xErrorOccurred = pdTRUE;
            }

            taskYIELD();
        }

        /* Having given it back the same number of times as it was taken, we
         * should no longer be the mutex owner, so the next give should fail. */
        if( xMutexGive( xMutex ) == pdPASS )
        {
            xErrorOccurred = pdTRUE;
        }

        /* Keep count of the number of cycles this task has performed so a
         * stall can be detected. */
        uxControllingCycles++;

        /* Suspend ourselves so the blocking task can execute. */
        xControllingIsSuspended = pdTRUE;
        xTaskSuspend( NULL );
        xControllingIsSuspended = pdFALSE;
    }
}
/*---------------------------------------------------------------------------*/

static void prvRecursiveMutexBlockingTask( void *pvParameters )
{
    portUnsignedBaseType uxBlockingCyclesTmp;

    /* Just to remove compiler warning. */
    ( void ) pvParameters;

    for( ;; )
    {
        /* This task will run while the controlling task is blocked, and the
         * controlling task will block only once it has the mutex - therefore
         * this call should block until the controlling task has given up the
         * mutex, and not actually execute past this call until the controlling
         * task is suspended. */
        if( pdPASS == xMutexTake( xMutex, portMAX_DELAY ) )
        {
            if( xControllingIsSuspended != pdTRUE )
            {
                /* Did not expect to execute until the controlling task was
                 * suspended. */
                xErrorOccurred = pdTRUE;
            }
            else
            {
                /* Give the mutex back before suspending ourselves to allow
                 * the polling task to obtain the mutex. */
                if( pdPASS != xMutexGive( xMutex ) )
                {
                    xErrorOccurred = pdTRUE;
                }

                xBlockingIsSuspended = pdTRUE;
                xTaskSuspend( NULL );
                xBlockingIsSuspended = pdFALSE;
            }
        }
        else
        {
            /* We should not leave the xSemaphoreTakeRecursive() function
             * until the mutex was obtained. */
            xErrorOccurred = pdTRUE;
        }

        /* The controlling and blocking tasks should be in lock step. */
        uxBlockingCyclesTmp = uxBlockingCycles + 1;
        if( uxControllingCycles != uxBlockingCyclesTmp )
        {
            xErrorOccurred = pdTRUE;
        }

        /* Keep count of the number of cycles this task has performed so a
         * stall can be detected. */
        uxBlockingCycles++;
    }
}
/*---------------------------------------------------------------------------*/

static void prvRecursiveMutexPollingTask( void *pvParameters )
{
    portUnsignedBaseType uxCurrentPriority = 0UL;

    /* Just to remove compiler warning. */
    ( void ) pvParameters;

    for( ;; )
    {
        /* Keep attempting to obtain the mutex. We should only obtain it when
         * the blocking task has suspended itself, which in turn should only
         * happen when the controlling task is also suspended. */
        if( pdPASS == xMutexTake( xMutex, taskmutexNO_DELAY ) )
        {
            /* Is the blocking task suspended? */
            if( ( pdTRUE != xBlockingIsSuspended ) || ( pdTRUE != xControllingIsSuspended ) )
            {
                xErrorOccurred = pdTRUE;
            }
            else
            {
                /* Keep count of the number of cycles this task has performed,
                 * so a stall can be detected. */
                uxPollingCycles++;

                /* We can resume the other tasks here even though they have a
                 * higher priority than the polling task. When they execute,
                 * they will attempt to obtain the mutex but fail because the
                 * polling task is still the mutex holder. The polling task
                 * (this task) will then inherit the higher priority. The
                 * Blocking task will block indefinitely when it attempts to
                 * obtain the mutex, the Controlling task will only block for a
                 * fixed period and an error will be latched if the polling task
                 * has not returned the mutex by the time this fixed period has
                 * expired. */
                xTaskResume( xBlockingTaskHandle );
                taskYIELD();

                xTaskResume( xControllingTaskHandle );
                taskYIELD();

                /* The other two tasks should now have executed and no longer
                 * be suspended. */
                if( ( pdTRUE == xBlockingIsSuspended ) || ( pdTRUE == xControllingIsSuspended ) )
                {
                    xErrorOccurred = pdTRUE;
                }

                if( pdPASS != xTaskPriorityGet( NULL, &uxCurrentPriority ) )
                {
                    xErrorOccurred = pdTRUE;
                }
                if( taskmutexCONTROLLING_TASK_PRIORITY != uxCurrentPriority )
                {
                    xErrorOccurred = pdTRUE;
                }

                /* Release the mutex, disinheriting the higher priority again. */
                if( pdPASS != xMutexGive( xMutex ) )
                {
                    xErrorOccurred = pdTRUE;
                }

                if( pdPASS != xTaskPriorityGet( NULL, &uxCurrentPriority ) )
                {
                    xErrorOccurred = pdTRUE;
                }
                if( taskmutexPOLLING_TASK_PRIORITY != uxCurrentPriority )
                {
                    xErrorOccurred = pdTRUE;
                }
            }
        }
        taskYIELD();
    }
}
/*---------------------------------------------------------------------------*/

/* This is called to check that all the created tasks are still running. */
portBaseType xAreRecursiveMutexTasksStillRunning( void )
{
    portBaseType xReturn;
    portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType uxLastControllingCycles = 0;
    portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType uxLastBlockingCycles = 0;
    portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType uxLastPollingCycles = 0;

    /* Is the controlling task still cycling? */
    if( uxLastControllingCycles == uxControllingCycles )
    {
        xErrorOccurred = pdTRUE;
    }
    else
    {
        uxLastControllingCycles = uxControllingCycles;
    }

    /* Is the blocking task still cycling? */
    if( uxLastBlockingCycles == uxBlockingCycles )
    {
        xErrorOccurred = pdTRUE;
    }
    else
    {
        uxLastBlockingCycles = uxBlockingCycles;
    }

    /* Is the polling task still cycling? */
    if( uxLastPollingCycles == uxPollingCycles )
    {
        xErrorOccurred = pdTRUE;
    }
    else
    {
        uxLastPollingCycles = uxPollingCycles;
    }

    if( xErrorOccurred == pdTRUE )
    {
        xReturn = pdFAIL;
    }
    else
    {
        xReturn = pdPASS;
    }

    return xReturn;
}
/*---------------------------------------------------------------------------*/
