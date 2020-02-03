#include "ecs.h"

//decided to use AoS because in the iteration we work with one system before moving to the next one
//->AoS is better for caching in this case
static uint8_t systemUpdateCount; //number of systems that are called on update
static uint8_t systemDrawCount; //number of systems that are called on draw
static uint8_t systemCount; //total number of systems
static System* systems; //array of system callbacks
static uint8_t systemTypeCounts[2] = {0}; 

static uint8_t taskUpdateCount; //number of tasks that are called on update
static uint8_t taskDrawCount; //number of tasks that are called on draw
static uint8_t taskCount; //total number of tasks
static Task*   tasks; //array of task callbacks
static uint8_t taskTypeCounts[2] = {0}; //is used only at beginning of the program so system_task_register knows 
// at which index to put the task into;
//all update tasks are stored next to each other, the order is also the order they will get executed by
// directly next to the last update tast, the first draw task begins

void systemManager_init(uint8_t const n_systemUpdateCount, uint8_t const n_systemDrawCount, 
						uint8_t const n_taskUpdateCount, uint8_t const n_taskDrawCount)
{
	systemUpdateCount   = n_systemUpdateCount;
	systemDrawCount     = n_systemDrawCount;
	systemCount         = systemUpdateCount + systemDrawCount;
	systems             = (System*)malloc(sizeof(System) * systemCount);
	systemTypeCounts[1] = systemUpdateCount;

	taskUpdateCount     = n_taskUpdateCount;
	taskDrawCount       = n_taskDrawCount;
	taskCount           = taskUpdateCount + taskDrawCount;
	tasks               = (Task*)malloc(sizeof(Task) * taskCount);
	taskTypeCounts[1]   = taskUpdateCount;
}


void _systemManager_system_register(void(*callback)(ComponentTable*), CallType const callType)
{
	systems[systemTypeCounts[callType]++] = (System){.callback = callback, .active = true};
	//the system array consists of two parts. the first part is made of systems, that are called on update, 
	//and the second part are systems that are called on draw. How big each part is, is defined in systemUpdateCount
	//(number of systems that get called on update) and systemDrawCount. taskTypeCounts has two counter, each one for one system type
	//to know where to insert newly registered systems. the counters are thus indices into the systems array. each time a new
	//system of a type is registered, the counter for this type gets incremeneted by one so the next system that gets registered will be one 
	//the right of it(if they have the same sytem type)
}


void _systemManager_system_component_add(void(*callback)(ComponentTable*), ComponentSignature const signature, size_t const size)
{

}


void systemManager_systems_call(CallType const callType)
{
	uint8_t i, iMax;
	if(callType == UPDATE) { i = 0; iMax = systemUpdateCount; } else { i = systemUpdateCount; iMax = systemCount; }
	for(; i < iMax; ++i)
		if (systems[i].active)
			systems[i].callback(&systems[i].componentTable);
}


void systemManager_task_register(void(*callback)(void), CallType const callType)
{
	tasks[taskTypeCounts[callType]++] = (Task){.callback = callback, .active = true};
}


void systemManager_tasks_call(CallType const callType)
{
	//the index starting point in the array depends on which type of task is called now
	uint8_t i, iMax;
	if(callType == UPDATE) { i = 0; iMax = taskUpdateCount; } else { i = taskUpdateCount; iMax = taskCount; }
	for(; i < iMax; ++i)
		if (tasks[i].active)
			tasks[i].callback();
}