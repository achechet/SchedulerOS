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

#ifndef PORTMPU_H
#define PORTMPU_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
 * Public Port MPU Macros
 *---------------------------------------------------------------------------*/

/* Kernel Function area definitions ARE required for this Port. */
#define portmpuKERNEL_FUNC_DEF      _Pragma("location=\"__kernel_functions__\"")
#define portmpuKERNEL_DATA_DEF      _Pragma("location=\"__kernel_data__\"")


/*-----------------------------------------------------------------------------
 * Public Port MPU Constants
 *---------------------------------------------------------------------------*/

/* MPU region actual byte size limits. */
#define portmpuSMALLEST_REGION_SIZE_ACTUAL      ( 32U )
#define portmpuLARGEST_REGION_SIZE_ACTUAL       ( 0x80000000U )

/* MPU configurable system region identifiers. */
#define portmpuUNPRIVILEGED_FLASH_REGION        ( 0U )
#define portmpuGLOBAL_CONFIGURABLE_REGION_FIRST ( 1U )
#define portmpuGLOBAL_CONFIGURABLE_REGION_LAST  ( 1U )
#define portmpuCONFIGURABLE_REGION_FIRST        ( 2U )
#define portmpuCONFIGURABLE_REGION_LAST         ( 4U )
#define portmpuSTACK_REGION                     ( 5U )
#define portmpuKERNEL_FLASH_REGION              ( 6U )
#define portmpuKERNEL_RAM_REGION                ( 7U )

/* MPU configurable task regions (LAST - FIRST + 1). */
#define portmpuCONFIGURABLE_REGION_NUM          ( 3U )

/* MPU configurable Task regions plus 1 for task stack region.*/
#define portmpuTASK_REGION_NUM                  ( portmpuCONFIGURABLE_REGION_NUM + 1U )

/*
 * Constants used to configure the region attributes.
 */

/* Region Share enable bit */
#define portmpuREGION_SHAREABLE                 ( 0x01UL << 18U )

/* Instruction Executable access disable bit */
#define portmpuREGION_EXECUTE_NEVER             ( 0x01UL << 28U )

/* Sub-Region Disable definitions */
#define portmpuREGION_ALL_SUB_REGIONS_ENABLED   ( 0x00UL )
#define portmpuREGION_1ST_SUB_REGION_DISABLE    ( 0x01UL <<  8U )
#define portmpuREGION_2ND_SUB_REGION_DISABLE    ( 0x01UL <<  9U )
#define portmpuREGION_3RD_SUB_REGION_DISABLE    ( 0x01UL << 10U )
#define portmpuREGION_4TH_SUB_REGION_DISABLE    ( 0x01UL << 11U )
#define portmpuREGION_5TH_SUB_REGION_DISABLE    ( 0x01UL << 12U )
#define portmpuREGION_6TH_SUB_REGION_DISABLE    ( 0x01UL << 13U )
#define portmpuREGION_7TH_SUB_REGION_DISABLE    ( 0x01UL << 14U )
#define portmpuREGION_8TH_SUB_REGION_DISABLE    ( 0x01UL << 15U )
#define portmpuREGION_SUB_REGION_DISABLE_MASK   ( 0x0000FF00UL )

/* Region Data Access Permission field bit definitions */
#define portmpuREGION_PRIVILEGED_NO_ACCESS_USER_NO_ACCESS       ( 0x00UL << 24U )
#define portmpuREGION_PRIVILEGED_READ_WRITE_USER_NO_ACCESS      ( 0x01UL << 24U )
#define portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_ONLY      ( 0x02UL << 24U )
#define portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE     ( 0x03UL << 24U )
#define portmpuREGION_PRIVILEGED_READ_ONLY_USER_NO_ACCESS       ( 0x05UL << 24U )
#define portmpuREGION_PRIVILEGED_READ_ONLY_USER_READ_ONLY       ( 0x06UL << 24U )

/* Region Type extension, Cached and Buffered field definitions */
#define portmpuREGION_STRONGLY_ORDERED                                                                  ( 0x00UL << 16U )
#define portmpuREGION_SHARED_DEVICE                                                                     ( 0x01UL << 16U )
#define portmpuREGION_OUTER_AND_INNER_WRITE_THROUGH_NO_WRITE_ALLOCATE                                   ( 0x02UL << 16U )
#define portmpuREGION_OUTER_AND_INNER_WRITE_BACK_NO_WRITE_ALLOCATE                                      ( 0x03UL << 16U )
#define portmpuREGION_OUTER_AND_INNER_NONCACHEABLE                                                      ( 0x08UL << 16U )
#define portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE                                ( 0x0BUL << 16U )
#define portmpuREGION_NONSHARED_DEVICE                                                                  ( 0x10UL << 16U )
#define portmpuREGION_OUTER_NONCACHEABLE_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE                       ( 0x21UL << 16U )
#define portmpuREGION_OUTER_NONCACHEABLE_INNER_WRITE_THROUGH_NO_WRITE_ALLOCATE                          ( 0x22UL << 16U )
#define portmpuREGION_OUTER_NONCACHEABLE_INNER_WRITE_BACK_NO_WRITE_ALLOCATE                             ( 0x23UL << 16U )
#define portmpuREGION_OUTER_WRITE_BACK_WRITE_AND_READ_ALLOCATE_INNER_NONCACHEABLE                       ( 0x28UL << 16U )
#define portmpuREGION_OUTER_WRITE_BACK_WRITE_AND_READ_ALLOCATE_INNER_WRITE_THROUGH_NO_WRITE_ALLOCATE    ( 0x2AUL << 16U )
#define portmpuREGION_OUTER_WRITE_BACK_WRITE_AND_READ_ALLOCATE_INNER_WRITE_BACK_NO_WRITE_ALLOCATE       ( 0x2BUL << 16U )
#define portmpuREGION_OUTER_WRITE_THROUGH_NO_WRITE_ALLOCATE_INNER_NONCACHEABLE                          ( 0x30UL << 16U )
#define portmpuREGION_OUTER_WRITE_THROUGH_NO_WRITE_ALLOCATE_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE    ( 0x31UL << 16U )
#define portmpuREGION_OUTER_WRITE_THROUGH_NO_WRITE_ALLOCATE_INNER_WRITE_BACK_NO_WRITE_ALLOCATE          ( 0x33UL << 16U )
#define portmpuREGION_OUTER_WRITE_BACK_NO_WRITE_ALLOCATE_INNER_NONCACHEABLE                             ( 0x38UL << 16U )
#define portmpuREGION_OUTER_WRITE_BACK_NO_WRITE_ALLOCATE_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE       ( 0x39UL << 16U )
#define portmpuREGION_OUTER_WRITE_BACK_NO_WRITE_ALLOCATE_INNER_WRITE_THROUGH_NO_WRITE_ALLOCATE          ( 0x3AUL << 16U )

