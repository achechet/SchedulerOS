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


#ifndef MPU_H
#define MPU_H

/* SafeRTOS Includes */
#include "mpuAPI.h"
#include "portmpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
 * MPU Constant Definitions
 *---------------------------------------------------------------------------*/

/* Definitions used to define tasks as privileged or unprivileged. */
#define mpuUNPRIVILEGED_TASK    ( ( portUnsignedBaseType ) 0 )
#define mpuPRIVILEGED_TASK      ( ( portUnsignedBaseType ) 1 )


/*-----------------------------------------------------------------------------
 * MPU Structure Definitions
 *---------------------------------------------------------------------------*/

/* Defines MPU Region Memory Settings Structure */
typedef struct mpuRegionConfig
{
    void            *pvBaseAddress;
    portUInt32Type  ulLengthInBytes;
    portUInt32Type  ulAccessPermissions;
    portUInt32Type  ulSubRegionControl;

} mpuRegionConfigType;

/* Defines MPU Task Parameter Settings Structure */
/* Set of memory ranges allocated to a task when an MPU
 * is used along with the privilege level of the task. */
typedef struct mpuTaskParam
{
    portUnsignedBaseType    uxPrivilegeLevel;

    mpuRegionConfigType     axRegions[ portmpuCONFIGURABLE_REGION_NUM ];

} mpuTaskParamType;


/*-----------------------------------------------------------------------------
 * MPU Public Functions
 *---------------------------------------------------------------------------*/

KERNEL_FUNCTION portBaseType xMPUSetTaskRegions( portTaskHandleType pxSetTaskToModify,
                                                 const mpuTaskParamType *const pxSetTaskMpuParams );

KERNEL_FUNCTION portBaseType xMPUConfigureGlobalRegion( portUInt32Type ulRegionNumber,
                                                        portUInt32Type ulRegionBeginAddress,
                                                        portUInt32Type ulRegionAccess,
                                                        portUInt32Type ulRegionSize,
                                                        portUInt32Type ulSubRegionDisable );

/*-----------------------------------------------------------------------------
 * MPU Private API Functions
 *---------------------------------------------------------------------------*/
/* Functions beyond this point are not part of the public API
 * and are intended for use by the kernel only. */

KERNEL_FUNCTION void vMPUSetupMPU( void );

KERNEL_FUNCTION portBaseType xMPUCheckTaskRegions( const mpuRegionConfigType *pxCheckTaskRegions );

KERNEL_FUNCTION void vMPUStoreTaskRegions( portTaskHandleType pxStoreTaskToModify,
                                           const mpuTaskParamType *const pxStoreTaskMpuRegions );

/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif  /* MPU_H */
