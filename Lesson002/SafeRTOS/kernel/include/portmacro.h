/*
    Copyright (C)2006 onward WITTENSTEIN aerospace & simulation limited. All rights reserved.

    This file is part of the SafeRTOS product, see projdefs.h for version number
    information.

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

#ifndef PORTMACRO_H
#define PORTMACRO_H

#include <intrinsics.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
 * Portable layer version number
 *---------------------------------------------------------------------------*/

/* Portable layer major version number */
#define portPORT_MAJOR_VERSION  ( 2 )

/* Portable layer minor version number */
#define portPORT_MINOR_VERSION  ( 0 )

/*-----------------------------------------------------------------------------
 * Type definitions
 *---------------------------------------------------------------------------*/

typedef char                    portCharType;

typedef signed char             portInt8Type;
typedef signed short            portInt16Type;
typedef signed long             portInt32Type;
typedef signed long long        portInt64Type;

typedef unsigned char           portUInt8Type;
typedef unsigned short          portUInt16Type;
typedef unsigned long           portUInt32Type;
typedef unsigned long long      portUInt64Type;

typedef float                   portFloat32Type;
typedef double                  portFloat64Type;

typedef unsigned long           portStackType;
typedef signed long             portBaseType;
typedef unsigned long           portUnsignedBaseType;
typedef unsigned long           portTickType;
typedef void                   *portTaskHandleType;

typedef unsigned long           portCodeAddressType;
typedef unsigned long           portDataAddressType;

#ifndef NULL
    #ifdef __cplusplus
        #define NULL    ( 0 )
    #else
        #define NULL    ( ( void * ) 0 )
    #endif
#endif


/*-----------------------------------------------------------------------------
 * Program Hook function pointers
 *---------------------------------------------------------------------------*/

typedef void ( *portTASK_DELETE_HOOK )( portTaskHandleType xTaskBeingDeleted );
typedef void ( *portERROR_HOOK )( portTaskHandleType xHandleOfTaskWithError,
                                  const portCharType *pcErrorString,
                                  portBaseType xErrorCode );
typedef void ( *portIDLE_HOOK )( void );
typedef void ( *portTICK_HOOK )( void );
typedef void ( *portSETUP_TICK_HOOK )( portUInt32Type ulClockHz, portUInt32Type ulRateHz );
typedef void ( *portSVC_HOOK )( portUnsignedBaseType uxSvcNumber );
typedef portBaseType ( *portINHIBIT_SLEEP_HOOK )( void );
typedef void ( *portPRE_SLEEP_HOOK )( void );
typedef void ( *portPOST_SLEEP_HOOK )( void );


/*-----------------------------------------------------------------------------
 * Architecture specifics
 *---------------------------------------------------------------------------*/

#define portMAX_DELAY                   ( ( portTickType ) 0xFFFFFFFFU )
#define portMAX_LIST_ITEM_VALUE         ( ( portTickType ) 0xFFFFFFFFU )
#define portWORD_ALIGNMENT              ( 4U )
#define portWORD_ALIGNMENT_MASK         ( ( portDataAddressType )( portWORD_ALIGNMENT - 1U ) )
#define portSTACK_ALIGNMENT             ( 8U )
#define portSTACK_ALIGNMENT_MASK        ( ( portDataAddressType )( portSTACK_ALIGNMENT - 1U ) )
/* SAFERTOSTRACE PORTQUEUEOVERHEADBYTES */
#define portQUEUE_OVERHEAD_BYTES        ( 124U )
/* SAFERTOSTRACE PORTQUEUEOVERHEADBYTESENDIF */
#define portCONTEXT_SIZE_BYTES_NO_FPU   ( ( portUnsignedBaseType ) 19U * sizeof( portStackType ) )
#define portCONTEXT_SIZE_BYTES_WITH_FPU ( ( portUnsignedBaseType ) 53U * sizeof( portStackType ) )
#define portSTACK_IN_USE                ( ( portStackType ) 0xA5A5A5A5U )
#define portSTACK_NOT_IN_USE            ( ( portStackType ) 0x5A5A5A5AU )

#define portTICK_COUNT_NUM_BITS         ( sizeof( portTickType ) * 8U )

/* Prevent inlining of functions outside the kernel_func section. */
#define portNO_INLINE_FUNC_DEF          _Pragma( "optimize=no_inline" )

#define portSTOP_ENTRY_WFI              ( 0 )
#define portSTOP_ENTRY_WFE              ( 1 )

