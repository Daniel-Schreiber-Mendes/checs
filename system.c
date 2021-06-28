#include "ecs.h"


void system_construct(System *const sys, SystemCallback callback, ComponentKey const key, uintEC const maxEntitysHint, EntityAddedCallback const on_entity_added)
{

}


void system_destruct(System const *const sys)
{
	
}


void system_entity_add(System *const sys, EntityId const entity)
{

}


void system_entity_remove(System *const sys, EntityId const entity)
{
	sparseSet_erase(&sys->sparseSet, EntityId, entity);
}