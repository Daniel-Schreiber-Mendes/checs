#ifndef ECS_H
#define ECS_H

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <checl/containers.h>



//#define PositionComponent 1


#ifndef MAX_ENTITYS
	#define MAX_ENTITYS 1
#endif 

#ifndef COMPONENT_COUNT
	#define COMPONENT_COUNT 1
#endif 

#ifndef SYSTEM_UPDATE_COUNT //number of systems with update CallType
	#define SYSTEM_UPDATE_COUNT 1
#endif 

#ifndef SYSTEM_DRAW_COUNT //number of systems with update CallType
	#define SYSTEM_DRAW_COUNT 1
#endif 

#ifndef TASK_UPDATE_COUNT //number of tasks with update CallType
	#define TASK_UPDATE_COUNT 1
#endif

#ifndef TASK_DRAW_COUNT //number of tasks with draw CallType
	#define TASK_DRAW_COUNT 1
#endif


#define TASK_COUNT TASK_DRAW_COUNT + TASK_UPDATE_COUNT
#define SYSTEM_COUNT SYSTEM_DRAW_COUNT + SYSTEM_UPDATE_COUNT

typedef enum
{
	PRE_UPDATE,
	ON_UPDATE,
	POST_UPDATE,
	PRE_DRAW,
	ON_DRAW,
	POST_DRAW
}CallbackType;


typedef enum
{
	UPDATE,
	DRAW	
}CallType;


typedef uint64_t ComponentKey;
typedef uint64_t EntityId;
typedef uint8_t ComponentSignature; //the signature of a component. the signature depends on the order 
									//of registering but is only important inside the ecs, not for the use


typedef struct
{
	void* components;
	ComponentSignature componentSignature;
	uint16_t componentCount;
	size_t componentSize; //size of component in bytes
}ComponentArray;


typedef struct
{
	ComponentArray* componentArrays;
	uint8_t componentArrayCount;
}ComponentTable;


typedef struct
{
	bool active;
	CallType callType;
	void(*callback)(ComponentTable*);
	ComponentTable componentTable;
}System;


typedef struct 
{
	bool active;
	CallType callType;
	void(*callback)(void);
}Task;

//#####################################################################################################entityManager.c


void     entityManager_init(void);
EntityId entityManager_entity_generate(void);
void     entityManager_entity_key_set(ComponentKey const key);
void     entityManager_entity_erase(EntityId const e);



//##################################################################################################componentManager.c


void    _componentManager_component_register(ComponentSignature const componentSignature, size_t const componentSize);

//gets size and name of component and passes it to create func
#define componentManager_component_register(ComponentType)\
	_componentManager_component_register((ComponentType##Component), sizeof(ComponentType));

#define componentManager_component_use(ComponentTable, ComponentType, name)\
	ComponentType *const name = ComponentTable->componentTable_componentArray_get(ComponentType##Component);


//###################################################################################################componentTable.c


void  componentTable_construct(void);
void* componentTable_componentArray_get(ComponentSignature const componentSignature);


//########################################################################################################systemManager.c



void systemManager_system_register(void(*callback)(ComponentTable*), CallType const callType);
void _systemManager_system_component_add(void(*callback)(ComponentTable*));
void systemManager_systems_call(CallType const callType);

#define systemManager_system_component_add(callback, ComponentType)\
	_systemManager_system_component_add(callback, ComponentType##Component);
	

void systemManager_task_register(void(*callback)(void), CallType const callType);
void systemManager_tasks_call(CallType const callType);


//###########################################################################################################

#endif