/*-----------------------------------------------------------------------------
 * Scheduler utilities
 *---------------------------------------------------------------------------*/

#define portNVIC_INT_CTRL   ( ( volatile portUnsignedBaseType * ) 0xE000ED04U )
#define portNVIC_PENDSVSET  ( 0x10000000U )

#define portYIELD_IMMEDIATE()                                   \
do                                                              \
{                                                               \
    *portNVIC_INT_CTRL = portNVIC_PENDSVSET;                    \
    __DSB();                                                    \
    __ISB();                                                    \
    /* MCDC Test Point: STD_IN_MACRO "portYIELD_IMMEDIATE" */   \
} while( 0 )
/*---------------------------------------------------------------------------*/

/* Yielding from an ISR should set the pend bit and nothing else.
 * The yield will occur when basepri returns back to 0. */
#define portYIELD_FROM_ISR( xSwitchRequired )                       \
do                                                                  \
{                                                                   \
    if( pdFALSE != ( portBaseType )( xSwitchRequired ) )            \
    {                                                               \
        portYIELD_IMMEDIATE();                                      \
        /* MCDC Test Point: STD_IF_IN_MACRO "portYIELD_FROM_ISR" */ \
    }                                                               \
    /* MCDC Test Point: ADD_ELSE_IN_MACRO "portYIELD_FROM_ISR" */   \
} while( 0 )
/*---------------------------------------------------------------------------*/

/* No clobber list specified as we expect to clobber everything
 * and return elsewhere. */
#define portYIELD()                                 \
    asm volatile                                    \
    (                                               \
        "   svc %[SVCNUM]   "                       \
        ::  [SVCNUM] "i" ( portSVC_YIELD )          \
    )
/*---------------------------------------------------------------------------*/

#define portYIELD_WITHIN_API()                                  \
do                                                              \
{                                                               \
    *portNVIC_INT_CTRL = portNVIC_PENDSVSET;                    \
    portCLEAR_INTERRUPT_MASK();                                 \
    __DSB();                                                    \
    __ISB();                                                    \
    /* MCDC Test Point: STD_IN_MACRO "portYIELD_WITHIN_API" */  \
} while( 0 )
/*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Critical Section management
 *---------------------------------------------------------------------------*/

void vPortEnterCritical( void );
void vPortExitCritical( void );
portUnsignedBaseType uxPortSetInterruptMaskFromISR( void );


#define portSET_INTERRUPT_MASK()                                    \
do                                                                  \
{                                                                   \
    __set_BASEPRI( configSYSTEM_INTERRUPT_PRIORITY );               \
    __ISB();                                                        \
    __DSB();                                                        \
    /* MCDC Test Point: STD_IN_MACRO "portSET_INTERRUPT_MASK" */    \
} while( 0 )

#define portCLEAR_INTERRUPT_MASK()      __set_BASEPRI( 0U )

#define portSET_INTERRUPT_MASK_FROM_ISR()                       uxPortSetInterruptMaskFromISR()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxOriginalPriority ) __set_BASEPRI( uxOriginalPriority )

#define portENTER_CRITICAL_WITHIN_API()     vPortEnterCritical()
#define portEXIT_CRITICAL_WITHIN_API()      vPortExitCritical()

/* These macros are defined away because portTickType is atomic. */
#define portTICK_TYPE_ENTER_CRITICAL()
#define portTICK_TYPE_EXIT_CRITICAL()
#define portTICK_TYPE_SET_INTERRUPT_MASK_FROM_ISR()                         ( 0U )
#define portTICK_TYPE_CLEAR_INTERRUPT_MASK_FROM_ISR( uxOriginalPriority )   ( ( void )( uxOriginalPriority ) )
#define portTASK_HANDLE_ENTER_CRITICAL()
#define portTASK_HANDLE_EXIT_CRITICAL()

#define portENTER_CRITICAL()                            \
    asm volatile                                        \
    (                                                   \
        "   svc %[SVCNUM]   "                           \
        ::  [SVCNUM] "i" ( portSVC_ENTER_CRITICAL )     \
        :   "LR"                                        \
    )

#define portEXIT_CRITICAL()                             \
    asm volatile                                        \
    (                                                   \
        "   svc %[SVCNUM]   "                           \
        ::  [SVCNUM] "i" ( portSVC_EXIT_CRITICAL )      \
        :   "LR"                                        \
    )


/*-----------------------------------------------------------------------------
 * Privilege level management
 *---------------------------------------------------------------------------*/
