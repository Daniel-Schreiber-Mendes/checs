#ifndef CHECS_H
#define CHECS_H
#include "ecs.h"

/*no functionality is allowed here. Only redefinitions and grouping of functions */

#define checs_terminate()\
	entityManager_terminate();\
	systemManager_terminate();\
	componentManager_terminate();\
	commandManager_terminate();\
	eventManager_terminate();


#define checs_init(systemUpdateCount, systemDrawCount, taskUpdateCount, taskDrawCount, tag_count, componentCount, maxEntitysHint, commandCount, eventCount)\
	systemManager_init(systemUpdateCount, systemDrawCount, taskUpdateCount, taskDrawCount);\
	entityManager_init(tag_count);\
	componentManager_init(componentCount, maxEntitysHint);\
	commandManager_init(commandCount);\
	eventManager_init(eventCount);


#define checs_component_register(ComponentType, maxComponentsHint, component_destructor, component_constructor) componentManager_component_register(ComponentType, maxComponentsHint, component_destructor, component_constructor);
#define checs_component_mut_use(ComponentType, alias) componentManager_component_mut_use(ComponentType, alias)
#define checs_component_use(ComponentType, alias) componentManager_component_use(ComponentType, alias)
#define checs_component_get(ComponentType, alias, entity) componentManager_component_get(ComponentType, alias, entity)
#define checs_component_get_once(ComponentType, alias, entity) componentManager_component_get_once(ComponentType, alias, entity)
#define checs_components_foreach(ComponentType, alias, entityAlias) componentManager_components_foreach(ComponentType, alias, entityAlias)
#define checs_componentMatches_foreach(entity, smallestComponentTypeHint, ...) componentManager_componentMatches_foreach(entity, smallestComponentTypeHint, __VA_ARGS__)
#define checs_component_entity_foreach(Type, entity) componentManager_component_entity_foreach(Type, entity)
#define checs_entity_has_component(Type, entity) componentManager_entity_has_component(Type, entity)

#define checs_system_parameters EntityId *const entitys, uintEC const entityCount
#define checs_system_register(callback, CallType, maxEntitysHint, maxEntitysDevnHint, ...) systemManager_system_register(callback, CallType, maxEntitysHint, maxEntitysDevnHint, __VA_ARGS__)
#define checs_update() systemManager_update()
#define checs_draw() systemManager_draw()


#define checs_task_register(callback, callType) systemManager_task_register(callback, callType)


#define checs_entity_erase(entity) entityManager_entity_erase(entity)
#define checs_entity_foreach(entity) entityManager_foreach(entity)
#define checs_entity_generate(...) entityManager_entity_generate(__VA_ARGS__)
#define checs_entity_tag_add(entity, tag) entityManager_entity_tag_add(entity, tag)
#define checs_entity_get_by_tag(tag) entityManager_entity_get_by_tag(tag)


#define checs_command_subscribe(signature, callback) commandManager_command_subscribe(signature, callback)
#define checs_command_publish(signature, data) commandManager_command_publish(signature, (data))
#define checs_command_parameters void *const data
/* this need to be inside the function head */


#define checs_event_register(EventDataType, signature, maxEventsHint) eventManager_event_register(EventDataType, signature, maxEventsHint)
#define checs_event_publish(EventDataType, signature, data) eventManager_event_publish(EventDataType, signature, data)
#define checs_events_poll(EventDataType, signature, alias) eventManager_events_poll(EventDataType, signature, alias)
#define checs_eventBuffers_swap() eventManager_buffers_swap()

#endif