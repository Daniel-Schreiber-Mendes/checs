#include "ecs.h"

//hashmap of vectors of entitys
HashMap attributes;


void attributeManager_init(uintA const maxAttributesHint)
{
	hashMap_construct(&attributes, maxAttributesHint);
}


void _attributeManager_attribute_register(AttributeSignature const sig, uintA const attributeCount)
{
	Vector *vec = malloc(sizeof(Vector));
	vector_construct(vec, sizeof(EntityId));
	vector_reserve(vec, attributeCount);
	hashMap_insert(&attributes, sig, vec);
}


void attributeManager_terminate(void)
{
	hashMap_foreach(&attributes, Vector*, vec,
	({
		vector_destruct(vec);
		checs_free(vec);
	}));
	hashMap_destruct(&attributes);
}