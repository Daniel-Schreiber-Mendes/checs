#ifndef CHECS
#define CHECS

#include "ecs.h"

#define checs_init(systemUpdateCount, systemDrawCount, taskUpdateCount, taskDrawCount, componentCount)\
	systemManager_init(systemUpdateCount, systemDrawCount, taskUpdateCount, taskDrawCount);\
	entityManager_init();\
	componentManager_init(componentCount);

#define checs_terminate()\
	entityManager_terminate();\
	systemManager_terminate();\
	componentManager_terminate();

#define checs_entity_generate(...) entityManager_entity_generate(__VA_ARGS__);
#define checs_component_register(ComponentType) componentManager_component_register(ComponentType);
#define checs_system_register(callback, CallType, ...) systemManager_system_register(callback, CallType, __VA_ARGS__);

#endif