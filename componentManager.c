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
	checs_assert_msg(sig < componentCount, "Too many components registered");
	checs_assert_msg(maxComponentsHint, "Component with 0 maximum count registered");
	checs_assert_msg(componentSize, "Empty component registered. Use Attribute instead.");
	componentSet_construct(&sets[sig], componentSize, maxComponentsHint, component_destructor, component_constructor);
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
	keys[entity] = 0;
}


//the difference between key and keys[entity] is that key signifies only the new components while keys[entity] signifies
//all components the entity has. because of this it is important that key is used when iterating over all sparsesets.
void componentManager_entity_components_add(EntityId const entity, ComponentKey const key)
{
	if (entity >= keysCapacity)
	{
		keys = realloc(keys, sizeof(ComponentKey) * (keysCapacity = keysCapacity * 1.2f));
	}
	if (keys[entity] | key == keys[entity]) //if entity already has component
	{
		return;
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
	checs_assert_msg(keys[entity] == keys[entity] | key, "Couldn't remove component because entity does not have this component");
	keys[entity] -= key;
	for(uintCS i=0; i < componentCount; ++i)
	{
		if(key_match(1 << i, key))
		{
			componentSet_entity_remove(&sets[i], entity);
		}
	}
}