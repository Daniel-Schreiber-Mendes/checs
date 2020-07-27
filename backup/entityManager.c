 #include "ecs.h"

//next entityId that will be given. It is NOT the total number of currently used Entitys
//because unused entitys are stored in the unused entitys stack
//everytime a new entity is to be created, the entitymanager looks if there is already an entity on the unusedEntitys stack
//and returns it. only if there are no unused entitys on the stack a completely new entity is used. Its basically Entity-Recycling
static uintEC nextEntityId; 
static Stack unusedEntitys;
static EntityId *tags;


void entityManager_init(uintEC const tag_count)
{
	stack_construct(&unusedEntitys, sizeof(EntityId), 16);
	tags = checs_malloc(tag_count);
}


void entityManager_terminate(void)
{
	checs_stats_log(printf("Total number of used entitys: %u\n", nextEntityId));
	stack_destruct(&unusedEntitys);
	checs_free(tags);
}


EntityId _entityManager_entity_generate(ComponentKey const key)
{
	checs_assert(key != 0);
	EntityId entity = stack_empty(&unusedEntitys) ? nextEntityId++ : stack_pop(&unusedEntitys, EntityId);
	componentManager_entity_register(entity, key);
	return entity;
}


void entityManager_entity_erase(EntityId const entity)
{
	checs_assert(keys[entity] != 0);
	stack_push(&unusedEntitys, EntityId, entity);
	componentManager_entity_erase(entity);
	systemManager_entity_erase(entity);
}


void entityManager_entity_tag_add(EntityId const entity, uintEC const tag)
{
	tags[tag] = entity;
}


EntityId entityManager_entity_get_by_tag(uintEC const tag)
{
	return tags[tag];
}