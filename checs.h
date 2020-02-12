#ifndef CHECS
#define CHECS

#include "ecs.h"


//no functionality is allowed here. Only redefinitions and grouping of functions

#define checs_terminate()\
	entityManager_terminate();\
	systemManager_terminate();\
	componentManager_terminate();\
	commandManager_terminate();

#define checs_component_register(ComponentType, maxComponentsHint, maxComponentsDevnHint) componentManager_component_register(ComponentType, maxComponentsHint, maxComponentsDevnHint);
#define checs_entity_generate(...) entityManager_entity_generate(__VA_ARGS__)
#define checs_system_register(callback, CallType, maxEntitysHint, maxEntitysDevnHint, ...) systemManager_system_register(callback, CallType, maxEntitysHint, maxEntitysDevnHint, __VA_ARGS__)
#define checs_component_use(ComponentType, alias) componentManager_component_use(ComponentType, alias)
#define checs_component_get(ComponentType, alias, entity) componentManager_component_get(ComponentType, alias, entity)
#define checs_componentMatches_foreach(entity, smallestComponentTypeHint, ...) componentManager_componentMatches_foreach(entity, smallestComponentTypeHint, __VA_ARGS__)
#define checs_entity_foreach(entity) entityManager_foreach(entity)
#define checs_system_parameters EntityId *const entitys, uintEC const entityCount
#define checs_entity_erase(entity) entityManager_entity_erase(entity)

#define checs_command_subscribe(signature, callback) commandManager_command_subscribe(signature, callback)
#define checs_command_publish(CommandDataType, signature, data) commandManager_command_publish(CommandDataType, signature, data)

#endif