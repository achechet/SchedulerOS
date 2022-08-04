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


#ifndef MPU_API_H
#define MPU_API_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
 * MPU Macro Definitions
 *---------------------------------------------------------------------------*/

/* KERNEL_SOURCE_FILE will only be defined when this header file is included
 * from one of the kernel source files. When the header file is included from
 * the host application, the API functions are #defined onto the wrapper functions
 * which are implemented in mpuAPI.c. The wrapper functions are necessary to
 * ensure that the API functions are always called in privileged mode and that
 * the correct operating mode is restored afterwards. */
#ifdef KERNEL_SOURCE_FILE

/* Define Kernel Function Area Definitions */
#define KERNEL_FUNCTION                    portmpuKERNEL_FUNC_DEF
#define KERNEL_DATA                        portmpuKERNEL_DATA_DEF

#else

/* Remove Kernel Function Area Definitions to have no effect */
#define KERNEL_FUNCTION
#define KERNEL_DATA

/* Map Kernel Functions via MPU Wrapper functions */
#define xTaskCreate                     MPU_xTaskCreate
#define xTaskDelete                     MPU_xTaskDelete
#define xTaskDelay                      MPU_xTaskDelay
#define xTaskDelayUntil                 MPU_xTaskDelayUntil
#define xTaskPriorityGet                MPU_xTaskPriorityGet
#define xTaskPrioritySet                MPU_xTaskPrioritySet
#define xTaskSuspend                    MPU_xTaskSuspend
#define xTaskResume                     MPU_xTaskResume
#define xTaskStartScheduler             MPU_xTaskStartScheduler
#define vTaskSuspendScheduler           MPU_vTaskSuspendScheduler
#define xTaskResumeScheduler            MPU_xTaskResumeScheduler
#define xTaskGetTickCount               MPU_xTaskGetTickCount
#define xTaskInitializeScheduler        MPU_xTaskInitializeScheduler
#define xTaskGetCurrentTaskHandle       MPU_xTaskGetCurrentTaskHandle
#define xTaskIsSchedulerStarted         MPU_xTaskIsSchedulerStarted
#define pvTaskTLSObjectGet              MPU_pvTaskTLSObjectGet

#define xTaskNotifyWait                 MPU_xTaskNotifyWait
#define xTaskNotifySend                 MPU_xTaskNotifySend

#define xQueueCreate                    MPU_xQueueCreate
#define xQueueSend                      MPU_xQueueSend
#define xQueueSendToFront               MPU_xQueueSendToFront
#define xQueueReceive                   MPU_xQueueReceive
#define xQueuePeek                      MPU_xQueuePeek
#define xQueueMessagesWaiting           MPU_xQueueMessagesWaiting
#define xQueueTakeMutex                 MPU_xQueueTakeMutex
#define xQueueGiveMutex                 MPU_xQueueGiveMutex

#define xCalculateCPUUsage              MPU_xCalculateCPUUsage

#define xSemaphoreCreateBinary          MPU_xSemaphoreCreateBinary
#define xSemaphoreCreateCounting        MPU_xSemaphoreCreateCounting
#define xMutexCreate                    MPU_xMutexCreate

#define xMPUSetTaskRegions              MPU_xMPUSetTaskRegions
#define xMPUConfigureGlobalRegion       MPU_xMPUConfigureGlobalRegion

#define xTimerCreate                    MPU_xTimerCreate
#define xTimerStart                     MPU_xTimerStart
#define xTimerReset                     MPU_xTimerReset
#define xTimerStop                      MPU_xTimerStop
#define xTimerChangePeriod              MPU_xTimerChangePeriod
#define xTimerDelete                    MPU_xTimerDelete
#define xTimerIsTimerActive             MPU_xTimerIsTimerActive
#define xTimerGetTimerID                MPU_xTimerGetTimerID
#define pvTimerTLSObjectGet             MPU_pvTimerTLSObjectGet

#define xEventGroupCreate               MPU_xEventGroupCreate
#define xEventGroupGetBits              MPU_xEventGroupGetBits
#define xEventGroupSetBits              MPU_xEventGroupSetBits
#define xEventGroupClearBits            MPU_xEventGroupClearBits
#define xEventGroupWaitBits             MPU_xEventGroupWaitBits
#define xEventGroupDelete               MPU_xEventGroupDelete

#endif /* KERNEL_SOURCE_FILE */

/*-----------------------------------------------------------------------------
 * MPU API Functions
 *---------------------------------------------------------------------------*/

void vMPUTaskExecuteInUnprivilegedMode( void );

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif  /* MPU_API_H */
