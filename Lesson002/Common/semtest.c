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
 * Creates two sets of two tasks.  The tasks within a set share a variable, access
 * to which is guarded by a semaphore.
 *
 * Each task starts by attempting to obtain the semaphore.  On obtaining a
 * semaphore a task checks to ensure that the guarded variable has an expected
 * value.  It then clears the variable to zero before counting it back up to the
 * expected value in increments of 1.  After each increment the variable is checked
 * to ensure it contains the value to which it was just set. When the starting
 * value is again reached the task releases the semaphore giving the other task in
 * the set a chance to do exactly the same thing.  The starting value is high
 * enough to ensure that a tick is likely to occur during the incrementing loop.
 *
 * An error is flagged if at any time during the process a shared variable is
 * found to have a value other than that expected.  Such an occurrence would
 * suggest an error in the mutual exclusion mechanism by which access to the
 * variable is restricted.
 *
 * The first set of two tasks poll their semaphore.  The second set use blocking
 * calls.
 *
 */

/* PortSpecifics.h includes some 'declare once' declarations. */
#define SEMAPHORE_TEST_C

#include <stdlib.h>

/* Scheduler include files. */
#include "SafeRTOS_API.h"

/* Demo app include files. */
#include "semtest.h"
#include "PortSpecifics.h"

#if ( configQUEUE_REGISTRY_SIZE > 0 )
    #include "queue_register.h"
#endif

/* The value to which the shared variables are counted. */
#define semtstBLOCKING_EXPECTED_VALUE       ( ( portUInt32Type ) 0xFFF )
#define semtstNON_BLOCKING_EXPECTED_VALUE   ( ( portUInt32Type ) 0xFF  )

#define semtstNUM_TASKS                     ( 4 )
#define semtstDELAY_FACTOR                  ( ( portTickType ) 10 )

/* The task function as described at the top of the file. */
static void prvSemaphoreTest( void *pvParameters );

/* Structure used to pass parameters to each task. */
typedef struct SEMAPHORE_PARAMETERS
{
    xSemaphoreHandle xSemaphore;
    volatile portUInt32Type ulSharedVariable;
    portTickType xBlockTime;
} xSemaphoreParameters;

/* Semaphore Parameters. */
portspecSEMAPHORE_TASK_DATA_SECTION static xSemaphoreParameters xFirstSemaphoreParameters = { NULL, 0, 0 };
portspecSEMAPHORE_TASK_DATA_SECTION static xSemaphoreParameters xSecondSemaphoreParameters = { NULL, 0, 0 };

/* Variables used to check that all the tasks are still running without errors. */
portspecSEMAPHORE_TASK_DATA_SECTION static volatile portInt16Type asCheckVariables[ semtstNUM_TASKS ] = { 0 };
portspecSEMAPHORE_TASK_DATA_SECTION static volatile portInt16Type sNextCheckVariable = 0;

/* Semaphore Test Task TCBs. */
portspecTCB_DATA_SECTION static xTCB xSemaphoreTestTask1TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xSemaphoreTestTask2TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xSemaphoreTestTask3TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xSemaphoreTestTask4TCB = { 0 };

/*---------------------------------------------------------------------------*/

