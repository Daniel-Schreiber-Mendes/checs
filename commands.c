#include <checs.h>


typedef struct
{
	uint8_t size, cap;
	void(**callbacks)(void*);;
	bool registered;
}
Command;


static void command_sig_check(CommandSignature const sig);


static Command *commands;
static uint8_t commandCount = 1; //number of different commands registered


void checs_commands_init(void)
{
	
}


void checs_commands_terminate(void)
{
	for (uint8_t i=1; i < commandCount; ++i)
	{
		free(commands[i].callbacks);
	}
	free(commands);
}


void checs_command_register(CommandSignature const sig)
{
	checs_assert_msg(sig != 0, "Tried to register NULL Command");

	if (sig >= commandCount)
	{
		++commandCount;
		commands = realloc(commands, sizeof(Command) * commandCount);
	}

	commands[sig] = (Command){
		.size = 0,
		.cap = 1,
		.callbacks = NULL,
		.registered = true
	};
}


void checs_command_publish(CommandSignature const sig, void *const data)
{
	command_sig_check(sig);

	for (uint8_t i=0; i < commands[sig].size; ++i)
	{
		commands[sig].callbacks[i](data);
	}
}


void checs_command_subscribe(CommandSignature const sig, void(*callback)(void*))
{
	command_sig_check(sig);

	if (commands[sig].size + 1 == commands[sig].cap)
	{
		commands[sig].cap *= 2;
		commands[sig].callbacks = realloc(commands[sig].callbacks, commands[sig].cap * sizeof(void*));
	}

	commands[sig].callbacks[commands[sig].size] = callback;
	commands[sig].size++;
}


void checs_command_unsuscribe(ComponentSignature const sig, void(*callback)(void*))
{
	command_sig_check(sig);

	for (uint8_t i=0; i < commands[sig].size; ++i)
	{
		if (commands[sig].callbacks[i] == callback)
		{
			commands[sig].callbacks[i] = commands[sig].callbacks[--commands[sig].size]; //move last command to current position and decrease size by 1
			return;
		}
	}
	checs_assert_msg(false, "Tried to remove CommandCallback that couldnt be found");
}

void checs_command_unsuscribe_all(ComponentSignature const sig)
{
	command_sig_check(sig);
	commands[sig].size = 0;
}


static void command_sig_check(CommandSignature sig)
{
	checs_assert_msg(sig != 0, "Tried to acess NULL Command");
	checs_assert_msg(sig < commandCount, "Command Signature too high");
	checs_assert_msg(commands[sig].registered, "Used Command that has not been registered");
	/* TODO: can allocating 0 bytes return a null pointer? if so then rewrite code @ event_register */
}