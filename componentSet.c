#include "ecs.h"
#include <string.h>


void componentSet_construct(ComponentSet* set, size_t const componentSize, ComponentKeyIndex const cki, uintEC const maxComponentsHint, 
						 void(*component_destructor)(void *const), void(*component_constructor)(void *const))
{
	*set = (ComponentSet)
	{
		.sparse = checs_malloc(sizeof(uintEC) * maxComponentsHint), 
		.sparseCapacity = maxComponentsHint, 

		.dense = checs_calloc(maxComponentsHint, sizeof(uintEC)), 
		.denseCapacity = maxComponentsHint, 
		.denseSize = 0,

		.cki = cki, 
		.componentSize = componentSize,
		.component_destructor = component_destructor,
		.component_constructor = component_constructor,
		.components = checs_malloc(componentSize * maxComponentsHint)
	};
}


void componentSet_destruct(ComponentSet *const set)
{
	checs_assert(set);
	checs_free(set->dense);
	checs_free(set->sparse); 
}


void componentSet_entity_add(ComponentSet *const set, EntityId const entity)
{
	if(entity >= set->sparseCapacity)
	{
		set->sparse = realloc(set->sparse, sizeof(uintEC) * (set->sparseCapacity = entity * 2));
	}
	checs_assert(set->denseSize + 1 <= set->denseCapacity);
	set->dense[set->sparse[entity] = set->denseSize++] = entity;

	if (set->component_constructor)
	{
		set->component_constructor(set->components + set->sparse[entity] * set->componentSize);
	}
}


void componentSet_entity_remove(ComponentSet *const set, EntityId const entity)
{
	//copy last compont to the place that just got freed because the entity that owns the component got destroyed
	//this check needs to be there because if it evaluates to false that means the entity to be removed is the last entity in the array
	//this would mean the data of the last element in the components array would be copied to itself, because the last element gets
	//copied to the place that just got freed. when memory overlaps you need to use memmove which is much slower. the operation
	//would anyway be wasted since only the size needs to be decremented.
	checs_assert(entity < set->sparseCapacity);
	if (set->component_destructor)
	{
		set->component_destructor(set->components + set->sparse[entity] * set->componentSize);
	}
	if(set->sparse[entity] < set->denseSize - 1) //this needs to be checked because if it evaluates to false that means the entity to be removed
	{
		memcpy(set->components + set->sparse[entity] * set->componentSize, set->components + set->denseSize * set->componentSize, set->componentSize); 
		set->dense[set->sparse[entity]] = set->dense[set->denseSize];
		set->sparse[entity] = set->sparse[set->dense[set->denseSize]];
	}
	--set->denseSize;
}