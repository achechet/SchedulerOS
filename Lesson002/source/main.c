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

/*
 * This is a 'quick start' project to demonstrate the use of SafeRTOS running
 * on an STM32L476xx platform with MPU enabled.  It can be used to quickly
 * evaluate SafeRTOS and as a reference for the project settings and syntax
 * necessary to utilise SafeRTOS in an IAR based STM32L476xx project.
 *
 * This demo program is structured as follows:
 * - Kernel: this folder contains the product source code.
 * - Common: this folder contains standard demo tasks that are used to
 *          illustrate the RTOS operation.
 * - Libraries: the ST provided peripheral library files.
 * - FullDemo.c: Initialises the standard demo tasks and provides a check task
 *          to monitor the operation of the standard demo tasks.
 * - ParTest.c: provides simple routines to initialise and control the LEDs on
 *          the STM32L476xx board.
 * - PortSpecifics.c/h: Provide macros and declarations that tune the standard
 *          demo tasks for operation with IAR and the STM32L476xx processor.
 * - SafeRTOSConfig.c/h: Provide macro's and declaration that provide an example
 *          of configuring and initialising the SafeRTOS system. Note that these
 *          declarations are considered to be part of the host application but
 *          must be provided for correct operation of SafeRTOS.
 * - main.c: This provides an example of managing the scheduler initialisation,
 *          performing any required application initialisation and starting the
 *          scheduler.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* SafeRTOS includes. */
#include "SafeRTOS_API.h"

/* Firmware includes. */
#include "stm32l4xx_hal.h"

#if defined( USE_STM32L4XX_NUCLEO )
    #include "stm32l4xx_nucleo.h"
#endif

/* Demo application includes. */
#include "FullDemo.h"

#if ( INCLUDE_DEMO_DISPLAY == 1 )
    #include "DemoDisplay.h"
#endif

#include "ParTest.h"

/* AIRCR->PRIGROUP register setting:
  16 Group Priorities / 16 Subgroup Priorities */
#define mainPRIORITY_GROUPING       ( 3U )

/*---------------------------------------------------------------------------*/

/* Configure the processor for use with the demo board. */
static void prvSetupHardware( void );

/* The call to xTaskInitializeScheduler is included within a scheduler init
 * function. */
extern portBaseType xInitializeScheduler( void );

/* Determines which stack was in use when the MPU fault occurred, extracts the
 * stacked PC and displays an informative message on the LCD display. */
void vMainMPUFaultHandler( void );

/* Error Handler stub function. */
static void prvErrorHandler( void );

/*---------------------------------------------------------------------------*/

/*
 * Calls routines to:
 * - Initialise the hardware;
 * - Initialise the scheduler;
 * - Create the demo application tasks;
 * - Start the scheduler.
 */
int main( void )
{
    portBaseType xStatus = pdPASS;

    /* Perform any hardware set up necessary, QVGA, IO ports, etc. */
    prvSetupHardware();

    /* Initialise the kernel passing in a pointer to the xPortInit structure. */
    if( xInitializeScheduler() == pdPASS )
    {
#if ( INCLUDE_DEMO_DISPLAY == 1 )
        /* Create the Display Task and Queue */
        xStatus = xInitialiseDemoDisplay();
#endif

        /* Create the demonstration tasks. */
        if( pdPASS == xStatus )
        {
            xStatus = xFullDemoCreate();
        }

        /* If everything is OK so far, start scheduler here. */
        if( pdPASS == xStatus )
        {
            xStatus = xTaskStartScheduler( pdTRUE );
        }
    }

    /* The scheduler should now be running the tasks so this line should not
     * ever be reached. If it is reached, then the system initialisation
     * failed. */
    for( ;; );
}
/*---------------------------------------------------------------------------*/

static void prvSetupHardware( void )
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct;

    /* STM32L4xx HAL library initialisation. */
    HAL_Init();

    /* Disable the SYSTICK and clear any pending interrupts. The systick is
    automatically started by HAL_Init(). */
    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

    /*
     * System Clock Configuration
     */

