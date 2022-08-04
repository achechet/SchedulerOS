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

	http://www.HighIntegritySystems.com
*/

/* Scheduler includes. */
#include "SafeRTOS.h"
#include "queue.h"

/*
 * The queue registry is just a means for kernel aware debuggers to locate
 * queue structures.  It has no other purpose so is an optional component.
 */
#if ( configQUEUE_REGISTRY_SIZE > 0 )

	/* The type stored within the queue registry array.  This allows a name
	to be assigned to each queue making kernel aware debugging a little
	more user friendly. */
	typedef struct QUEUE_REGISTRY_ITEM
	{
		const portCharType * pcQueueName;
		xQueueHandle xHandle;
	} xQueueRegistryItem;

	/* The queue registry is simply an array of xQueueRegistryItem structures.
	The pcQueueName member of a structure being NULL is indicative of the
	array position being vacant. */
	xQueueRegistryItem xQueueRegistry[ configQUEUE_REGISTRY_SIZE ];

	/*-----------------------------------------------------------*/

	void vQueueAddToRegistry( xQueueHandle xQueue, const portCharType * pcQueueName )
	{
	    portUnsignedBaseType uxIndex;

		/* See if there is an empty space in the registry.  A NULL name denotes
		a free slot. */
		for( uxIndex = 0; uxIndex < configQUEUE_REGISTRY_SIZE; uxIndex++ )
		{
			if( xQueueRegistry[ uxIndex ].pcQueueName == NULL )
			{
				/* Store the information on this queue. */
				xQueueRegistry[ uxIndex ].pcQueueName = pcQueueName;
				xQueueRegistry[ uxIndex ].xHandle = xQueue;
				break;
			}
		}
	}
	/*-----------------------------------------------------------*/

	void vQueueUnregisterQueue( xQueueHandle xQueue )
	{
	    portUnsignedBaseType uxIndex;

		/* See if the handle of the queue being unregistered in actually in the
		registry. */
		for( uxIndex = 0; uxIndex < configQUEUE_REGISTRY_SIZE; uxIndex++ )
		{
			if( xQueueRegistry[ uxIndex ].xHandle == xQueue )
			{
				/* Set the name to NULL to show that this slot if free again. */
				xQueueRegistry[ uxIndex ].pcQueueName = NULL;
				break;
			}
		}

	}
	/*-----------------------------------------------------------*/



#ifdef SAFERTOS_MODULE_TEST
    #include "QueueRegisterTestHeaders.h"
    #include "QueueRegisterTest.h"
#endif /* SAFERTOS_MODULE_TEST */


#endif /* ( configQUEUE_REGISTRY_SIZE > 0 ) */
