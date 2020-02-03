 #include "ecs.h"

static EntityId maxEntitys;
static EntityId* entitys;
static EntityId entityCounter = 0; //number of active entitys

void entityManager_init(EntityId const n_maxEntitys)
{
	maxEntitys = n_maxEntitys;
	entitys = (EntityId*)malloc(sizeof(EntityId) * n_maxEntitys);
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