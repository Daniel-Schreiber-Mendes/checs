#include "ecs.h"

//decided to use AoS because in the iteration we work with one system before moving to the next one
//->AoS is better for caching in this case
static System systems[SYSTEM_COUNT];
static Task tasks[TASK_COUNT];

//all update tasks are stored next to each other, the order is also the order they will get executed by
// directly next to the last update tast, the first draw task begins

void systemManager_system_register(void(*callback)(ComponentTable*), CallType const callType)
{
	static uint8_t systemUpdateCount = 0; //this is just used to insert each system at the correct place
	static uint8_t systemDrawCount = SYSTEM_UPDATE_COUNT; //this means the insertion of draw-calltype-systems 
	//begins at the end of the update-calltype-systems
	uint8_t i = (callType == UPDATE) ? systemUpdateCount : systemDrawCount; //which index is used depends on the callType

	systems[i].callback = callback;
	systems[i].active = true;
	systems[i].callType = callType;
}


void _systemManager_system_component_add(void(*callback)(ComponentTable*))
{

}


void systemManager_systems_call(CallType const callType)
{
	for(uint8_t i=0; i < SYSTEM_COUNT; ++i)
		if (systems[i].callType == callType && systems[i].active)
			systems[i].callback(&systems[i].componentTable);
}


void systemManager_task_register(void(*callback)(void), CallType const callType)
{
	static uint8_t systemUpdateCount = 0; //this is just used to insert each system at the correct place
	static uint8_t systemDrawCount = SYSTEM_UPDATE_COUNT; //this means the insertion of draw-calltype-systems 
	//begins at the end of the update-calltype-systems
	uint8_t i = (callType == UPDATE) ? systemUpdateCount : systemDrawCount; //which index is used depends on the callType

	systems[i].callback = callback;
	systems[i].active = true;
	systems[i].callType = callType;
}


void systemManager_tasks_call(CallType const callType)
{
	for(uint8_t i=0; i < TASK_COUNT; ++i)
		if (tasks[i].callType == callType && tasks[i].active)
			tasks[i].callback();
}