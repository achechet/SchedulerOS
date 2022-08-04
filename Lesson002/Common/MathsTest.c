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

/* Creates eight tasks, each of which loops continuously performing a floating
 * point calculation - using single precision variables.
 *
 * All the tasks run at the idle priority and never block or yield. This causes
 * all eight tasks to time slice with the idle task. Running at the idle
 * priority means that these tasks will get preempted any time another task is
 * ready to run or a time slice occurs. More often than not, the preemption
 * will occur mid calculation, creating a good test of the schedulers context
 * switch mechanism - a calculation producing an unexpected result could be a
 * symptom of a corruption in the context of a task. */

/* PortSpecifics.h includes some 'declare once' declarations. */
#define MATHS_TEST_C

/* Library includes. */
#include <stdlib.h>
#include <math.h>

/* Scheduler includes. */
#include "SafeRTOS_API.h"

/* Demo application includes. */
#include "MathsTest.h"
#include "PortSpecifics.h"

/*-----------------------------------------------------------------------------
 * Constant Definitions.
 *---------------------------------------------------------------------------*/
#define mathsUNPRIV_TASK_1      ( 0u )
#define mathsUNPRIV_TASK_2      ( 1u )
#define mathsUNPRIV_TASK_3      ( 2u )
#define mathsUNPRIV_TASK_4      ( 3u )

#define mathsPRIV_TASK_1        ( 4u )
#define mathsPRIV_TASK_2        ( 5u )
#define mathsPRIV_TASK_3        ( 6u )
#define mathsPRIV_TASK_4        ( 7u )

#define mathsNUMBER_OF_TASKS    ( 8u )

#define mathsTEST_ARRAY_SIZE    ( 10u )


/*-----------------------------------------------------------------------------
 * Filescope Function Declarations
 *---------------------------------------------------------------------------*/
static void prvCompetingMathTask1( void *pvParameters );
static void prvCompetingMathTask2( void *pvParameters );
static void prvCompetingMathTask3( void *pvParameters );
static void prvCompetingMathTask4( void *pvParameters );

/*-----------------------------------------------------------------------------
 * Filescope Variables
 *---------------------------------------------------------------------------*/
/* Task TCBs */
portspecTCB_DATA_SECTION static xTCB xUnprivilegedTask1TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xUnprivilegedTask2TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xUnprivilegedTask3TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xUnprivilegedTask4TCB = { 0 };

portspecTCB_DATA_SECTION static xTCB xPrivilegedTask1TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xPrivilegedTask2TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xPrivilegedTask3TCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xPrivilegedTask4TCB = { 0 };

/* Keep a history of check variables so we know if they have been
 * incremented since the last call. */
portspecCOMMON_PRIV_DATA_SECTION static portUnsignedBaseType auxLastTaskCheck[ mathsNUMBER_OF_TASKS ] = { 0U };

/* These variables are used to check that all the tasks are still running.
 * If a task gets a calculation wrong it will stop incrementing its check variable. */
