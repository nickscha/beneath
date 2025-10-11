#ifndef PLATFORM_BENEATH_OPENGL_H
#define PLATFORM_BENEATH_OPENGL_H

#include "beneath.h"
#include "sb.h"

typedef struct beneath_mesh
{
    unsigned int id;
    beneath_bool changed; /* If the mesh has changed we may need to initialize it again in the platform layer */
    beneath_bool dynamic;

    unsigned int vertices_count;
    unsigned int uvs_count;
    unsigned int normals_count;
    unsigned int tangents_count;
    unsigned int bitangents_count;
    unsigned int colors_count;
    unsigned int indices_count;

    float *vertices;
    float *uvs;
    float *normals;
    float *tangents;
    float *bitangents;
    float *colors;
    int *indices;

} beneath_mesh;

/* SoA style draw call */
typedef struct beneath_draw_call
{
    unsigned int id;
    beneath_bool changed; /* Did the draw call change? If yes we may need to resend buffer data */

    beneath_mesh *mesh;

    unsigned int models_count;          /* Number of models == Number of instances */
    unsigned int colors_count;          /* Per model color */
    unsigned int texture_indices_count; /* Per model texture_index. If a texture index is specified we ignore colors */

    float *models;
    float *colors;
    int *texture_indices;

} beneath_draw_call;

static unsigned int beneath_hash_draw_call(beneath_draw_call *dc)
{
    unsigned int hash = 2166136261u; /* FNV-1a offset basis */
    unsigned int prime = 16777619u;  /* FNV-1a prime */

    /* draw call fields */
    hash ^= dc->id;
    hash *= prime;
    hash ^= (unsigned int)dc->changed;
    hash *= prime;
    hash ^= dc->models_count;
    hash *= prime;
    hash ^= dc->colors_count;
    hash *= prime;
    hash ^= dc->texture_indices_count;
    hash *= prime;

    if (dc->mesh)
    {
        beneath_mesh *m = dc->mesh;

        hash ^= m->id;
        hash *= prime;
        hash ^= (unsigned int)m->changed;
        hash *= prime;
        hash ^= (unsigned int)m->dynamic;
        hash *= prime;

        hash ^= m->vertices_count;
        hash *= prime;
        hash ^= m->uvs_count;
        hash *= prime;
        hash ^= m->normals_count;
        hash *= prime;
        hash ^= m->tangents_count;
        hash *= prime;
        hash ^= m->bitangents_count;
        hash *= prime;
        hash ^= m->colors_count;
        hash *= prime;
        hash ^= m->indices_count;
        hash *= prime;
    }

    return hash;
}

static void platform_beneath_draw(
    beneath_draw_call *draw_call,
    float projection_view[16])
{
    if (!draw_call || draw_call->models_count == 0)
    {
        return;
    }

    if (draw_call->colors_count == 0 && draw_call->texture_indices_count == 0)
    {
        /* Use the mesh color per vertex information */
    }

    /* if models = 1 -> normal single draw call       */
    /* if models > 1 -> instanced draw call           */
    /* if colors = 1 -> bind uniform for single color */
    /* if colors > 1 -> instanced attribute pointer for color */
    /* if texture_indices = 1 -> bind uniform for single texture_index */
    /* if texture_indices > 1 -> instanced attribute pointer for texture_indices */
}

#include "stdio.h"

