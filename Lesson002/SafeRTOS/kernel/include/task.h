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

/*
 * API documentation is included in the user manual.  Do not refer to this
 * file for documentation.
 */

#ifndef TASK_H
#define TASK_H

#ifdef __cplusplus
extern "C" {
#endif


/*-----------------------------------------------------------------------------
 * MACROS AND DEFINITIONS
 *---------------------------------------------------------------------------*/

/* Actions that can be performed when vTaskNotify() is called. */
#define taskNOTIFICATION_NO_ACTION                      ( 0 )   /* Notify the task without updating its notify value. */
#define taskNOTIFICATION_SET_BITS                       ( 1 )   /* Set bits in the task's notification value. */
#define taskNOTIFICATION_INCREMENT                      ( 2 )   /* Increment the task's notification value. */
#define taskNOTIFICATION_SET_VALUE_WITH_OVERWRITE       ( 3 )   /* Set the task's notification value to a specific value even if the previous value has not yet been read by the task. */
#define taskNOTIFICATION_SET_VALUE_WITHOUT_OVERWRITE    ( 4 )   /* Set the task's notification value if the previous value has been read by the task. */

/* Notification states. */
#define taskNOTIFICATION_NOT_WAITING                    ( ( portBaseType ) 0 )
#define taskNOTIFICATION_WAITING                        ( ( portBaseType ) 1 )
#define taskNOTIFICATION_NOTIFIED                       ( ( portBaseType ) 2 )

typedef struct xTIME_OUT
{
    portUnsignedBaseType uxOverflowCount;
    portTickType xTimeOnEntering;
} xTimeOutType;

#define taskIDLE_PRIORITY                               ( 0U )

#define taskYIELD                                       portYIELD
#define taskYIELD_FROM_ISR                              portYIELD_FROM_ISR
#define taskENTER_CRITICAL                              portENTER_CRITICAL
#define taskEXIT_CRITICAL                               portEXIT_CRITICAL
#define taskSET_INTERRUPT_MASK_FROM_ISR                 portSET_INTERRUPT_MASK_FROM_ISR
#define taskCLEAR_INTERRUPT_MASK_FROM_ISR               portCLEAR_INTERRUPT_MASK_FROM_ISR


/*-----------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

KERNEL_FUNCTION portBaseType xTaskCreate( const xTaskParameters * const pxTaskParameters, portTaskHandleType *pxCreatedTask );
KERNEL_FUNCTION portBaseType xTaskDelete( portTaskHandleType xTaskToDelete );
KERNEL_FUNCTION portBaseType xTaskDelay( portTickType xTicksToDelay );
KERNEL_FUNCTION portBaseType xTaskDelayUntil( portTickType *pxPreviousWakeTime, portTickType xTimeIncrement );
KERNEL_FUNCTION portBaseType xTaskPriorityGet( portTaskHandleType xTask, portUnsignedBaseType *puxPriority );
KERNEL_FUNCTION portBaseType xTaskPrioritySet( portTaskHandleType xTask, portUnsignedBaseType uxNewPriority );
KERNEL_FUNCTION portBaseType xTaskSuspend( portTaskHandleType xTaskToSuspend );
KERNEL_FUNCTION portBaseType xTaskResume( portTaskHandleType xTaskToResume );
KERNEL_FUNCTION portBaseType xTaskStartScheduler( portBaseType xUseKernelConfigurationChecks );
KERNEL_FUNCTION void vTaskSuspendScheduler( void );
KERNEL_FUNCTION portBaseType xTaskResumeScheduler( void );
KERNEL_FUNCTION portTickType xTaskGetTickCount( void );
KERNEL_FUNCTION portTickType xTaskGetTickCountFromISR( void );
KERNEL_FUNCTION portBaseType xTaskInitializeScheduler( const xPORT_INIT_PARAMETERS * const pxPortInitParameters );
KERNEL_FUNCTION portTaskHandleType xTaskGetCurrentTaskHandle( void );
KERNEL_FUNCTION portBaseType xTaskIsSchedulerStarted( void );
KERNEL_FUNCTION portBaseType xTaskIsSchedulerStartedFromISR( void );

KERNEL_FUNCTION portBaseType xTaskNotifyWait( portUnsignedBaseType uxBitsToClearOnEntry,
                                              portUnsignedBaseType uxBitsToClearOnExit,
                                              portUnsignedBaseType *puxNotificationValue,
                                              portTickType xTicksToWait );
KERNEL_FUNCTION portBaseType xTaskNotifySend( portTaskHandleType xTaskToNotify,
                                              portBaseType xAction,
                                              portUnsignedBaseType uxValue );
KERNEL_FUNCTION portBaseType xTaskNotifySendFromISR( portTaskHandleType xTaskToNotify,
                                                     portBaseType xAction,
                                                     portUnsignedBaseType uxValue,
                                                     portBaseType *pxHigherPriorityTaskWoken );

/* Returns a pointer to the Task Local Storage Object */
KERNEL_FUNCTION void *pvTaskTLSObjectGet( void );

/* SAFERTOSTRACE TASKGETTASKNUMBERDEF */

/*-----------------------------------------------------------------------------
 * Functions for internal use only.
 * Not to be called directly from a host application or task.
 *---------------------------------------------------------------------------*/

#define taskYIELD_WITHIN_API    portYIELD_WITHIN_API

KERNEL_FUNCTION void vTaskPlaceOnEventList( xList *pxEventList, portTickType xTicksToWait );
KERNEL_FUNCTION portBaseType xTaskRemoveFromEventList( const xList *pxEventList );
KERNEL_FUNCTION void vTaskPlaceOnUnorderedEventList( xList *pxEventList, const portTickType xItemValue, const portTickType xTicksToWait );
KERNEL_FUNCTION portBaseType xTaskRemoveFromUnorderedEventList( xListItem *pxEventListItem, const portTickType xItemValue );
KERNEL_FUNCTION portTickType xTaskResetEventItemValue( void );
KERNEL_FUNCTION void vTaskSelectNextTask( void );
KERNEL_FUNCTION portBaseType xTaskIsSchedulerSuspended( void );
KERNEL_FUNCTION void vTaskStackCheckFailed( void );
KERNEL_FUNCTION void vTaskSetTimeOut( xTimeOutType *pxTimeOut );
KERNEL_FUNCTION portBaseType xTaskCheckForTimeOut( xTimeOutType *pxTimeOut, portTickType *pxTicksToWait );
KERNEL_FUNCTION void vTaskPendYield( void );

/*
 * The idle task, which, as all tasks, is implemented as a never ending loop.
 * The idle task is automatically created and added to the ready lists when
 * the scheduler is started. It is declared with global scope so that the port
 * layer can link it into the idle task xTaskParameter structure.
 */
KERNEL_FUNCTION void vIdleTask( void *pvParameters );

KERNEL_FUNCTION void vTaskLogMutexTaken( xListItem *pxNewListItem );
KERNEL_FUNCTION void vTaskPriorityInherit( const xListItem *pxMutexListItem );
KERNEL_FUNCTION portBaseType xTaskPriorityDisinherit( xListItem *pxMutexListItem );
KERNEL_FUNCTION void vTaskReEvaluateInheritedPriority( const xListItem * const pxMutexListItem );

/*
 * Handler that should be called from the system tick handler.
 * If an alternative tick handler is not being used then this function should be
 * installed as the interrupt handler for the interrupt that will generate the
 * system tick. (see SafeRTOSConfig.h).
 */
KERNEL_FUNCTION void vTaskProcessSystemTickFromISR( void );


/*-----------------------------------------------------------------------------
 * Variables for internal use only.
 * Not to be accessed directly from a host application or task.
 *---------------------------------------------------------------------------*/

/* pxCurrentTCB (defined in task.c) is accessed by various scheduler modules. */
extern xTCB * volatile pxCurrentTCB;

extern xList xReadyTasksLists[ configMAX_PRIORITIES ];
extern xList xPendingReadyList;

extern volatile portUnsignedBaseType uxMissedTicks;
extern volatile portTickType xTickCount;
extern volatile portTickType xTickCountMirror;
extern volatile portTickType xNextTaskUnblockTime;

/*-----------------------------------------------------------------------------
 * TCB Related Macros.
 *---------------------------------------------------------------------------*/

/* Several functions take an portTaskHandleType parameter that can optionally be
 * NULL, where NULL is used to indicate that the handle of the currently
 * executing task should be used in place of the parameter.
 * This macro simply checks to see if the parameter is NULL and returns a
 * pointer to the appropriate TCB. */
#define taskGET_TCB_FROM_HANDLE( pxHandle ) ( ( NULL == ( pxHandle ) ) ? ( xTCB * ) pxCurrentTCB : ( xTCB * )( pxHandle ) )

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* TASK_H */
