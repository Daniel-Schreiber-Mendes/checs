#ifndef CHECS_H
#define CHECS_H
#include "ecs.h"

/*no functionality is allowed here. Only renaming and grouping of functions */

#define checs_terminate()\
	entityManager_terminate();\
	systemManager_terminate();\
	componentManager_terminate();\
	commandManager_terminate();\
	eventManager_terminate();\
	attributeManager_terminate();


#define checs_init(systemUpdateCount, systemDrawCount, taskUpdateCount, taskDrawCount, tag_count, componentCount, maxEntitysHint, commandCount, eventCount, maxAttributesHint)\
	systemManager_init(systemUpdateCount, systemDrawCount, taskUpdateCount, taskDrawCount);\
	entityManager_init(tag_count);\
	componentManager_init(componentCount, maxEntitysHint);\
	commandManager_init(commandCount);\
	eventManager_init(eventCount);\
	attributeManager_init(maxAttributesHint);


#define checs_system_parameters EntityId *const entitys, uintEC const entityCount
#define checs_update() systemManager_update()
#define checs_draw() systemManager_draw()

#define checs_task_register(callback, callType) systemManager_task_register(callback, callType)

#define checs_entity_erase(entity) entityManager_entity_erase(entity)
#define checs_entity_tag_add(entity, tag) entityManager_entity_tag_add(entity, tag)
#define checs_entity_get_by_tag(tag) entityManager_entity_get_by_tag(tag)

#define checs_command_parameters void *const data

#define checs_eventBuffers_swap() eventManager_buffers_swap()

#endif