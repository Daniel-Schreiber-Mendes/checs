#include <checs.h>


typedef struct
{
	uintEC *sparse; //sparse packed array of indices to dense array
	uintEC sparseCapacity; //maximum number of elements

	uintEC *dense; //dense array of entityId's
	uintEC denseCapacity; //maximum number of elements
	uintEC denseSize; //current number of elements;

	size_t componentSize; //size of component
	void(*component_destructor)(void*);
	void(*component_constructor)(void*);
	void* components; 

	ComponentKey dependency_key; //key of other components that it needs to have
}
ComponentSet;


static ComponentSet *sets;
static uint8_t componentCount = 10;

static ComponentKey *keys; //array of componentKeys
static uintEC keysCapacity = 1; //highest id that can be stored

static void checs_entity_component_add(EntityId const entity, ComponentSignature sig);
static void csig_check(ComponentSignature sig);


//use calloc because the pointers inside the set are checked to be NULL to check if the component has been registered. 
//first componentset is entirely 0 and should never be used
void checs_components_init()
{
	sets = calloc(sizeof(ComponentSet), (componentCount)); 
}


//auto erase all entitys that the user forgot to erase himself to avoid memory leaks
//if an entity gets erased its key is set back to 0. this means if we encounter a key that is not 0, the user forgot to erase that entity
void checs_components_terminate(void)
{
	for (uint8_t i=1; i < componentCount; ++i)
	{
		if (!sets[i].component_destructor)
			continue;

		for (uintEC d=0; d < sets[i].denseSize; ++d)
		{
			sets[i].component_destructor(sets[i].components + sets[i].componentSize * d);
		}
	}

	for(uintEC i=0; i < componentCount; ++i)
	{
		free(sets[i].dense);
		free(sets[i].sparse); 
		free(sets[i].components);
	}
	free(sets);
	free(keys);
}


void checs_component_register(ComponentSignature const sig, size_t const componentSize, 
	void(*component_destructor)(void *const), void(*component_constructor)(void*const), ComponentKey dependency_key)
{
	checs_assert_msg(sig < componentCount, "Component Signature too high");
	checs_assert_msg(sig != 0, "Tried to register NULL Component");
	checs_assert_msg(componentSize != 0, "Component with size 0 registered");

	sets[sig].componentSize = componentSize;
	sets[sig].component_destructor = component_destructor;
	sets[sig].component_constructor = component_constructor;
	sets[sig].dependency_key = dependency_key;
}

//remove all components from entity
//TODO: what if someone iterates over components while destroying an entity, then the last entity in the set which is moved to the loc of destroyed
// entity will not get iteratet over?
void checs_components_entity_erase(EntityId const entity)
{
	checs_entity_components_remove(entity, keys[entity]);
}


void checs_entity_components_add(EntityId const entity, ComponentKey key)
{
	checs_assert_msg((key & 1) != 1, "Tried to add unregistered Component");
	checs_assert(key);
	checs_assert(entity);

	//check if any component has any missing component dependencys and add those
	key = ckey_add_dependencys(key);

	if (entity >= keysCapacity)
	{
		keysCapacity = entity * 2;
		keys = realloc(keys, sizeof(ComponentKey) * keysCapacity);
		memset(keys + entity, 0, entity * sizeof(ComponentKey));
	}

	checs_assert_msg((keys[entity] | key) != keys[entity], "Tried to add component to entity that entity already has");
	keys[entity] |= key;

	//add entity to componentsets
	for(uint8_t i=1; i < componentCount; ++i) //iterate over every componentset that exists
	{
		if((1 << i) & key)
		{
			checs_entity_component_add(entity, i);
		}
	}
}


static void checs_entity_component_add(EntityId const entity, ComponentSignature sig) 
{
	assert(sets[sig].componentSize);

	ComponentSet *set = &sets[sig];
	if(entity >= set->sparseCapacity)
	{
		set->sparseCapacity = entity * 2;
		set->sparse = realloc(set->sparse, sizeof(uintEC) * set->sparseCapacity);
	}

	if (set->denseSize >= set->denseCapacity)
		checs_component_capacity_set(sig, set->denseCapacity + 1 * 2);
	
	set->dense[set->sparse[entity] = set->denseSize++] = entity;

	if (set->component_constructor)
	{
		set->component_constructor(set->components + set->sparse[entity] * set->componentSize);
	}

	checs_assert(entity == set->dense[set->sparse[entity]]);
}


void checs_entity_components_remove(EntityId const entity, ComponentKey const key)
{
	checs_assert_msg(keys[entity] == keys[entity] | key, "Couldn't remove component because entity does not have this component");
	checs_assert(key);
	checs_assert((key & 1) != 1);
	checs_assert(entity);
	checs_assert(entity < keysCapacity);

	keys[entity] -= key;
	for(uint8_t i=1; i < componentCount; ++i)
	{
		if((1 << i) & key)
		{
			//copy last component to the place that just got freed because the entity that owns the component got destroyed
			//this check needs to be there because if it evaluates to false that means the entity to be removed is the last entity in the array
			//this would mean the data of the last element in the components array would be copied to itself, because the last element gets
			//copied to the place that just got freed. when memory overlaps you need to use memmove which is much slower. the operation
			//would anyway be wasted since only the size needs to be decremented.
			ComponentSet *set = &sets[i];
			checs_assert(entity <= set->sparseCapacity);

			uintEC dense_index = set->sparse[entity];

			if (set->component_destructor)
			{
				set->component_destructor(set->components + dense_index * set->componentSize);
			}
			if(dense_index < (set->denseSize - 1)) // copy last component to freed place
			{
				memcpy(set->components + dense_index * set->componentSize, set->components + (set->denseSize - 1) * set->componentSize, set->componentSize);
				set->dense[dense_index] = set->dense[set->denseSize - 1];
				set->sparse[set->dense[set->denseSize - 1]] = dense_index;
			}
			--set->denseSize;
		}
	}

}