portBaseType xPortRaisePrivilege( void );
portBaseType xPortIsPrivilegedMode( void );

#define portRAISE_PRIVILEGE()   xPortRaisePrivilege()

#define portLOWER_PRIVILEGE()                           \
    asm volatile                                        \
    (                                                   \
        "   svc %[SVCNUM]   "                           \
        ::  [SVCNUM] "i" ( portSVC_LOWER_PRIVILEGE )    \
        :   "LR"                                        \
    )

/* portRESTORE_PRIVILEGE( xRunningPrivileged ) - used in mpuAPI.c */
#define portRESTORE_PRIVILEGE( xRunningPrivileged )                     \
do                                                                      \
{                                                                       \
    if( pdFALSE == ( xRunningPrivileged ) )                             \
    {                                                                   \
        portLOWER_PRIVILEGE();                                          \
        /* MCDC Test Point: STD_IF_IN_MACRO "portRESTORE_PRIVILEGE" */  \
    }                                                                   \
    /* MCDC Test Point: ADD_ELSE_IN_MACRO "portRESTORE_PRIVILEGE" */    \
} while( 0 )


/*-----------------------------------------------------------------------------
 * Run-Time Statistics management
 *---------------------------------------------------------------------------*/

#define portGET_ELAPSED_CPU_TIME( pulTaskSwitchedInTime, pulElapsedTime )           \
do                                                                                  \
{                                                                                   \
    extern portUInt32Type ulTickLastRTS;                                            \
    portUInt32Type ulCountNow = ( portUInt32Type ) configRTS_COUNTER_VALUE;         \
    portUInt32Type ulCountLast = ( portUInt32Type )( *( pulTaskSwitchedInTime ) );  \
    portUInt32Type ulTickNow = ( portUInt32Type ) xTickCount;                       \
    portUInt32Type ulTimeValue;                                                     \
                                                                                    \
    /* First, consider any elapsed ticks. */                                        \
    ulTimeValue = ( ulTickNow - ulTickLastRTS ) * ( configRTS_COUNTER_MAX + 1U );   \
                                                                                    \
    /* Store the tick count for the next execution. */                              \
    ulTickLastRTS = xTickCount;                                                     \
                                                                                    \
    /* Now consider the actual count values. */                                     \
    if( ulCountNow >= ulCountLast )                                                 \
    {                                                                               \
        ulTimeValue += ( ulCountNow - ulCountLast );                                \
        /* MCDC Test Point: STD_IF_IN_MACRO "portGET_ELAPSED_CPU_TIME" */           \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        ulTimeValue -= ( ulCountLast - ulCountNow );                                \
        /* MCDC Test Point: STD_ELSE_IN_MACRO "portGET_ELAPSED_CPU_TIME" */         \
    }                                                                               \
                                                                                    \
    /* Set new time value. */                                                       \
    *( pulElapsedTime ) = ulTimeValue;                                              \
    *( pulTaskSwitchedInTime ) = ulCountNow;                                        \
} while( 0 )
/*---------------------------------------------------------------------------*/

/* This port uses the same timer for run time statistics as is used to generate
 * the tick interrupt, therefore no other timer setup is required. */
#define portRTS_TIMER_INITIALISATION()

/*---------------------------------------------------------------------------*/

/* vPortSetWordAlignedBuffer() is used for zeroing out memory blocks.
 * We know the last parameter is a constant (sizeof) and the middle parameter
 * is 0. */
void vPortSetWordAlignedBuffer( void *pvDestination, portUInt32Type ulValue, portUnsignedBaseType uxLength );

/* vPortCopyBytes copies where we do not know if bytes or words.
 * It will check if aligned and sized by words and copy by words, if so, as it
 * is faster. */
void vPortCopyBytes( void *pvDestination, const void *pvSource, portUnsignedBaseType uxLength );

/*---------------------------------------------------------------------------*/

/* MCDC Test Point: PROTOTYPE */

/*-----------------------------------------------------------------------------
 * SVC numbers for various services
 *---------------------------------------------------------------------------*/

#define portSVC_START_SCHEDULER     ( 0 )
#define portSVC_YIELD               ( 1 )
#define portSVC_RAISE_PRIVILEGE     ( 2 )
#define portSVC_ENTER_CRITICAL      ( 3 )
#define portSVC_EXIT_CRITICAL       ( 4 )
#define portSVC_LOWER_PRIVILEGE     ( 5 )


/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