#define portmpuREGION_ACCESS_PERMISSIONS_MASK   ( 0x173F0000UL )

/* Attributes for every task stack region. */
#define portmpuTASK_STACK_ATTRIBUTES    ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_READ_WRITE | \
                                          portmpuREGION_EXECUTE_NEVER | \
                                          portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE )


/*-----------------------------------------------------------------------------
 * Public Port MPU Types
 *---------------------------------------------------------------------------*/

/* MPU region register settings - holds actual MPU region values. */
typedef struct portmpuRegionRegisters
{
    portUInt32Type ulRegionBaseAddress;
    portUInt32Type ulRegionAttribute;

} portmpuRegionRegistersType;


/*-----------------------------------------------------------------------------
 * Public Port MPU External Variables
 *---------------------------------------------------------------------------*/

/* Kernel Function & Kernel Data region boundaries.
 * These are imported from the linker command file. */
extern portUInt32Type __ICFEDIT_intvec_start__[ 1 ];
extern portUInt32Type __kernel_functions_block__$$Limit[ 1 ];

extern portUInt32Type lnkStartFlashAddress[ 1 ];
extern portUInt32Type lnkEndFlashAddress[ 1 ];

extern portUInt32Type __kernel_data_block__$$Base[ 1 ];
extern portUInt32Type __kernel_data_block__$$Limit[ 1 ];


/*-----------------------------------------------------------------------------
 * Public Port MPU Variables
 *---------------------------------------------------------------------------*/

/* System MPU configurable region definitions. */
#define portmpuREGION_SYS_CONFIGURATION                                 \
{                                                                       \
    portmpuUNPRIVILEGED_FLASH_REGION,                                   \
    ( portUInt32Type ) lnkStartFlashAddress,                            \
    ( portUInt32Type ) lnkEndFlashAddress,                              \
    ( portmpuREGION_PRIVILEGED_READ_ONLY_USER_READ_ONLY |               \
      portmpuREGION_OUTER_AND_INNER_WRITE_THROUGH_NO_WRITE_ALLOCATE     \
    ),                                                                  \
    portmpuREGION_ALL_SUB_REGIONS_ENABLED                               \
},                                                                      \
{                                                                       \
    portmpuKERNEL_FLASH_REGION,                                         \
    ( portUInt32Type ) __ICFEDIT_intvec_start__,                        \
    ( portUInt32Type ) __kernel_functions_block__$$Limit,               \
    ( portmpuREGION_PRIVILEGED_READ_ONLY_USER_NO_ACCESS |               \
      portmpuREGION_OUTER_AND_INNER_WRITE_THROUGH_NO_WRITE_ALLOCATE     \
    ),                                                                  \
    portmpuREGION_ALL_SUB_REGIONS_ENABLED                               \
},                                                                      \
{                                                                       \
    portmpuKERNEL_RAM_REGION,                                           \
    ( portUInt32Type ) __kernel_data_block__$$Base,                     \
    ( portUInt32Type ) __kernel_data_block__$$Limit,                    \
    ( portmpuREGION_PRIVILEGED_READ_WRITE_USER_NO_ACCESS |              \
      portmpuREGION_EXECUTE_NEVER |                                     \
      portmpuREGION_OUTER_AND_INNER_WRITE_BACK_WRITE_AND_READ_ALLOCATE  \
    ),                                                                  \
    portmpuREGION_ALL_SUB_REGIONS_ENABLED                               \
}


/*-----------------------------------------------------------------------------
 * Public Port MPU Functions
 *---------------------------------------------------------------------------*/

KERNEL_FUNCTION void vPortMPUEnable( void );

KERNEL_FUNCTION void vPortMPUConfigureSystemRegion( portUInt32Type ulRegionIdentifier,
                                                    portUInt32Type ulRegionBeginAddress,
                                                    portUInt32Type ulRegionAccess,
                                                    portUInt32Type ulRegionSize,
                                                    portUInt32Type ulSubRegionDisable );

KERNEL_FUNCTION void vPortMPUEncodeTCBRegion( portmpuRegionRegistersType *pxPortMPURegion,
                                              portUInt32Type ulRegionIdentifier,
                                              portUInt32Type ulBaseAddress,
                                              portUInt32Type ulAccessPermissions,
                                              portUInt32Type ulLengthInBytes,
                                              portUInt32Type ulSubRegionDisable );

KERNEL_FUNCTION void vPortMPUCreateTCBMirrorRegion( const portmpuRegionRegistersType *pxPortMPURegion,
                                                    portmpuRegionRegistersType *pxPortMPUMirror );


/*---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* PORTMPU_H */
