#include <checs.h>


void checs_init(void)
{
	ezd_init();
	checs_entitys_init();
	checs_components_init();
	checs_commands_init();
}


void checs_run(void)
{
	checs_stateMachine_run();
}


void checs_terminate(void)
{
	checs_entitys_terminate();
	checs_systems_terminate();
	checs_components_terminate();
	checs_commands_terminate();
	ezd_terminate();
}


void checs_update(void)
{
	checs_systems(CHECS_ON_UPDATE);
}


void checs_draw(void)
{
	checs_systems(CHECS_ON_DRAW);
}


void checs_reset(void)
{
	checs_entitys_reset();
	checs_components_reset();
	checs_tags_reset();
}

//save all data in file
void checs_save(void)
{

}