static void compile_shader(beneath_draw_call *draw_call)
{
    beneath_bool use_mesh_color = draw_call->mesh->colors_count > 0 && draw_call->colors_count == 0 && draw_call->texture_indices_count == 0;
    unsigned int hash = beneath_hash_draw_call(draw_call);

    char shader_code_buffer[1024];
    sb sb;
    sb_init(&sb, shader_code_buffer, sizeof(shader_code_buffer));

    sb_append_cstr(&sb, "/* Beneath Vertex Shader (hash=");
    sb_append_ulong(&sb, hash, 8, SB_PAD_NONE);
    sb_append_cstr(&sb, ") */\n");
    sb_append_cstr(&sb, "#version 330 core\n\n");

    /* Layout */
    sb_append_cstr(&sb, "/* Layouts */\n");
    sb_append_cstr(&sb, "layout (location = 0) in vec3 position;       /* Mesh data      */\n");

    if (draw_call->mesh->uvs_count > 0)
    {
        sb_append_cstr(&sb, "layout (location = 1) in vec2 uv;             /* Mesh data      */\n");
    }

    if (draw_call->mesh->normals_count > 0)
    {
        sb_append_cstr(&sb, "layout (location = 2) in vec3 normal;         /* Mesh data      */\n");
    }

    if (draw_call->mesh->tangents_count > 0)
    {
        sb_append_cstr(&sb, "layout (location = 3) in vec3 tangent;        /* Mesh data      */\n");
    }
    if (draw_call->mesh->bitangents_count > 0)
    {
        sb_append_cstr(&sb, "layout (location = 4) in vec3 bitangent;      /* Mesh data      */\n");
    }
    if (use_mesh_color)
    {
        sb_append_cstr(&sb, "layout (location = 5) in vec3 color;          /* Mesh data      */\n");
    }
    if (draw_call->models_count > 1)
    {
        sb_append_cstr(&sb, "layout (location = 6) in mat4 model;          /* Instanced data */\n");
    }
    if (draw_call->colors_count > 1)
    {
        sb_append_cstr(&sb, "layout (location = 10) in vec3 color;         /* Instanced data */\n");
    }
    if (draw_call->texture_indices_count > 1)
    {
        sb_append_cstr(&sb, "layout (location = 13) in vec3 texture_index; /* Instanced data */\n");
    }
    sb_append_cstr(&sb, "\n");

    /* Uniforms */
    sb_append_cstr(&sb, "/* Uniforms */\n");

    sb_append_cstr(&sb, "uniform float time;            /* Elapsed seconds             */\n");
    sb_append_cstr(&sb, "uniform float delta_time;      /* Seconds since last frame    */\n");
    sb_append_cstr(&sb, "uniform vec2  resolution;      /* Screen width and height     */\n");
    sb_append_cstr(&sb, "uniform vec3  camera_position; /* World space camera position */\n");
    sb_append_cstr(&sb, "uniform mat4  pv;\n");

    /* If there is only one model it is better to pass it as a uniform and not as a layout */
    if (draw_call->models_count == 1)
    {
        sb_append_cstr(&sb, "uniform mat4  model;\n");
    }

    if (!use_mesh_color && draw_call->colors_count == 1)
    {
        sb_append_cstr(&sb, "uniform vec3  color;\n");
    }

    if (!use_mesh_color && draw_call->texture_indices_count == 1)
    {
        sb_append_cstr(&sb, "uniform int  texture_index;\n");
    }

    sb_append_cstr(&sb, "\n");

    /* Outputs */
    sb_append_cstr(&sb, "/* Outputs */\n");
    sb_append_cstr(&sb, "out vec3 out_color;\n");
    sb_append_cstr(&sb, "\n");

    /* Main */
    sb_append_cstr(&sb, "void main()\n{\n");
    sb_append_cstr(&sb, "  out_color = color;\n");
    sb_append_cstr(&sb, "  \n");
    sb_append_cstr(&sb, "  gl_Position = pv * model * vec4(position, 1.0f);\n");
    sb_append_cstr(&sb, "}\n\n");

    sb_term(&sb);

    printf("%s\n", sb.buf);
}

void test(void)
{
    beneath_mesh mesh = {0};
    mesh.id = 0;
    mesh.changed = true;
    mesh.dynamic = true;
    mesh.vertices_count = 12;
    mesh.indices_count = 8;
    mesh.uvs_count = 1;
    mesh.normals_count = 6;

    beneath_draw_call draw_call = {0};
    draw_call.id = 0;
    draw_call.changed = true;
    draw_call.mesh = &mesh;
    draw_call.models_count = 1;
    draw_call.colors_count = 1;
    draw_call.texture_indices_count = 0;

    compile_shader(&draw_call);
    platform_beneath_draw(&draw_call, 0);
}

int main(void)
{
    test();
    return 0;
}

#endif /* PLATFORM_BENEATH_OPENGL_H */