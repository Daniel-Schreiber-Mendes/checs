#include "ecs.h"

//this eventManager consists of two event buffers. each buffer has its own eventqueues and eventCounts. when for example buffer 0 is 
//active, all events that are polled are polled out of buffer 0 and all events that are send are send into buffer 1. this solves
//the problem that if we had a single buffered queue, we dont't know when to remove the events from the queue because we can
//not say for sure that ever system had the chance to poll them. this is because if we clear the buffer each frame and one event 
//is send by the last buffer for example no one receives the event because the queue directly after gets cleared. and letting the
//systems remove the events is also a bad idea because how does one system know, if every system that needed the events had already
//polled them? and this is also extremely bad for concurrency. a double buffer solves this. the capacitys array is the same for both
//buffers because they should always have the same size, only the content should differ.


//double buffered event queue
//db stands for double buffer
void **events_db[2];
uint8_t *eventCounts_db[2]; //number of current events in each event queue
uint8_t db_index = 0; 
uint8_t *eventCapacitys; //number of maximum events in each eventqueue

static uintE signatureCount;

void eventManager_init(uintE const n_signatureCount)
{
	for (uint8_t i=0; i < 2; ++i)
	{
		events_db[i] = malloc_debug(sizeof(void**) * (signatureCount = n_signatureCount));
		eventCounts_db[i] = calloc_debug(signatureCount, sizeof(uint8_t));
	}
	eventCapacitys = malloc_debug(sizeof(uint8_t) * signatureCount);
}


void eventManager_terminate(void)
{
	for (uint8_t i=0; i < 2; ++i)
	{
		for (uintE j=0; j < signatureCount; ++j)
		{
			//free(events_db[i][j]);
		}
		free_debug(events_db[i]);
		free_debug(eventCounts_db[i]);
	}
	free_debug(eventCapacitys);
}


void eventManager_buffers_swap(void)
{
	db_index = 1 - db_index;
	memset(eventCounts_db[db_index], 0, sizeof(uint8_t) * signatureCount);
	//swapping the buffers and then clearing the one that is now not going to be used
}