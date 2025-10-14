#include "win32_beneath_opengl.h"

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

static int cube_indices[] = {
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

#include "stdio.h"

int main(void)
{
    beneath_mesh mesh = {0};
    beneath_draw_call draw_call = {0};
    float projection_view[16];
    float model[16];
    float color[3];

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
    draw_call.models = model;
    draw_call.models_count = 1;
    draw_call.colors = color;
    draw_call.colors_count = 1;

    char vertex_code[2048];
    char fragment_code[2048];

    beneath_bool success = beneath_opengl_shader_generate(
        &draw_call,
        vertex_code, BENEATH_ARRAY_SIZE(vertex_code),
        fragment_code, BENEATH_ARRAY_SIZE(fragment_code));

    if (!success)
    {
        printf("[opengl] failed to generate shaders!\n");
    }

    printf("[opengl] Vertex Shader Code:\n%s", vertex_code);
    printf("[opengl] Fragment Shader Code:\n%s", fragment_code);

    return 0;
}