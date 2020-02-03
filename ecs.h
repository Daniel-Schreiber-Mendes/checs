#ifndef ECS_H
#define ECS_H

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <checl/containers.h>


//#undef size_t
//#define size_t uint16_t


typedef enum
{
	UPDATE,
	DRAW	
}CallType;


typedef uint8_t ComponentKey; //8bits mean 8 components can be 
typedef uint16_t EntityId;
typedef uint8_t ComponentSignature; //the signature of a component. the signature depends on the order 
									//of registering but is only important inside the ecs, not for the user


typedef struct
{
	void* components;
	ComponentSignature componentSignature;
	uint8_t size;
	size_t componentSize; //size of component in bytes
}ComponentArray;


typedef struct
{
	ComponentArray* arrays;
	uint8_t size;
}ComponentTable;


typedef struct
{
	bool active;
	void(*callback)(ComponentTable*);
	ComponentTable componentTable;
}System;


typedef struct 
{
	bool active;
	void(*callback)(void);
}Task;

//#####################################################################################################entityManager.c


void     entityManager_init(EntityId const n_maxEntitys);
EntityId entityManager_entity_generate(void);
void     entityManager_entity_key_set(ComponentKey const key);
void     entityManager_entity_erase(EntityId const e);



//##################################################################################################componentManager.c

void 			componentManager_init(uint8_t const n_componentCount);
void    		_componentManager_component_register(ComponentSignature const componentSignature, size_t const componentSize);
ComponentArray* _componentManager_componentArray_get(ComponentSignature const componentSignature);

//gets size and name of component and passes it to create func
#define componentManager_component_register(ComponentType)\
	_componentManager_component_register((ComponentType##Component), sizeof(ComponentType));

#define componentManager_component_use(ComponentTable, ComponentType, name)\
	ComponentType *const name = ComponentTable->componentTable_componentArray_get(ComponentType##Component);


//###################################################################################################componentTable.c


void  componentTable_construct(void);
void* componentTable_componentArray_get(ComponentSignature const componentSignature);

#define componentTable_componentArray_foreach(array, ComponentType, element)\
	ComponentType *const = (ComponentType*)(array)->components[0];\
	for (uint8_t i=0; i < (array)->size; element = ((ComponentType*)(array)->components)[i + 1], ++i)


//########################################################################################################systemManager.c

void systemManager_init(uint8_t const n_systemUpdateCount, uint8_t const systemDrawCount, 
						uint8_t const n_taskUpdateCount, uint8_t const n_taskDrawCount);
void _systemManager_system_register(void(*callback)(ComponentTable*), CallType const callType);
void _systemManager_system_component_add(void(*callback)(ComponentTable*), ComponentSignature const signature, size_t const size);
void systemManager_systems_call(CallType const callType);
void systemManager_task_register(void(*callback)(void), CallType const callType);
void systemManager_tasks_call(CallType const callType);

#define systemManager_system_register(callback, CallType, ...)\
	_systemManager_system_register(callback, CallType);\
	BOOST_PP_SEQ_FOR_EACH(systemManager_system_component_add, callback, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
	//for each component that is passed call system_component_add with callback as its first argument and one 
	//element of __VA_ARGS__ as the second

#define systemManager_system_component_add(r, callback, ComponentType)\
	_systemManager_system_component_add(callback, BOOST_PP_CAT(ComponentType, Component), sizeof(ComponentType));
	//concatenate type name of the struct/component with `Component` which then gets replaced with 
	//the signature that was previously defined by the user
	//BOOST_PP_CAT is used instead of ## because ComponentType is an element of a BOOST_PP_SEQUENCE

//###########################################################################################################

#endif