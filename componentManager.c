#include "ecs.h"

static SparseSet* sets;
static uintCS componentCount;

void componentManager_init(uintCS const n_componentCount)
{
	componentCount = n_componentCount;
	sets = (componentCount > 0) ? (SparseSet*)malloc_debug(sizeof(SparseSet) * componentCount) : NULL;
}


void componentManager_terminate(void)
{
	//ckeck if pointers are null because that could be if there are no components registered
	if(sets)
	{	
		for(uintCS i=0; i < componentCount; ++i)
			sparseSet_destruct(&sets[i]);
		free_debug(sets);
	}
}


void _componentManager_component_register(ComponentSignature const signature, size_t const componentSize)
{
	sparseSet_construct(&sets[signature], componentSize, signature);
}


void componentManager_entity_register(EntityId const entity, ComponentKey const key)
{
	for(uintCS i=0; i < componentCount; ++i)
		if(keyMatch(1 << i, key))
			sparseSet_entity_add(&sets[i], entity);

}


void componentManager_entity_erase(EntityId const entity)
{
	for(uintCS i=0; i < componentCount; ++i)
		if(((uintEC*)sets[i].sparse)[entity])
			sparseSet_entity_remove(&sets[i], entity);
}



SparseSet* componentManager_sparseSet_get(ComponentSignature const signature)
{
	return &sets[signature];
}