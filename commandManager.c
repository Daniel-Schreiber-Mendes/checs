#include "ecs.h"

//I know 2D arrays are slow but it is not possible to put everything inside a 1D array because the size changes at runtime
static CommandCallback **callbacks; //array of arrays of callbacks that are called when a command occurs
static uint8_t *callbackCounts; //array of numbers of registered callbacks for every commandtype

static CommandSignature *signatures;
static uintC signatureCount; //number of different types of commands


void commandManager_init(uintC const n_signatureCount)
{
	callbacks = checs_calloc((signatureCount = n_signatureCount), sizeof(CommandCallback*));
	//why calloc? in command_subscribe when we realloc the array for the first time, the pointer does not point to a valid place in 
	//memory yet. to indicate this, it has to be NULL
	signatures = checs_malloc(sizeof(CommandSignature) * signatureCount);
	callbackCounts = checs_calloc(signatureCount, sizeof(uint8_t)); //every count starts with 0
}


void commandManager_terminate(void)
{
	for (uintC i=0; i < signatureCount; ++i)
	{
		checs_free(callbacks[i]);
	}
	checs_free(callbacks);

	checs_free(signatures);
	checs_free(callbackCounts);
}


void commandManager_command_publish(CommandSignature const signature, void* data)
{
	for (uint8_t i=0; i < callbackCounts[signature]; ++i)
	{
		callbacks[signature][callbackCounts[i]](data);
	}
}


void commandManager_command_subscribe(CommandSignature const signature, CommandCallback callback)
{
	callbacks[signature] = realloc(callbacks[signature], sizeof(CommandCallback) * ++callbackCounts[signature]);
	callbacks[signature][callbackCounts[signature]] = callback;
}