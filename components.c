#include "ecs.h"

ComponentSet *sets; //hashmap of componentSets
static uintCS componentCount;

ComponentKey *keys; //array of componentKeys
static uintEC keysCapacity; //highest id that can be currently stored


void checs_components_init(uintCS const n_componentCount)
{
	sets = checs_malloc(sizeof(ComponentSet) * (componentCount = n_componentCount + 1)); //+1 because the first component set is invalid
	keys = checs_calloc(keysCapacity = 256, sizeof(ComponentKey));
}


//auto erase all entitys that the user forgot to erase himself to avoid memory leaks
//if an entity gets erased its key is set back to 0. this means if we encounter a key that is not 0, the user forgot to erase that entity
void checs_components_terminate(void)
{
	for(uintEC i=0; i < componentCount; ++i)
	{
		checs_free(sets[i].dense);
		checs_free(sets[i].sparse); 
		checs_free(sets[i].components);
	}
	checs_free(sets);
	checs_free(keys);
}


void checs_component_register(ComponentSignature const sig, size_t const componentSize, uintEC const maxComponentsHint, void(*component_destructor)(void *const), void(*component_constructor)(void*const))
{
	checs_assert_msg(sig < componentCount, "Too many components registered");
	checs_assert_msg(maxComponentsHint, "Component with 0 maximum count registered");
	checs_assert_msg(componentSize, "Empty component registered. Use Attribute instead.");
	checs_component_assert(sig);
	//componentSet_construct(&sets[sig], componentSize, maxComponentsHint, component_destructor, component_constructor);
	sets[sig] = (ComponentSet)
	{
		.sparse = checs_malloc(sizeof(uintEC) * maxComponentsHint), 
		.sparseCapacity = maxComponentsHint, 

		.dense = checs_calloc(maxComponentsHint, sizeof(uintEC)), 
		.denseCapacity = maxComponentsHint, 
		.denseSize = 0,
 
		.componentSize = componentSize,
		.component_destructor = component_destructor,
		.component_constructor = component_constructor,
		.components = checs_malloc(componentSize * maxComponentsHint)
	};
}


//the difference between key and keys[entity] is that key signifies only the new components while keys[entity] signifies
//all components the entity has. because of this it is important that key is used when iterating over all sparsesets.
void checs_components_entity_add(EntityId const entity, ComponentKey const key)
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

	for(uintCS i=1; i < componentCount; ++i)
	{
		if(checs_key_match(1 << i, key))
		{
			if(entity >= sets[i].sparseCapacity)
			{
				sets[i].sparse = realloc(sets[i].sparse, sizeof(uintEC) * (sets[i].sparseCapacity = entity * 2));
			}
			checs_assert_msg(sets[i].denseSize + 1 <= sets[i].denseCapacity, "Too many entitys added to componentset");
			sets[i].dense[sets[i].sparse[entity] = sets[i].denseSize++] = entity;

			if (sets[i].component_constructor)
			{
				sets[i].component_constructor(sets[i].components + sets[i].sparse[entity] * sets[i].componentSize);
			}
		}
	}
}


void checs_components_entity_remove(EntityId const entity, ComponentKey const key)
{
	checs_assert_msg(keys[entity] == keys[entity] | key, "Couldn't remove component because entity does not have this component");
	keys[entity] -= key;
	for(uintCS i=1; i < componentCount; ++i)
	{
		if(checs_key_match(1 << i, key))
		{
			//copy last compont to the place that just got freed because the entity that owns the component got destroyed
			//this check needs to be there because if it evaluates to false that means the entity to be removed is the last entity in the array
			//this would mean the data of the last element in the components array would be copied to itself, because the last element gets
			//copied to the place that just got freed. when memory overlaps you need to use memmove which is much slower. the operation
			//would anyway be wasted since only the size needs to be decremented.
			checs_assert(entity < sets[i].sparseCapacity);
			if (sets[i].component_destructor)
			{
				sets[i].component_destructor(sets[i].components + sets[i].sparse[entity] * sets[i].componentSize);
			}
			if(sets[i].sparse[entity] < (sets[i].denseSize - 1))
			{
				memmove(sets[i].components + sets[i].sparse[entity] * sets[i].componentSize, sets[i].components + (sets[i].denseSize - 1) * sets[i].componentSize, sets[i].componentSize);
				sets[i].dense[sets[i].sparse[entity]] = sets[i].dense[sets[i].denseSize - 1];
				sets[i].sparse[sets[i].dense[sets[i].denseSize - 1]] = sets[i].sparse[entity];

			}
			--sets[i].denseSize;
		}
	}
}


void* checs_component_get(ComponentSignature sig, EntityId e)
{
	checs_component_assert(sig);
	checs_entity_assert(sig, e);
	return sets[sig].components + sets[sig].sparse[e] * sets[sig].componentSize;
}


uintEC checs_component_count(ComponentSignature sig)
{
	checs_component_assert(sig);
	return sets[sig].denseSize;
}

uintEC checs_max_component_count(ComponentSignature sig)
{
	checs_component_assert(sig);
	return sets[sig].denseCapacity;
}

bool checs_entity_has_component(ComponentSignature sig, EntityId entity)
{
	checs_component_assert(sig);
	return (1 << sig) & keys[entity];
}


bool checs_key_match(ComponentKey requiredKey, ComponentKey providedKey)
{
	return (requiredKey & providedKey) == requiredKey;
}