#include "ecs.h"

Vector *attributes;
static uintA size;


void attributeManager_init(uintA const maxAttributesHint)
{
	attributes = checs_malloc(sizeof(Vector) * (size = maxAttributesHint));
}


void checs_attribute_register(AttributeSignature const sig, uintA const attributeCount)
{
	checs_assert(sig < size);
	checs_assert(attributeCount);
	vector_construct(&attributes[sig], sizeof(EntityId));
	vector_reserve(&attributes[sig], attributeCount);
}


void attributeManager_terminate(void)
{
	for (uintA i=0; i < size; ++i)
	{
		vector_destruct(&attributes[i]);
	}
	checs_free(attributes);
}