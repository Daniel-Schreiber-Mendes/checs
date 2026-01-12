#ifndef ECS_H
#define ECS_H
//#include <checl/containers.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define EZD_MEMORY_DEBUG
#include <ezd/ezd.h>

typedef uint32_t EntityId; //unique identifier for an instanciated entity
typedef uint32_t uintEC; //any number that stores values that goes from 0 to the maximum value EntityId can hold

typedef uint32_t ComponentKey; //32bits mean 32 components can be indicated. this is a key which corresponds to an entity
typedef uint8_t ComponentSignature;

typedef uint8_t CommandSignature;

typedef uint8_t TagSignature;
typedef uint32_t TagKey;

#define NULL_TEMPLATE 0

typedef enum
{
	CHECS_ON_UPDATE,
	CHECS_ON_DRAW	
}
CallType; //specifies when the system/task is called

//INTERNAL:

#define CHECS_ASSERT

#ifdef CHECS_ASSERT
	#define checs_assert(expr)\
		if (!(expr))\
		{\
			printf("Checs-Assertion: %s failed. Line: %u, File: %s\n", #expr, __LINE__, __FILE__);\
			exit(-1);\
		}

	#define checs_assert_msg(expr, ...)\
		if (!(expr))\
		{\
			printf("\033[0;33m");\
			printf("Checs-Assertion failed:\nLine: %u\nFunc: %s\nFile: %s\n", __LINE__, __func__, __FILE__);\
			printf(__VA_ARGS__);\
			printf("\n\033[0m");\
			exit(-1);\
		}
#else
	#define checs_assert(expr) (void)0
	#define checs_assert_msg(expr, msg) (void)0
#endif
/* name all internal functions with prefix checsi_ */

void checs_entitys_init(void);
void checs_entitys_terminate(void);
void checs_entitys_reset(void);

void checs_components_init(void);
void checs_components_terminate(void);
void checs_components_entity_erase(EntityId entity);
void checs_components_reset(void);
ComponentKey ckey_get(EntityId e);
ComponentKey ckey_add_dependencys(ComponentKey key);

void checs_systems_init(void);
void checs_systems_terminate(void);
void checs_systems(CallType callType);

void checs_commands_init(void);
void checs_commands_terminate(void);


// API:


/* checs.c */
void checs_init(void);
void checs_run(void);
void checs_terminate(void);
void checs_update(void);
void checs_draw(void);
void checs_reset(void);


/* entitys.c */
EntityId checs_entity(ComponentKey key);
void checs_entity_erase(EntityId e);
void checs_entity_tag_add(EntityId entity, uintEC tag);
EntityId checs_entity_get_by_tag(uintEC tag);
EntityId checs_entity_copy(EntityId e);
void checs_entity_assert_if_erased(EntityId);


/* components.c */
void 	 checs_component_register(ComponentSignature sig, size_t componentSize, void(*component_destructor)(void*), void(*component_constructor)(void*), ComponentKey dependency_key);
void 	 checs_entity_components_add(EntityId entity, ComponentKey key);
void 	 checs_entity_components_remove(EntityId entity, ComponentKey key);
void* 	 checs_component_get(ComponentSignature sig, EntityId e);
void* 	 checs_components_foreach(ComponentSignature sig, EntityId *entity, uintEC index); //DEPRECATED
bool 	 checs_components_iterate(ComponentSignature sig, EntityId *entity, uintEC index);
EntityId checs_component_entitys_foreach(ComponentSignature sig, uintEC index);
uintEC	 checs_component_count(ComponentSignature sig);
void	 checs_component_capacity_set(ComponentSignature sig, uintEC capacity);
bool	 checs_entity_has_component(ComponentSignature sig, EntityId entity); 
void	 checs_component_copy(ComponentSignature sig, EntityId src, EntityId dest); //copy one component of entity to another
void	 checs_components_copy(EntityId src, EntityId dest); //copy all components of entity to another
void 	 checs_entity_name_add(EntityId e, char *name);
EntityId checs_entity_get_by_name(char *name);
EntityId checs_entity_get_by_key(ComponentKey key); //get random entity with at least given components. if 

/* systems.c */
void checs_tasks_register(CallType callType, void(*callback)(void), uint8_t frames_per_call); //	checs_system_parameters: EntityId *const entitys, uintEC const entityCount
void checs_task_active_set(void(*callback)(void), bool active);


/* commands.c */
void checs_command_register(CommandSignature sig);
void checs_command_publish(CommandSignature sig, void* data); /* because only void* are passed this is much faster than passing each element by value*/
void checs_command_subscribe(CommandSignature sig, void(*callback)(void*));
void checs_command_unsuscribe(ComponentSignature sig, void(*callback)(void*));
void checs_command_unsuscribe_all(ComponentSignature sig);

/* tags.c */
void 	 checs_tags_reset(void);
void 	 checs_tag_register(TagSignature tag);
void 	 checs_tags_add(TagKey key, EntityId e);
void 	 checs_tags_remove(TagKey key, EntityId e);
void 	 checs_tags_entity_erase(EntityId e);
EntityId checs_tags_iterate(TagSignature sig, uint32_t index);
void 	 checs_tags_copy(EntityId src, EntityId dest);
EntityId checs_entity_get_by_tag(TagKey key);


/* statemachine.c */

typedef struct State State;
struct State 
{
    void(*destruct)(State *const);
    void(*events)(State *const);
    void(*update)(State *const);
    void(*draw)(State *const);
};

void checs_state_pop(State *const state);
void checs_state_push(void(*destruct)(State *const));
void checs_running_set(bool const set);
float checs_ms_per_frame_get(void);
void checs_stateMachine_run(void);

#endif