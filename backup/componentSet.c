#include "ecs.h"
#include <string.h>


void componentSet_construct(ComponentSet *const set, size_t const componentSize, ComponentKeyIndex const cki, uintEC const maxComponentsHint, 
						 void(*component_destructor)(void *const), void(*component_constructor)(void *const))
{
	*set = (ComponentSet)
	{
		.cki = cki, 
		.componentSize = componentSize,
		.component_destructor = component_destructor,
		.component_constructor = component_constructor
	};
	dsparseSet_construct(&set->set, sizeof(EntityId), componentSize, maxComponentsHint);
}


void componentSet_destruct(ComponentSet *const set)
{
	checs_assert(set);
	dsparseSet_destruct(&set->set);
}


void componentSet_entity_add(ComponentSet *const set, EntityId const entity)
{
	//dsparseSet_insert(&set->set, EntityId, entity);

	if(entity >= set->set.sparseCapacity)
	{
		set->set.sparse = realloc(set->set.sparse, sizeof(uintEC) * (set->set.sparseCapacity = entity * 2));
	}

	checs_assert(set->set.denseSize + 1 <= set->set.denseCapacity);
	((uintEC*)set->set.dense)[((uintEC*)set->set.sparse)[entity] = set->set.denseSize++] = entity;

	if (set->component_constructor)
	{
		set->component_constructor(set->set.data + ((uintEC*)(set->set.sparse))[entity] * set->componentSize);
	}
}


void componentSet_entity_remove(ComponentSet *const set, EntityId const entity)
{
	//copy last compont to the place that just got freed because the entity that owns the component got destroyed
	//this check needs to be there because if it evaluates to false that means the entity to be removed is the last entity in the array
	//this would mean the data of the last element in the components array would be copied to itself, because the last element gets
	//copied to the place that just got freed. when memory overlaps you need to use memmove which is much slower. the operation
	//would anyway be wasted since only the size needs to be decremented.
	if (set->component_destructor)
	{
		set->component_destructor(set->set.data + ((uintEC*)(set->set.sparse))[entity] * set->componentSize);
	}
	//dsparseSet_erase(&set->set, EntityId, entity);
}