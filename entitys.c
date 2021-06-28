 #include "ecs.h"


static uintEC nextEntityId; 
static Stack unusedEntitys;
static EntityId *tags;


void checs_entitys_init(uintEC const tag_count)
{
	stack_construct(&unusedEntitys, sizeof(EntityId), 256);
	tags = checs_malloc(tag_count);
}


void checs_entitys_terminate(void)
{
	checs_stats_log(printf("Total number of used entitys: %u\n", nextEntityId));
	stack_destruct(&unusedEntitys);
	checs_free(tags);
}


EntityId checs_entity_generate(ComponentKey const key)
{
	EntityId entity = stack_empty(&unusedEntitys) ? nextEntityId++ : stack_pop(&unusedEntitys, EntityId);
	checs_components_entity_add(entity, key);
	checs_systems_entity_register(entity, key);
	return entity;
}


void checs_entity_erase(EntityId const entity)
{
	stack_push(&unusedEntitys, EntityId, entity);
	checs_components_entity_remove(entity, keys[entity]);
	checs_systems_entity_erase(entity);
}


void checs_entity_tag_add(EntityId const entity, uintEC const tag)
{
	tags[tag] = entity;
}


EntityId checs_entity_get_by_tag(uintEC const tag)
{
	return tags[tag];
}