portspecMATHS_TEST_DATA_SECTION static volatile portUnsignedBaseType auxTaskCheck[ mathsNUMBER_OF_TASKS ] = { 0U };


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/
portBaseType xStartMathsTasks( void )
{
    portBaseType xStatus = pdPASS;
    xTaskParameters xUnprivilegedTask1Parameters =
    {
        prvCompetingMathTask1,                              /* Task code */
        "Unprivileged Task 1",                              /* Task name */
        &xUnprivilegedTask1TCB,                             /* TCB */
        acUnprivilegedTask1Stack,                           /* Stack buffer */
        portspecMATHS_TEST_STACK_SIZE,                      /* Stack depth bytes */
        ( void * )( &auxTaskCheck[ mathsUNPRIV_TASK_1 ] ),   /* Parameters */
        taskIDLE_PRIORITY,                                  /* Priority */
        NULL,                                               /* TLS object */
        xMathsUnprivTask1PortParameters                     /* Port-specific task parameters */
    };

    xTaskParameters xUnprivilegedTask2Parameters =
    {
        prvCompetingMathTask2,                              /* Task code */
        "Unprivileged Task 2",                              /* Task name */
        &xUnprivilegedTask2TCB,                             /* TCB */
        acUnprivilegedTask2Stack,                           /* Stack buffer */
        portspecMATHS_TEST_STACK_SIZE,                      /* Stack depth bytes */
        ( void * )( &auxTaskCheck[ mathsUNPRIV_TASK_2 ] ),   /* Parameters */
        taskIDLE_PRIORITY,                                  /* Priority */
        NULL,                                               /* TLS object */
        xMathsUnprivTask2PortParameters                     /* Port-specific task parameters */
    };

    xTaskParameters xUnprivilegedTask3Parameters =
    {
        prvCompetingMathTask3,                              /* Task code */
        "Unprivileged Task 3",                              /* Task name */
        &xUnprivilegedTask3TCB,                             /* TCB */
        acUnprivilegedTask3Stack,                           /* Stack buffer */
        portspecMATHS_TEST_STACK_SIZE,                      /* Stack depth bytes */
        ( void * )( &auxTaskCheck[ mathsUNPRIV_TASK_3 ] ),   /* Parameters */
        taskIDLE_PRIORITY,                                  /* Priority */
        NULL,                                               /* TLS object */
        xMathsUnprivTask3PortParameters                     /* Port-specific task parameters */
    };

    xTaskParameters xUnprivilegedTask4Parameters =
    {
        prvCompetingMathTask4,                              /* Task code */
        "Unprivileged Task 4",                              /* Task name */
        &xUnprivilegedTask4TCB,                             /* TCB */
        acUnprivilegedTask4Stack,                           /* Stack buffer */
        portspecMATHS_TEST_STACK_SIZE,                      /* Stack depth bytes */
        ( void * )( &auxTaskCheck[ mathsUNPRIV_TASK_4 ] ),   /* Parameters */
        taskIDLE_PRIORITY,                                  /* Priority */
        NULL,                                               /* TLS object */
        xMathsUnprivTask4PortParameters                     /* Port-specific task parameters */
    };

    xTaskParameters xPrivilegedTask1Parameters =
    {
        prvCompetingMathTask1,                              /* Task code */
        "Privileged Task 1",                                /* Task name */
        &xPrivilegedTask1TCB,                               /* TCB */
        acPrivilegedTask1Stack,                             /* Stack buffer */
        portspecMATHS_TEST_STACK_SIZE,                      /* Stack depth bytes */
        ( void * )( &auxTaskCheck[ mathsPRIV_TASK_1 ] ),     /* Parameters */
        taskIDLE_PRIORITY,                                  /* Priority */
        NULL,                                               /* TLS object */
        xMathsPrivTask1PortParameters                       /* Port-specific task parameters */
    };

    xTaskParameters xPrivilegedTask2Parameters =
    {
        prvCompetingMathTask2,                              /* Task code */
        "Privileged Task 2",                                /* Task name */
        &xPrivilegedTask2TCB,                               /* TCB */
        acPrivilegedTask2Stack,                             /* Stack buffer */
        portspecMATHS_TEST_STACK_SIZE,                      /* Stack depth bytes */
        ( void * )( &auxTaskCheck[ mathsPRIV_TASK_2 ] ),     /* Parameters */
        taskIDLE_PRIORITY,                                  /* Priority */
        NULL,                                               /* TLS object */
        xMathsPrivTask2PortParameters                       /* Port-specific task parameters */
    };

    xTaskParameters xPrivilegedTask3Parameters =
    {
        prvCompetingMathTask3,                              /* Task code */
        "Privileged Task 3",                                /* Task name */
        &xPrivilegedTask3TCB,                               /* TCB */
        acPrivilegedTask3Stack,                             /* Stack buffer */
        portspecMATHS_TEST_STACK_SIZE,                      /* Stack depth bytes */
        ( void * )( &auxTaskCheck[ mathsPRIV_TASK_3 ] ),     /* Parameters */
        taskIDLE_PRIORITY,                                  /* Priority */
        NULL,                                               /* TLS object */
        xMathsPrivTask3PortParameters                       /* Port-specific task parameters */
    };

    xTaskParameters xPrivilegedTask4Parameters =
    {
        prvCompetingMathTask4,                              /* Task code */
        "Privileged Task 4",                                /* Task name */
        &xPrivilegedTask4TCB,                               /* TCB */
        acPrivilegedTask4Stack,                             /* Stack buffer */
        portspecMATHS_TEST_STACK_SIZE,                      /* Stack depth bytes */
        ( void * )( &auxTaskCheck[ mathsPRIV_TASK_4 ] ),     /* Parameters */
        taskIDLE_PRIORITY,                                  /* Priority */
        NULL,                                               /* TLS object */
        xMathsPrivTask4PortParameters                       /* Port-specific task parameters */
    };

    /* Create the unprivileged test tasks. */
    if( xTaskCreate( &xUnprivilegedTask1Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xUnprivilegedTask2Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xUnprivilegedTask3Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xUnprivilegedTask4Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }

    /* Create the privileged test tasks. */
    if( xTaskCreate( &xPrivilegedTask1Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xPrivilegedTask2Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xPrivilegedTask3Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }
    if( xTaskCreate( &xPrivilegedTask4Parameters, NULL ) != pdPASS )
    {
        xStatus = pdFAIL;
    }

    return xStatus;
}
/*---------------------------------------------------------------------------*/

/* This is called to check that all the created tasks are still running. */
portBaseType xAreMathsTasksStillRunning( void )
{
    portUnsignedBaseType uxIndex;
    portBaseType xReturn = pdTRUE;

    /* Check that the maths tasks are still running by ensuring that their
     * check variables are still incrementing... */
    for( uxIndex = 0U; uxIndex < mathsNUMBER_OF_TASKS; uxIndex++ )
    {
        if( auxTaskCheck[ uxIndex ] == auxLastTaskCheck[ uxIndex ] )
        {
            /* The check has not incremented so an error exists. */
            xReturn = pdFALSE;
        }

        auxLastTaskCheck[ uxIndex ] = auxTaskCheck[ uxIndex ];
    }

    return xReturn;
}
/*-----------------------------------------------------------------------------
 * Filescope Functions
 *---------------------------------------------------------------------------*/

static void prvCompetingMathTask1( void *pvParameters )
{
    volatile portFloat32Type fVariable1, fVariable2, fVariable3;
    volatile portUnsignedBaseType *puxTaskCheckVariable;
    portFloat32Type fExpectedAnswer, fTestAnswer;
    portBaseType xError = pdFALSE;

    /* Perform a calculation and record the answer - this recorded answer will
     * be used to verify that the task continues to generate the same answer. */
    fVariable1 = 123.4567F;
    fVariable2 = 2345.6789F;
    fVariable3 = -918.222F;
    fExpectedAnswer = fVariable1;
    fExpectedAnswer += fVariable2;
    fExpectedAnswer *= fVariable3;

    /* The variable this task increments to show it is still running is passed
     * in as the parameter. */
    puxTaskCheckVariable = ( portUnsignedBaseType * ) pvParameters;

    /* Keep performing the calculation and checking the result against the
     * answer recorded above. */
    for( ;; )
    {
        /* Perform the calculation once more. */
        fVariable1 = 123.4567F;
        fVariable2 = 2345.6789F;
        fVariable3 = -918.222F;
        fTestAnswer = fVariable1;
        fTestAnswer += fVariable2;
        fTestAnswer *= fVariable3;

        /* If the calculation does not match the expected constant, stop the
        * increment of the check variable. */
        if( fabs( ( portFloat64Type )( fTestAnswer - fExpectedAnswer ) ) > ( ( portFloat64Type ) 0.001 ) )
        {
            xError = pdTRUE;
        }

        if( pdFALSE == xError )
        {
            /* If the calculation has always been correct, increment the check
             * variable so we know this task is still running okay. */
            ( *puxTaskCheckVariable )++;
        }
    }
}
/*---------------------------------------------------------------------------*/

static void prvCompetingMathTask2( void *pvParameters )
{
    volatile portFloat32Type fVariable1, fVariable2, fVariable3;
    volatile portUnsignedBaseType *puxTaskCheckVariable;
    portFloat32Type fExpectedAnswer, fTestAnswer;
    portBaseType xError = pdFALSE;

    /* Perform a calculation and record the answer - this recorded answer will
     * be used to verify that the task continues to generate the same answer. */
    fVariable1 = -389.38F;
    fVariable2 = 32498.2F;
    fVariable3 = -2.0001F;
    fExpectedAnswer = fVariable1;
    fExpectedAnswer /= fVariable2;
    fExpectedAnswer *= fVariable3;

    /* The variable this task increments to show it is still running is passed
     * in as the parameter. */
    puxTaskCheckVariable = ( portUnsignedBaseType * ) pvParameters;

    /* Keep performing the calculation and checking the result against the
     * answer recorded above. */
    for( ;; )
    {
        fVariable1 = -389.38F;
        fVariable2 = 32498.2F;
        fVariable3 = -2.0001F;
        fTestAnswer = fVariable1;
        fTestAnswer /= fVariable2;
        fTestAnswer *= fVariable3;

        /* If the calculation does not match the expected constant, stop the
         * increment of the check variable. */
        if( fabs( ( portFloat64Type )( fTestAnswer - fExpectedAnswer ) ) > ( ( portFloat64Type ) 0.001 ) )
        {
            xError = pdTRUE;
        }

        if( pdFALSE == xError )
        {
            /* If the calculation has always been correct, increment the check
             * variable so we know this task is still running okay. */
            ( *puxTaskCheckVariable )++;
        }
    }
}
/*---------------------------------------------------------------------------*/

static void prvCompetingMathTask3( void *pvParameters )
{
    volatile portFloat32Type fTotal1, fTotal2, fPosition;
    volatile portUnsignedBaseType *puxTaskCheckVariable;
    portFloat32Type fDifference, fTemp;
    portUnsignedBaseType uxIndex;
    portBaseType xError = pdFALSE;

    /* An array of floats - declared on the task stack as there will be 2 tasks
     * executing this code. */
    volatile portFloat32Type afArray[ mathsTEST_ARRAY_SIZE ] = { 0.0F };

    /* The variable this task increments to show it is still running is passed
     * in as the parameter. */
    puxTaskCheckVariable = ( portUnsignedBaseType * ) pvParameters;

    /* Keep filling an array, keeping a running total of the values placed in
     * the array. Then run through the array adding up all the values. If the
     * two totals do not match, stop the check variable from incrementing. */
    for( ;; )
    {
        fTotal1 = 0.0F;
        fTotal2 = 0.0F;
        fPosition = 0.0F;

        /* Fill the array with known values. */
        for( uxIndex = 0U; uxIndex < mathsTEST_ARRAY_SIZE; uxIndex++ )
        {
            afArray[ uxIndex ] = fPosition + 5.5F;
            fTemp = fPosition + 5.5F;
            fTotal1 += fTemp;
            fPosition += 0.1F;
        }

        /* Calculate the total of all the array elements. */
        for( uxIndex = 0U; uxIndex < mathsTEST_ARRAY_SIZE; uxIndex++ )
        {
            fTemp = afArray[ uxIndex ];
            fTotal2 += fTemp;
        }

        /* Check the two totals match. */
        fDifference = fTotal1;
        fDifference -= fTotal2;
        if( fabs( ( portFloat64Type ) fDifference ) > ( ( portFloat64Type ) 0.001 ) )
        {
            xError = pdTRUE;
        }

        if( pdFALSE == xError )
        {
            /* If the calculation has always been correct, increment the check
             * variable so we know this task is still running okay. */
            ( *puxTaskCheckVariable )++;
        }
    }
}
/*---------------------------------------------------------------------------*/

static void prvCompetingMathTask4( void *pvParameters )
{
    volatile portFloat32Type fTotal1, fTotal2, fPosition;
    volatile portUnsignedBaseType *puxTaskCheckVariable;
    portFloat32Type fTemp, fDifference;
    portUnsignedBaseType uxIndex;
    portBaseType xError = pdFALSE;

    /* An array of floats - declared on the task stack as there will be 2 tasks
     * executing this code. */
    volatile portFloat32Type afArray[ mathsTEST_ARRAY_SIZE ] = { 0.0F };

    /* The variable this task increments to show it is still running is passed
     * in as the parameter. */
    puxTaskCheckVariable = ( portUnsignedBaseType * ) pvParameters;

    /* Keep filling an array, keeping a running total of the values placed in
     * the array. Then run through the array adding up all the values. If the
     * two totals do not match, stop the check variable from incrementing. */
    for( ;; )
    {
        fTotal1 = 0.0F;
        fTotal2 = 0.0F;
        fPosition = 0.0F;

        /* Fill the array with known values. */
        for( uxIndex = 0U; uxIndex < mathsTEST_ARRAY_SIZE; uxIndex++ )
        {
            afArray[ uxIndex ] = fPosition * 12.123F;
            fTemp = fPosition * 12.123F;
            fTotal1 += fTemp;
            fPosition += 0.1F;
        }

        /* Calculate the total of all the array elements. */
        for( uxIndex = 0U; uxIndex < mathsTEST_ARRAY_SIZE; uxIndex++ )
        {
            fTemp = afArray[ uxIndex ];
            fTotal2 += fTemp;
        }

        /* Check the two totals match. */
        fDifference = fTotal1;
        fDifference -= fTotal2;
        if( fabs( ( portFloat64Type ) fDifference ) > ( ( portFloat64Type ) 0.001 ) )
        {
            xError = pdTRUE;
        }

        if( pdFALSE == xError )
        {
            /* If the calculation has always been correct, increment the check
             * variable so we know this task is still running okay. */
            ( *puxTaskCheckVariable )++;
        }
    }
}
/*---------------------------------------------------------------------------*/
