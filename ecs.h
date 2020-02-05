#ifndef ECS_H
#define ECS_H

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/cat.hpp>
#include <checl/containers.h>


//if DEBUG is defined, print memory allocations
#define DEBUG


#define malloc_debug(size)\
({\
	printf("%lu bytes of memory were allocated in line %i in file %s\n", size, __LINE__, __FILE__ );\
	void* retVal = malloc((size));\
	retVal;\
})
//because the newline only is printed if memory was allocated successfully, one can easyly see in the terminal if thee allocation failed

#define free_debug(p)\
	printf("Memory was freed in line %i in file %s\n", __LINE__, __FILE__);\
	free((p));


#ifndef DEBUG
	#undef free_debug
	#undef malloc_debug
	#define free_debug(p) free(p)
	#define malloc_debug(size) malloc(size)
#endif



#ifdef size_t
	#undef size_t
#endif				//redefine size_t because it is by standard 64 bits in size which is way too big for any data to ever be
#define size_t uint16_t



typedef uint8_t ComponentKey; //8bits mean 8 components can be indicated
typedef uint16_t EntityId; //unique identifier for an instanciated entity
typedef uint16_t ComponentId; //unique identifier for a instanciated component
typedef uint16_t Id; //Type for storing EntityId's or ComponentId's
typedef uint16_t uintEC; //any number that stores values that goes from 0 to the maximum value EntityId can hold
typedef uint8_t  uintST; //any number that goes from 0 to the maximum number of systems/tasks
typedef uint8_t ComponentSignature;//the signature of a ComponentType which depends on the order of registering
typedef uint8_t uintCS; //stores any number that goes from 0 to the maximum number of ComponentSignatures
typedef void(*SystemCallback)(EntityId *const entitys, uintEC const size);
typedef void(*TaskCallback)(void);

typedef enum
{
	UPDATE,
	DRAW	
}CallType; //specifies when the system/task is called

typedef enum
{
	OWN //means a system owns a ComponentType, so they are stored and managed by the systems themselves
}SystemType;

typedef struct
{
	EntityId* sparse; //sparse packed array of indices to dense array
	void* dense; //dense array of components
	ComponentSignature signature; //signature of components that are stored
	size_t componentSize; //size of components in bytes
	uintEC capacity; //maximum number of elements
	uintEC size; //current number of elements;
}SparseSet;


typedef struct
{
	bool active;
	SystemCallback callback;
	Vector entitys; //array of entitys it is interested in
	uintEC entityCount;
	ComponentKey key; //key shows which components at least an entity has to have to be processed by this system
}System;


typedef struct 
{
	bool active;
	TaskCallback callback;
}Task;


#define getBitCount(type) sizeof(type) * 8
#define keyMatch(requiredKey, providedKey) ({((requiredKey) & (providedKey)) == (requiredKey);})


void     entityManager_init(void);
void 	 entityManager_terminate(void);
EntityId _entityManager_entity_generate(ComponentKey const key);
void     entityManager_entity_key_set(ComponentKey const key);
void     entityManager_entity_erase(EntityId const e);

//the signatures come in as numbers like 1, 2, 3 and so forth. to create a componentKey, which could look like e.g. this 0101 0110
//we have to or 0000 0000 with the bits that we want to set. 
//for example: Say a component has the signature 3. this means the third bit(counting from the right to left) has to be set.
//when we shift the number 1 three to the right, we get 0000 0100. Now we or the current key(because we have multiple signatures
//that we want to set) we get the appropriate componentKey
#define entityManager_entity_generate(...)\
	({\
	ComponentKey key = 0;\
	BOOST_PP_SEQ_FOR_EACH(evaluateComponentKey, &key, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	_entityManager_entity_generate(key);\
	})

#define evaluateComponentKey(r, key, ComponentType)\
	*(key) |= 1 << BOOST_PP_CAT(ComponentType, Component);
	//before we begin the shifting we first have to get the signature of the component by concatenating it. not with ## but with cat 
	//because this has to be done when it is a element of a BOOST_PP_SEQUENCE

#define entityManager_foreach(entity)\
	for(uintEC i=0, entity=entitys[0]; i < entityCount; entity = entitys[i], ++i)
//this will only be called inside a callback. 
//The Signature of a callback is always void foo(EntityId *const entitys, uintEC const entityCount);
//this means one doesnt have to give the data as an argument since their name is already know.
//entity is only the alias that is going to be used for the entityId's inside the array

void componentManager_init(uintCS const n_componentCount);
void componentManager_terminate(void);
void _componentManager_component_register(ComponentSignature const signature, size_t const componentSize);
SparseSet* componentManager_sparseSet_get(ComponentSignature const signature);

//gets size and name of component and passes it to create func
#define componentManager_component_register(ComponentType)\
	_componentManager_component_register((ComponentType##Component), sizeof(ComponentType));

//@alias is the alias that is going to be used for the component, like for example pos, or vel
#define componentManager_component_use(ComponentType, alias)\
	SparseSet* ComponentType##SparseSet = componentManager_sparseSet_get(ComponentType##Component); \
	ComponentType* alias;
	//create vairable that can be used by component_get
	//get the sparse array which is going to be indexed for this ComponentType

#define componentManager_component_get(ComponentType, alias, entity)\
	alias = &((ComponentType*)ComponentType##SparseSet->dense)[ComponentType##SparseSet->sparse[(entity)]];\
	//updating the value of the alias for a member of a component
	//	printf("%p\n", ComponentType##SparseSet->dense);

void systemManager_init(uintST const n_systemUpdateCount, uintST const systemDrawCount, 
						uintST const n_taskUpdateCount, uintST const n_taskDrawCount);
void systemManager_terminate(void);
void _systemManager_system_register(SystemCallback callback, CallType const callType);
void _systemManager_system_component_add(SystemCallback callback, ComponentSignature const signature);
void systemManager_systems_call(CallType const callType);
void systemManager_systems_entity_add(EntityId const entity, ComponentKey const key);
void systemManager_task_register(TaskCallback const callback, CallType const callType);
void systemManager_tasks_call(CallType const callType);

#define systemManager_system_register(callback, CallType, ...)\
	_systemManager_system_register(callback, CallType);\
	BOOST_PP_SEQ_FOR_EACH(systemManager_system_component_add, callback, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
	//for each component that is passed call system_component_add with callback as its first argument and one 
	//element of __VA_ARGS__ as the second

#define systemManager_system_component_add(r, callback, ComponentType)\
	_systemManager_system_component_add(callback, BOOST_PP_CAT(ComponentType, Component));
	//concatenate type name of the struct/component with `Component` which then gets replaced with 
	//the signature that was previously defined by the user
	//BOOST_PP_CAT is used instead of ## because ComponentType is an element of a BOOST_PP_SEQUENCE





#endif