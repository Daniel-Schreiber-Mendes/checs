#include "ecs.h"
#include <string.h>

//this eventManager consists of two event buffers. each buffer has its own eventqueues and eventCounts. when for example buffer 0 is 
//active, all events that are polled are polled out of buffer 0 and all events that are send are send into buffer 1. this solves
//the problem that if we had a single buffered queue, we dont't know when to remove the events from the queue because we can
//not say for sure that ever system had the chance to poll them. this is because if we clear the buffer each frame and one event 
//is send by the last buffer for example no one receives the event because the queue directly after gets cleared. and letting the
//systems remove the events is also a bad idea because how does one system know, if every system that needed the events had already
//polled them? and this is also extremely bad for concurrency. a double buffer solves this. the capacitys array is the same for both
//buffers because they should always have the same size, only the content should differ.
static EventQueue eq0, eq1;
static uintE signatureCount;
static EventQueue *hidden = &eq0;
EventQueue *exposed = &eq1;
static uintE *eventCapacitys; //number of maximum events in each eventqueue
 
void eventManager_init(uintE const n_signatureCount)
{
	eventCapacitys = checs_malloc(signatureCount = n_signatureCount);
	eq0.events = checs_malloc(sizeof(void**) * signatureCount);
	eq0.sizes = checs_calloc(signatureCount, 1);
	eq1.events = checs_malloc(sizeof(void**) * signatureCount);
	eq1.sizes = checs_calloc(signatureCount, 1);
}


void eventManager_terminate(void)
{/*
	for (uintE i=0; i < signatureCount; ++i)
	{
		checs_free(exposed->events[i]);
		checs_free(hidden->events[i]);
	}	
	*/
	checs_free(eq0.events);
	checs_free(eq0.sizes);
	checs_free(eq1.events);
	checs_free(eq1.sizes);
	checs_free(eventCapacitys);
}


//swapping the buffers and then clearing the one that is not going to be polled from the next frame
void eventManager_buffers_swap(void)
{
	swap(EventQueue*, hidden, exposed)
	memset(exposed->sizes, 0, signatureCount);
}


void eventManager_event_publish(EventSignature const sig, EventSize const size, void *const data)
{
	if (hidden->sizes[sig] == eventCapacitys[sig])
	{
		hidden->events[sig] = realloc(hidden->events[sig], (eventCapacitys[sig] *= 2) * size);
	}
	memcpy((hidden->events + hidden->sizes[sig]++), data, size);
}


void eventManager_event_register(EventSignature const sig, EventSize const size, uintE const maxEvents)
{
	hidden->events[sig] = checs_malloc(size * maxEvents);
	exposed->events[sig] = checs_malloc(size * maxEvents);
	eventCapacitys[sig] = maxEvents;
}