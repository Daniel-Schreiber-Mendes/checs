#include "ecs.h"

static SparseSet* sets;
static uintCS componentCount;

void componentManager_init(uintCS const n_componentCount)
{
	componentCount = n_componentCount;
	sets = (componentCount > 0) ? (SparseSet*)malloc_debug(sizeof(SparseSet) * componentCount) : NULL;
}


void componentManager_terminate(void)
{
	//ckeck if pointers are null because that could be if there are no components registered
	if(sets)
	{	
		for(uintCS i=0; i < componentCount; ++i)
			if(sets[i].sparse)
			{
				free_debug(sets[i].sparse);
				free_debug(sets[i].dense);
			}
		free_debug(sets);
	}
}


void _componentManager_component_register(ComponentSignature const signature, size_t const componentSize)
{
	//-1 because the signatures begin at 1 
	SparseSet* set = &sets[signature - 1];
	*set = (SparseSet){.componentSize = componentSize, .signature = signature};
	set->sparse = malloc_debug(sizeof(uintEC) * 1);
	set->dense = malloc_debug(set->componentSize * 1);
}


SparseSet* componentManager_sparseSet_get(ComponentSignature const signature)
{
	return &sets[signature - 1];
}