#include "ecs.h"

SparseSet *sets;
static uintCS componentCount;

static uintEC maxEntitysDevnHint;

ComponentKey *keys; //array of componentKeys
static uintEC keysCapacity; //highest id that can be currently stored

void componentManager_init(uintCS const n_componentCount, uintEC const maxEntitysHint, uintEC const n_maxEntitysDevnHint)
{
	//if componentCount is 0 it is undefined behaviour
	maxEntitysDevnHint = n_maxEntitysDevnHint;
	sets =  malloc_debug(sizeof(SparseSet) * (componentCount = n_componentCount));
	keys = malloc_debug(sizeof(ComponentKey) * (keysCapacity = maxEntitysHint));
}


void componentManager_terminate(void)
{
	for(uintCS i=0; i < componentCount; ++i)
		sparseSet_destruct(&sets[i]);
	free_debug(sets);
	free_debug(keys);
}


void _componentManager_component_register(ComponentSignature const signature, size_t const componentSize, 
										  uintEC const maxComponentsHint, uintEC const maxComponentsDevnHint)
{
	assert(signature < componentCount);
	sparseSet_construct(&sets[signature], componentSize, signature, maxComponentsHint, maxComponentsDevnHint);
}


void componentManager_entity_register(EntityId const entity, ComponentKey const key)
{
	if(entity >= keysCapacity)
		keys = realloc(keys, sizeof(ComponentKey) * (keysCapacity += maxEntitysDevnHint));
	keys[entity] = 0;
	_componentManager_entity_components_add(entity, key);
}


void componentManager_entity_erase(EntityId const entity)
{
	for(uintCS i=0; i < componentCount; ++i)
		if(key_match(1 << i, keys[entity]))
			sparseSet_entity_remove(&sets[i], entity);
}


void _componentManager_entity_components_add(EntityId const entity, ComponentKey const key)
{
	//the difference between key and keys[entity] is that key signifies only the new components while keys[entity] signifies
	//all components the entity has. because of this it is important that key is used when iterating over all sparsesets.
	keys[entity] |= key;
	for(uintCS i=0; i < componentCount; ++i)
		if(key_match(1 << i, key))
			sparseSet_entity_add(&sets[i], entity);
}