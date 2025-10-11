#include "beneath.h"

typedef struct app_state
{
    unsigned int test;

} app_state;

static beneath_bool is_fullscreen;

void beneath_update(
    beneath_memory *memory,          /* The total block of memory handed to the application */
    beneath_controller_input *input, /* The input (keyboard, mouse, joystick) state */
    beneath_api *api                 /* Platform specific api calls that made accessible for the application */
)
{
    beneath_state *state = (beneath_state *)memory->memory;
    app_state *state_app = (app_state *)(((unsigned char *)memory->memory) + memory->memory_offset);

    if (!memory->memory_initialized)
    {
        state_app->test = 1;

        memory->memory_initialized = true;

        api->io_print(__FILE__, __LINE__, "Hello from application :)\n");

        /* Test changing the state */
        beneath_strcpy(state->window_title, "Application XY", sizeof(state->window_title));
        state->window_clear_color_r = 0.5f;
        state->window_width = 600;
        state->window_height = 400;
        state->changed_flags = BENEATH_STATE_CHANGED_FLAG_WINDOW;
    }

    if (state_app->test)
    {
        api->io_print(__FILE__, __LINE__, "Testflag was set\n");
        state_app->test = 0;
    }

    api->time_sleep(1);

    (void)state;

    if (input->keys[BENEATH_KEY_RETURN].ended_down)
    {
        api->io_print(__FILE__, __LINE__, "Application requested ending\n");
        state->running = false;
    }

    if (input->keys[BENEATH_KEY_F6].active && !is_fullscreen)
    {
        state->window_mode = BENEATH_WINDOW_MODE_BORDERLESS;
        state->changed_flags = BENEATH_STATE_CHANGED_FLAG_WINDOW;
        is_fullscreen = true;
    }
    else if (!input->keys[BENEATH_KEY_F6].active && is_fullscreen)
    {
        state->window_mode = BENEATH_WINDOW_MODE_WINDOWED;
        state->changed_flags = BENEATH_STATE_CHANGED_FLAG_WINDOW;
        is_fullscreen = false;
    }
}
