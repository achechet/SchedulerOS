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
 * Creates two tasks that operate on an interrupt driven serial port.  A
 * loopback connector should be used so that everything that is transmitted is
 * also received.  The serial port does not use any flow control.  On a
 * standard 9way 'D' connector pins two and three should be connected together.
 *
 * The first task posts a sequence of characters to the Tx queue, toggling an
 * LED on each successful post.  At the end of the sequence it sleeps for a
 * pseudo-random period before resending the same sequence.
 *
 * The UART Tx end interrupt is enabled whenever data is available in the Tx
 * queue.  The Tx end ISR removes a single character from the Tx queue and
 * passes it to the UART for transmission.
 *
 * The second task blocks on the Rx queue waiting for a character to become
 * available.  When the UART Rx end interrupt receives a character it places
 * it in the Rx queue, waking the second task.  The second task checks that the
 * characters removed from the Rx queue form the same sequence as those posted
 * to the Tx queue, and toggles an LED for each correct character.
 *
 * The receiving task is spawned with a higher priority than the transmitting
 * task.  The receiver will therefore wake every time a character is
 * transmitted so neither the Tx or Rx queue should ever hold more than a few
 * characters.
 */

/* PortSpecifics.h may include some 'declare once' declarations. */
#define COMTEST_C

/* Scheduler include files. */
#include <stdlib.h>
#include "SafeRTOS_API.h"

/* Demo program include files. */
#include "comtest.h"
#include "PortSpecifics.h"
#include "serial.h"
#include "partest.h"

#define comTX_LED_OFFSET            ( 0 )
#define comRX_LED_OFFSET            ( 1 )
#define comTOTAL_PERMISSIBLE_ERRORS ( 2 )

/* The Tx task will transmit the sequence of characters at a pseudo random
interval.  This is the maximum and minimum block time between sends. */
#define comTX_MAX_BLOCK_TIME        ( ( portTickType ) 0x96 )
#define comTX_MIN_BLOCK_TIME        ( ( portTickType ) 0x32 )
#define comOFFSET_TIME              ( ( portTickType ) 3 )

/* We should find that each character can be queued for Tx immediately and we
don't have to block to send. */
#define comNO_BLOCK                 ( ( portTickType ) 0 )

/* The Rx task will block on the Rx queue for a long period. */
#define comRX_BLOCK_TIME            ( ( portTickType ) 0xffff )

/* The sequence transmitted is from comFIRST_BYTE to and including comLAST_BYTE. */
#define comFIRST_BYTE               ( 'A' )
#define comLAST_BYTE                ( 'X' )

#define comBUFFER_LEN               ( ( portUnsignedBaseType )( comLAST_BYTE - comFIRST_BYTE ) + ( portUnsignedBaseType ) 1 )
#define comINITIAL_RX_COUNT_VALUE   ( 0 )

/* The transmit task as described at the top of the file. */
static void vComTxTask( void *pvParameters );

/* The receive task as described at the top of the file. */
static void vComRxTask( void *pvParameters );

/* The LED that should be toggled by the Rx and Tx tasks. The Rx task will
 * toggle LED ( uxBaseLED + comRX_LED_OFFSET). The Tx task will toggle LED
 * ( uxBaseLED + comTX_LED_OFFSET ). */
portspecCOM_TEST_DATA_SECTION static portUnsignedBaseType uxBaseLED = 2;

/* Check variable used to ensure no error have occurred. The Rx task will
 * increment this variable after every successfully received sequence. If at any
 * time the sequence is incorrect the the variable will stop being incremented. */
portspecCOM_TEST_ZERO_DATA_SECTION static volatile portUnsignedBaseType uxRxLoops = comINITIAL_RX_COUNT_VALUE;

/* TCBs for the two tasks. */
portspecTCB_DATA_SECTION static xTCB xTxTaskTCB = { 0 };
portspecTCB_DATA_SECTION static xTCB xRxTaskTCB = { 0 };

/*---------------------------------------------------------------------------*/

portBaseType xAltStartComTestTasks( portUnsignedBaseType uxPriority, portUnsignedBaseType uxLED )
{
    portBaseType xReturn = pdPASS;
    xTaskParameters xComTxTaskParameters =
    {
        vComTxTask,                         /* Task code */
        "COMTx",                            /* Task name */
        &xTxTaskTCB,                        /* TCB */
        acTxStack,                          /* Stack buffer */
        portspecCOM_TEST_TASK_STACK_SIZE,   /* Stack depth bytes */
        NULL,                               /* Parameters */
        taskIDLE_PRIORITY,                  /* Priority */
        NULL,                               /* TLS object */
        xComTxTaskPortParameters            /* Port-specific task parameters */
    };
    xTaskParameters xComRxTaskParameters =
    {
        vComRxTask,                         /* Task code */
        "COMRx",                            /* Task name */
        &xRxTaskTCB,                        /* TCB */
        acRxStack,                          /* Stack buffer */
        portspecCOM_TEST_TASK_STACK_SIZE,   /* Stack depth bytes */
        NULL,                               /* Parameters */
        taskIDLE_PRIORITY,                  /* Priority */
        NULL,                               /* TLS object */
        xComRxTaskPortParameters            /* Port-specific task parameters */
    };


    /* Initialise the com port then spawn the Rx and Tx tasks. */
    uxBaseLED = uxLED;
    vSerialPortInit();

    /* The Tx task is spawned with a lower priority than the Rx task. */
    xComTxTaskParameters.uxPriority = uxPriority - 1;
    if( xTaskCreate( &xComTxTaskParameters, ( portTaskHandleType * ) NULL ) != pdPASS )
    {
        xReturn = pdFAIL;
    }

    xComRxTaskParameters.uxPriority = uxPriority;
    if( xTaskCreate( &xComRxTaskParameters, ( portTaskHandleType * ) NULL ) != pdPASS )
    {
        xReturn = pdFAIL;
    }

    return xReturn;
}
/*---------------------------------------------------------------------------*/

