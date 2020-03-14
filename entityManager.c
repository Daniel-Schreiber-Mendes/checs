 #include "ecs.h"

static Stack unusedEntitys;
static uintEC nextEntityId = 0; //next entityId that will be given. It is NOT the total number of currently used Entitys
//because unused entitys are stored in the unused entitys stack
//everytime a new entity is to be created, the entitymanager looks if there is already an entity on the unusedEntitys stack
//and returns it. only if there are no unused entitys on the stack a completely new entity is used. Its basically Entity-Recycling
static EntityId *tags;
static uintEC tagsCapacity = 0;


void entityManager_init(void)
{
	stack_construct(&unusedEntitys, sizeof(EntityId));
	stack_reserve(&unusedEntitys, 16);
	tags = checs_malloc(0);
}


void entityManager_terminate(void)
{
	checs_stats_log(printf("Maximum number of entitys: %u\n", nextEntityId + 1));
	stack_destruct(&unusedEntitys);
	checs_free(tags);
}


EntityId _entityManager_entity_generate(ComponentKey const key)
{
	EntityId entity = stack_isEmpty(&unusedEntitys) ? nextEntityId++ : stack_element_pop(&unusedEntitys, EntityId);
	systemManager_entity_register(entity, key);
	componentManager_entity_register(entity, key);
	return entity;
}


void entityManager_entity_erase(EntityId const entity)
{
	checs_assert(keys[entity] != 0);
	stack_element_push(&unusedEntitys, EntityId, entity);
	componentManager_entity_erase(entity);
	systemManager_entity_erase(entity);
}


void entityManager_entity_tag_add(EntityId const entity, uintEC const tag)
{
	tags = realloc(tags, sizeof(EntityId) * ++tagsCapacity);
	tags[tag] = entity;
}


EntityId entityManager_entity_get_by_tag(uintEC const tag)
{
	return tags[tag];
}
