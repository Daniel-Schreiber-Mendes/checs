#include "ecs.h"

static uint8_t componentCount;
static ComponentArray* componentArrays;


void componentManager_init(uint8_t const n_componentCount)
{
	componentCount = n_componentCount;
	componentArrays = (ComponentArray*)malloc(sizeof(ComponentArray) * componentCount);
}


void _componentManager_component_register(ComponentSignature const componentSignature, size_t const componentSize)
{
	//-1 because the signatures begin at 1 
	componentArrays[componentSignature - 1] = (ComponentArray){.componentSize = componentSize, .componentSignature = componentSignature};
}


ComponentArray* _componentManager_componentArray_get(ComponentSignature const componentSignature)
{
	return &componentArrays[componentSignature - 1];
}