#if defined( USE_STM32L476G_DISCO_REVC )

    /* The system Clock is configured as follows:
     *  System Clock source     = PLL (MSI)
     *  SYSCLK(Hz)              = 8000000
     *  HCLK(Hz)                = 8000000
     *  AHB Prescaler           = 1
     *  APB1 Prescaler          = 1
     *  APB2 Prescaler          = 1
     *  MSI Frequency(Hz)       = 4000000
     *  PLL_M                   = 1
     *  PLL_N                   = 40
     *  PLL_R                   = 2
     *  PLL_P                   = 7
     *  PLL_Q                   = 4
     *  Flash Latency(WS)       = 4
     */

    /* MSI is enabled after System reset, activate PLL with MSI as source. */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLP = 7;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK )
    {
        /* Initialisation error */
        prvErrorHandler();
    }

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
        prvErrorHandler();
    }

#elif defined( USE_STM32L4XX_NUCLEO )

    /* The system Clock is configured as follows:
     *  System Clock source     = PLL (MSI)
     *  SYSCLK(Hz)              = 80000000
     *  HCLK(Hz)                = 80000000
     *  AHB Prescaler           = 1
     *  APB1 Prescaler          = 1
     *  APB2 Prescaler          = 1
     *  MSI Frequency(Hz)       = 4000000
     *  PLL_M                   = 1
     *  PLL_N                   = 40
     *  PLL_R                   = 2
     *  PLL_P                   = 7
     *  PLL_Q                   = 4
     *  Flash Latency(WS)       = 4
     */

    /* MSI is enabled after System reset, activate PLL with MSI as source. */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLP = 7;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK )
    {
        /* Initialisation error. */
        prvErrorHandler();
    }

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
        prvErrorHandler();
    }

#else
#error "Board not defined."
#endif

    /* Disable the SYSTICK and clear any pending interrupts. The systick is
    automatically started by HAL_RCC_OscConfig() and HAL_RCC_ClockConfig(). */
    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

    /* Enable LSE clock */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        prvErrorHandler();
    }

    RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LPTIM1;
    RCC_PeriphCLKInitStruct.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_LSE;
    if(HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        prvErrorHandler();
    }

#if defined( USE_STM32L4XX_NUCLEO )
    /* Initialise User Button to EXTI line with interrupt generation capability */
    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
#endif

    /* Enable the GPIO required by the on board LEDs. */
    vParTestInitialise();
}
/*---------------------------------------------------------------------------*/

#ifdef  USE_FULL_ASSERT

void assert_failed(char *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number */
    while (1)
    {
        /* Infinite loop */
    }
}
#endif
/*---------------------------------------------------------------------------*/

/* User Button interrupt handler */
void EXTI15_10_IRQHandler(void)
{
#if defined( USE_STM32L4XX_NUCLEO )
    HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_PIN);
#endif
}
/*---------------------------------------------------------------------------*/

void vMainMPUFaultHandler( void )
{
    portUInt32Type ulStackedPC = 0UL;

    /* Determine which stack was in use when the MPU fault occurred and extract
     * the stacked PC. */
    asm volatile
    (
        "	tst lr, #4			\n"
        "	ite eq				\n"
        "	mrseq r0, msp		\n"    /* The code that generated the exception was using the main stack. */
        "	mrsne r0, psp		\n"    /* The code that generated the exception was using the process stack. */
        "	ldr %0, [r0, #24]	\n" /* Extract the value of the stacked PC and store it into ulStacked_pc. */
        :   "=r"( ulStackedPC )
    );

    /* ulStacked_pc is passed by reference rather than by value to ensure that the
     * parameter is stored on the stack, allowing the above assembler code to
     * access it. */
#if ( INCLUDE_DEMO_DISPLAY == 1 )
    vDisplayMPUFaultMessage( &ulStackedPC );
#else
    /* Avoid unused variable warning. */
    ( void ) ulStackedPC;
#endif

    prvErrorHandler();
}
/*---------------------------------------------------------------------------*/

static void prvErrorHandler( void )
{
    /* A generic stub called for all system errors. Check the call stack to
     * find out where the error occurred. */
    while( pdTRUE )
    {
    }
}

/*---------------------------------------------------------------------------*/

/* Provides a tick value in millisecond. */
uint32_t HAL_GetTick(void)
{
    return xTaskGetTickCount();
}
/*---------------------------------------------------------------------------*/

/* This function provides accurate delay (in milliseconds) based on variable
 * incremented. */
void HAL_Delay( uint32_t ulDelay )
{
    ( void ) xTaskDelay( ulDelay );
}
/*---------------------------------------------------------------------------*/
