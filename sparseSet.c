#include "ecs.h"


void sparseSet_construct(SparseSet* set, size_t const componentSize, ComponentSignature const signature)
{
	*set = (SparseSet){.sparse = malloc_debug(sizeof(uintEC) * 8), .sparseCapacity = 8, 
					   .dense = malloc_debug(sizeof(uintEC) * 8), .denseCapacity = 8, .denseSize = 0,
				   .components = malloc_debug(componentSize * 8), .signature = signature, .componentSize = componentSize};
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
	{
		set->sparse = realloc(set->sparse, set->componentSize * (entity + 8));
		set->sparseCapacity = entity + 8;
	}
	set->sparse[entity] = set->denseSize++;
}


void sparseSet_entity_remove(SparseSet *const set, EntityId const entity)
{
	//copy last compont to the place that just got free because the entity that owns the component got destroyed
	memcpy(&set->components + set->sparse[entity] * set->denseSize, &set->components + set->denseSize * set->componentSize, set->componentSize); 
	set->dense[set->sparse[entity]] = set->dense[set->denseSize--];
	set->sparse[entity] = 0;
}