portBaseType xStartSemaphoreTasks( portUnsignedBaseType uxPriority )
{
    portBaseType xStatus = pdPASS;
    const portTickType xBlockTime = ( portTickType ) 100;
    xTaskParameters xTestTask1Parameters =
    {
        prvSemaphoreTest,                       /* Task code */
        "PolSEM1",                              /* Task name */
        &xSemaphoreTestTask1TCB,                /* TCB */
        acSemaphoreTestTask1Stack,              /* Stack buffer */
        portspecSEMAPHORE_TASK_STACK_SIZE,      /* Stack depth bytes */
        ( void * ) &xFirstSemaphoreParameters,  /* Parameters */
        taskIDLE_PRIORITY,                      /* Priority */
        NULL,                                   /* TLS object */
        xSemaphoreTestTask1PortParameters       /* Port-specific task parameters */
    };
    xTaskParameters xTestTask2Parameters =
    {
        prvSemaphoreTest,                       /* Task code */
        "PolSEM2",                              /* Task name */
        &xSemaphoreTestTask2TCB,                /* TCB */
        acSemaphoreTestTask2Stack,              /* Stack buffer */
        portspecSEMAPHORE_TASK_STACK_SIZE,      /* Stack depth bytes */
        ( void * ) &xFirstSemaphoreParameters,  /* Parameters */
        taskIDLE_PRIORITY,                      /* Priority */
        NULL,                                   /* TLS object */
        xSemaphoreTestTask2PortParameters       /* Port-specific task parameters */
    };
    xTaskParameters xTestTask3Parameters =
    {
        prvSemaphoreTest,                       /* Task code */
        "BlkSEM1",                              /* Task name */
        &xSemaphoreTestTask3TCB,                /* TCB */
        acSemaphoreTestTask3Stack,              /* Stack buffer */
        portspecSEMAPHORE_TASK_STACK_SIZE,      /* Stack depth bytes */
        ( void * ) &xSecondSemaphoreParameters, /* Parameters */
        taskIDLE_PRIORITY,                      /* Priority */
        NULL,                                   /* TLS object */
        xSemaphoreTestTask3PortParameters       /* Port-specific task parameters */
    };
    xTaskParameters xTestTask4Parameters =
    {
        prvSemaphoreTest,                       /* Task code */
        "BlkSEM2",                              /* Task name */
        &xSemaphoreTestTask4TCB,                /* TCB */
        acSemaphoreTestTask4Stack,              /* Stack buffer */
        portspecSEMAPHORE_TASK_STACK_SIZE,      /* Stack depth bytes */
        ( void * ) &xSecondSemaphoreParameters, /* Parameters */
        taskIDLE_PRIORITY,                      /* Priority */
        NULL,                                   /* TLS object */
        xSemaphoreTestTask4PortParameters       /* Port-specific task parameters */
    };


    /* Create the semaphore used by the first two tasks. */
    xSemaphoreCreateBinary( acSemaphore1, &xFirstSemaphoreParameters.xSemaphore );

    if( xFirstSemaphoreParameters.xSemaphore != NULL )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        /* Add to Queue Registry */
        vQueueAddToRegistry( xFirstSemaphoreParameters.xSemaphore, "BinarySemaphore #1" );
#endif

        /* Initialise the share variable to the value the tasks expect. */
        xFirstSemaphoreParameters.ulSharedVariable = semtstNON_BLOCKING_EXPECTED_VALUE;

        /* The first two tasks do not block on semaphore calls. */
        xFirstSemaphoreParameters.xBlockTime = ( portTickType ) 0;

        /* Spawn the first two tasks.  As they poll they operate at the idle priority. */
        if( xTaskCreate( &xTestTask1Parameters, ( portTaskHandleType * ) NULL ) != pdPASS )
        {
            xStatus = pdFAIL;
        }
        if( xTaskCreate( &xTestTask2Parameters, ( portTaskHandleType * ) NULL ) != pdPASS )
        {
            xStatus = pdFAIL;
        }
    }
    else
    {
        xStatus = pdFAIL;
    }


    /* Do exactly the same to create the second set of tasks, only this time
    provide a block time for the semaphore calls. */
    xSemaphoreCreateBinary( acSemaphore2, &xSecondSemaphoreParameters.xSemaphore );

    if( xSecondSemaphoreParameters.xSemaphore != NULL )
    {
#if ( configQUEUE_REGISTRY_SIZE > 0 )
        /* Add to Queue Registry */
        vQueueAddToRegistry( xSecondSemaphoreParameters.xSemaphore, "BinarySemaphore #2" );
#endif

        xSecondSemaphoreParameters.ulSharedVariable = semtstBLOCKING_EXPECTED_VALUE;
        xSecondSemaphoreParameters.xBlockTime = xBlockTime / configTICK_RATE_MS;

        xTestTask3Parameters.uxPriority = uxPriority;
        xTestTask4Parameters.uxPriority = uxPriority;
        if( xTaskCreate( &xTestTask3Parameters, ( portTaskHandleType * ) NULL ) != pdPASS )
        {
            xStatus = pdFAIL;
        }
        if( xTaskCreate( &xTestTask4Parameters, ( portTaskHandleType * ) NULL ) != pdPASS )
        {
            xStatus = pdFAIL;
        }
    }
    else
    {
        xStatus = pdFAIL;
    }

    return xStatus;
}
/*---------------------------------------------------------------------------*/

