#include "ecs.h"


static ComponentArray componentsArrays[COMPONENT_COUNT];


void _componentManager_component_register(ComponentSignature const componentSignature, size_t const componentSize)
{
	//-1 because the signatures begin at 1 
	componentsArrays[componentSignature - 1].componentSize = componentSize;
	componentsArrays[componentSignature - 1].componentSignature = componentSignature;
}