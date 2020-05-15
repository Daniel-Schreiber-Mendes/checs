#include "ecs.h"

static uintST  systemUpdateCount; //number of systems that are called on update
static uintST  systemDrawCount; //number of systems that are called on draw
static uintST  systemCount; //total number of systems
static System* systems; //array of system callbacks
static uint8_t systemTypeCounts[2]; 

static uintST  taskUpdateCount; //number of tasks that are called on update
static uintST  taskDrawCount; //number of tasks that are called on draw
static uintST  taskCount; //total number of tasks
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
0           systemUpdateCount        systemCount
*/


static void tasks_call(uintST i, uintST const i_end);
static void systems_call(uintST i, uintST const i_end);


void systemManager_init(uintST const n_systemUpdateCount, uintST const systemDrawCount, 
						uintST const n_taskUpdateCount, uintST const taskDrawCount)
{
	systemUpdateCount   = n_systemUpdateCount;
	systemCount         = systemUpdateCount + systemDrawCount;
	systems             = checs_malloc(sizeof(System) * systemCount);
	systemTypeCounts[1] = systemUpdateCount;

	taskUpdateCount     = n_taskUpdateCount;
	taskCount           = taskUpdateCount + taskDrawCount;
	tasks               = checs_malloc(sizeof(Task) * taskCount);
	taskTypeCounts[1]   = taskUpdateCount;
}


void systemManager_terminate(void)
{
	for(uintST i=0; i < systemCount; ++i)
	{
		system_destruct(&systems[i]);
	}

	checs_free(systems);
	checs_free(tasks);
}


void systemManager_system_register(SystemCallback callback, CallType const callType, ComponentKey const key, uintEC const maxEntitysHint, uintEC const maxEntitysDevnHint)
{
	system_construct(&systems[systemTypeCounts[callType]++], callback, key, maxEntitysHint, maxEntitysDevnHint);
}
//the system array consists of two parts. the first part is made of systems, that are called on update, 
//and the second part are systems that are called on draw. How big each part is, is defined in systemUpdateCount
//(number of systems that get called on update) and systemDrawCount. taskTypeCounts has two counter, each one for one system type
//to know where to add newly registered systems. the counters are thus indices into the systems array. each time a new
//system of a type is registered, the counter for this type gets incremeneted by one so the next system that gets registered will be one 
//the right of it(if they have the same sytem type)


void systemManager_update(void)
{
	tasks_call(0, taskUpdateCount);
	systems_call(0, systemUpdateCount);
}


void systemManager_draw(void)
{
	tasks_call(taskUpdateCount, taskCount);
	systems_call(systemUpdateCount, systemCount);
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
	while (i < i_end)
	{
		if(systems[i].active)
		{
			systems[i].callback(systems[i].dense, systems[i].denseSize);
		}
		++i;
	}
}


void systemManager_entity_register(EntityId const entity, ComponentKey const key)
{
	for(uintEC i=0; i < systemCount; ++i)
	{
		if(key_match(systems[i].key, key))
		{
			system_entity_add(&systems[i], entity);
		}
	}
}


//set entity key to 0 because otherwise when iterating over all entitys with a given component, an invalid entityId could be used
void systemManager_entity_erase(EntityId const entity)
{
	for(uintEC i=0; i < systemCount; ++i)
	{
		if(key_match(systems[i].key, keys[entity]))
		{			
			system_entity_remove(&systems[i], entity);
		}
	}
	keys[entity] = 0;
}


void systemManager_task_register(TaskCallback const callback, CallType const callType)
{
	tasks[taskTypeCounts[callType]++] = (Task){true, callback};
}