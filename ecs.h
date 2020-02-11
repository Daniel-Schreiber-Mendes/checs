#ifndef ECS_H
#define ECS_H

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/cat.hpp>
#include <checl/containers.h>


//if DEBUG is defined, print memory allocations
#define DEBUG

//changes font color to blue, print message, reset font color
#define malloc_debug(size)\
({\
	printf("\033[0;34m");\
	printf("bytes: %.4u | allocated   | line: %.3i | file: %s\n", (uint16_t)size, __LINE__, __FILE__ );\
	printf("\033[0m");\
	void* retVal = malloc((size));\
	retVal;\
})


#define calloc_debug(num, size)\
({\
	printf("\033[0;34m");\
	printf("bytes: %.4u | allocated   | line: %.3i | file: %s\n", (uint16_t)size, __LINE__, __FILE__ );\
	printf("\033[0m");\
	void* retVal = calloc((num), (size));\
	retVal;\
})

//changes font color to green, print message, reset font color
#define free_debug(p)\
({\
	printf("\033[0;32m");\
	printf("	      deallocated | line: %.3i | file: %s\n", __LINE__, __FILE__);\
	printf("\033[0m");\
	free((p));\
})


#ifndef DEBUG
	#undef free_debug
	#undef malloc_debug
	#undef calloc_debug
	#define free_debug(p) free(p)
	#define malloc_debug(size) malloc(size)
	#define calloc_debug(num, size) calloc(num, size)
#endif



#ifdef size_t
	#undef size_t
#endif				//redefine size_t because it is by standard 64 bits in size which is way too big for any data to ever be
#define size_t uint16_t




//the signatures come in as numbers like 1, 2, 3 and so forth. to create a componentKey, which could look like e.g. this 0101 0110
//we have to or 0000 0000 with the bits that we want to set. 
//for example: Say a component has the signature 3. this means the third bit(counting from the right to left) has to be set.
//when we shift the number 1 three to the right, we get 0000 0100. Now we or the current key(because we have multiple signatures
//that we want to set) we get the appropriate componentKey


typedef uint8_t ComponentKey; //8bits mean 8 components can be indicated
typedef uint16_t EntityId; //unique identifier for an instanciated entity
typedef uint16_t ComponentId; //unique identifier for a instanciated component
typedef uint16_t uintEC; //any number that stores values that goes from 0 to the maximum value EntityId can hold
typedef uint8_t  uintST; //any number that goes from 0 to the maximum number of systems/tasks
typedef uint8_t ComponentSignature;//the signature of a ComponentType which depends on the order of registering
typedef uint8_t uintCS; //stores any number that goes from 0 to the maximum number of ComponentSignatures
typedef uint16_t EventSignature; //hashesh name of EventType
typedef void(*SystemCallback)(EntityId *const entitys, uintEC const size);
typedef void(*TaskCallback)(void);

typedef enum
{
	UPDATE,
	DRAW	
}CallType; //specifies when the system/task is called


typedef struct
{
	uintEC* sparse; //sparse packed array of indices to dense array
	uintEC sparseCapacity; //maximum number of elements

	uintEC* dense; //dense array of components
	uintEC denseCapacity; //maximum number of elements
	uintEC denseSize; //current number of elements;

	uintEC maxComponentsDevnHint;

	void* components;
	ComponentSignature signature; //signature of components that are stored
	size_t componentSize; //size of component
}SparseSet;


typedef struct
{
	uintEC* sparse; //sparse packed array of indices to dense array
	uintEC sparseCapacity; //maximum number of elements

	uintEC* dense; //dense array of entitys
	uintEC denseCapacity; //maximum number of elements
	uintEC denseSize; //current number of elements;

	uintEC maxEntitysDevnHint;

	bool active;
	SystemCallback callback;	
	ComponentKey key; //key shows which components at least an entity has to have to be processed by this system
}System;
//it is perfectly legal to create or erase an entity inside a system call. but there is no guarantee that the entity will be 
//accessable during this call. because of this the components of an entity that just got created should not be modified during it


typedef struct 
{
	bool active;
	TaskCallback callback;
}Task;


typedef struct
{
	void* data; //pointer to struct which contains the actual informations
	EventSignature signature;
}Event;



//TODO: 
// - Giving hints to systems about the number of components and entitys that are going to be used
// - Telling the ecs to print out the number of entitys each system and sparseSet has at its maximum 
//   and how many entitys were registered. These values can in the next run be fed back into the system for minimal memory allocationsd
// 

#define getBitCount(type) sizeof(type) * 8
#define key_match(requiredKey, providedKey) ({((requiredKey) & (providedKey)) == (requiredKey);})
#define key_set(key, index) (key |= 1 << index)

//yeah global variables are bad but in this case calling a getter function everytime would be a big performance hit
extern SparseSet* sets; //in componentManager
extern ComponentKey* keys; //in componentManager


void      entityManager_init(void);
void 	  entityManager_terminate(void);
EntityId _entityManager_entity_generate(ComponentKey const key);
void      entityManager_entity_erase(EntityId const e);

#define   entityManager_entity_generate(...)\
	({_entityManager_entity_generate(components_convertToKey(__VA_ARGS__));})

#define   entityManager_foreach(entity) \
	for(uintEC i=0, entity=entitys[0]; i < entityCount; entity = entitys[++i])
