#include <checs.h>
#include <string.h>


typedef struct 
{
	bool active; //dinamically activate or deactivate
	uint8_t frames_per_call; //if it is 1 task is called every frame, if 2 every second frame and so on
	uint8_t frames_per_call_counter; //counts every frame one up until at frames_per_call and resets to 0
	CallType callType;
	void(*callback)(void);
}
Task;


static Task* tasks; //array of task callbacks
static uint8_t taskCount;


void checs_systems_terminate(void)
{
	if (tasks)
		free(tasks);
}


void checs_tasks_register(CallType callType, void(*callback)(void), uint8_t frames_per_call)
{
	tasks = realloc(tasks, sizeof(Task) * (taskCount + 1));
	tasks[taskCount++] = (Task){true, frames_per_call, callType, 0, callback};
}


void checs_systems(CallType callType)
{
	for (uint8_t i=0; i < taskCount; ++i)
	{
		if (tasks[i].callType == callType && tasks[i].active)
		{
			++tasks[i].frames_per_call_counter;
			if (tasks[i].frames_per_call_counter == tasks[i].frames_per_call)
			{
				tasks[i].frames_per_call_counter = 0;
				tasks[i].callback();
			}
		}
	}
}


void checs_task_active_set(void(*callback)(void), bool active)
{
	for (uint8_t i=0; i < taskCount; ++i)
	{
		if (tasks[i].callback == callback)
		{
			tasks[i].active = active;
			return;
		}
	}
}