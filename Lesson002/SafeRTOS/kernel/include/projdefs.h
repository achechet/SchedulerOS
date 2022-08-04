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

#ifndef PROJDEFS_H
#define PROJDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Kernel major version number */
#define pdKERNEL_MAJOR_VERSION                  ( 7 )

/* Kernel minor version number */
#define pdKERNEL_MINOR_VERSION                  ( 3 )


typedef void ( *pdTASK_CODE )( void *pvParameters );

#define pdTRUE                                  ( ( portBaseType ) 1 )
#define pdFALSE                                 ( ( portBaseType ) 0 )

#define pdPASS                                  ( ( portBaseType ) 1 )
#define pdFAIL                                  ( ( portBaseType ) 0 )

/* Error definitions. */
#define errSUPPLIED_BUFFER_TOO_SMALL            ( ( portBaseType ) -1 )
#define errINVALID_PRIORITY                     ( ( portBaseType ) -2 )
/* -3 has been removed. */
#define errQUEUE_FULL                           ( ( portBaseType ) -4 )
#define errSEMAPHORE_ALREADY_GIVEN              ( errQUEUE_FULL )
#define errINVALID_BYTE_ALIGNMENT               ( ( portBaseType ) -5 )
#define errNULL_PARAMETER_SUPPLIED              ( ( portBaseType ) -6 )
#define errINVALID_QUEUE_LENGTH                 ( ( portBaseType ) -7 )
#define errINVALID_TASK_CODE_POINTER            ( ( portBaseType ) -8 )
#define errSCHEDULER_IS_SUSPENDED               ( ( portBaseType ) -9 )
#define errINVALID_TASK_HANDLE                  ( ( portBaseType ) -10 )
#define errDID_NOT_YIELD                        ( ( portBaseType ) -11 )
#define errTASK_ALREADY_SUSPENDED               ( ( portBaseType ) -12 )
#define errTASK_WAS_NOT_SUSPENDED               ( ( portBaseType ) -13 )
#define errNO_TASKS_CREATED                     ( ( portBaseType ) -14 )
#define errSCHEDULER_ALREADY_RUNNING            ( ( portBaseType ) -15 )
/* -16 has been removed. */
#define errINVALID_QUEUE_HANDLE                 ( ( portBaseType ) -17 )
#define errINVALID_SEMAPHORE_HANDLE             ( errINVALID_QUEUE_HANDLE )
#define errINVALID_MUTEX_HANDLE                 ( errINVALID_QUEUE_HANDLE )
#define errERRONEOUS_UNBLOCK                    ( ( portBaseType ) -18 )
#define errQUEUE_EMPTY                          ( ( portBaseType ) -19 )
#define errSEMAPHORE_ALREADY_TAKEN              ( errQUEUE_EMPTY )
#define errMUTEX_ALREADY_TAKEN                  ( errQUEUE_EMPTY )
#define errINVALID_TICK_VALUE                   ( ( portBaseType ) -20 )
#define errINVALID_TASK_SELECTED                ( ( portBaseType ) -21 )
#define errTASK_STACK_OVERFLOW                  ( ( portBaseType ) -22 )
#define errSCHEDULER_WAS_NOT_SUSPENDED          ( ( portBaseType ) -23 )
#define errINVALID_BUFFER_SIZE                  ( ( portBaseType ) -24 )
#define errBAD_OR_NO_TICK_RATE_CONFIGURATION    ( ( portBaseType ) -25 )
#define errBAD_HOOK_FUNCTION_ADDRESS            ( ( portBaseType ) -26 )
#define errERROR_IN_VECTOR_TABLE                ( ( portBaseType ) -27 )
#define errINVALID_MPU_REGION_CONFIGURATION     ( ( portBaseType ) -28 )
#define errINVALID_MMU_REGION_CONFIGURATION     ( errINVALID_MPU_REGION_CONFIGURATION )
#define errTASK_STACK_ALREADY_IN_USE            ( ( portBaseType ) -29 )
#define errNO_MPU_IN_DEVICE                     ( ( portBaseType ) -30 )
#define errEXECUTING_IN_UNPRIVILEGED_MODE       ( ( portBaseType ) -31 )
#define errRTS_CALCULATION_ERROR                ( ( portBaseType ) -32 )
#define errINVALID_PERCENTAGE_HANDLE            ( ( portBaseType ) -33 )
#define errINVALID_INITIAL_SEMAPHORE_COUNT      ( ( portBaseType ) -34 )
#define errROM_INTEGRITY_CHECK_FAILED           ( ( portBaseType ) -35 )
#define errIN_PROGRESS                          ( ( portBaseType ) -36 )
#define errINVALID_PARAMETERS                   ( ( portBaseType ) -37 )
#define errSPURIOUS_INTERRUPT                   ( ( portBaseType ) -38 )
#define errSPURIOUS_FAST_INTERRUPT              ( ( portBaseType ) -39 )
#define errRAM_INTEGRITY_CHECK_FAILED           ( ( portBaseType ) -40 )
#define errINVALID_TIMER_HANDLE                 ( ( portBaseType ) -41 )
#define errINVALID_TIMER_TASK_INSTANCE          ( ( portBaseType ) -42 )
#define errTIMER_ALREADY_IN_USE                 ( ( portBaseType ) -43 )
#define errNOTIFICATION_NOT_RECEIVED            ( ( portBaseType ) -44 )
#define errNOTIFICATION_ALREADY_PENDING         ( ( portBaseType ) -45 )
#define errTASK_WAS_ALSO_ON_EVENT_LIST          ( ( portBaseType ) -46 )
#define errQUEUE_ALREADY_IN_USE                 ( ( portBaseType ) -47 )
#define errEVENT_GROUP_ALREADY_IN_USE           ( ( portBaseType ) -48 )
#define errINVALID_EVENT_GROUP_HANDLE           ( ( portBaseType ) -49 )
#define errEVENT_GROUP_BITS_NOT_SET             ( ( portBaseType ) -50 )
#define errEVENT_GROUP_DELETED                  ( ( portBaseType ) -51 )
#define errMUTEX_NOT_OWNED_BY_CALLER            ( ( portBaseType ) -52 )
#define errMUTEX_CORRUPTED                      ( ( portBaseType ) -53 )
#define errNEXT_UNBLOCK_TIME_EXPIRED            ( ( portBaseType ) -54 )
#define errWOKEN_UP_AFTER_NEXT_UNBLOCK_TIME     ( ( portBaseType ) -55 )
#define errTICKLESS_MODE_NOT_SUPPORTED          ( ( portBaseType ) -56 )

/* Error Code -1000 has been removed. */
/* Error Code -1001 has been removed. */
/* Error Code -1002 has been removed. */


#ifdef __cplusplus
}
#endif

#endif /* PROJDEFS_H */
