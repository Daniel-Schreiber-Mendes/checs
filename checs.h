#ifndef CHECS
#define CHECS

#include "ecs.h"

#define NO_SYSTEM_HINT 64
#define NO_SYSTEM_DEVN_HINT 8


#define checs_init(systemUpdateCount, systemDrawCount, taskUpdateCount, taskDrawCount, componentCount, maxEntitysHint, maxEntitysDevnHint)\
	systemManager_init(systemUpdateCount, systemDrawCount, taskUpdateCount, taskDrawCount);\
	entityManager_init(maxEntitysHint, maxEntitysDevnHint);\
	componentManager_init(componentCount);

#define checs_terminate()\
	entityManager_terminate();\
	systemManager_terminate();\
	componentManager_terminate();

#define checs_entity_generate(...) entityManager_entity_generate(__VA_ARGS__)
#define checs_components_register(...) componentManager_components_register(__VA_ARGS__)
#define checs_system_register(callback, CallType, maxEntitysHint, maxEntitysDevnHint, ...) systemManager_system_register(callback, CallType, maxEntitysHint, maxEntitysDevnHint, __VA_ARGS__)
#define checs_component_use(ComponentType, alias) componentManager_component_use(ComponentType, alias)
#define checs_component_get(ComponentType, alias, entity) componentManager_component_get(ComponentType, alias, entity)
#define checs_entity_foreach(entity) entityManager_foreach(entity)
#define checs_system_parameters EntityId *const entitys, uintEC const entityCount

#endif