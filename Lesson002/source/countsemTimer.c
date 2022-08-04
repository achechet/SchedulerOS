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

/*--------------------------------------------------------------------------
 * Counting Semaphore Timer Initialisation and Configuration Function.
 *--------------------------------------------------------------------------*/

/* Scheduler include files. */
#include "SafeRTOS_API.h"

/* Hardware include files. */
#include "stm32l4xx_hal.h"


/* Demo Program include files. */
#include "PortSpecifics.h"
#include "countsem.h"
#include "countsemTimer.h"


/* Public Function Decalration */
extern portBaseType xCountingSemaphoreTimerHandler( void );

/* Semaphore ISR Test Task Operational State & Counter */
#define countsemTIMER_CLOCK_HZ      ( 80000000 )

#define countsemTIMER_PRESCALER     ( 800UL )

#define countsemIRQ_PRIORITY        ( 13U )

/*-----------------------------------------------------------*/
/* Counting Semaphore ISR Timer Initialisation               */
void vCountSemTimerInit( void )
{
    portUInt32Type  ulInitFrequency;

    TIM_HandleTypeDef xHALTimer;

    /* Enable Timer Peripheral Clock */
    __TIM2_CLK_ENABLE();

    ulInitFrequency  = countsemTIMER_CLOCK_HZ ;
    ulInitFrequency /= countsemTIMER_PRESCALER;
    ulInitFrequency /= countsemCOUNTSEM_ISR_TIMER_FREQUENCY;
    ulInitFrequency &= 0xFFFFUL;
    ulInitFrequency -= 1;

    xHALTimer.Init.Prescaler         = countsemTIMER_PRESCALER;
    xHALTimer.Init.CounterMode       = 0;
    xHALTimer.Init.Period            = ( portUInt16Type ) ulInitFrequency;
    xHALTimer.Init.ClockDivision     = 0;
    xHALTimer.Init.RepetitionCounter = 0;
    xHALTimer.Instance = TIM2;
    xHALTimer.State = HAL_TIM_STATE_RESET;

    HAL_TIM_Base_Init( &xHALTimer );

    /* Ensure Interrupts Do Not Start until Scheduler is Running */
    portSET_INTERRUPT_MASK( );

    /* Set Timer Interrupt Priority Above Kernel - Highest */
    NVIC_SetPriority( TIM2_IRQn, countsemIRQ_PRIORITY );
    NVIC_EnableIRQ( TIM2_IRQn );

    /* Enable Timer Interrupt in Peripheral */
    /* Start Timer... */
    HAL_TIM_Base_Start_IT( &xHALTimer );

}
/*--------------------------------------------------------------------------*/

/* Timer Counter IRQ Handler used for SEMA-ISR Operations */
void  vTimerCountSemIRQHandler( void )
{
    TIM_HandleTypeDef xHALTimer;

    xHALTimer.Instance = TIM2;

    /* Clear Semaphore Timer Counter Interrupt Source */
    __HAL_TIM_CLEAR_IT( &xHALTimer, TIM_IT_UPDATE );

    /* Action Counting Semaphore Timer Expiry Handler Operations */
    xCountingSemaphoreTimerHandler( );
}
/*--------------------------------------------------------------------------*/

