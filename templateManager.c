#include "ecs.h"


static TemplateCallback *callbacks;


void templateManager_init(uintT const templatecount)
{
	callbacks = checs_malloc(sizeof(TemplateCallback) * templatecount);
}

void templateManager_terminate(void)
{
	checs_free(callbacks);
}


void templateManager_template_register(TemplateSignature const sig, TemplateCallback const cb)
{
	callbacks[sig] = cb;
}


void templateManager_entity_templates_add(EntityId const e, TemplateKey const key)
{
	for (uintT i=0; i < sizeof(TemplateKey) * 8; ++i)
	{
		if ((1 << i & key) == key)
		{
			callbacks[i](e);			
		}
	}
}