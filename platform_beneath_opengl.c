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

    /* Draw call fields */
    hash ^= dc->id;
    hash *= prime;
    hash ^= (unsigned int)dc->changed;
    hash *= prime;

    /* Models: 0 = none, 1 = uniform, >1 = layout */
    hash ^= (dc->models_count == 0 ? 0 : (dc->models_count == 1 ? 1 : 2));
    hash *= prime;

    /* Colors: 0 = none, 1 = uniform, >1 = layout */
    hash ^= (dc->colors_count == 0 ? 0 : (dc->colors_count == 1 ? 1 : 2));
    hash *= prime;

    /* Texture indices: 0 = none, 1 = uniform, >1 = layout */
    hash ^= (dc->texture_indices_count == 0 ? 0 : (dc->texture_indices_count == 1 ? 1 : 2));
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

        /* Mesh attributes: 0 = unused, >0 = used as layout */
        hash ^= (m->vertices_count > 0 ? 1 : 0);
        hash *= prime;
        hash ^= (m->uvs_count > 0 ? 1 : 0);
        hash *= prime;
        hash ^= (m->normals_count > 0 ? 1 : 0);
        hash *= prime;
        hash ^= (m->tangents_count > 0 ? 1 : 0);
        hash *= prime;
        hash ^= (m->bitangents_count > 0 ? 1 : 0);
        hash *= prime;

        /* Vertex colors: only used if draw_call colors_count and texture_indices_count are zero */
        beneath_bool use_mesh_color = (m->colors_count > 0 && dc->colors_count == 0 && dc->texture_indices_count == 0);
        hash ^= (use_mesh_color ? 1 : 0);
        hash *= prime;

        /* Indices count probably doesn't affect shader layout, but we can include it for safety */
        hash ^= (m->indices_count > 0 ? 1 : 0);
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

typedef enum beneath_shader_layout
{
    BENEATH_SHADER_LAYOUT_POSITION = 0,                /* Mesh Vertex Position*/
    BENEATH_SHADER_LAYOUT_UV = 1,                      /* Mesh Texture Coordinates */
    BENEATH_SHADER_LAYOUT_NORMAL = 2,                  /* Mesh Normals */
    BENEATH_SHADER_LAYOUT_TANGENT = 3,                 /* Mesh Tangents*/
    BENEATH_SHADER_LAYOUT_BITANGENT = 4,               /* Mesh Bitangents */
    BENEATH_SHADER_LAYOUT_COLOR = 5,                   /* Mesh Vetex Color */
    BENEATH_SHADER_LAYOUT_INSTANCE_MODEL = 6,          /* Instance Model Matrix */
    BENEATH_SHADER_LAYOUT_INSTANCE_COLOR = 10,         /* Instance Color */
    BENEATH_SHADER_LAYOUT_INSTANCE_TEXTURE_INDEX = 13, /* Instance Texture Index */
    BENEATH_SHADER_LAYOUT_COUNT                        /* Terminator */

} beneath_shader_layout;

static char *beneath_shader_layout_names[14] = {
    "position",     /* Mesh Vertex Position*/
    "uv",           /* Mesh Texture Coordinates */
    "normal",       /* Mesh Normals */
    "tangent",      /* Mesh Tangents */
    "bitangent",    /* Mesh Bitangents */
    "color",        /* Mesh Vetex Color */
    "model",        /* Instance Model Matrix */
    "",             /* Padding because instance data stretches accross multiple layout location codes */
    "",             /* Padding because instance data stretches accross multiple layout location codes */
    "",             /* Padding because instance data stretches accross multiple layout location codes */
    "color",        /* Instance Color */
    "",             /* Padding because instance data stretches accross multiple layout location codes */
    "",             /* Padding because instance data stretches accross multiple layout location codes */
    "texture_index" /* Instance Texture Index*/
};

typedef struct beneath_shader
{
    unsigned int hash;
    char *vertex_code;
    char *fragment_code;

} beneath_shader;

