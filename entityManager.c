 #include "ecs.h"


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
	EntityId entity = stack_empty(&unusedEntitys) ? nextEntityId++ : stack_pop(&unusedEntitys, EntityId);
	componentManager_entity_components_add(entity, key);
	return entity;
}


void entityManager_entity_erase(EntityId const entity)
{
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
