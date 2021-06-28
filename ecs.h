#ifndef ECS_H
#define ECS_H
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <checl/containers.h>

#define EZD_MEMORY_DEBUG
#include <ezd/ezd.h>


#define CHECS_STATS
#define CHECS_ASSERT


#define checs_free(p) free(p)
#define checs_malloc(size) malloc(size)
#define checs_calloc(num, size) calloc(num, size)


#ifdef CHECS_ASSERT
	#define checs_assert(expr)\
		if (!(expr))\
		{\
			printf("Checs-Assertion: %s failed. Line: %u, File: %s\n", #expr, __LINE__, __FILE__);\
			exit(-1);\
		}

	#define checs_assert_msg(expr, msg)\
		if (!(expr))\
		{\
			printf("\033[0;33m");\
			printf("Checs-Assertion failed:\n%s\nLine: %u\nFile: %s\n", msg, __LINE__, __FILE__);\
			printf("\033[0m");\
			exit(-1);\
		}

	//checks if it is allowed to acces the entity
	#define checs_entity_assert(sig, e)\
		if (!checs_entity_has_component(sig, e))\
		{\
			printf("\033[0;33m");\
			printf("Checs-Assertion failed:\n Entity %u can not access component %u \nLine: %u\nFile: %s\n", e, sig, __LINE__, __FILE__);\
			printf("\033[0m");\
			exit(-1);\
		}

	#define checs_component_assert(sig)\
		if (!sig)\
		{\
			printf("Tried to access invalid Component in Function %s\n", __func__);\
			exit(-1);\
		}

	#define checs_event_assert(sig)\
		if (!sig)\
		{\
			printf("Tried to access invalid Event in Function %s\n", __func__);\
			exit(-1);\
		}

	#define checs_command_assert(sig)\
		if (!sig)\
		{\
			printf("Tried to access invalid Command in Function %s\n", __func__);\
			exit(-1);\
		}
#else
	#define checs_assert(expr) (void)0
	#define checs_entity_assert(sig, e) (void)0
	#define checs_component_assert(Type) (void)0
#endif

#ifdef CHECS_STATS
	#define checs_stats_log(expr) expr
#else
	#define checs_stats_log(expr) (void)0
#endif


typedef uint16_t ComponentKey; //8bits mean 8 components can be indicated. this is a key which corresponds to an entity
typedef uint16_t EntityId; //unique identifier for an instanciated entity
typedef uint16_t uintEC; //any number that stores values that goes from 0 to the maximum value EntityId can hold
typedef uint8_t uintST; //any number that goes from 0 to the maximum number of systems/tasks
typedef uint8_t uintCS; //stores any number that goes from 0 to the maximum number of ComponentSignatures
typedef uint8_t uintC; //stores any number that goes from 0 to the maximum number of commands
typedef uint8_t uintE; //stores any number that goes from 0 to the maximum number of events
typedef uint8_t uintA; //stores any number that goes from 0 to the maximum number of attributes
typedef uintA AttributeSignature; //the signature of a AttributeType which depends on the order of registering this is the hashed value of the name of the attribute
typedef uint8_t EventSignature;
typedef uintEC ComponentSignature;//the signature of a ComponentType which depends on the order of registering this is the hashed value of the name of the component
typedef uintC CommandSignature;
typedef uint16_t EventSize;
typedef void(*SystemCallback)(EntityId *entitys, uintEC size);
typedef void(*TaskCallback)(void);
typedef void(*CommandCallback)(void*);
typedef void(*EntityAddedCallback)(EntityId);


typedef enum
{
	CHECS_ON_UPDATE,
	CHECS_ON_DRAW	
}
CallType; //specifies when the system/task is called


typedef enum 
{
	CHECS_PAUSED,
	CHECS_RUNNING
}
ChecsStatus;


typedef struct
{
	uintEC *sparse; //sparse packed array of indices to dense array
	uintEC sparseCapacity; //maximum number of elements

	uintEC *dense; //dense array of entityId's
	uintEC denseCapacity; //maximum number of elements
	uintEC denseSize; //current number of elements;

	size_t componentSize; //size of component
	void(*component_destructor)(void*);
	void(*component_constructor)(void*);
	void* components; //type is not char but the type of the components. can not be void because it is a unsized array so no void* is possible
}
ComponentSet;


typedef struct
{
	SparseSet sparseSet;
	bool active;
	SystemCallback callback;	
	ComponentKey key;
	EntityAddedCallback on_entity_added;
}
System;


typedef struct 
{
	bool active;
	TaskCallback callback;
}
Task;


typedef struct
{
	uint8_t size, cap;
	CommandCallback callbacks[];
}
Command;


typedef struct
{
	void **events;
	uint8_t *sizes;
}
EventQueue;


extern ChecsStatus checs_status;
extern EventQueue *exposed;
extern Vector *attributes;
extern ComponentSet *sets; 
extern ComponentKey *keys; 


void checs_init(uint8_t systemUpdateCount, uint8_t systemDrawCount, uint8_t taskUpdateCount, uint8_t taskDrawCount, uint8_t tag_count, uint8_t componentCount, uint8_t commandCount, uint8_t eventCount, uint8_t maxAttributesHint);
void checs_terminate();

void      checs_entitys_init(uintEC tag_count);
void 	  checs_entitys_terminate(void);
EntityId  checs_entity_generate(ComponentKey key);
void      checs_entity_erase(EntityId e);
void      checs_entity_tag_add(EntityId entity, uintEC tag);
EntityId  checs_entity_get_by_tag(uintEC tag);
#define   checs_entity(...) checs_entity_generate(components_convertToKey(__VA_ARGS__))
#define   checs_entity_foreach(entityAlias, expr) for (uintEC entityAlias##i=0, entityAlias=entitys[0]; entityAlias##i < entityCount; entityAlias = entitys[++entityAlias##i]) {expr}//called inside system

void 	checs_components_init(uintCS n_componentCount);
void 	checs_components_terminate(void);
void    checs_component_register(ComponentSignature sig, size_t componentSize, uintEC maxComponentsHint, void(*component_destructor)(void*), void(*component_constructor)(void*));
void    checs_components_entity_add(EntityId entity, ComponentKey key);
void    checs_components_entity_remove(EntityId entity, ComponentKey key);
void* 	checs_component_get(ComponentSignature sig, EntityId e);
uintEC  checs_component_count(ComponentSignature sig);
uintEC  checs_max_component_count(ComponentSignature sig);
bool 	checs_entity_has_component(ComponentSignature sig, EntityId entity);
bool    checs_key_match(ComponentKey requiredKey, ComponentKey providedKey);

#define checs_entity_components_add(entity, ...) checs_components_entity_add(entity, components_convertToKey(__VA_ARGS__))
#define checs_entity_components_remove(entity, ...) checs_components_entity_remove(entity, components_convertToKey(__VA_ARGS__))
#define checs_component_get_t(sig, Type, e) ((Type*)checs_component_get(sig, e))

#define checs_components_foreach(sig, alias, entityAlias, expr)\
	checs_component_assert(sig);\
	uintEC entityAlias __attribute__ ((unused)) = sets[sig].dense[0];\
	for (uintEC entityAlias##i=0; entityAlias##i < sets[sig].denseSize; entityAlias = sets[sig].dense[++entityAlias##i])\
	{\
		alias = sets[sig].components + entityAlias##i * sets[sig].componentSize;\
		expr;\
	}

#define checs_componentMatches_foreach(entityAlias, smallestTypeHint, expr, ...)\
	checs_assert(smallestTypeHint);\
	for (uintEC entityAlias##i=0, entityAlias=sets[smallestTypeHint].dense[entityAlias##i], key=keys[entityAlias]; entityAlias##i < sets[smallestTypeHint].denseSize; ++entityAlias##i, key = keys[++entityAlias])\
	{\
		if (checs_key_match(components_convertToKey(__VA_ARGS__), key))\
		{\
			expr;\
		}\
	}

#define checs_component_entity_foreach(sig, entityAlias, expr)\
	checs_component_assert(sig);\
	for (uintEC checs_i=0, entityAlias=sets[sig].dense[checs_i]; checs_i < sets[sig].denseSize; ++checs_i) {expr;}

#define key_evaluate(r, key, sig) | (1 << sig)
#define components_convertToKey(...) (0 BOOST_PP_SEQ_FOR_EACH(key_evaluate, key, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

void    	checs_systems_init(uintST n_systemUpdateCount, uintST n_systemDrawCount, uintST n_taskUpdateCount, uintST taskDrawCount);
void    	checs_systems_terminate(void);
void    	_checs_system_register(SystemCallback callback, CallType callType, ComponentKey key, uintEC maxEntitysHint, EntityAddedCallback on_entity_added);
void    	checs_systems_update(void);
void 		checs_systems_draw(void);
EntityId    checs_systems_entity_register(EntityId entity, ComponentKey key);
void    	checs_systems_entity_erase(EntityId entity);
void 		checs_task_register(CallType callType, TaskCallback callback);
#define 	checs_system_register(callback, CallType, maxEntitysHint, on_entity_added, ...) _checs_system_register(callback, CallType, components_convertToKey(__VA_ARGS__), maxEntitysHint, on_entity_added);

void    checs_commands_init(uintC commandCount);
void 	checs_commands_terminate(void);
void    checs_command_register(CommandSignature sig);
void    checs_command_publish(CommandSignature sig, void* data); /* because only void* are passed this is much faster than passing each element by value*/
void    checs_command_subscribe(CommandSignature sig, CommandCallback callback);

void 	checs_events_init(uintE n_signatureCount);
void 	checs_events_terminate(void);
void    checs_eventbuffers_swap(void);
void	checs_event_register(EventSignature sig, EventSize size);
void 	checs_event_publish(EventSignature sig, void *data);
void*   checs_event_poll(EventSignature sig, uintE *index);

#define checs_events_poll(sig, Type, alias, expr) \
	checs_event_assert(sig)\
	Type *alias;\
	for (uintE i=0; i < exposed->sizes[sig]; ++i)\
	{\
		alias = &((Type*)exposed->events[sig])[i];\
		expr;\
	}

void attributeManager_init(uintA maxAttributesHint);
void attributeManager_terminate(void);
void checs_attribute_register(AttributeSignature sig, uintA attributeCount);
#define checs_entity_attribute_add(sig, entity_adr) vector_push_back_func(&attributes[sig], entity_adr)
#define checs_entity_attribute_remove(sig, entity) vector_find(&attributes[sig], EntityId, entity)
#define checs_attribute_entity_foreach(sig, entityAlias) vector_vforeach(&attributes[sig], EntityId, entityAlias)

#define swap(x, y)\
	{\
		__auto_type tmp = x;\
		x = y;\
		y = tmp;\
	}\


#endif