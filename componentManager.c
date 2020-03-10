#include "ecs.h"

uint16_t *setIndices; //this maps a cki into a sparseset inside the hashmap
uint8_t registeredComponentsCount = 0; //used to know what cki to assign to a registered component

HashMap sets; //hashmap of sparseSets
static uintCS componentCount;

static uintEC maxEntitysDevnHint;

ComponentKey *keys; //array of componentKeys
static uintEC keysCapacity; //highest id that can be currently stored


void componentManager_init(uintCS const n_componentCount, uintEC const maxEntitysHint, uintEC const n_maxEntitysDevnHint)
{
	//if componentCount is 0 it is undefined behaviour
	maxEntitysDevnHint = n_maxEntitysDevnHint;
	hashMap_construct(&sets, (componentCount = n_componentCount));
	keys = checs_malloc(sizeof(ComponentKey) * (keysCapacity = maxEntitysHint));
	setIndices = checs_calloc(sizeof(uint16_t), componentCount);
}


void componentManager_terminate(void)
{
	for(uintCS i=0; i < sets.capacity; ++i)
	{
		if (sets.data[i] != NULL)
		{
			sparseSet_destruct(sets.data[i]);
			checs_free(sets.data[i]);
		}
	}
	hashMap_destruct(&sets);
	checs_free(keys);
	checs_free(setIndices);
}


void _componentManager_component_register(ComponentSignature const sig, size_t const componentSize, uintEC const maxComponentsHint, uintEC const maxComponentsDevnHint)
{
	SparseSet *const set = checs_malloc(sizeof(SparseSet));
	hashMap_element_insert(&sets, sig, set);
	sparseSet_construct(set, componentSize, registeredComponentsCount, maxComponentsHint, maxComponentsDevnHint);
	setIndices[registeredComponentsCount++] = sig;
}


void componentManager_entity_register(EntityId const entity, ComponentKey const key)
{
	if(entity >= keysCapacity)
	{
		keys = realloc(keys, sizeof(ComponentKey) * (keysCapacity += maxEntitysDevnHint));
	}
	keys[entity] = 0;
	_componentManager_entity_components_add(entity, key);
	//printf("registered entity: %u, key: %u\n", entity, key);
}


void componentManager_entity_erase(EntityId const entity)
{
	for(uintCS i=0; i < componentCount; ++i)
	{
		if(key_match(1 << i, keys[entity]))
		{
			sparseSet_entity_remove(hashMap_element_get(&sets, SparseSet, setIndices[i]), entity);
		}
	}
}


void _componentManager_entity_components_add(EntityId const entity, ComponentKey const key)
{
	//the difference between key and keys[entity] is that key signifies only the new components while keys[entity] signifies
	//all components the entity has. because of this it is important that key is used when iterating over all sparsesets.
	keys[entity] |= key;
	for(uintCS i=0; i < componentCount; ++i)
	{
		if(key_match(1 << i, key))
		{
			//printf("Entity %u received component %u\n", entity, 1 << i);
			sparseSet_entity_add(hashMap_element_get(&sets, SparseSet, setIndices[i]), entity);
		}
	}
}