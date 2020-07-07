#include "ecs.h"


void system_construct(System *const sys, SystemCallback callback, ComponentKey const key, uintEC const maxEntitysHint, EntityAddedCallback const on_entity_added)
{
	*sys = (System)
	{
		.key = key,
		.active = true, 
		.callback = callback,
		.on_entity_added = on_entity_added
	};
	sparseSet_construct(&sys->sparseSet, sizeof(EntityId), maxEntitysHint);
}


void system_destruct(System const *const sys)
{
	sparseSet_destruct(&sys->sparseSet);
}


void system_entity_add(System *const sys, EntityId const entity)
{
	sparseSet_insert(&sys->sparseSet, EntityId, entity);

	if (sys->on_entity_added)
	{
		sys->on_entity_added(entity);
	}
}


void system_entity_remove(System *const sys, EntityId const entity)
{
	sparseSet_erase(&sys->sparseSet, EntityId, entity);
}