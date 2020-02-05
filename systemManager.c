#include "ecs.h"

//decided to use AoS because in the iteration we work with one system before moving to the next one
//->AoS is better for caching in this case
static uintST systemUpdateCount; //number of systems that are called on update
static uintST systemDrawCount; //number of systems that are called on draw
static uintST systemCount; //total number of systems
static System* systems; //array of system callbacks
static uint8_t systemTypeCounts[2] = {0}; 

static uintST taskUpdateCount; //number of tasks that are called on update
static uintST taskDrawCount; //number of tasks that are called on draw
static uintST taskCount; //total number of tasks
static Task*   tasks; //array of task callbacks
static uint8_t taskTypeCounts[2] = {0}; //is used only at beginning of the program so system_task_register knows 
// at which index to put the task into;
//all update tasks are stored next to each other, the order is also the order they will get executed by
// directly next to the last update tast, the first draw task begins

void systemManager_init(uintST const n_systemUpdateCount, uintST const n_systemDrawCount, 
						uintST const n_taskUpdateCount, uintST const n_taskDrawCount)
{
	systemUpdateCount   = n_systemUpdateCount;
	systemDrawCount     = n_systemDrawCount;
	systemCount         = systemUpdateCount + systemDrawCount;
	systems             = (systemCount > 0) ? (System*)malloc_debug(sizeof(System) * systemCount) : NULL;
	systemTypeCounts[1] = systemUpdateCount;

	taskUpdateCount     = n_taskUpdateCount;
	taskDrawCount       = n_taskDrawCount;
	taskCount           = taskUpdateCount + taskDrawCount;
	tasks               = (taskCount > 0) ? (Task*)malloc_debug(sizeof(Task) * taskCount) : NULL;
	taskTypeCounts[1]   = taskUpdateCount;

	for(uintST i=0; i < systemCount; ++i)
	{
		vector_construct(&systems[i].entitys, sizeof(EntityId));
	}
}


void _systemManager_system_register(SystemCallback callback, CallType const callType)
{
	systems[systemTypeCounts[callType]].callback = callback;
	systems[systemTypeCounts[callType]].entityCount = 0;
	systems[systemTypeCounts[callType]].key = 0;
	systems[systemTypeCounts[callType]++].active = true;
	//the system array consists of two parts. the first part is made of systems, that are called on update, 
	//and the second part are systems that are called on draw. How big each part is, is defined in systemUpdateCount
	//(number of systems that get called on update) and systemDrawCount. taskTypeCounts has two counter, each one for one system type
	//to know where to insert newly registered systems. the counters are thus indices into the systems array. each time a new
	//system of a type is registered, the counter for this type gets incremeneted by one so the next system that gets registered will be one 
	//the right of it(if they have the same sytem type)
}


void systemManager_terminate(void)
{
	for(uintST i=0; i < systemCount; ++i)
		vector_destruct(&systems[i].entitys);;
	if(systems)
		free_debug(systems);
	if(tasks)
		free_debug(tasks);
}


void _systemManager_system_component_add(SystemCallback callback, ComponentSignature const signature)
{
	for(uintST i=0; i < systemCount; ++i)
		if(systems[i].callback == callback)
			systems[i].key |= 1 << signature; 
			//by or-ing the componentSignature and current system key the bit indicating this component in the key is set 
}


void systemManager_systems_call(CallType const callType)
{
	uintST i, iMax;
	if(callType == UPDATE) { i = 0; iMax = systemUpdateCount; } else { i = systemUpdateCount; iMax = systemCount; }
	for(; i < iMax; ++i)
		if (systems[i].active)
			systems[i].callback(systems[i].entitys.data, systems[i].entityCount);
}


void systemManager_systems_entity_add(EntityId const entity, ComponentKey const key)
{
	for(uintST i=0; i < systemCount; ++i)
	{
		if(keyMatch(systems[i].key, key))
		{
			vector_element_push(&systems[i].entitys, uintEC, entity);
			++systems[i].entityCount;
		}
	}
}


void systemManager_task_register(TaskCallback const callback, CallType const callType)
{
	tasks[taskTypeCounts[callType]++] = (Task){.callback = callback, .active = true};
}


void systemManager_tasks_call(CallType const callType)
{
	//the index starting point in the array depends on which type of task is called now
	uintST i, iMax;
	if(callType == UPDATE) { i = 0; iMax = taskUpdateCount; } else { i = taskUpdateCount; iMax = taskCount; }
	for(; i < iMax; ++i)
		if (tasks[i].active)
			tasks[i].callback();
}