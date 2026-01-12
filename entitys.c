#include <checs.h>

static uintEC nextEntityId = 1; //entity with id 0 shall never be used 

//array of deleted entitys waiting to be used
static struct
{
	EntityId *entitys;
	uintEC size;
	uintEC capacity;
} unused;


//unique names added to entitys
static struct
{
	EntityId *entitys;
	char **strings;
	uintEC size;
	uintEC capacity;
} names;


void checs_entitys_init(void)
{

}


void checs_entitys_terminate(void)
{
	if (unused.entitys)
		free(unused.entitys);

	if (names.entitys)
	{
		free(names.entitys);
		free(names.strings);
	}
}


EntityId checs_entity(ComponentKey const key)
{
	EntityId entity;
	if (unused.size >= 1)
	{
		entity = unused.entitys[--unused.size];
	}
	else
	{
		entity = nextEntityId++;
	}

	checs_entity_components_add(entity, key);
	checs_assert(ckey_get(entity) | key == key);
	//printf("CREATE Entity %u with key %u\n", entity, ckey_get(entity));
	return entity;
}


//completely delete entity
void checs_entity_erase(EntityId const entity)
{
	ComponentKey key = ckey_get(entity);

	//printf("ERASE Entity %u with key %u \n", entity, key);
	checs_assert_msg(entity != 0, "Tried to erase NULL Entity");
	checs_assert_msg(entity < nextEntityId, "Tried to erase Entity that hasnt been created");
	if (!key)
	{
		#ifndef NDEBUG
		for (uintEC i=0; i < unused.size; ++i)
		{
			if (unused.entitys[i] == entity)
			{
				checs_assert_msg(false, "Tried to erase entity that has already been erased");
			}
		}
		#endif
	}


	if (unused.size + 1 >= unused.capacity)
	{
		unused.capacity = unused.capacity * 2 + 1;
		unused.entitys = realloc(unused.entitys, unused.capacity * sizeof(uintEC));
	}
	unused.entitys[unused.size++] = entity;
	/* the order of the two functions is important: the components_remove call 
	changes the entity key to 0, so the systems call has to be before that in order to remove 
	the entity from all systems correctly */
	checs_entity_components_remove(entity, key);
	for (uint8_t i=0; i < names.size; ++i)
	{
		if (names.entitys[i] == entity)
		{
			names.entitys[i] = names.entitys[names.size - 1];
			names.strings[i] = names.strings[names.size - 1];
			--names.size;
		}
	}
	checs_assert_msg(ckey_get(entity) == 0, "Error while erasing Entity");

	checs_tags_entity_erase(entity);
}


void checs_entitys_reset(void)
{
	names.size = 0;
	nextEntityId = 1;
	unused.size = 0;
}


EntityId checs_entity_copy(EntityId e)
{
	assert(e);
	checs_assert(e < nextEntityId);
	EntityId duplicate = checs_entity(ckey_get(e));
	checs_components_copy(e, duplicate);
	checs_tags_copy(e, duplicate);
	return duplicate;
}


void checs_entity_name_add(EntityId e, char *name)
{
	assert(e);
	checs_assert(e < nextEntityId);
	if (names.size + 1 >= names.capacity)
	{
		names.capacity = names.capacity * 2 + 1;
		names.entitys = realloc(names.entitys, sizeof(EntityId) * names.capacity);
		names.strings = realloc(names.strings, sizeof(char*) * names.capacity);
	}

	names.entitys[names.size] = e;
	names.strings[names.size] = name;

	++names.size;
}


EntityId checs_entity_get_by_name(char *name)
{
	for (uint8_t i=0; i < names.capacity; ++i)
	{
		if (!strcmp(name, names.strings[i]))
		{
			return names.entitys[i];
		}
	}
	checs_assert_msg(false, "Tried to get entity by name \"%s\" but name was not found", name);
	return 0;
}


void checs_entity_assert_if_erased(EntityId entity)
{
	#ifndef NDEBUG
		for (uintEC i=0; i < unused.size; ++i)
		{
			if (unused.entitys[i] == entity)
			{
				checs_assert_msg(false, "Tried to access entity that has already been erased");
			}
		}
	#endif
}