inline void* checs_component_get(ComponentSignature sig, EntityId e)
{
	checs_entity_assert_if_erased(e);
	checs_assert_msg(e, "Tried to acess Null Entity");
	checs_assert_msg(sig < componentCount, "Component Signature too high");
	checs_assert_msg(sig != 0, "Tried to access Null Component");
	checs_assert_msg((1 << sig) & keys[e], "Tried to acess Component that an Entity does not have");
	checs_assert(sets[sig].dense[sets[sig].sparse[e]] != 0); //&& sig && e && sets[sig].components);
	checs_assert(sets[sig].dense[sets[sig].sparse[e]] == e);
	checs_assert(e < sets[sig].sparseCapacity)

	void *p = sets[sig].components + sets[sig].sparse[e] * sets[sig].componentSize;
	assert(p);
	return p;
}


inline uintEC checs_component_count(ComponentSignature sig)
{
	csig_check(sig);
	return sets[sig].denseSize;
}


void checs_component_capacity_set(ComponentSignature sig, uintEC capacity)
{
	csig_check(sig);
	ComponentSet *set = &sets[sig];

	checs_assert_msg(capacity > set->denseSize, "Tried to make DenseSet Capacity smaller than DenseSet size");

	set->dense = realloc(set->dense, sizeof(uintEC) * capacity); //reallocate denseSet and zero out unused space
	memset(&set->dense[set->denseSize], 0, (capacity - set->denseSize) * sizeof(uintEC));

	set->denseCapacity = capacity;

	set->components = realloc(set->components, set->componentSize * capacity);
	memset(set->components + set->denseSize * set->componentSize, 0, (set->denseCapacity - set->denseSize) * set->componentSize);
}


//deletes all values of all components but allocated size stays the same 
void checs_components_reset(void)
{
	memset(keys, 0, sizeof(ComponentKey) * keysCapacity);
	for(uintEC i=0; i < componentCount; ++i)
	{
		memset(sets[i].components, 0, sets[i].componentSize * sets[i].denseCapacity);
		memset(sets[i].dense, 0, sizeof(EntityId) * sets[i].denseCapacity);
		memset(sets[i].sparse, 0, sizeof(uintEC) * sets[i].sparseCapacity); 
		sets[i].denseSize = 0;
	}
}


//copy component of one entity to another
void checs_component_copy(ComponentSignature sig, EntityId src, EntityId dest)
{
	csig_check(sig);
	checs_assert(src != dest && dest && src);
	checs_assert(sets[sig].sparse[dest] < sets[sig].denseSize && sets[sig].sparse[src] < sets[sig].denseSize);

	void *src_component = sets[sig].components + sets[sig].sparse[src] * sets[sig].componentSize;
	void *dest_component = sets[sig].components + sets[sig].sparse[dest] * sets[sig].componentSize;
	memcpy(dest_component, src_component, sets[sig].componentSize);
}


inline bool checs_entity_has_component(ComponentSignature sig, EntityId entity)
{
	csig_check(sig);
	return (1 << sig) & keys[entity];
}


void* checs_components_foreach(ComponentSignature sig, EntityId *entity, uintEC index)
{
	csig_check(sig);
	if (index < sets[sig].denseSize)
	{
		*entity = sets[sig].dense[index];
		return sets[sig].components + index *sets[sig].componentSize;
	}

	return NULL;
}


bool checs_components_iterate(ComponentSignature sig, EntityId *entity, uintEC index)
{
	if (index < sets[sig].denseSize)
	{
		*entity = sets[sig].dense[index];
		return true;
	}
	return false;
}


EntityId checs_component_entitys_foreach(ComponentSignature sig, uintEC index)
{
	csig_check(sig);
	if (index < sets[sig].denseSize)
	{
		return sets[sig].dense[index];
	}

	return 0;
}


void checs_components_copy(EntityId src, EntityId dest)
{
	for(uint8_t i=1; i < componentCount; ++i)
	{
		if((1 << i) & keys[src])
		{
			checs_component_copy(i, src, dest);
		}
	}
}

/*
 * get random entity with at least given components. if there is no entity that fits they key then return 0
 * iterate over all entitys in first component in given key
 */
EntityId checs_entity_get_by_key(ComponentKey key)
{
	checs_assert(key);

	//no need to add dependencys to key because if 



	return 0;
}


void csig_check(ComponentSignature sig)
{
	checs_assert_msg(sig < componentCount, "Component Signature too high");
	checs_assert_msg(sig != 0, "Tried to access NULL Component");
	checs_assert_msg(sets[sig].componentSize, "Component has not been registered");
}

ComponentKey ckey_get(EntityId e)
{
	return keys[e];
}


ComponentKey ckey_add_dependencys(ComponentKey key)
{
	for(uint8_t i=1; i < componentCount; ++i)
	{
		if(((1 << i) & key) && sets[i].dependency_key) //if given key has this component and component has dependency
		{
			for (uint8_t j=1; j < componentCount; ++j) //for every dependency
			{
				if (sets[i].dependency_key & (1 << j)) //is component dependent on this component?
				{
					if ((key & 1 << j)) continue; //does entity already have this dependency component?

					key |= 1 << j;		
				}
			}
		}
	}
	return key;
}