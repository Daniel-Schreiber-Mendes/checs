#include "ecs.h"
#include <string.h>


void sparseSet_construct(SparseSet* set, size_t const componentSize, ComponentKeyIndex const cki, uintEC const maxComponentsHint, void(*component_destructor)(void*))
{
	*set = (SparseSet)
	{
		.sparse = checs_malloc(sizeof(uintEC) * maxComponentsHint), 
		.sparseCapacity = maxComponentsHint, 

		.dense = checs_calloc(sizeof(uintEC), maxComponentsHint), 
		.denseCapacity = maxComponentsHint, 
		.denseSize = 0,

		.components = checs_malloc(componentSize * maxComponentsHint), 
		.cki = cki, 
		.componentSize = componentSize,
		.component_destructor = component_destructor
	};
}


void sparseSet_destruct(SparseSet const *const set)
{
	checs_free(set->dense);
	checs_free(set->sparse);
	checs_free(set->components);
}


void sparseSet_entity_add(SparseSet *const set, EntityId const entity)
{
	if(entity >= set->sparseCapacity)
	{
		set->sparse = realloc(set->sparse, sizeof(uintEC) * (set->sparseCapacity = entity * 2));
	}

	set->sparse[entity] = set->denseSize++;
	checs_assert(set->denseSize <= set->denseCapacity);
	set->dense[set->sparse[entity]] = entity;
}


void sparseSet_entity_remove(SparseSet *const set, EntityId const entity)
{
	//copy last compont to the place that just got freed because the entity that owns the component got destroyed
	//this check needs to be there because if it evaluates to false that means the entity to be removed is the last entity in the array
	//this would mean the data of the last element in the components array would be copied to itself, because the last element gets
	//copied to the place that just got freed. when memory overlaps you need to use memmove which is much slower. the operation
	//would anyway be wasted since only the size needs to be decremented.
	if(set->sparse[entity] < set->denseSize - 1) //this needs to be checked because if it evaluates to false that means the entity to be removed
	{
		memcpy(set->components + set->sparse[entity] * set->componentSize, set->components + set->denseSize * set->componentSize, set->componentSize); 
		set->dense[set->sparse[entity]] = set->dense[set->denseSize];
		set->sparse[entity] = set->sparse[set->dense[set->denseSize]];
	}
	--set->denseSize;
}