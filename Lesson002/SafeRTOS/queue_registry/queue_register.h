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

#ifndef QUEUE_REGISTER_H
#define	QUEUE_REGISTER_H
/*
 * The queue registry is just a means for kernel aware debuggers to locate
 * queue structures.  It has no other purpose so is an optional component.
 */
#if configQUEUE_REGISTRY_SIZE > 0

	/* add a queue to the registry */
	void vQueueAddToRegistry( xQueueHandle xQueue, const portCharType * pcQueueName );

	/* Removes a queue from the registry by simply setting the pcQueueName
	member to NULL. We cannot actually delete a queue in SafeRTOS but it is
	feasible to remove it from the registry list. */
	void vQueueUnregisterQueue( xQueueHandle xQueue );


#endif

#endif
