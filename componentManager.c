#include "ecs.h"

ComponentSet *sets; //hashmap of componentSets
static uintCS componentCount;

ComponentKey *keys; //array of componentKeys
static uintEC keysCapacity; //highest id that can be currently stored


void componentManager_init(uintCS const n_componentCount, uintEC const maxEntitys)
{
	sets = checs_malloc(sizeof(ComponentSet) * (componentCount = n_componentCount));
	keys = checs_calloc(keysCapacity = maxEntitys, sizeof(ComponentKey));
}


//auto erase all entitys that the user forgot to erase himself to avoid memory leaks
//if an entity gets erased its key is set back to 0. this means if we encounter a key that is not 0, the user forgot to erase that entity
void componentManager_terminate(void)
{
	for(uintEC i=0; i < componentCount; ++i)
	{
		componentSet_destruct(&sets[i]);
	}
	checs_free(sets);
	checs_free(keys);
}


void componentManager_component_register(ComponentSignature const sig, size_t const componentSize, uintEC const maxComponentsHint, void(*component_destructor)(void *const), void(*component_constructor)(void*const))
{
	checs_assert(sig < componentCount);
	checs_assert(maxComponentsHint);
	checs_assert(componentSize);
	componentSet_construct(&sets[sig], componentSize, sig, maxComponentsHint, component_destructor, component_constructor);
}


void componentManager_entity_erase(EntityId const entity)
{
	for(uintCS i=0; i < componentCount; ++i)
	{
		if(key_match(1 << i, keys[entity]))
		{
			componentSet_entity_remove(&sets[i], entity);
		}
	}
}


//the difference between key and keys[entity] is that key signifies only the new components while keys[entity] signifies
//all components the entity has. because of this it is important that key is used when iterating over all sparsesets.
void componentManager_entity_components_add(EntityId const entity, ComponentKey const key)
{
	checs_assert(key != 0);
	if (entity >= keysCapacity)
	{
		keys = realloc(keys, sizeof(ComponentKey) * (keysCapacity = keysCapacity * 1.2f));
	}
	keys[entity] |= key;

	for(uintCS i=0; i < componentCount; ++i)
	{
		if(key_match(1 << i, key))
		{
			componentSet_entity_add(&sets[i], entity);
		}
	}
}


void componentManager_entity_components_remove(EntityId const entity, ComponentKey const key)
{
	checs_assert(key != 0 && keys[entity] == keys[entity] | key);
	keys[entity] -= key;
	for(uintCS i=0; i < componentCount; ++i)
	{
		if(key_match(1 << i, key))
		{
			componentSet_entity_remove(&sets[i], entity);
		}
	}
}