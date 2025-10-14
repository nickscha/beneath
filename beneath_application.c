#include "beneath.h"
#include "vm.h"
#include "camera.h"

static float cube_vertices[] = {
    -0.5f, -0.5f, -0.5f, /* Bottom-left back */
    0.5f, -0.5f, -0.5f,  /* Bottom-right back */
    0.5f, 0.5f, -0.5f,   /* Top-right back */
    -0.5f, 0.5f, -0.5f,  /* Top-left back */

    -0.5f, -0.5f, 0.5f, /* Bottom-left front */
    0.5f, -0.5f, 0.5f,  /* Bottom-right front */
    0.5f, 0.5f, 0.5f,   /* Top-right front */
    -0.5f, 0.5f, 0.5f   /* Top-left front */
};

static unsigned int cube_indices[] = {
    /* Back face (counter-clockwise) */
    0, 3, 2,
    2, 1, 0,

    /* Front face (counter-clockwise) */
    4, 5, 6,
    6, 7, 4,

    /* Left face (counter-clockwise) */
    0, 4, 7,
    7, 3, 0,

    /* Right face (counter-clockwise) */
    1, 2, 6,
    6, 5, 1,

    /*  Bottom face (counter-clockwise)*/
    0, 1, 5,
    5, 4, 0,

    /* Top face (counter-clockwise) */
    3, 7, 6,
    6, 2, 3};

typedef struct app_state
{
    unsigned int test;
    beneath_bool is_fullscreen;

} app_state;

static beneath_mesh mesh = {0};
static beneath_draw_call draw_call = {0};
static m4x4 model;
static float color[3];

void beneath_update(
    beneath_memory *memory,          /* The total block of memory handed to the application */
    beneath_controller_input *input, /* The input (keyboard, mouse, joystick) state */
    beneath_api *api                 /* Platform specific api calls that made accessible for the application */
)
{
    beneath_state *state = (beneath_state *)memory->memory;
    app_state *app = (app_state *)(((unsigned char *)memory->memory) + memory->memory_offset);

    if (!memory->memory_initialized)
    {

        app->test = 1;

        memory->memory_initialized = true;

        api->io_print(__FILE__, __LINE__, "Hello from application :)\n");

        /* Test changing the state */
        beneath_strcpy(state->window_title, "Beneath Testapplication", sizeof(state->window_title));
        state->window_clear_color_r = 0.5f;
        state->window_width = 600;
        state->window_height = 400;
        state->changed_flags = BENEATH_STATE_CHANGED_FLAG_WINDOW;

        model = vm_m4x4_translate(vm_m4x4_identity, vm_v3_zero);

        color[0] = 0.8f;
        color[1] = 0.8f;
        color[2] = 0.8f;

        mesh.id = 0;
        mesh.changed = true;
        mesh.dynamic = true;
        mesh.vertices_count = BENEATH_ARRAY_SIZE(cube_vertices);
        mesh.indices_count = BENEATH_ARRAY_SIZE(cube_indices);
        mesh.vertices = cube_vertices;
        mesh.indices = cube_indices;

        draw_call.id = 0;
        draw_call.data_capacity = 1;
        draw_call.changed = true;
        draw_call.mesh = &mesh;
        draw_call.models = model.e;
        draw_call.models_count = 1;
        draw_call.colors = color;
        draw_call.colors_count = 1;
    }

    if (app->test)
    {
        api->io_print(__FILE__, __LINE__, "Testflag was set\n");
        app->test = 0;
    }

    (void)state;

    if (input->keys[BENEATH_KEY_RETURN].ended_down)
    {
        api->io_print(__FILE__, __LINE__, "Application requested ending\n");
        state->running = false;
    }

    if (input->keys[BENEATH_KEY_F6].active && !app->is_fullscreen)
    {
        state->window_mode = BENEATH_WINDOW_MODE_BORDERLESS;
        state->changed_flags = BENEATH_STATE_CHANGED_FLAG_WINDOW;
        app->is_fullscreen = true;
    }
    else if (!input->keys[BENEATH_KEY_F6].active && app->is_fullscreen)
    {
        state->window_mode = BENEATH_WINDOW_MODE_WINDOWED;
        state->changed_flags = BENEATH_STATE_CHANGED_FLAG_WINDOW;
        app->is_fullscreen = false;
    }

    /* Draw Call Test */
    {
        camera cam = camera_init();
        m4x4 projection;
        m4x4 view;
        m4x4 projection_view;

        cam.position.x = -1.0f;
        cam.position.y = 1.0f;
        cam.position.z = 3.0f;
        camera_update_vectors(&cam);

        projection = vm_m4x4_perspective(vm_radf(cam.fov), (float)state->window_width / (float)state->window_height, 0.1f, 1000.0f);
        view = vm_m4x4_lookAt(cam.position, vm_v3_zero, cam.up);
        projection_view = vm_m4x4_mul(projection, view);

        api->graphics_draw(state, &draw_call, projection_view.e);
    }
}
