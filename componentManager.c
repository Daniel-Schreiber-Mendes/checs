#include "ecs.h"

static SparseSet* sets;
static uintCS componentCount;

static ComponentKey* keys; //array of componentKeys
static uintEC keysCapacity; //highest id that can be currently stored

void componentManager_init(uintCS const n_componentCount)
{
	//if componentCount is 0 it is undefined behaviour
	sets =  malloc_debug(sizeof(SparseSet) * (componentCount = n_componentCount));
	keys = malloc_debug(sizeof(ComponentKey) * (keysCapacity = max_entitys_hint));
}


void componentManager_terminate(void)
{
	for(uintCS i=0; i < componentCount; ++i)
		sparseSet_destruct(&sets[i]);
	free_debug(sets);
	free_debug(keys);
}


void _componentManager_component_register(ComponentSignature const signature, size_t const componentSize)
{
	sparseSet_construct(&sets[signature], componentSize, signature);
}


void componentManager_entity_register(EntityId const entity, ComponentKey const key)
{
	if(entity >= keysCapacity)
		keys = realloc(keys, sizeof(ComponentKey) * (keysCapacity += max_entitys_devn_hint));
	keys[entity] = key;

	for(uintCS i=0; i < componentCount; ++i)
		if(key_match(1 << i, key))
			sparseSet_entity_add(&sets[i], entity);

}


void componentManager_entity_erase(EntityId const entity)
{
	for(uintCS i=0; i < componentCount; ++i)
		if(key_match(1 << i, keys[entity]))
			sparseSet_entity_remove(&sets[i], entity);
}



SparseSet* componentManager_sparseSet_get(ComponentSignature const signature)
{
	return &sets[signature];
}


ComponentKey componentManager_key_get(EntityId const entity)
{
	return keys[entity];
}