static void vComTxTask( void *pvParameters )
{
    portCharType cByteToSend;
    portTickType xTimeToWait;

    /* Just to stop compiler warnings. */
    ( void ) pvParameters;

    for( ;; )
    {
        /* Simply transmit a sequence of characters from comFIRST_BYTE to comLAST_BYTE. */
        for( cByteToSend = comFIRST_BYTE; cByteToSend <= comLAST_BYTE; cByteToSend++ )
        {
            if( xSerialPutChar( cByteToSend, comNO_BLOCK ) == pdPASS )
            {
                vParTestToggleLED( uxBaseLED + comTX_LED_OFFSET );
            }
        }

        /* Turn the LED off while we are not doing anything. */
        vParTestSetLED( uxBaseLED + comTX_LED_OFFSET, pdFALSE );

        /* We have posted all the characters in the string - wait before re-sending.
         * Wait a pseudo-random time as this will provide a better test. */
        xTimeToWait = xTaskGetTickCount() + comOFFSET_TIME;

        /* Make sure we don't wait too long... */
        xTimeToWait %= comTX_MAX_BLOCK_TIME;

        /* ...but we do want to wait. */
        if( xTimeToWait < comTX_MIN_BLOCK_TIME )
        {
            xTimeToWait = comTX_MIN_BLOCK_TIME;
        }

        xTaskDelay( xTimeToWait );
    }
}
/*---------------------------------------------------------------------------*/

static void vComRxTask( void *pvParameters )
{
    portCharType cExpectedByte;
    portCharType cByteRxed = '0';
    portBaseType xResyncRequired = pdFALSE, xErrorOccurred = pdFALSE;

    /* Just to stop compiler warnings. */
    ( void ) pvParameters;

    for( ;; )
    {
        /* We expect to receive the characters from comFIRST_BYTE to
         * comLAST_BYTE in an incrementing order.  Loop to receive each byte. */
        for( cExpectedByte = comFIRST_BYTE; cExpectedByte <= comLAST_BYTE; cExpectedByte++ )
        {
            /* Block on the queue that contains received bytes until a byte is
            available. */
            if( pdTRUE == xSerialGetChar( &cByteRxed, comRX_BLOCK_TIME ) )
            {
                /* Was this the byte we were expecting? If so, toggle the LED,
                 * otherwise we are out on sync and should break out of the loop
                 * until the expected character sequence is about to restart. */
                if( cByteRxed == cExpectedByte )
                {
                    vParTestToggleLED( uxBaseLED + comRX_LED_OFFSET );
                }
                else
                {
                    xResyncRequired = pdTRUE;
                    break;
                }
            }
        }

        /* Turn the LED off while we are not doing anything. */
        vParTestSetLED( uxBaseLED + comRX_LED_OFFSET, pdFALSE );

        /* Did we break out of the loop because the characters were received in
         * an unexpected order? If so wait here until the character sequence is
         * about to restart. */
        if( xResyncRequired == pdTRUE )
        {
            while( cByteRxed != comLAST_BYTE )
            {
                /* Block until the next char is available. */
                ( void )xSerialGetChar( &cByteRxed, comRX_BLOCK_TIME );
            }

            /* Note that an error occurred which caused us to have to resync.
             * We use this to stop incrementing the loop counter so
             * sAreComTestTasksStillRunning() will return false - indicating an
             * error. */
            xErrorOccurred++;

            /* We have now resynced with the Tx task and can continue. */
            xResyncRequired = pdFALSE;
        }
        else
        {
            if( xErrorOccurred < comTOTAL_PERMISSIBLE_ERRORS )
            {
                /* Increment the count of successful loops. As error occurring
                 * (i.e. an unexpected character being received) will prevent
                 * this counter being incremented for the rest of the execution.
                 * Don't worry about mutual exclusion on this variable - it
                 * doesn't really matter as we just want it to change. */
                uxRxLoops++;
            }
        }
    }
}
/*---------------------------------------------------------------------------*/

portBaseType xAreComTestTasksStillRunning( void )
{
    portBaseType xReturn;

    /* If the count of successful reception loops has not changed than at some
     * time an error occurred (i.e. a character was received out of sequence)
     * and we will return false. */
    if( comINITIAL_RX_COUNT_VALUE == uxRxLoops )
    {
        xReturn = pdFALSE;
    }
    else
    {
        xReturn = pdTRUE;
    }

    /* Reset the count of successful Rx loops. When this function is called
     * again we expect this to have been incremented. */
    uxRxLoops = comINITIAL_RX_COUNT_VALUE;

    return xReturn;
}
/*---------------------------------------------------------------------------*/
