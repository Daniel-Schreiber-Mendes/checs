# checs


Examples:

```C
#include<checs/checs.h>

#define PositionComponent 0
typedef struct
{
    int x, y;
}
Position;

#define MovementComponent 1
typedef struct
{
    int dx, dy;
}
Movement;



void movementSystem(checs_system_parameters)
{
    checs_component_use(Position, p);
    checs_component_use(Movement, m);

    checs_entity_foreach(entity)
    {
	checs_component_get(Position, p, entity);
    	checs_component_get(Movement, m, entity);
    
    	p->x += m->dx;
    	p->y += m->dy;
    }
}

int main(void)
{
    systemManager_init(1, 0, 0, 0);
    entityManager_init();
    componentManager_init(2, 1000, 5);
    commandManager_init(0);
    eventManager_init(0);
    
    checs_component_register(Position, 1000, 5);
    checs_component_register(Movement, 1000, 5);
    checs_system_register(movementSystem, ON_UPDATE, 1000, 5, Position, Movement);

    for (uintEC i=0; i < 1000; ++i)
    {
    	checs_entity_generate(Position, Movement);
    }
    
    while(1)
    {
        checs_systems_call(UPDATE);
    }
}
```
