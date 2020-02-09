#include "ecs.h"


void system_construct(System *const sys, SystemCallback callback, uintEC const maxEntitysHint, uintEC const maxEntitysDevnHint)
{
	*sys = (System)
	{
		.sparse = malloc_debug(sizeof(uintEC) * maxEntitysHint), 
		.sparseCapacity = maxEntitysHint,

		.dense  = malloc_debug(sizeof(uintEC) * maxEntitysHint),
		.denseCapacity  = maxEntitysHint, 
		.denseSize = 0,

		.maxEntitysDevnHint = maxEntitysDevnHint,
		.active = true, 
		.callback = callback
	};
}


void system_destruct(System const *const sys)
{
	free_debug(sys->dense);
	free_debug(sys->sparse);
}


void system_entity_add(System *const sys, EntityId const entity)
{
	if(entity >= sys->sparseCapacity)
	{ //											 * 2 because sparse can get a lot bigger than dense
		sys->sparse = realloc(sys->sparse, sizeof(uintEC) * (entity + sys->maxEntitysDevnHint * 2));
		sys->sparseCapacity = entity + sys->maxEntitysDevnHint * 2;
	}

	if((sys->sparse[entity] = sys->denseSize++) >= sys->denseCapacity)
	{
		sys->dense = realloc(sys->dense, sizeof(EntityId) * (entity + sys->maxEntitysDevnHint));
		sys->denseCapacity = entity + sys->maxEntitysDevnHint;
	}

	sys->dense[sys->sparse[entity]] = entity;
}


void system_entity_remove(System *const sys, EntityId const entity)
{
	sys->dense[sys->sparse[entity]] = sys->dense[sys->denseSize--];
	sys->sparse[entity] = 0;
}