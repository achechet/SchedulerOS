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

/*-----------------------------------------------------------------------------
 * Features specific to this product variant.
 *---------------------------------------------------------------------------*/

#ifndef PORT_FEATURES_H
#define PORT_FEATURES_H

/* Include the MPU functionality. */
#include "mpu.h"

/* Include the Run-Time Statistics functionality. */
#include "runtimestats.h"

/* The list module definitions are needed by several kernel modules. */
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
 * Structure definitions.
 *---------------------------------------------------------------------------*/

/* Task control block. Each task has its own task control block.
 * The TCB is defined here so that it is accessible to the portable layer
 * and can conveniently contain the port specific elements. */
typedef struct xPortTaskControlBlock
{
    volatile portStackType          *pxTopOfStack;                              /* Points to the location of the last item placed on the tasks stack when the task was switched out.
                                                                                 * THIS MUST BE THE FIRST MEMBER OF THE STRUCT. */
    portStackType                   *pxStackLimit;                              /* Points to the limit of the stack that still leaves enough space for the context that is saved by the kernel.
                                                                                 * The stack cannot legitimately grow past this address otherwise a context switch will cause a corruption. */
    portDataAddressType             uxTopOfStackMirror;                         /* The uxTopOfStackMirror will be set to the bitwise inverse (XOR) of pxTopOfStack. */
    portmpuRegionRegistersType      axMPURegions[ portmpuTASK_REGION_NUM ];     /* The MPU register configuration for this task's MPU regions. */
    portUInt32Type                  ulUnprivilegedBitSetting;                   /* The privilege level of the processor is determined by the setting of the 'Unprivileged' bit in the CONTROL register. */
    portmpuRegionRegistersType      axMPURegionsMirror[ portmpuTASK_REGION_NUM ];   /* The mirror of the MPU register configuration for this task's MPU regions. */
    portStackType                   *pxStackInUseMarker;                        /* Contains a fixed code to indicate that this task stack is in use. */
    portInt8Type                    *pcStackBaseAddress;                        /* The base address of the task stack. */
    portUnsignedBaseType            uxPriority;                                 /* The priority of the task where 0 is the lowest priority. */
    xListItem                       xStateListItem;                             /* List item used to place the TCB in ready and blocked queues. */
    xListItem                       xEventListItem;                             /* List item used to place the TCB in event lists. */
    const portCharType              *pcNameOfTask;                              /* Descriptive name given to the task when created. Facilitates debugging only. */
    xRTS                            xRunTimeStats;                              /* The run-time statistics data. */
    volatile portUnsignedBaseType   uxNotifiedValue;                            /* The Notification value. */
    volatile portBaseType           xNotifyState;                               /* Notify state of the task. */
    void                            *pvObject;                                  /* Points to the instance of the C++ object that tracks this structure. */
    portUnsignedBaseType            uxBasePriority;                             /* The priority last assigned to the task - used by the priority inheritance mechanism. */
    xList                           xMutexesHeldList;                           /* A list used to manage held mutexes. */
    portDataAddressType             uxStackLimitMirror;                         /* The uxStackLimitMirror will be set to the bitwise inverse (XOR) of pxStackLimit. */
    /* SAFERTOSTRACE TCBTASKNUMBER */

} xTCB;

/*---------------------------------------------------------------------------*/

/* The structure supplied to xTaskCreate(). */
typedef struct xTASK_PARAMETERS
{
    pdTASK_CODE             pvTaskCode;
    const portCharType      *pcTaskName;
    xTCB                    *pxTCB;
    portInt8Type            *pcStackBuffer;
    portUnsignedBaseType    uxStackDepthBytes;
    void                    *pvParameters;
    portUnsignedBaseType    uxPriority;
    void                    *pvObject;
    portBaseType            xUsingFPU;
    mpuTaskParamType        xMPUParameters;

} xTaskParameters;

/*---------------------------------------------------------------------------*/

/* The structure supplied to xTaskInitializeScheduler(). */
typedef struct PORT_INIT_PARAMETERS
{
    portUInt32Type          ulCPUClockHz;
    portUInt32Type          ulTickRateHz;
    portSETUP_TICK_HOOK     pxSetupTickInterruptHookFunction;
    portTASK_DELETE_HOOK    pxTaskDeleteHookFunction;
    portERROR_HOOK          pxErrorHookFunction;
    portIDLE_HOOK           pxIdleHookFunction;
    portTICK_HOOK           pxTickHookFunction;
    portSVC_HOOK            pxSvcHookFunction;
    portINHIBIT_SLEEP_HOOK  pxInhibitSleepHookFunction;
    portPRE_SLEEP_HOOK      pxPreSleepHookFunction;
    portPOST_SLEEP_HOOK     pxPostSleepHookFunction;
    portUnsignedBaseType    uxSystemStackSizeBytes;
    portUnsignedBaseType    uxAdditionalStackCheckMarginBytes;
    portInt8Type            *pcIdleTaskStackBuffer;
    portUnsignedBaseType    uxIdleTaskStackSizeBytes;
    portBaseType            xIdleTaskUsingFPU;
    mpuTaskParamType        xIdleTaskMPUParameters;
    void                    *pvIdleTaskTLSObject;
    portUnsignedBaseType    uxTimerTaskPriority;
    portUnsignedBaseType    uxTimerTaskStackSize;
    portInt8Type            *pcTimerTaskStackBuffer;
    portUnsignedBaseType    uxTimerCommandQueueLength;
    portUnsignedBaseType    uxTimerCommandQueueBufferSize;
    portInt8Type            *pcTimerCommandQueueBuffer;
    portTickType            xMaxIdleTime;
    portBaseType            xStopModeEntry;

} xPORT_INIT_PARAMETERS;

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* PORT_FEATURES_H */
