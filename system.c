#include "ecs.h"


void system_construct(System* sys)
{
	*sys = (System){.sparse = malloc_debug(sizeof(uintEC) * 8), .sparseCapacity = 8,
					.dense  = malloc_debug(sizeof(uintEC) * 8), .denseCapacity  = 8, .denseSize = 0,
					.active = true};
}


void system_destruct(System const *const sys)
{
	free_debug(sys->dense);
	free_debug(sys->sparse);
}


void system_entity_add(System *const sys, EntityId const entity)
{
	if(entity >= sys->sparseCapacity)
	{
		sys->sparse = realloc(sys->sparse, sizeof(uintEC) * (entity + 8));
		sys->sparseCapacity = entity + 8;
	}
	sys->sparse[entity] = sys->denseSize++;
}


void system_entity_remove(System *const sys, EntityId const entity)
{
	sys->dense[sys->sparse[entity]] = sys->dense[sys->denseSize--];
	sys->sparse[entity] = 0;
}