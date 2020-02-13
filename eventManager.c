#include "ecs.h"


void ***events; //array of arrys of void pointers
uint8_t *eventCounts; //number of current events in each eventqueue
static uintE signatureCount;

void eventManager_init(uintE const n_signatureCount)
{
	events = calloc_debug(signatureCount = n_signatureCount, sizeof(void**));
	eventCounts = calloc_debug(signatureCount, sizeof(uint8_t));

	for (uintE i=0; i < signatureCount; ++i)
		events[i] = malloc(sizeof(void*) * 10);
}


void eventManager_terminate(void)
{
	for (uintE i=0; i < signatureCount; ++i)
		free(events[i]);
	free_debug(events);
	free_debug(eventCounts);
}


void eventManager_event_publish(EventSignature const signature, void* data)
{
	events[signature][eventCounts[signature]++] = data;
}