#include "ecs.h"

uint16_t *setIndices; //this maps a cki into a sparseset inside the hashmap
uint8_t registeredComponentsCount; //used to know what cki to assign to a registered component

HashMap sets; //hashmap of sparseSets
static uintCS componentCount;

ComponentKey *keys; //array of componentKeys
static uintEC keysCapacity; //highest id that can be currently stored


void componentManager_init(uintCS const n_componentCount, uintEC const maxEntitysHint)
{
	checs_assert(n_componentCount > 0 && maxEntitysHint > 0);
	hashMap_construct(&sets, (componentCount = n_componentCount));
	keys = checs_calloc((keysCapacity = maxEntitysHint), sizeof(ComponentKey));
	setIndices = checs_calloc(componentCount, sizeof(uint16_t));
}


//auto erase all entitys that the user forgot to erase himself to avoid memory leaks
//if an entity gets erased its key is set back to 0. this means if we encounter a key that is not 0, the user forgot to erase that entity
void componentManager_terminate(void)
{
	for (uintEC i=0; i < keysCapacity; ++i)
	{
		if (keys[i] != 0)
		{
			checs_stats_log(printf("Entity %u had to be cleaned up by checs. Shame on you!\n", i););
			componentManager_entity_erase(i);
		}
	}

	hashMap_foreach(&sets, SparseSet*, set,
	({
		sparseSet_destruct(set);
		checs_free(set);
	}));
	
	hashMap_destruct(&sets);
	checs_free(keys);
	checs_free(setIndices);
}


void _componentManager_component_register(ComponentSignature const sig, size_t const componentSize, uintEC const maxComponentsHint, void(*component_destructor)(void *const), void(*component_constructor)(void*const))
{
	SparseSet *const set = checs_malloc(sizeof(SparseSet));
	hashMap_insert(&sets, sig, set);
	sparseSet_construct(set, componentSize, registeredComponentsCount, maxComponentsHint, component_destructor, component_constructor);
	checs_assert(registeredComponentsCount + 1 <= componentCount);
	setIndices[registeredComponentsCount++] = sig;
}


void componentManager_entity_register(EntityId const entity, ComponentKey const key)
{
	checs_assert(entity < keysCapacity);
	_componentManager_entity_components_add(entity, key);
}

void componentManager_entity_erase(EntityId const entity)
{
	for(uintCS i=0; i < componentCount; ++i)
	{
		if(key_match(1 << i, keys[entity]))
		{
			sparseSet_entity_remove(hashMap_get(&sets, SparseSet, setIndices[i]), entity);
		}
	}
	keys[entity] = 0;
}


//the difference between key and keys[entity] is that key signifies only the new components while keys[entity] signifies
//all components the entity has. because of this it is important that key is used when iterating over all sparsesets.
void _componentManager_entity_components_add(EntityId const entity, ComponentKey const key)
{
	checs_assert(key != 0);
	keys[entity] |= key;
	for(uintCS i=0; i < componentCount; ++i)
	{
		if(key_match(1 << i, key))
		{
			sparseSet_entity_add(hashMap_get(&sets, SparseSet, setIndices[i]), entity);
		}
	}
}