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
#define COMTEST_C

#include "SafeRTOS_API.h"
#include "serial.h"
#include "PortSpecifics.h"
#include "queue_register.h"

#include "stm32l4xx_hal.h"

/* Serial interrupt priority.
   This is set to the maximum interrupt priority level that SafeRTOS API calls
   are permitted to use. */
#define serINTERRUPT_PRIORITY           ( configSYSTEM_INTERRUPT_PRIORITY >> 4U )

/* Some driver specifics. */
#define serNO_BLOCK                     ( ( portTickType ) 0 )

/* Define the buffers to be used by the queues.  These can hold 30 bytes. */
#define serQUEUE_LENGTH                 ( 30 )
#define serRX_QUEUE_BUFFER_LEN          ( serQUEUE_LENGTH + portQUEUE_OVERHEAD_BYTES )
#define serTX_QUEUE_BUFFER_LEN          ( serQUEUE_LENGTH + portQUEUE_OVERHEAD_BYTES )
#pragma data_alignment=4
static portInt8Type acRxQueueBuffer[ serRX_QUEUE_BUFFER_LEN ];
#pragma data_alignment=4
static portInt8Type acTxQueueBuffer[ serTX_QUEUE_BUFFER_LEN ];

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
portspecCOM_TEST_ZERO_DATA_SECTION static xQueueHandle xRxedChars= NULL;
portspecCOM_TEST_ZERO_DATA_SECTION static xQueueHandle xCharsForTx = NULL;
portspecCOM_TEST_ZERO_DATA_SECTION static portBaseType xCharsWaitingForTx = 0L;

/*-----------------------------------------------------------*/

void vSerialPortInit( void )
{
    portBaseType xRxQueueStatus, xTxQueueStatus;
    UART_HandleTypeDef xUSARTDef = { 0 };
    GPIO_InitTypeDef GPIO_Init_Structure;

    /* Create the queues used to hold Rx and Tx characters. */
    xRxQueueStatus = xQueueCreate( acRxQueueBuffer, serRX_QUEUE_BUFFER_LEN, serQUEUE_LENGTH, ( portUnsignedBaseType ) sizeof( portCharType ), &xRxedChars );
    xTxQueueStatus = xQueueCreate( acTxQueueBuffer, serTX_QUEUE_BUFFER_LEN, serQUEUE_LENGTH, ( portUnsignedBaseType ) sizeof( portCharType ), &xCharsForTx );
#if configQUEUE_REGISTRY_SIZE > 0
    vQueueAddToRegistry( xRxedChars, "Rx Chars" );
    vQueueAddToRegistry( xCharsForTx, "Tx Chars" );
#endif

    if( ( xRxQueueStatus == pdPASS ) && ( xTxQueueStatus == pdPASS ) )
    {
        portENTER_CRITICAL();
        {

            /* Enable GPIO clock */
            __HAL_RCC_GPIOB_CLK_ENABLE();

            /* Enable UART clock */
            __HAL_RCC_USART1_CLK_ENABLE();

            /* Connect PXx to USARTx_Tx*/
            /* Connect PXx to USARTx_Rx*/
            /* Configure USART Tx as alternate function  */
            /* Configure USART Rx as alternate function  */

            GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
            GPIO_Init_Structure.Pull = GPIO_PULLUP;
            GPIO_Init_Structure.Speed = GPIO_SPEED_HIGH;
            GPIO_Init_Structure.Alternate = GPIO_AF7_USART1;
            GPIO_Init_Structure.Pin = GPIO_PIN_6 | GPIO_PIN_7;

            HAL_GPIO_Init(GPIOB, &GPIO_Init_Structure);

            xUSARTDef.Instance = USART1;
            xUSARTDef.Init.BaudRate = 38400;
            xUSARTDef.Init.HwFlowCtl = UART_HWCONTROL_NONE;
            xUSARTDef.Init.Mode = UART_MODE_TX_RX;
            xUSARTDef.Init.Parity = UART_PARITY_NONE;
            xUSARTDef.Init.StopBits = UART_STOPBITS_1;
            xUSARTDef.Init.WordLength = UART_WORDLENGTH_8B;
            xUSARTDef.Init.OverSampling = 0;

            HAL_UART_Init( &xUSARTDef );

            /* Enable the USART Interrupt */
            __HAL_UART_ENABLE_IT( &xUSARTDef, UART_IT_RXNE );

            NVIC_SetPriority( USART1_IRQn, serINTERRUPT_PRIORITY );
            NVIC_EnableIRQ( USART1_IRQn );

        }
        portEXIT_CRITICAL();
    }
}

/*-----------------------------------------------------------*/

portBaseType xSerialGetChar( portCharType *pcRxedChar, portTickType xBlockTime )
{
    /* Get the next character from the buffer.  Return false if no characters
    are available, or arrive before xBlockTime expires. */
    if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) == pdPASS )
    {
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}

/*-----------------------------------------------------------*/

portBaseType xSerialPutChar( portCharType cOutChar, portTickType xBlockTime )
{
    portBaseType xReturn = pdPASS;
    UART_HandleTypeDef xUSARTDef;

    xUSARTDef.Instance = USART1;

    /* Place the character in the queue of characters to be transmitted. */
    portENTER_CRITICAL();
    {
        if( xCharsWaitingForTx == pdFALSE )
        {
            xCharsWaitingForTx = pdTRUE;
            xUSARTDef.Instance->TDR = cOutChar;
        }
        else
        {
            if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
            {
                xReturn = pdFAIL;
            }
            else
            {
                /*
                 * Depending on queue sizing and task prioritisation:  While we
                 * were blocked waiting to post, interrupts were not disabled.
                 * It is possible that the serial ISR has emptied the Tx queue,
                 * in which case we need to start the Tx off again.
                 */
                if( pdFALSE == xCharsWaitingForTx )
                {
                    xQueueReceive( xCharsForTx, &cOutChar, serNO_BLOCK );
                    xCharsWaitingForTx = pdTRUE;
                    xUSARTDef.Instance->TDR = cOutChar;
                }
            }
        }
    }
    portEXIT_CRITICAL();

    return xReturn;
}
/*-----------------------------------------------------------*/

void vSerialTestIntrHandler( void )
{
    portUInt8Type cChar;
    portBaseType xHigherPriorityTaskWoken = pdFALSE;
    extern portBaseType xTestInterruptHook( void );
    UART_HandleTypeDef xUSARTDef;

    xUSARTDef.Instance = USART1;

    if( __HAL_UART_GET_FLAG( &xUSARTDef, UART_FLAG_RXNE ) != RESET )
    {
        /* Clear the USART Receive interrupt */
        __HAL_UART_CLEAR_FLAG( &xUSARTDef, UART_FLAG_RXNE );
        cChar = xUSARTDef.Instance->RDR;

        xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );

        /* Are there any more characters to transmit? */
        if( xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
        {
            /* A character was retrieved from the queue so can be sent now. */
            xUSARTDef.Instance->TDR = cChar;
        }
        else
        {
            xCharsWaitingForTx = pdFALSE;
        }
    }

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/*-----------------------------------------------------------*/


