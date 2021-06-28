#include "ecs.h"


void checs_init(uint8_t systemUpdateCount, uint8_t systemDrawCount, uint8_t taskUpdateCount, uint8_t taskDrawCount, uint8_t tag_count, uint8_t componentCount, uint8_t commandCount, uint8_t eventCount, uint8_t maxAttributesHint)
{
	ezd_init();
	checs_systems_init(systemUpdateCount, systemDrawCount, taskUpdateCount, taskDrawCount);
	checs_entitys_init(tag_count);
	checs_components_init(componentCount);
	checs_commands_init(commandCount);
	checs_events_init(eventCount);
	attributeManager_init(maxAttributesHint);
}


void checs_terminate()
{
	checs_entitys_terminate();
	checs_systems_terminate();
	checs_components_terminate();
	checs_commands_terminate();
	checs_events_terminate();
	attributeManager_terminate();
	ezd_terminate();
}