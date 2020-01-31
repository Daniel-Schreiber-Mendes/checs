 #include "ecs.h"


static EntityId entitys[MAX_ENTITYS];
static EntityId entityCounter = 0; //number of active entitys

void entityManager_init(void)
{

}


EntityId entityManager_entity_generate(void)
{
	return entityCounter++;
}


void entityManager_entity_key_set(ComponentKey const key)
{

}


void entityManager_entity_erase(EntityId const e)
{

}