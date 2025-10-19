#include "beneath.h"
#include "vm.h"
#include "sb.h"
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

/* Per-vertex normals */
static float cube_normals[] = {
    /* 0: Bottom-left back  */ -0.577f, -0.577f, -0.577f,
    /* 1: Bottom-right back */ 0.577f, -0.577f, -0.577f,
    /* 2: Top-right back    */ 0.577f, 0.577f, -0.577f,
    /* 3: Top-left back     */ -0.577f, 0.577f, -0.577f,

    /* 4: Bottom-left front */ -0.577f, -0.577f, 0.577f,
    /* 5: Bottom-right front*/ 0.577f, -0.577f, 0.577f,
    /* 6: Top-right front   */ 0.577f, 0.577f, 0.577f,
    /* 7: Top-left front    */ -0.577f, 0.577f, 0.577f};

/* Per-vertex color (R, G, B) - normalized to 0..1 range like a normal map */
static float cube_colors[] = {
    0.25f, 0.25f, 0.25f,
    0.75f, 0.25f, 0.25f,
    0.75f, 0.75f, 0.25f,
    0.25f, 0.75f, 0.25f,
    0.25f, 0.25f, 0.75f,
    0.75f, 0.25f, 0.75f,
    0.75f, 0.75f, 0.75f,
    0.25f, 0.75f, 0.75f};

typedef struct app_state
{
    unsigned int test;
    beneath_bool is_fullscreen;

} app_state;

static float models[16 * 16];
static beneath_mesh mesh = {0};
static beneath_draw_call draw_call = {0};
static beneath_lightning ligthning = {0};
static m4x4 model;
static m4x4 model_floor;
static m4x4 model_other;
static m4x4 model_next;
static camera cam;
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

        /* Temporary Tests */
        cam = camera_init();
        cam.position.x = -1.0f;
        cam.position.y = 1.0f;
        cam.position.z = 3.0f;
        camera_update_vectors(&cam);

        model = vm_m4x4_translate(vm_m4x4_identity, vm_v3_zero);
        model_floor = vm_m4x4_scale(vm_m4x4_translate(vm_m4x4_identity, vm_v3(0.0f, -1.0f, 0.0f)), vm_v3(10.0f, 0.1f, 10.0f));
        model_other = vm_m4x4_translate(vm_m4x4_identity, vm_v3(-2.0f, 2.0f, 0.5f));
        model_next = vm_m4x4_scale(vm_m4x4_translate(vm_m4x4_identity, vm_v3(1.0f, 1.0f, -1.0f)), vm_v3(0.2f, 0.2f, 5.0f));

        color[0] = 0.8f;
        color[1] = 0.8f;
        color[2] = 0.8f;

        mesh.id = 0;
        mesh.changed = true;
        mesh.dynamic = true;
        mesh.vertices_count = BENEATH_ARRAY_SIZE(cube_vertices);
        mesh.indices_count = BENEATH_ARRAY_SIZE(cube_indices);
        mesh.normals_count = BENEATH_ARRAY_SIZE(cube_normals);
        mesh.colors_count = BENEATH_ARRAY_SIZE(cube_colors);
        mesh.vertices = cube_vertices;
        mesh.normals = cube_normals;
        mesh.indices = cube_indices;
        mesh.colors = cube_colors;

        draw_call.id = 0;
        draw_call.data_capacity = 16;
        draw_call.changed = true;
        draw_call.mesh = &mesh;

        draw_call.models = models;

        beneath_draw_call_append(&draw_call, model.e, (void *)0, -1);
        beneath_draw_call_append(&draw_call, model_floor.e, (void *)0, -1);
        beneath_draw_call_append(&draw_call, model_other.e, (void *)0, -1);
        beneath_draw_call_append(&draw_call, model_next.e, (void *)0, -1);

        /*
        draw_call.colors = color;
        draw_call.colors_count = 1;
        */

        /* Setup ligthning*/
        {
            v3 dl_direction = vm_v3_normalize(vm_v3(5.0f, -7.0f, -4.0f));

            /* Colors */
            v3 dl_ambient = vm_v3(0.3f, 0.17f, 0.15f);
            v3 dl_diffuse = vm_v3(1.0f, 0.95f, 0.8f);
            v3 dl_specular = vm_v3(0.3f, 0.3f, 0.3f);

            ligthning.directional.direction[0] = dl_direction.x;
            ligthning.directional.direction[1] = dl_direction.y;
            ligthning.directional.direction[2] = dl_direction.z;
            ligthning.directional.ambient[0] = dl_ambient.x;
            ligthning.directional.ambient[1] = dl_ambient.y;
            ligthning.directional.ambient[2] = dl_ambient.z;
            ligthning.directional.diffuse[0] = dl_diffuse.x;
            ligthning.directional.diffuse[1] = dl_diffuse.y;
            ligthning.directional.diffuse[2] = dl_diffuse.z;
            ligthning.directional.specular[0] = dl_specular.x;
            ligthning.directional.specular[1] = dl_specular.y;
            ligthning.directional.specular[2] = dl_specular.z;

            draw_call.lightning = &ligthning;
        }
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

    if (input->keys[BENEATH_KEY_W].ended_down)
    {
        cam.position.z -= 5.0f * (float)state->delta_time;
    }

    if (input->keys[BENEATH_KEY_A].ended_down)
    {
        cam.position.x -= 5.0f * (float)state->delta_time;
    }

    if (input->keys[BENEATH_KEY_S].ended_down)
    {
        cam.position.z += 5.0f * (float)state->delta_time;
    }

    if (input->keys[BENEATH_KEY_D].ended_down)
    {
        cam.position.x += 5.0f * (float)state->delta_time;
    }

    if (input->keys[BENEATH_KEY_CONTROL].ended_down)
    {
        cam.position.y -= 5.0f * (float)state->delta_time;
    }

    if (input->keys[BENEATH_KEY_SPACE].ended_down)
    {
        cam.position.y += 5.0f * (float)state->delta_time;
    }

    if (input->keys[BENEATH_KEY_F5].pressed)
    {
        cam.position.x = -1.0f;
        cam.position.y = 1.0f;
        cam.position.z = 3.0f;
    }

    draw_call.pixelize = input->keys[BENEATH_KEY_F1].active;
    draw_call.shadow = true;
    draw_call.volumetric = true;

    /* Print FPS */
    if (input->keys[BENEATH_KEY_F2].pressed)
    {
        char buffer[128];
        sb dt = {0};
        sb_init(&dt, buffer, 128);
        sb_append_cstr(&dt, "[fps] : ");
        sb_append_ulong(&dt, state->frames_per_second, 8, SB_PAD_LEFT);
        sb_append_cstr(&dt, "\n");
        sb_term(&dt);
        api->io_print(__FILE__, __LINE__, buffer);
    }

    /* Draw Call Test */
    {
        m4x4 projection;
        m4x4 projection_inverse;
        m4x4 view;
        m4x4 view_inverse;
        m4x4 projection_view;
        float *camera_pos;

        projection = vm_m4x4_perspective(vm_radf(cam.fov), (float)state->window_width / (float)state->window_height, 0.1f, 1000.0f);
        projection_inverse = vm_m4x4_inverse(projection);
        view = vm_m4x4_lookAt(cam.position, vm_v3_zero, cam.up);
        view_inverse = vm_m4x4_inverse(view);
        projection_view = vm_m4x4_mul(projection, view);
        camera_pos = vm_v3_data(&cam.position);

        api->graphics_draw(
            state,
            &draw_call,
            projection_view.e,
            projection_inverse.e,
            view_inverse.e,
            camera_pos);
    }
}
