#include "ecs.h"


void system_construct(System *const sys, SystemCallback callback, ComponentKey const key, uintEC const maxEntitysHint, uintEC const maxEntitysDevnHint)
{
	*sys = (System)
	{
		.sparse = checs_malloc(sizeof(uintEC) * maxEntitysHint), 
		.sparseCapacity = maxEntitysHint,

		.dense  = checs_malloc(sizeof(uintEC) * maxEntitysHint),
		.denseCapacity  = maxEntitysHint, 
		.denseSize = 0,

		.maxEntitysDevnHint = maxEntitysDevnHint,

		.key = key,
		.active = true, 
		.callback = callback
	};
}
//maxEntitysHint tells the system how many entitys the system will approximately at its peak have. maxEntitysDevnHint stand for 
//max-entitys-deviation-hint which says by how much the number of maxmimum entitys could differ. both numbers do not have to be correct.
//if the maxmimum number of entitys is reached the system allocates the difference between the biggest entity currently in the system
//and the new entity. to this number maxEntitysDevnHint is then added. This ensures, that there will always be enough memory for
//the new entitys but it also aloccates not too much or to less because the user can specify how much 
//he thinks the maxEntitys could differ. for the sparse set always the double amount of the memory that gets allocated for dense 
//gets allocated because it needs to grow bigger in size since the entitys are not everytime next to each other

void system_destruct(System const *const sys)
{
	checs_free(sys->dense);
	checs_free(sys->sparse);
}


void system_entity_add(System *const sys, EntityId const entity)
{
	if(entity >= sys->sparseCapacity)
		sys->sparse = realloc(sys->sparse, sizeof(uintEC) * (sys->sparseCapacity = entity + sys->maxEntitysDevnHint * 2));
//	* 2 because sparse can get a lot bigger than dense
	if((sys->sparse[entity] = sys->denseSize++) >= sys->denseCapacity)
		sys->dense = realloc(sys->dense, sizeof(EntityId) * (sys->denseCapacity = entity + sys->maxEntitysDevnHint));

	sys->dense[sys->sparse[entity]] = entity;
}


void system_entity_remove(System *const sys, EntityId const entity)
{
	sys->dense[sys->sparse[entity]] = sys->dense[--sys->denseSize];
	sys->sparse[sys->dense[sys->sparse[entity]]] = sys->dense[sys->sparse[entity]];
}