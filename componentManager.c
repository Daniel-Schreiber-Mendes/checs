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
		{
			vector_destruct(&sets[i].sparse);
			vector_destruct(&sets[i].dense);
		}
		free_debug(sets);
	}
}


void _componentManager_component_register(ComponentSignature const signature, size_t const componentSize)
{
	SparseSet* set = &sets[signature];
	*set = (SparseSet){.componentSize = componentSize, .signature = signature};
	vector_construct(&set->dense, sizeof(uintEC));
	vector_construct(&set->sparse, set->componentSize);
	vector_reserve(&set->sparse, 8);
	vector_reserve(&set->dense, 8);
}


void componentManager_entity_register(EntityId const entity, ComponentKey const key)
{
	for(uintCS i=0; i < getBitCount(key); ++i)
		if(keyMatch(1 << i, key))
		{
			vector_element_insert(&sets[i].sparse, EntityId, entity, entity);
			vector_size_increment(&sets[i].dense);
		}

}


SparseSet* componentManager_sparseSet_get(ComponentSignature const signature)
{
	return &sets[signature];
}