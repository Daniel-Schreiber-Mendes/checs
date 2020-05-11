#ifndef ECS_H
#define ECS_H
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <checl/containers.h>
#include <stdlib.h>


#define CHECS_STATS
//#define CHECS_MEMLOG
#define CHECS_ASSERT

#ifdef CHECS_MEMLOG
	#define checs_malloc(size)\
	({\
		void* retVal = malloc((size));\
		printf("\033[0;34m");\
		printf("Checs-Allocation:   bytes: %.4u | line: %.3i | file: %s\n", (uint16_t)size, __LINE__, __FILE__ );\
		printf("\033[0m");\
		retVal;\
	})/*changes font color to blue, print message, reset font color*/


	#define checs_calloc(num, size)\
	({\
		void* retVal = calloc((num), (size));\
		printf("\033[0;34m");\
		printf("Checs-Allocation:   bytes: %.4u | line: %.3i | file: %s\n", (uint16_t)size, __LINE__, __FILE__ );\
		printf("\033[0m");\
		retVal;\
	})

	//changes font color to green, print message, reset font color
	#define checs_free(p)\
	({\
		free((p));\
		printf("\033[0;32m");\
		printf("Checs-Deallocation:               line: %.3i | file: %s\n", __LINE__, __FILE__);\
		printf("\033[0m");\
	})
#else
	#define checs_free(p) free(p)
	#define checs_malloc(size) malloc(size)
	#define checs_calloc(num, size) calloc(num, size)
#endif

#ifdef CHECS_ASSERT
	#define checs_assert(expr)\
		if (!(expr))\
		{\
			printf("Checs-Assertion: %s failed. Line: %u, File: %s\n", #expr, __LINE__, __FILE__);\
			exit(-1);\
		}

	//checks if it is allowed to acces the entity
	#define checs_entity_assert(Type, entity)\
		{\
			EntityId e = entity;\
			checs_assert(e < getSparseSet(Type)->sparseCapacity);\
			checs_assert(getSparseSet(Type)->sparse[e] < getSparseSet(Type)->denseCapacity);\
		}
#else
	#define checs_assert(expr) (void)0
	#define checs_entity_assert(Type, entity) (void)0
#endif

#ifdef CHECS_STATS
	#define checs_stats_log(expr) expr
#else
	#define checs_stats_log(expr) (void)0
#endif

//the signatures come in as numbers like 1, 2, 3 and so forth. to create a componentKey, which could look like e.g. this 0101 0110
//we have to or 0000 0000 with the bits that we want to set. 
//for example: Say a component has the signature 3. this means the third bit(counting from the right to left) has to be set.
//when we shift the number 1 three to the right, we get 0000 0100. Now we or the current key(because we have multiple signatures
//that we want to set) we get the appropriate componentKey


typedef uint16_t ComponentKey; //8bits mean 8 components can be indicated. this is a key which corresponds to an entity
typedef uint16_t EntityId; //unique identifier for an instanciated entity
typedef uint16_t ComponentId; //unique identifier for a instanciated component
typedef uint16_t uintEC; //any number that stores values that goes from 0 to the maximum value EntityId can hold
typedef uint8_t uintST; //any number that goes from 0 to the maximum number of systems/tasks
typedef uint16_t ComponentSignature;//the signature of a ComponentType which depends on the order of registering this is the hashed value of the name of the component
typedef uint8_t ComponentKeyIndex; //this is the place of the bit that indicates that an entity has this component
typedef uint8_t uintCS; //stores any number that goes from 0 to the maximum number of ComponentSignatures
typedef uint8_t EventSignature; 
typedef uint8_t CommandSignature;
typedef uint8_t uintC; //stores any number that goes from 0 to the maximum number of commands
typedef uint8_t uintE; //stores any number that goes from 0 to the maximum number of eventtypes
typedef void(*SystemCallback)(EntityId *const entitys, uintEC const size);
typedef void(*TaskCallback)(void);
typedef void(*CommandCallback)(void*);

typedef enum
{
	CHECS_ON_UPDATE,
	CHECS_ON_DRAW	
}
CallType; //specifies when the system/task is called


typedef struct
{
	uintEC* sparse; //sparse packed array of indices to dense array
	uintEC sparseCapacity; //maximum number of elements

	uintEC* dense; //dense array of entityId's
	uintEC denseCapacity; //maximum number of elements
	uintEC denseSize; //current number of elements;

	void* components;
	ComponentKeyIndex cki; //signature of components that are stored
	size_t componentSize; //size of component
	void(*component_destructor)(void *const);
	void(*component_constructor)(void *const);
}
SparseSet;


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
}
System;
/*it is perfectly legal to create or erase an entity inside a system call. but there is no guarantee that the entity will be 
accessable during this call. because of this the components of an entity that just got created should not be modified during it*/


