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
	attributeManager_terminate();\
	templateManager_terminate();


#define checs_init(systemUpdateCount, systemDrawCount, taskUpdateCount, taskDrawCount, tag_count, componentCount, commandCount, eventCount, maxAttributesHint, templateCount)\
	systemManager_init(systemUpdateCount, systemDrawCount, taskUpdateCount, taskDrawCount);\
	entityManager_init(tag_count);\
	componentManager_init(componentCount);\
	commandManager_init(commandCount);\
	eventManager_init(eventCount);\
	attributeManager_init(maxAttributesHint);\
	templateManager_init(templateCount);


#define checs_system_parameters EntityId *const entitys, uintEC const entityCount
#define checs_update() systemManager_update()
#define checs_draw() systemManager_draw()

#define checs_entity_erase(entity) entityManager_entity_erase(entity)
#define checs_entity_tag_add(entity, tag) entityManager_entity_tag_add(entity, tag)
#define checs_entity_get_by_tag(tag) entityManager_entity_get_by_tag(tag)

#define checs_command_register(Type, sig, callbackCount) commandManager_command_register(sig, callbackCount);
#define checs_command_publish(Type, sig, data) commandManager_command_publish(sig, data);
#define checs_command_subscribe(Type, sig, callback) commandManager_command_subscribe(sig, callback);

#define checs_eventBuffers_swap() eventManager_buffers_swap()


#define checs_template_register(sig, cb) templateManager_template_register(sig, cb)
#define checs_entity_templates_add(e, tkey) templateManager_entity_templates_add(e, tkey)

#endif