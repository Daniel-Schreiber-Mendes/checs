#ifndef ECS_H
#define ECS_H
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
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
		printf("freed %p\n", p);\
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

	#define checs_assert_msg(expr, msg)\
		if (!(expr))\
		{\
			printf("\033[0;33m");\
			printf("Checs-Assertion failed:\n%s\nLine: %u\nFile: %s\n", msg, __LINE__, __FILE__);\
			printf("\033[0m");\
			exit(-1);\
		}

	//checks if it is allowed to acces the entity
	#define checs_entity_assert(sig, e) checs_assert_msg(checs_entity_has_component(sig, e), "Entity can not access component")

	#define checs_component_assert(Type)\
		if (!getComponentSet(Type))\
		{\
			printf("Component \"%s\" does not exist\n", #Type);\
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
typedef uint8_t TemplateKey; //every bit indicates one template
typedef uint8_t TemplateSignature; 
typedef uint16_t EntityId; //unique identifier for an instanciated entity
typedef uint16_t uintEC; //any number that stores values that goes from 0 to the maximum value EntityId can hold
typedef uint8_t uintST; //any number that goes from 0 to the maximum number of systems/tasks
typedef uint8_t uintCS; //stores any number that goes from 0 to the maximum number of ComponentSignatures
typedef uint8_t uintC; //stores any number that goes from 0 to the maximum number of commands
typedef uint8_t uintE; //stores any number that goes from 0 to the maximum number of events
typedef uint8_t uintA; //stores any number that goes from 0 to the maximum number of attributes
typedef TemplateKey uintT;
typedef uintA AttributeSignature; //the signature of a AttributeType which depends on the order of registering this is the hashed value of the name of the attribute
typedef uint8_t EventSignature;
typedef uintEC ComponentSignature;//the signature of a ComponentType which depends on the order of registering this is the hashed value of the name of the component
typedef uintC CommandSignature;
typedef uint16_t EventSize;
typedef void(*SystemCallback)(EntityId *entitys, uintEC size);
typedef void(*TaskCallback)(void);
typedef void(*CommandCallback)(void*);
typedef void(*EntityAddedCallback)(EntityId);
typedef void(*TemplateCallback)(EntityId);


#define CHECS_NO_TEMPLATE -1


typedef enum
{
	CHECS_ON_UPDATE,
	CHECS_ON_DRAW	
}
CallType; //specifies when the system/task is called


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


/*TODO:  
 -creating a seperate memory pool for the values that are passed to the commands
 */


#define key_match(requiredKey, providedKey) ({((requiredKey) & (providedKey)) == (requiredKey);})


void      entityManager_init(uintEC tag_count);
void 	  entityManager_terminate(void);
EntityId _entityManager_entity_generate(ComponentKey key);
void      entityManager_entity_erase(EntityId e);
void      entitymanager_entity_tag_add(EntityId entity, uintEC tag);
EntityId  entityManager_entity_get_by_tag(uintEC tag);
#define   checs_entity_generate(template, ...)\
({\
	EntityId entity = _entityManager_entity_generate(components_convertToKey(__VA_ARGS__));\
	if (template != CHECS_NO_TEMPLATE)\
	{\
		templateManager_entity_templates_add(entity, 1 << template);\
	}\
 	systemManager_entity_register(entity, components_convertToKey(__VA_ARGS__));\
 	entity;\
})

#define   checs_entity_foreach(entityAlias, expr) for (uintEC entityAlias##i=0, entityAlias=entitys[0]; entityAlias##i < entityCount; entityAlias = entitys[++entityAlias##i]) {expr}//called inside system


extern ComponentSet *sets; 
extern ComponentKey* keys; 

void 	componentManager_init(uintCS n_componentCount, uintEC maxEntitysHint);
void 	componentManager_terminate(void);
void    componentManager_component_register(ComponentSignature sig, size_t componentSize, uintEC maxComponentsHint, void(*component_destructor)(void*), void(*component_constructor)(void*));
void    componentManager_entity_erase(EntityId entity);
void    componentManager_entity_components_add(EntityId entity, ComponentKey key);
void    componentManager_entity_components_remove(EntityId entity, ComponentKey key);
#define checs_component_register(sig, Type, maxComponentsHint, component_destructor, component_constructor) componentManager_component_register(sig, sizeof(Type), maxComponentsHint, component_destructor, component_constructor);
#define checs_component_mut_use(Type, alias) Type *alias
#define checs_component_use(Type, alias) Type const *alias
#define checs_entity_components_add(entity, ...) componentManager_entity_components_add(entity, components_convertToKey(__VA_ARGS__))
#define checs_entity_components_remove(entity, ...) componentManager_entity_components_remove(entity, components_convertToKey(__VA_ARGS__))

#define checs_component_get(sig, Type, alias, entity)\
	checs_entity_assert(sig, entity);\
	alias = &((Type*)sets[sig].components)[sets[sig].sparse[entity]];
	/*updating the value of the alias for a member of a component*/

#define checs_components_foreach(sig, Type, alias, entityAlias, expr)\
	alias = &((Type*)sets[sig].components)[0];\
	uintEC entityAlias __attribute__ ((unused)) = sets[sig].dense[0];\
	for (uintEC entityAlias##i=0; entityAlias##i < sets[sig].denseSize; entityAlias = sets[sig].dense[++entityAlias##i], alias = &((Type*)sets[sig].components)[entityAlias##i])\
	{\
		expr;\
	}

#define checs_component_get_once(sig, Type, alias, entity)\
	checs_entity_assert(sig, entity);\
	Type *const alias = &((Type*)sets[sig].components)[sets[sig].sparse[entity]];

#define checs_componentMatches_foreach(entityAlias, smallestTypeHint, expr, ...)\
	for (uintEC entityAlias##i=0, entityAlias=sets[smallestTypeHint].dense[entityAlias##i], key=keys[entityAlias]; entityAlias##i < sets[smallestTypeHint].denseSize; ++entityAlias##i, key = keys[++entityAlias])\
	{\
		if (key_match(components_convertToKey(__VA_ARGS__), key))\
		{\
			expr;\
		}\
	}


#define checs_component_entity_foreach(sig, entityAlias, expr) for (uintEC checs_i=0, entityAlias=sets[sig].dense[checs_i]; checs_i < sets[sig].denseSize; ++checs_i) {expr;}
#define checs_component_count(sig) sets[sig].denseSize
#define checs_entity_has_component(sig, entity) ((1 << sig) & keys[entity])
#define key_evaluate(r, key, sig) | (1 << sig)
#define components_convertToKey(...) (0 BOOST_PP_SEQ_FOR_EACH(key_evaluate, key, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))


void    	systemManager_init(uintST n_systemUpdateCount, uintST n_systemDrawCount, uintST n_taskUpdateCount, uintST taskDrawCount);
void    	systemManager_terminate(void);
void    	systemManager_system_register(SystemCallback callback, CallType callType, ComponentKey key, uintEC maxEntitysHint, EntityAddedCallback on_entity_added);
void    	systemManager_update(void);
void 		systemManager_draw(void);
EntityId    systemManager_entity_register(EntityId entity, ComponentKey key);
void    	systemManager_entity_erase(EntityId entity);
void    	systemManager_task_register(TaskCallback callback, CallType callType);
#define 	checs_system_register(callback, CallType, maxEntitysHint, on_entity_added, ...) systemManager_system_register(callback, CallType, components_convertToKey(__VA_ARGS__), maxEntitysHint, on_entity_added);


void componentSet_construct(ComponentSet* set, size_t componentSize, uintEC maxComponentsDevnHint, void(*component_destructor)(void*), void(*component_constructor)(void*));
void componentSet_destruct(ComponentSet *set);
void componentSet_entity_add(ComponentSet *set, EntityId entity);
void componentSet_entity_remove(ComponentSet *set, EntityId entity);


void system_construct(System *sys, SystemCallback callback, ComponentKey key, uintEC maxEntitysHint, EntityAddedCallback on_entity_added);
void system_destruct(System const *sys);
void system_entity_add(System *sys, EntityId entity);
void system_entity_remove(System *sys, EntityId entity);


void    commandManager_init(uintC commandCount);
void 	commandManager_terminate(void);
void    commandManager_command_register(CommandSignature sig, uintC callbackCount);
void    commandManager_command_publish(CommandSignature sig, void* data); /* because only void* are passed this is much faster than passing each element by value*/
void    commandManager_command_subscribe(CommandSignature sig, CommandCallback callback);

#define checs_command_parameters void *const data
#define checs_command_use(Type, _alias) Type __attribute__ ((alias ("data"))) *const _alias


void 	eventManager_init(uintE n_signatureCount);
void 	eventManager_terminate(void);
void    eventManager_buffers_swap(void);
void	eventManager_event_register(EventSignature sig, EventSize size, uintE maxEvents);
void 	eventManager_event_publish(EventSignature sig, EventSize size, void *data);
#define checs_event_publish(Type, sig, data) eventManager_event_publish(sig, sizeof(Type), data);
#define checs_event_register(Type, sig, maxEvents) eventManager_event_register(sig, sizeof(Type), maxEvents)

extern EventQueue *exposed;
#define checs_events_poll(Type, sig, alias, expr) \
	Type *alias;\
	for (uintE i=0; i < exposed->sizes[sig]; ++i)\
	{\
		alias = &((Type*)exposed->events[sig])[i];\
		expr;\
	}
 



extern Vector *attributes;

void attributeManager_init(uintA maxAttributesHint);
void attributeManager_terminate(void);
void _attributeManager_attribute_register(AttributeSignature sig, uintA attributeCount);
#define checs_attribute_register(sig, attributeCount) _attributeManager_attribute_register(sig, attributeCount)
#define checs_attribute_define(Type) typedef struct{}Type
#define checs_entity_attribute_add(sig, entity_adr) vector_push_back_func(&attributes[sig], entity_adr)
#define checs_entity_attribute_remove(sig, entity) vector_find(&attributes[sig], EntityId, entity)
#define checs_attribute_entity_foreach(sig, entityAlias) vector_vforeach(&attributes[sig], EntityId, entityAlias)


void templateManager_init(uintT templatecount);
void templateManager_terminate(void);
void templateManager_template_register(TemplateSignature sig, TemplateCallback cb);
void templateManager_entity_templates_add(EntityId e, TemplateKey key);


#define swap(x, y)\
	{\
		__auto_type tmp = x;\
		x = y;\
		y = tmp;\
	}\


#endif