//this will only be called inside a callback. 
//The Signature of a callback is always void foo(EntityId *const entitys, uintEC const entityCount);
//this means one doesnt have to give the data as an argument since their name is already know.
//entity is only the alias that is going to be used for the entityId's inside the array

void 		 componentManager_init(uintCS const n_componentCount, uintEC const maxEntitysHint, uintEC const n_maxEntitysDevnHint);
void 		 componentManager_terminate(void);
void 		_componentManager_component_register(ComponentSignature const signature, size_t const componentSize, 
												 uintEC const maxComponentsHint, uintEC const maxComponentsDevnHint);
void		 componentManager_entity_register(EntityId const entity, ComponentKey const key);
void         componentManager_entity_erase(EntityId const entity);
void 		_componentManager_entity_components_add(EntityId const entity, ComponentKey const key);

#define 	 componentManager_component_register(ComponentType, maxComponentsHint, maxComponentsDevnHint)\
	_componentManager_component_register(ComponentType##Component, sizeof(ComponentType), maxComponentsHint, maxComponentsDevnHint);

//@alias is the alias that is going to be used for the component, like for example pos, or vel
#define 	 componentManager_component_use(ComponentType, alias)\
	ComponentType* alias;
	//create vairable that can be used by component_get
	//get the sparse array which is going to be indexed for this ComponentType
	//sets is global to avoid calling a simple get() function everytime which decreases performance

#define componentManager_entity_components_add(entity, ...)\
	_componentManager_entity_components_add(entity, components_convertToKey(__VA_ARGS__))

#define 	 componentManager_component_get(ComponentType, alias, entity)\
	alias = &((ComponentType*)sets[ComponentType##Component].components)[sets[ComponentType##Component].sparse[entity]];
	//updating the value of the alias for a member of a component

#define 	 componentManager_componentMatches_foreach(entity, smallestComponentTypeHint, ...)\
	for(uintEC i=0, entity=sets[smallestComponentTypeHint##Component].dense[i], key=keys[entity]; i < sets[smallestComponentTypeHint##Component].denseSize; ++i, key = keys[++entity])\
		if(key_match(components_convertToKey(__VA_ARGS__), key))
//entity is the alias that is going to be used for the next entity in the array that matches the key
//iterates over all entitys in the sparseSet with the smallest size. it then looks up the key of the entity in the keys[] array.
//if the found key matches the required key, the code in the brackets after the if statement(the brackets and whats inside is written 
//by the user) is executed. For example: In the explode-system: a bomb explodes and every entity around it
//should take damage. creating a system for this would be bad, because its callback is not called that often. So we just fetch all
//entitys everytime we need this functionality. this results also in a much smaller memory use
//smallestComponentTypeHint is the component specified by the user which he thinks has the smallest number of elements. For each
//element in the dense array of the sparseSet with the smallest number of components, lookup its key and see if it matches the 
//required one. This makes iterating pretty fast.

#define   key_evaluate(r, key, ComponentType)\
	*(key) |= 1 << BOOST_PP_CAT(ComponentType, Component);
	//@ComponentType is the name of the component, that should be converterted to a key and then added to the key-pointer, 
	//given as parameter
	//before we begin the shifting we first have to get the signature of the component by concatenating it. not with ## but with cat 
	//because this has to be done when it is a element of a BOOST_PP_SEQUENCE

#define components_convertToKey(...)\
	({\
		ComponentKey key = 0;\
		BOOST_PP_SEQ_FOR_EACH(key_evaluate, &key, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
		key;\
	})


void    systemManager_init(uintST const n_systemUpdateCount, uintST const systemDrawCount, 
						uintST const n_taskUpdateCount, uintST const taskDrawCount);
void    systemManager_terminate(void);
void   _systemManager_system_register(SystemCallback callback, CallType const callType, ComponentKey const key, uintEC const maxEntitysHint, uintEC const maxEntitysDevnHint);
void    systemManager_systems_call(CallType const callType);
void    systemManager_entity_register(EntityId const entity, ComponentKey const key);
void    systemManager_entity_erase(EntityId const entity);
void    systemManager_task_register(TaskCallback const callback, CallType const callType);
void    systemManager_tasks_call(CallType const callType);
void    systemManager_entity_components_added(EntityId const entity, ComponentKey const key);

#define systemManager_system_register(callback, CallType, maxEntitysHint, maxEntitysDevnHint, ...)\
	_systemManager_system_register(callback, CallType, components_convertToKey(__VA_ARGS__), maxEntitysHint, maxEntitysDevnHint);


void sparseSet_construct(SparseSet* set, size_t const componentSize, ComponentSignature const signature, 
						 uintEC const maxComponentsHint, uintEC const maxComponentsDevnHint);
void sparseSet_destruct(SparseSet const *const set);
void sparseSet_entity_add(SparseSet *const set, EntityId const entity);
void sparseSet_entity_remove(SparseSet *const set, EntityId const entity);


void system_construct(System *const sys, SystemCallback callback, ComponentKey const key, uintEC const maxEntitysHint, uintEC const maxEntitysDevnHint);
void system_destruct(System const *const sys);
void system_entity_add(System *const sys, EntityId const entity);
void system_entity_remove(System *const sys, EntityId const entity);

void _event_send(EventSignature const signature, void const *const data);

#define event_send(EventType, data)\
	void* _data = malloc(sizeof(EventType));\
	*_data = (EventType)data;\
	_event_send(hash(EventType), _data);




#endif