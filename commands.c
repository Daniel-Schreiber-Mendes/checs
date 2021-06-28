#include "ecs.h"


static Command **commands;
static uintC size;


void checs_commands_init(uintC const commandCount)
{
	commands = checs_malloc(sizeof(Command*) * (size = commandCount + 1));//+1 because first command is null command
}


void checs_commands_terminate(void)
{
	for (uintC i=0; i < size; ++i)
	{
		checs_free(commands[i]);
	}
	checs_free(commands);
}


void checs_command_register(CommandSignature const sig)
{
	checs_command_assert(sig)
	Command *command = checs_malloc(sizeof(Command) + sizeof(CommandCallback) * 4); //because Command struct has variable size array
	command->cap = 4;
	command->size = 0;
	commands[sig] = command;
}


void checs_command_publish(CommandSignature const sig, void *const data)
{
	checs_command_assert(sig)
	for (uint8_t i=0; i < commands[sig]->size; ++i)
	{
		commands[sig]->callbacks[i](data);
	}
}


void checs_command_subscribe(CommandSignature const sig, CommandCallback callback)
{
	checs_command_assert(sig)
	checs_assert(commands[sig]->size < commands[sig]->cap);
	commands[sig]->callbacks[commands[sig]->size++] = callback;
}