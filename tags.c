#include <checs.h>


#define TAG_COUNT 32


static struct
{
	EntityId *entitys;
	uint32_t dense_size;
	uint32_t dense_capacity;

	uint32_t *sparse;
	uint32_t sparse_capacity;
	bool registered;
} sets[TAG_COUNT];


static struct
{	
	TagKey *keys;
	uint32_t capacity;
} key_array;


void checs_tags_init(void)
{

}


void checs_tags_terminate(void)
{
	free(key_array.keys);
}


void checs_tag_register(TagSignature sig)
{
	assert(sig);
	checs_assert_msg(!sets[sig].registered, "Tried to register Tag twice");
	sets[sig].registered = true;
}


void checs_tags_add(TagKey key, EntityId e)
{
	assert(key != 0);
	assert(e);

	if (e >= key_array.capacity)
	{
		uint32_t keys_capacity_new = e * 2;

		key_array.keys = realloc(key_array.keys, keys_capacity_new * sizeof(TagKey));
		memset(key_array.keys + key_array.capacity, 0, (keys_capacity_new - key_array.capacity) * sizeof(TagKey));

		key_array.capacity = keys_capacity_new;
	}

	key_array.keys[e] |= key;

	for (uint8_t i=1; i < TAG_COUNT; ++i)
	{
		if ((1 << i) & key)
		{
			assert(sets[i].registered);

			if (e >= sets[i].sparse_capacity)
			{ //its not enough to double the capacity like with dense, because maybe e is bigger than twice the previous capacity
				sets[i].sparse_capacity = e * 2; 
				sets[i].sparse = realloc(sets[i].sparse, sets[i].sparse_capacity * sizeof(EntityId));
			}

			sets[i].sparse[e] = sets[i].dense_size;
			uint32_t dense_index = sets[i].sparse[e];

			if (dense_index >= sets[i].dense_capacity)
			{
				sets[i].dense_capacity = (sets[i].dense_capacity + 1 ) * 2;
				sets[i].entitys = realloc(sets[i].entitys, sets[i].dense_capacity * sizeof(EntityId));
			}
			sets[i].entitys[dense_index] = e;
			++sets[i].dense_size;
		}
	}
}


void checs_tags_remove(TagKey key, EntityId e)
{
	assert(e && key);
	assert(e < key_array.capacity);
	for (uint8_t i=1; i < TAG_COUNT; ++i)
	{
		if ((1 << i) & key)
		{ //move last entity in array to freed place and decrement size
			assert(sets[i].registered);
			--sets[i].dense_size;

			EntityId last_entity = sets[i].entitys[sets[i].dense_size];
			uint32_t removed_entity_i = sets[i].sparse[e];
			sets[i].entitys[removed_entity_i] = last_entity;
			sets[i].sparse[last_entity] = removed_entity_i;
		}
	}
	key_array.keys[e] ^= key;
}


void checs_tags_entity_erase(EntityId e)
{
	checs_assert(e);
	if (e >= key_array.capacity) return;
	if (!key_array.keys[e]) return;
	
	checs_tags_remove(key_array.keys[e], e);
}


EntityId checs_tags_iterate(TagSignature sig, uint32_t index)
{
	assert(sig < TAG_COUNT && sig);
	assert(sets[sig].registered);
	if (index < sets[sig].dense_size)
	{
		return sets[sig].entitys[index];
	}
	return 0;
}


void checs_tags_copy(EntityId src, EntityId dest)
{
	assert(src && dest);

	if (src < key_array.capacity && key_array.keys[src])
		checs_tags_add(key_array.keys[src], dest);
}


void checs_tags_reset(void)
{
	for (uint8_t i=1; i < TAG_COUNT; ++i)
	{
		if (sets[i].registered)
		{
			sets[i].dense_size = 0;
		}
	}
	memset(key_array.keys, 0, key_array.capacity * sizeof(TagKey));
}


EntityId checs_entity_get_by_tag(TagKey key)
{
	//BUG
	checs_assert(key);

	for(uint8_t i=1; i < TAG_COUNT; ++i)
	{
		if((1 << i) & key)
		{
			checs_assert_msg(sets[i].registered, "Tried to get entity by tag with unregistered TagKey used");
			for (uint32_t j = 0; j < sets[i].dense_size; ++j)
			{
				EntityId e = sets[i].entitys[j];
				if ((key_array.keys[e] & key) == key)
				{
					return e;
				}
			}
			for (uint32_t j = 0; j < sets[i].dense_size; ++j)
			{
				EntityId e = sets[i].entitys[j];
			}
		}
	}
	return 0; //no entity found that fits key
}