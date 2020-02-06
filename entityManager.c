 #include "ecs.h"

static Stack unusedEntitys;
static uintEC nextEntityId = 0; //next entityId that will be given. It is NOT the total number of currently used Entitys
//because unused entitys are stored in the unused entitys stack
//everytime a new entity is to be created, the entitymanager looks if there is already an entity on the unusedEntitys stack
//and returns it. only if there are no unused entitys on the stack a completely new entity is used. Its basically Entity-Recycling

void entityManager_init(void)
{
	stack_construct(&unusedEntitys, sizeof(EntityId));
	stack_reserve(&unusedEntitys, 16);
}


void entityManager_terminate(void)
{
	stack_destruct(&unusedEntitys);
}


EntityId _entityManager_entity_generate(ComponentKey const key)
{
	EntityId entity = stack_isEmpty(&unusedEntitys) ? nextEntityId++ : stack_element_pop(&unusedEntitys, EntityId);
	systemManager_entity_register(entity, key);
	componentManager_entity_register(entity, key);
	return entity;
}


void entityManager_entity_key_set(ComponentKey const key)
{

}


void entityManager_entity_erase(EntityId const e)
{
	stack_element_push(&unusedEntitys, EntityId, e);
	//TODO: removing them from systems and sparseSets 
}
