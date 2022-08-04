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

/*-----------------------------------------------------------
 * Simple IO routines for the LED's.
 *-----------------------------------------------------------*/


/* Scheduler includes. */
#include "SafeRTOS_API.h"

/* ST driver library includes. */
#include "stm32l4xx_hal.h"

/* Demo application includes. */
#include "partest.h"

/*---------------------------------------------------------------------------*/

#if defined( USE_STM32L476G_DISCO_REVC )

#define partestMAX_USER_LED_NUMBER  ( 2U )
/* Pin mapping for the user LEDs. */
const portUInt16Type usLEDPinMapping[ partestMAX_USER_LED_NUMBER ] =
{
    GPIO_PIN_2,
    GPIO_PIN_8
};
/* Port mapping for the user LEDs. */
GPIO_TypeDef *const pxLEDPortMapping[ partestMAX_USER_LED_NUMBER ] =
{
    GPIOB,
    GPIOE
};

#elif defined( USE_STM32L4XX_NUCLEO )

#define partestMAX_USER_LED_NUMBER  ( 1U )

/* Pin mapping for the user LEDs. */
const portUInt16Type usLEDPinMapping[ partestMAX_USER_LED_NUMBER ] =
{
    GPIO_PIN_5
};

/* Port mapping for the user LEDs. */
GPIO_TypeDef *const pxLEDPortMapping[ partestMAX_USER_LED_NUMBER ] =
{
    GPIOA
};

#else
#error "Board not defined!"
#endif

/*---------------------------------------------------------------------------*/

void vParTestInitialise( void )
{
    /* GPIO structure defined in library files. */
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

#ifdef USE_STM32L476G_DISCO_REVC

    /* Enable the GPIO_LED Clocks. */
    __GPIOB_CLK_ENABLE();
    __GPIOE_CLK_ENABLE();

    /* Configure the LED output pins - PB2 and PE8 as output push-pull. */

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStructure.Pin = GPIO_PIN_2;
    HAL_GPIO_Init( GPIOB, &GPIO_InitStructure );

    GPIO_InitStructure.Pin = GPIO_PIN_8;
    HAL_GPIO_Init( GPIOE, &GPIO_InitStructure );

#else

    /* Enable the GPIO_LED Clocks. */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure the LED output pin - PA5 */

    GPIO_InitStructure.Pin = GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStructure );

#endif

    vParTestSetLED( 0u, pdTRUE );
}
/*---------------------------------------------------------------------------*/

void vParTestSetLED( portUnsignedBaseType uxLED, portBaseType xValue )
{
    if( uxLED < partestMAX_USER_LED_NUMBER )
    {
        /* Set or clear the output. */
        if( pdFALSE != xValue )
        {
            /* Switch on the specified LED. */
            HAL_GPIO_WritePin( pxLEDPortMapping[ uxLED ], usLEDPinMapping[ uxLED ], GPIO_PIN_SET );
        }
        else
        {
            /* Switch off the specified LED. */
            HAL_GPIO_WritePin( pxLEDPortMapping[ uxLED ], usLEDPinMapping[ uxLED ], GPIO_PIN_RESET );
        }
    }
}
/*---------------------------------------------------------------------------*/

void vParTestToggleLED( portUnsignedBaseType uxLED )
{
    if( uxLED < partestMAX_USER_LED_NUMBER )
    {
        HAL_GPIO_TogglePin( pxLEDPortMapping[ uxLED ], usLEDPinMapping[ uxLED ] );
    }
}
/*---------------------------------------------------------------------------*/