static void prvSemaphoreTest( void *pvParameters )
{
    xSemaphoreParameters *pxParameters;
    portUInt32Type ulExpectedValue;
    portUInt32Type ulCounter;
    portBaseType xError = pdFALSE;
    portInt16Type sCheckVariableToUse;

    /* See which check variable to use.  sNextCheckVariable is not semaphore
    protected! */
    taskENTER_CRITICAL();
    {
        sCheckVariableToUse = sNextCheckVariable;
        sNextCheckVariable++;
    }
    taskEXIT_CRITICAL();

    /* A structure is passed in as the parameter.  This contains the shared
    variable being guarded. */
    pxParameters = ( xSemaphoreParameters * ) pvParameters;

    /* If we are blocking we use a much higher count to ensure loads of context
    switches occur during the count. */
    if( pxParameters->xBlockTime > ( portTickType ) 0 )
    {
        ulExpectedValue = semtstBLOCKING_EXPECTED_VALUE;
    }
    else
    {
        ulExpectedValue = semtstNON_BLOCKING_EXPECTED_VALUE;
    }

    for( ;; )
    {
        /* Try to obtain the semaphore. */
        if( xSemaphoreTake( pxParameters->xSemaphore, pxParameters->xBlockTime ) == pdPASS )
        {
            /* We have the semaphore and so expect any other tasks using the
            shared variable to have left it in the state we expect to find
            it. */
            if( pxParameters->ulSharedVariable != ulExpectedValue )
            {
                xError = pdTRUE;
            }

            /* Clear the variable, then count it back up to the expected value
            before releasing the semaphore.  Would expect a context switch or
            two during this time. */
            for( ulCounter = ( portUInt32Type ) 0; ulCounter <= ulExpectedValue; ulCounter++ )
            {
                pxParameters->ulSharedVariable = ulCounter;
                if( pxParameters->ulSharedVariable != ulCounter )
                {
                    xError = pdTRUE;
                }
            }

            /* Release the semaphore, and if no errors have occurred increment the check
            variable. */
            if( xSemaphoreGive( pxParameters->xSemaphore ) == pdFALSE )
            {
                xError = pdTRUE;
            }

            if( pdFALSE == xError )
            {
                if( sCheckVariableToUse < semtstNUM_TASKS )
                {
                    ( asCheckVariables[ sCheckVariableToUse ] )++;
                }
            }

            /* If we have a block time then we are running at a priority higher
            than the idle priority.  This task takes a long time to complete
            a cycle (deliberately so to test the guarding) so will be starving
            out lower priority tasks.  Block for some time to allow give lower
            priority tasks some processor time. */
            if( xTaskDelay( pxParameters->xBlockTime * semtstDELAY_FACTOR ) != pdPASS )
            {
                xError = pdTRUE;
            }
        }
        else
        {
            if( pxParameters->xBlockTime == ( portTickType ) 0 )
            {
                /* We have not got the semaphore yet, so no point using the
                processor.  We are not blocking when attempting to obtain the
                semaphore. */
                taskYIELD();
            }
        }
    }
}
/*---------------------------------------------------------------------------*/

/* This is called to check that all the created tasks are still running. */
portBaseType xAreSemaphoreTasksStillRunning( void )
{
    portspecCOMMON_PRIV_DATA_SECTION static portInt16Type asLastCheckVariables[ semtstNUM_TASKS ] = { 0 };
    portBaseType xTask, xReturn = pdTRUE;

    for( xTask = 0; xTask < semtstNUM_TASKS; xTask++ )
    {
        if( asLastCheckVariables[ xTask ] == asCheckVariables[ xTask ] )
        {
            xReturn = pdFALSE;
        }

        asLastCheckVariables[ xTask ] = asCheckVariables[ xTask ];
    }

    return xReturn;
}


