#include "ecs.h"


void sparseSet_construct(SparseSet* set, size_t const componentSize, ComponentSignature const signature, 
						 uintEC const maxComponentsHint, uintEC const maxComponentsDevnHint)
{
	*set = (SparseSet)
	{
		.sparse = malloc_debug(sizeof(uintEC) * maxComponentsHint), 
		.sparseCapacity = 8, 

		.dense = malloc_debug(sizeof(uintEC) * maxComponentsHint), 
		.denseCapacity = 8, 
		.denseSize = 0,

		.maxComponentsDevnHint = maxComponentsDevnHint,

		.components = malloc_debug(componentSize * maxComponentsHint), 
		.signature = signature, 
		.componentSize = componentSize
	};
}


void sparseSet_destruct(SparseSet const *const set)
{
	free_debug(set->dense);
	free_debug(set->sparse);
	free_debug(set->components);
}


void sparseSet_entity_add(SparseSet *const set, EntityId const entity)
{
	if(entity >= set->sparseCapacity)
		set->sparse = realloc(set->sparse, sizeof(uintEC) * (set->sparseCapacity = entity * 2));

	if((set->sparse[entity] = set->denseSize++) >= set->denseCapacity)
	{
		set->dense = realloc(set->dense, sizeof(uintEC) * entity * 2);
		set->components = realloc(set->components, set->componentSize * entity * 2);
		set->denseCapacity = entity * 2;
	}
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