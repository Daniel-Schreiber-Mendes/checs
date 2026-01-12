#include <checs.h>
#include <time.h>

static State states[8];
static uint8_t state_count;
static bool running = true;

static float ms_per_frame;
static uint16_t frameCount;
static clock_t time_start;


static void stateMachine_run(void);

//a state shall only be popped inside state.update, NEVER in state.draw, reasons are explained below
void checs_state_pop(State *const state)
{
    assert(state == &states[state_count - 1]);
    --state_count;
    state->destruct(state);
}


/* it is important that the draw function is called before update because when a state is popped in update and then the draw
function would be called, one would access memory that already got freed */
void checs_stateMachine_run(void)
{
	while(running)
	{                
        for (uint8_t i=0; i < state_count; ++i)
        {
            State *state = &states[i];
            state->draw(state);
            state->events(state);
            state->update(state); 


            if (++frameCount == 100)
            {
                frameCount = 0;
                clock_t time_now = clock(); 
                ms_per_frame =  (time_now - time_start) / (float)CLOCKS_PER_SEC * 10;
                time_start = time_now;
            }
        }
	}

    for (uint8_t i=0; i < state_count; ++i)
    {
        states[i].destruct(&states[i]);
    }
}


float checs_ms_per_frame_get(void)
{
    return ms_per_frame;
}


void checs_state_push(void(*construct)(State *const))
{
    construct(&states[state_count++]);
}


void checs_running_set(bool const set)
{
    running = set;
}