typedef struct 
{
	bool active;
	TaskCallback callback;
}
Task;


/*TODO:  
 -creating a seperate memory pool for the values that are passed to the commands
 */

#define getBitCount(type) sizeof(type) * 8
#define key_match(requiredKey, providedKey) ({((requiredKey) & (providedKey)) == (requiredKey);})
#define key_set(key, index) (key |= 1 << index)

/*yeah global variables are bad but in this case calling a getter function everytime would be a big performance hit*/
extern HashMap sets; /*in componentManager*/
extern ComponentKey* keys; /*in componentManager*/
extern void **events_db[2];
extern uint8_t *eventCounts_db[2];
extern uint8_t db_index; 
extern uint8_t *eventCapacitys;


void      entityManager_init(uintEC const tag_count);
void 	  entityManager_terminate(void);
EntityId _entityManager_entity_generate(ComponentKey const key);
void      entityManager_entity_erase(EntityId const e);
void      entitymanager_entity_tag_add(EntityId const entity, uintEC const tag);
EntityId  entityManager_entity_get_by_tag(uintEC const tag);

#define   entityManager_entity_generate(...)\
	({  _entityManager_entity_generate(components_convertToKey(__VA_ARGS__)); })

#define   entityManager_foreach(entity)\
	for (uintEC entity##i=0, entity=entitys[0]; entity##i < entityCount; entity = entitys[++entity##i])
/*this will only be called inside a system. 
The Signature of a callback is always void foo(EntityId *const entitys, uintEC const entityCount);
this means one doesnt have to give the data as an argument since their name is already know.
entity is only the alias that is going to be used for the entityId's inside the array. We concatenate the counter variable i with the alias of the entitys to prevent 
name collision if the loop is used nested*/

void 	componentManager_init(uintCS const n_componentCount, uintEC const maxEntitysHint);
void 	componentManager_terminate(void);
void   _componentManager_component_register(ComponentSignature const sig, size_t const componentSize, uintEC const maxComponentsHint, void(*component_destructor)(void *const), void(*component_constructor)(void*const));
void	componentManager_entity_register(EntityId const entity, ComponentKey const key);
void    componentManager_entity_erase(EntityId const entity);
void   _componentManager_entity_components_add(EntityId const entity, ComponentKey const key);

//macro to make the code shorter and more expressive
#define getSparseSet(Type) hashMap_get(&sets, SparseSet, hashMap_hash(&sets, Type))

#define componentManager_component_register(Type, maxComponentsHint, component_destructor, component_constructor)\
	_componentManager_component_register(hashMap_hash(&sets, Type), sizeof(Type), maxComponentsHint, component_destructor, component_constructor);

/*@alias is the alias that is going to be used for the component, like for example pos, or vel*/
#define componentManager_component_mut_use(Type, alias) Type *alias
	/*create vairable that can be used by component_get
	get the sparse array which is going to be indexed for this ComponentType
	sets is global to avoid calling a simple get() function everytime which decreases performance*/

#define componentManager_component_use(Type, alias) Type const *alias //by using this one can not change the components, only get its values. this means by default
	//one can not modify a component, unless you exolicitely use the mut version of the macro


#define componentManager_entity_components_add(entity, ...)\
	_componentManager_entity_components_add(entity, components_convertToKey(__VA_ARGS__))

#define componentManager_component_get(Type, alias, entity)\
	checs_entity_assert(Type, entity);\
	alias = &((Type*)getSparseSet(Type)->components)[getSparseSet(Type)->sparse[entity]];
	/*updating the value of the alias for a member of a component*/

#define componentManager_components_foreach(Type, alias, entityAlias)\
	alias = &((Type*)getSparseSet(Type)->components)[0];\
	uintEC entityAlias = getSparseSet(Type)->dense[0];\
	for (uintEC indx=0; indx < getSparseSet(Type)->denseSize; entityAlias = getSparseSet(Type)->dense[++indx], alias = &((Type*)getSparseSet(Type)->components)[indx])

#define componentManager_component_get_once(Type, alias, entity)\
	checs_entity_assert(Type, entity);\
	Type *const alias = &(((Type*)(getSparseSet(Type)->components))[getSparseSet(Type)->sparse[entity]]);

#define componentManager_componentMatches_foreach(entityAlias, smallestTypeHint, ...)\
	for (uintEC i=0, entityAlias=getSparseSet(smallestTypeHint)->dense[i], key=keys[entityAlias]; i < getSparseSet(smallestTypeHint)->denseSize; ++i, key = keys[++entityAlias])\
		if (key_match(components_convertToKey(__VA_ARGS__), key))

//iterates over all entitys inside the sparseset of an component without getting components
#define componentManager_component_entity_foreach(Type, entity)\
	for (uintEC i=0, entity=getSparseSet(Type)->dense[i]; i < getSparseSet(Type)->denseSize; ++i)
/*entity is the alias that is going to be used for the next entity in the array that matches the key
iterates over all entitys in the sparseSet with the smallest size. it then looks up the key of the entity in the keys[] array.
if the found key matches the required key, the code in the brackets after the if statement(the brackets and whats inside is written 
by the user) is executed. For example: In the explode-system: a bomb explodes and every entity around it
should take damage. creating a system for this would be bad, because its callback is not called that often. So we just fetch all
entitys everytime we need this functionality. this results also in a much smaller memory use
smallestComponentTypeHint is the component specified by the user which he thinks has the smallest number of elements. For each
element in the dense array of the sparseSet with the smallest number of components, lookup its key and see if it matches the 
required one. This makes iterating pretty fast.*/

#define componentManager_entity_has_component(Type, entity)\
	({\
		ComponentKey key = (1 << hashMap_get(&sets, SparseSet, hashMap_hash(&sets, Type))->cki) | keys[entity];\
		key;\
	})


#define key_evaluate(r, key, Type)\
	*(key) |= 1 << hashMap_get(&sets, SparseSet, hashMap_hash(&sets, Type))->cki;
	/*@ComponentType is the name of the component, that should be converterted to a key and then added to the key-pointer, 
	given as parameter
	before we begin the shifting we first have to get the signature of the component by concatenating it. not with ## but with cat 
	because this has to be done when it is a element of a BOOST_PP_SEQUENCE*/

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
void    systemManager_update(void);
void 	systemManager_draw(void);
void    systemManager_entity_register(EntityId const entity, ComponentKey const key);
void    systemManager_entity_erase(EntityId const entity);
void    systemManager_task_register(TaskCallback const callback, CallType const callType);


#define systemManager_system_register(callback, CallType, maxEntitysHint, maxEntitysDevnHint, ...)\
	_systemManager_system_register(callback, CallType, components_convertToKey(__VA_ARGS__), maxEntitysHint, maxEntitysDevnHint);


void sparseSet_construct(SparseSet* set, size_t const componentSize, ComponentKeyIndex const cki, uintEC const maxComponentsDevnHint, void(*component_destructor)(void *const), void(*component_constructor)(void *const));
void sparseSet_destruct(SparseSet const *const set);
void sparseSet_entity_add(SparseSet *const set, EntityId const entity);
void sparseSet_entity_remove(SparseSet *const set, EntityId const entity);


void system_construct(System *const sys, SystemCallback callback, ComponentKey const key, uintEC const maxEntitysHint, uintEC const maxEntitysDevnHint);
void system_destruct(System const *const sys);
void system_entity_add(System *const sys, EntityId const entity);
void system_entity_remove(System *const sys, EntityId const entity);


/*altough callbacks are faster than events one should use them sparsly because they contradict the idea of grouping
logic together */
void    commandManager_init(uintC const n_signatureCount);
void 	commandManager_terminate(void);
void    commandManager_command_publish(CommandSignature const signature, void* data);
/* because only void* are passed this is much faster than passing each element by value*/
void    commandManager_command_subscribe(CommandSignature const signature, CommandCallback callback);
void 	eventManager_init(uintE const n_signatureCount);
void 	eventManager_terminate(void);
void    eventManager_buffers_swap(void);

#define eventManager_events_poll(EventDataType, signature, alias)\
	for (EventDataType* alias = &((EventDataType*)events_db[1 - db_index][signature])[eventCounts_db[1 - db_index][signature] - 1]; eventCounts_db[1 - db_index][signature]; alias = &((EventDataType*)events_db[1 - db_index][signature])[--eventCounts_db[1 - db_index][signature]])


#define eventManager_event_publish(EventDataType, signature, data)\
	if (eventCounts_db[db_index][signature] == eventCapacitys[signature])\
	{\
		events_db[db_index][signature] = realloc(events_db[db_index][signature], (eventCapacitys[signature] *= 2) * sizeof(EventDataType));\
	}\
	memcpy(&((EventDataType*)events_db[db_index][signature])[eventCounts_db[db_index][signature]++], &data, sizeof(EventDataType));


#define eventManager_event_register(EventDataType, signature, maxEventsHint)\
	events_db[db_index][signature] = checs_malloc(sizeof(EventDataType) * maxEventsHint);\
	events_db[1 - db_index][signature] = checs_malloc(sizeof(EventDataType) * maxEventsHint);\
	eventCapacitys[signature] = maxEventsHint;


#endif