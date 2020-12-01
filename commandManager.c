#include "ecs.h"


static Command **commands;
static uintC size;


void commandManager_init(uintC const commandCount)
{
	commands = checs_malloc(sizeof(Command*) * (size = commandCount));
}


void commandManager_terminate(void)
{
	for (uintC i=0; i < size; ++i)
	{
		checs_free(commands[i]);
	}
	checs_free(commands);
}


void commandManager_command_register(CommandSignature const sig, uintC const callbackCount)
{
	Command *command = checs_malloc(sizeof(Command) + sizeof(CommandCallback) * callbackCount); //because Command struct has variable size array
	command->cap = callbackCount;
	command->size = 0;
	commands[sig] = command;
}


void commandManager_command_publish(CommandSignature const sig, void *const data)
{
	for (uint8_t i=0; i < commands[sig]->size; ++i)
	{
		commands[sig]->callbacks[i](data);
	}
}


void commandManager_command_subscribe(CommandSignature const sig, CommandCallback callback)
{
	checs_assert(commands[sig]->size < commands[sig]->cap);
	commands[sig]->callbacks[commands[sig]->size++] = callback;
}