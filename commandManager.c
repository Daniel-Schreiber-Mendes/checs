#include "ecs.h"


HashMap commands; //map of array of arrays of callbacks that are called when a command occurs


void commandManager_init(uintC const commandCount)
{
	hashMap_construct(&commands, commandCount);
}


void commandManager_terminate(void)
{
	hashMap_foreach(&commands, Command*, command,
	({
		checs_free(command);
	}));
	hashMap_destruct(&commands);
}


void commandManager_command_register(CommandSignature const sig, uintC const callbackCount)
{
	Command *command = checs_malloc(sizeof(Command) + sizeof(CommandCallback) * callbackCount); //because Command struct has variable size array
	command->cap = callbackCount;
	command->size = 0;
	hashMap_insert(&commands, sig, command);
}


void commandManager_command_publish(CommandSignature const sig, void *const data)
{
	for (uint8_t i=0; i < hashMap_get(&commands, Command, sig)->size; ++i)
	{
		hashMap_get(&commands, Command, sig)->callbacks[i](data);
	}
}


void commandManager_command_subscribe(CommandSignature const sig, CommandCallback callback)
{
	checs_assert(hashMap_get(&commands, Command, sig)->size < hashMap_get(&commands, Command, sig)->cap)
	hashMap_get(&commands, Command, sig)->callbacks[hashMap_get(&commands, Command, sig)->size++] = callback;
}