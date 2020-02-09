#include "ecs.h"

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

void systemManager_init(uintST const n_systemUpdateCount, uintST const systemDrawCount, 
						uintST const n_taskUpdateCount, uintST const taskDrawCount)
{
	systemUpdateCount   = n_systemUpdateCount;
	systemCount         = systemUpdateCount + systemDrawCount;
	systems             = (systemCount > 0) ? (System*)malloc_debug(sizeof(System) * systemCount) : NULL;
	systemTypeCounts[1] = systemUpdateCount;

	taskUpdateCount     = n_taskUpdateCount;
	taskCount           = taskUpdateCount + taskDrawCount;
	tasks               = (taskCount > 0) ? (Task*)malloc_debug(sizeof(Task) * taskCount) : NULL;
	taskTypeCounts[1]   = taskUpdateCount;
}


void systemManager_terminate(void)
{
	for(uintST i=0; i < systemCount; ++i)
		system_destruct(&systems[i]);
	if(systems)
		free_debug(systems);
	if(tasks)
		free_debug(tasks);
	//has to be checked if memory for the array was allocated because if e.g there are no tasks,
	// 0 bytes of memory would be allocated which can not be handled properly
}


void _systemManager_system_register(SystemCallback callback, CallType const callType, uintEC const maxEntitysHint, uintEC const maxEntitysDevnHint)
{
	system_construct(&systems[systemTypeCounts[callType]], callback, maxEntitysHint, maxEntitysDevnHint);
}
//the system array consists of two parts. the first part is made of systems, that are called on update, 
//and the second part are systems that are called on draw. How big each part is, is defined in systemUpdateCount
//(number of systems that get called on update) and systemDrawCount. taskTypeCounts has two counter, each one for one system type
//to know where to add newly registered systems. the counters are thus indices into the systems array. each time a new
//system of a type is registered, the counter for this type gets incremeneted by one so the next system that gets registered will be one 
//the right of it(if they have the same sytem type)


void _systemManager_system_component_add(SystemCallback callback, ComponentSignature const signature)
{
	for(uintST i=0; i < systemCount; ++i)
		if(systems[i].callback == callback)
			key_set(systems[i].key, signature);
			//by or-ing the componentSignature and current system key the bit indicating this component in the key is set 
}


void systemManager_systems_call(CallType const callType)
{
	uintST i, iMax;
	if(callType == UPDATE) { i = 0; iMax = systemUpdateCount; } else { i = systemUpdateCount; iMax = systemCount; }
	for(; i < iMax; ++i)
		if(systems[i].active)
			systems[i].callback(systems[i].dense, systems[i].denseSize);
}


void systemManager_entity_register(EntityId const entity, ComponentKey const key)
{
	for(uintST i=0; i < systemCount; ++i)
		if(key_match(systems[i].key, key))
			system_entity_add(&systems[i], entity);
}


void systemManager_entity_erase(EntityId const entity)
{
	for(uintEC i=0; i < systemCount; ++i)
		if(key_match(systems[i].key, componentManager_key_get(entity)))
			system_entity_remove(&systems[i], entity);
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