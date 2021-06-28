#include "ecs.h"

static uintST  systemUpdateCount; //number of systems that are called on update
static uintST  systemDrawCount; //number of systems that are called on draw
static System* systems; //array of system callbacks
static uint8_t systemTypeCounts[2]; 

static uintST  taskUpdateCount; //number of tasks that are called on update
static uintST  taskDrawCount; //number of tasks that are called on draw
static Task*   tasks; //array of task callbacks
static uint8_t taskTypeCounts[2]; //is used only at beginning of the program so system_task_register knows 
// at which index to put the task into;
//all update tasks are stored next to each other, the order is also the order they will get executed by
// directly next to the last update tast, the first draw task begins
/* systems[] looks like this:
Systems that are called on update	
|					| Systems that are called on draw
|					|					|
-----------------------------------------
|	|	|	|	|	|	|	|	|	|	|
-----------------------------------------
^					^					^
0           systemUpdateCount        systemUpdateCount + systemDrawCount
*/


static void tasks_call(uintST i, uintST const i_end);
static void systems_call(uintST i, uintST const i_end);


void checs_systems_init(uintST const n_systemUpdateCount, uintST const n_systemDrawCount, 
						uintST const n_taskUpdateCount, uintST const n_taskDrawCount)
{
	systemUpdateCount   = n_systemUpdateCount;
	systemDrawCount     = n_systemDrawCount;
	systems             = checs_malloc(sizeof(System) * (systemUpdateCount + systemDrawCount));
	systemTypeCounts[1] = systemUpdateCount;

	taskUpdateCount     = n_taskUpdateCount;
	taskDrawCount       = n_taskDrawCount;
	tasks               = checs_malloc(sizeof(Task) * (taskUpdateCount + taskDrawCount));
	taskTypeCounts[1]   = taskUpdateCount;
}


void checs_systems_terminate(void)
{
	for(uintST i=0; i < systemUpdateCount + systemDrawCount; ++i)
	{
		sparseSet_destruct(&systems[i].sparseSet);
	}

	checs_free(systems);
	checs_free(tasks);
}


void _checs_system_register(SystemCallback callback, CallType const callType, ComponentKey const key, uintEC const maxEntitysHint, EntityAddedCallback const on_entity_added)
{
	System *system = &systems[systemTypeCounts[callType]++];
	*system = (System)
	{
		.key = key,
		.active = true, 
		.callback = callback,
		.on_entity_added = on_entity_added
	};
	sparseSet_construct(&system->sparseSet, sizeof(EntityId), maxEntitysHint);
}
//the system array consists of two parts. the first part is made of systems, that are called on update, 
//and the second part are systems that are called on draw. How big each part is, is defined in systemUpdateCount
//(number of systems that get called on update) and systemDrawCount. taskTypeCounts has two counter, each one for one system type
//to know where to add newly registered systems. the counters are thus indices into the systems array. each time a new
//system of a type is registered, the counter for this type gets incremeneted by one so the next system that gets registered will be one 
//the right of it(if they have the same sytem type)


void checs_systems_update(void)
{
	tasks_call(0, taskUpdateCount);
	systems_call(0, systemUpdateCount);
}


void checs_systems_draw(void)
{
	tasks_call(taskUpdateCount, taskUpdateCount + taskDrawCount);
	systems_call(systemUpdateCount, systemUpdateCount + systemDrawCount);
}


static void tasks_call(uintST i, uintST const i_end)
{
	while (i < i_end)
	{
		if (tasks[i].active)
		{
			tasks[i++].callback();
		}
	}
}


static void systems_call(uintST i, uintST const i_end)
{
	for (;i < i_end; ++i)
	{
		if(systems[i].active)
		{
			systems[i].callback(systems[i].sparseSet.dense, systems[i].sparseSet.denseSize);
		}
	}
}


EntityId checs_systems_entity_register(EntityId const entity, ComponentKey const key)
{
	for(uintST i=0; i < systemUpdateCount + systemDrawCount; ++i)
	{
		if(checs_key_match(systems[i].key, key))
		{
			sparseSet_insert(&systems[i].sparseSet, EntityId, entity);
			if (systems[i].on_entity_added)
			{
				systems[i].on_entity_added(entity);
			}
		}
	}
	return entity;
}


//set entity key to 0 because otherwise when iterating over all entitys with a given component, an invalid entityId could be used
void checs_systems_entity_erase(EntityId const entity)
{
	for(uintST i=0; i < systemUpdateCount + systemDrawCount; ++i)
	{
		if(checs_key_match(systems[i].key, keys[entity]))
		{			
			sparseSet_erase(&systems[i].sparseSet, EntityId, entity);
		}
	}
	keys[entity] = 0;
}


void checs_task_register(CallType const callType, TaskCallback const callback)
{
	tasks[taskTypeCounts[callType]++] = (Task){true, callback};
}