static void compile_shader(beneath_draw_call *draw_call)
{
    beneath_bool use_mesh_color = draw_call->mesh->colors_count > 0 && draw_call->colors_count == 0 && draw_call->texture_indices_count == 0;
    unsigned int hash = beneath_hash_draw_call(draw_call);
    int layout_location_current;

    char shader_code_buffer[2048];
    sb sb;
    sb_init(&sb, shader_code_buffer, sizeof(shader_code_buffer));

    sb_append_cstr(&sb, "/* Beneath Vertex Shader (hash=");
    sb_append_ulong(&sb, hash, 8, SB_PAD_NONE);
    sb_append_cstr(&sb, ") */\n");
    sb_append_cstr(&sb, "#version 330 core\n\n");

    /* Layout */
    sb_append_cstr(&sb, "/* Layouts */\n");

    layout_location_current = BENEATH_SHADER_LAYOUT_POSITION;
    sb_printf2(&sb, "layout (location = %d) in vec3 %s;       /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_shader_layout_names[layout_location_current]);

    if (draw_call->mesh->uvs_count > 0)
    {
        layout_location_current = BENEATH_SHADER_LAYOUT_UV;
        sb_printf2(&sb, "layout (location = %d) in vec2 %s;             /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_shader_layout_names[layout_location_current]);
    }

    if (draw_call->mesh->normals_count > 0)
    {
        layout_location_current = BENEATH_SHADER_LAYOUT_NORMAL;
        sb_printf2(&sb, "layout (location = %d) in vec3 %s;         /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_shader_layout_names[layout_location_current]);
    }

    if (draw_call->mesh->tangents_count > 0)
    {
        layout_location_current = BENEATH_SHADER_LAYOUT_TANGENT;
        sb_printf2(&sb, "layout (location = %d) in vec3 %s;        /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_shader_layout_names[layout_location_current]);
    }
    if (draw_call->mesh->bitangents_count > 0)
    {
        layout_location_current = BENEATH_SHADER_LAYOUT_BITANGENT;
        sb_printf2(&sb, "layout (location = %d) in vec3 %s;      /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_shader_layout_names[layout_location_current]);
    }
    if (use_mesh_color)
    {
        layout_location_current = BENEATH_SHADER_LAYOUT_COLOR;
        sb_printf2(&sb, "layout (location = %d) in vec3 %s;          /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_shader_layout_names[layout_location_current]);
    }
    if (draw_call->models_count > 1)
    {
        layout_location_current = BENEATH_SHADER_LAYOUT_INSTANCE_MODEL;
        sb_printf2(&sb, "layout (location = %d) in mat4 %s;          /* Instanced data */\n", (char *)&layout_location_current, (char *)beneath_shader_layout_names[layout_location_current]);
    }
    if (draw_call->colors_count > 1)
    {
        layout_location_current = BENEATH_SHADER_LAYOUT_INSTANCE_COLOR;
        sb_printf2(&sb, "layout (location = %d) in vec3 %s;         /* Instanced data */\n", (char *)&layout_location_current, (char *)beneath_shader_layout_names[layout_location_current]);
    }
    if (draw_call->texture_indices_count > 1)
    {
        layout_location_current = BENEATH_SHADER_LAYOUT_INSTANCE_TEXTURE_INDEX;
        sb_printf2(&sb, "layout (location = %d) in int  %s; /* Instanced data */\n", (char *)&layout_location_current, (char *)beneath_shader_layout_names[layout_location_current]);
    }
    sb_append_cstr(&sb, "\n");

    /* Uniforms */
    sb_append_cstr(&sb, "/* Uniforms */\n");
    sb_append_cstr(&sb, "uniform float time;            /* Global Elapsed seconds             */\n");
    sb_append_cstr(&sb, "uniform float delta_time;      /* Global Seconds since last frame    */\n");
    sb_append_cstr(&sb, "uniform vec2  resolution;      /* Global Screen width and height     */\n");
    sb_append_cstr(&sb, "uniform vec3  camera_position; /* Global World space camera position */\n");
    sb_append_cstr(&sb, "uniform mat4  pv;              /* Global Projection View Matrix      */\n");

    /* If there is only one model it is better to pass it as a uniform and not as a layout */
    if (draw_call->models_count == 1)
    {
        layout_location_current = BENEATH_SHADER_LAYOUT_INSTANCE_MODEL;
        sb_printf1(&sb, "uniform mat4  %s;           /* Instance Model Matrix */\n", (char *)beneath_shader_layout_names[layout_location_current]);
    }

    if (!use_mesh_color && draw_call->colors_count == 1)
    {
        layout_location_current = BENEATH_SHADER_LAYOUT_INSTANCE_COLOR;
        sb_printf1(&sb, "uniform vec3  %s;           /* Instance Color */\n", (char *)beneath_shader_layout_names[layout_location_current]);
    }

    if (!use_mesh_color && draw_call->texture_indices_count == 1)
    {
        layout_location_current = BENEATH_SHADER_LAYOUT_INSTANCE_TEXTURE_INDEX;
        sb_printf1(&sb, "uniform int   %s;   /* Instance Texture Index */\n", (char *)beneath_shader_layout_names[layout_location_current]);
    }

    sb_append_cstr(&sb, "\n");

    /* Outputs */
    sb_append_cstr(&sb, "/* Outputs */\n");
    sb_append_cstr(&sb, "out vec3 v_color;\n");
    sb_append_cstr(&sb, "\n");

    /* Main */
    sb_append_cstr(&sb, "void main()\n{\n");
    sb_append_cstr(&sb, "  v_color     = color;\n");
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
    mesh.indices_count = 6;
    mesh.normals_count = 6;
    mesh.uvs_count = 2;

    beneath_draw_call draw_call = {0};
    draw_call.id = 0;
    draw_call.changed = true;
    draw_call.mesh = &mesh;
    draw_call.models_count = 10;
    draw_call.colors_count = 1;
    draw_call.texture_indices_count = 1;

    compile_shader(&draw_call);
    platform_beneath_draw(&draw_call, 0);
}

int main(void)
{
    test();
    return 0;
}

#endif /* PLATFORM_BENEATH_OPENGL_H */