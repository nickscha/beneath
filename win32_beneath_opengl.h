#ifndef WIN32_BENEATH_OPENGL
#define WIN32_BENEATH_OPENGL

#include "beneath.h"
#include "win32_opengl.h"
#include "sb.h"

/******************************/
/* Types & Structs            */
/******************************/
typedef enum beneath_opengl_shader_layout
{
    BENEATH_OPENGL_SHADER_LAYOUT_POSITION = 0,                /* Mesh Vertex Position*/
    BENEATH_OPENGL_SHADER_LAYOUT_UV = 1,                      /* Mesh Texture Coordinates */
    BENEATH_OPENGL_SHADER_LAYOUT_NORMAL = 2,                  /* Mesh Normals */
    BENEATH_OPENGL_SHADER_LAYOUT_TANGENT = 3,                 /* Mesh Tangents*/
    BENEATH_OPENGL_SHADER_LAYOUT_BITANGENT = 4,               /* Mesh Bitangents */
    BENEATH_OPENGL_SHADER_LAYOUT_COLOR = 5,                   /* Mesh Vetex Color */
    BENEATH_OPENGL_SHADER_LAYOUT_INSTANCE_MODEL = 6,          /* Instance Model Matrix */
    BENEATH_OPENGL_SHADER_LAYOUT_INSTANCE_COLOR = 10,         /* Instance Color */
    BENEATH_OPENGL_SHADER_LAYOUT_INSTANCE_TEXTURE_INDEX = 13, /* Instance Texture Index */
    BENEATH_OPENGL_SHADER_LAYOUT_COUNT                        /* Terminator */

} beneath_opengl_shader_layout;

static char *beneath_opengl_shader_layout_names[14] = {
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

typedef enum beneath_opengl_shader_uniform_locations
{
    BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_TIME = 0,
    BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_DELTA_TIME,
    BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_RESOLUTION,
    BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_CAMERA_POSITION,
    BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_PROJECTION_VIEW,

    BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_INSTANCE_COLOR,
    BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_INSTANCE_TEXTURE_INDEX,

    BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_COUNT

} beneath_opengl_shader_uniform_locations;

static char *beneath_opengl_shader_uniform_names[8] = {
    "time",
    "delta_time",
    "resolution",
    "camera_position",
    "pv",
    "color",
    "texture_index"};

typedef struct beneath_opengl_shader
{
    unsigned int program_id;
    unsigned int hash;
    char code_vertex[2048];
    char code_fragment[2048];
    int uniform_locations[BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_COUNT];

} beneath_opengl_shader;

#define BENEATH_OPENGL_SHADERS_MAX 16
#define BENEATH_OPENGL_MESHES_MAX 64

typedef struct beneath_opengl_context
{
    beneath_bool initialized;

    /* Pregenerated Buffers and Vertex Arrays */
    unsigned int storage_vertex_array[BENEATH_OPENGL_MESHES_MAX];
    unsigned int storage_buffer_object[BENEATH_OPENGL_MESHES_MAX * BENEATH_OPENGL_SHADER_LAYOUT_COUNT];

    /* Shaders */
    beneath_opengl_shader shaders[BENEATH_OPENGL_SHADERS_MAX];
    unsigned int shaders_size;
    unsigned int shaders_previous_index;
    unsigned int shaders_active_index;

    /* Pixelation FBO */
    unsigned int fbo;
    unsigned int fbo_color;
    unsigned int fbo_depth;
    unsigned int fbo_vao;
    unsigned int fbo_vbo;
    unsigned int blit_program;
    int blit_tex_uniform;
    int fbo_width;
    int fbo_height;

} beneath_opengl_context;

/******************************/
/* Shader Functions           */
/******************************/
BENEATH_API beneath_bool beneath_opengl_shader_generate(
    beneath_draw_call *draw_call,
    char *vertex_shader_code_buffer,
    int vertex_shader_code_buffer_size,
    char *fragment_shader_code_buffer,
    int fragment_shader_code_buffer_size)
{
    beneath_bool use_mesh_color = draw_call->mesh->colors_count > 0 && draw_call->colors_count == 0 && draw_call->texture_indices_count == 0;
    unsigned int hash = beneath_draw_call_hash(draw_call);
    int layout_location_current;

    sb vc; /* Vertex Shader Code  */
    sb fc; /* Fragment Shader Code */

    sb_init(&fc, fragment_shader_code_buffer, fragment_shader_code_buffer_size);
    sb_append_cstr(&fc, "/* Beneath Fragment Shader (hash=");
    sb_append_ulong(&fc, hash, 8, SB_PAD_NONE);
    sb_append_cstr(&fc, ") */\n");
    sb_append_cstr(&fc, "#version 330 core\n\n");
    sb_append_cstr(&fc, "in vec3 v_color;\n\n");

    /* Uniforms */
    sb_append_cstr(&fc, "/* Uniforms */\n");
    sb_append_cstr(&fc, "uniform float time;            /* Global Elapsed seconds             */\n");
    sb_append_cstr(&fc, "uniform float delta_time;      /* Global Seconds since last frame    */\n");
    sb_append_cstr(&fc, "uniform vec2  resolution;      /* Global Screen width and height     */\n");
    sb_append_cstr(&fc, "uniform vec3  camera_position; /* Global World space camera position */\n");
    sb_append_cstr(&fc, "uniform mat4  pv;              /* Global Projection View Matrix      */\n");

    /* If there is only one color or texture index it is better to pass it as a uniform and not as a instanced layout */
    if (!use_mesh_color && draw_call->colors_count == 1)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_INSTANCE_COLOR;
        sb_printf1(&fc, "uniform vec3  %s;           /* Instance Color */\n", (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }

    if (!use_mesh_color && draw_call->texture_indices_count == 1)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_INSTANCE_TEXTURE_INDEX;
        sb_printf1(&fc, "uniform int   %s;   /* Instance Texture Index */\n", (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }
    sb_append_cstr(&fc, "\n");

    sb_append_cstr(&fc, "out vec4 FragColor;\n\n");
    sb_append_cstr(&fc, "void main()\n{\n");
    sb_append_cstr(&fc, "  FragColor = vec4(v_color, 1.0f);\n");
    sb_append_cstr(&fc, "}\n\n");
    sb_term(&fc);

    sb_init(&vc, vertex_shader_code_buffer, vertex_shader_code_buffer_size);

    sb_append_cstr(&vc, "/* Beneath Vertex Shader (hash=");
    sb_append_ulong(&vc, hash, 8, SB_PAD_NONE);
    sb_append_cstr(&vc, ") */\n");
    sb_append_cstr(&vc, "#version 330 core\n\n");

    /* Layout */
    sb_append_cstr(&vc, "/* Layouts */\n");

    layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_POSITION;
    sb_printf2(&vc, "layout (location = %d) in vec3 %s;       /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_opengl_shader_layout_names[layout_location_current]);

    if (draw_call->mesh->uvs_count > 0)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_UV;
        sb_printf2(&vc, "layout (location = %d) in vec2 %s;             /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }

    if (draw_call->mesh->normals_count > 0)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_NORMAL;
        sb_printf2(&vc, "layout (location = %d) in vec3 %s;         /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }

    if (draw_call->mesh->tangents_count > 0)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_TANGENT;
        sb_printf2(&vc, "layout (location = %d) in vec3 %s;        /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }
    if (draw_call->mesh->bitangents_count > 0)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_BITANGENT;
        sb_printf2(&vc, "layout (location = %d) in vec3 %s;      /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }
    if (use_mesh_color)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_COLOR;
        sb_printf2(&vc, "layout (location = %d) in vec3 %s;          /* Mesh data      */\n", (char *)&layout_location_current, (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }
    if (draw_call->models_count > 0)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_INSTANCE_MODEL;
        sb_printf2(&vc, "layout (location = %d) in mat4 %s;          /* Instanced data */\n", (char *)&layout_location_current, (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }
    if (draw_call->colors_count > 1)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_INSTANCE_COLOR;
        sb_printf2(&vc, "layout (location = %d) in vec3 %s;         /* Instanced data */\n", (char *)&layout_location_current, (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }
    if (draw_call->texture_indices_count > 1)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_INSTANCE_TEXTURE_INDEX;
        sb_printf2(&vc, "layout (location = %d) in int  %s; /* Instanced data */\n", (char *)&layout_location_current, (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }
    sb_append_cstr(&vc, "\n");

    /* Uniforms */
    sb_append_cstr(&vc, "/* Uniforms */\n");
    sb_append_cstr(&vc, "uniform float time;            /* Global Elapsed seconds             */\n");
    sb_append_cstr(&vc, "uniform float delta_time;      /* Global Seconds since last frame    */\n");
    sb_append_cstr(&vc, "uniform vec2  resolution;      /* Global Screen width and height     */\n");
    sb_append_cstr(&vc, "uniform vec3  camera_position; /* Global World space camera position */\n");
    sb_append_cstr(&vc, "uniform mat4  pv;              /* Global Projection View Matrix      */\n");

    /* If there is only one color or texture index it is better to pass it as a uniform and not as a instanced layout */
    if (!use_mesh_color && draw_call->colors_count == 1)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_INSTANCE_COLOR;
        sb_printf1(&vc, "uniform vec3  %s;           /* Instance Color */\n", (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }

    if (!use_mesh_color && draw_call->texture_indices_count == 1)
    {
        layout_location_current = BENEATH_OPENGL_SHADER_LAYOUT_INSTANCE_TEXTURE_INDEX;
        sb_printf1(&vc, "uniform int   %s;   /* Instance Texture Index */\n", (char *)beneath_opengl_shader_layout_names[layout_location_current]);
    }

    sb_append_cstr(&vc, "\n");

    /* Outputs */
    sb_append_cstr(&vc, "/* Outputs */\n");
    sb_append_cstr(&vc, "out vec3 v_color;\n");
    sb_append_cstr(&vc, "\n");

    /* Main */
    sb_append_cstr(&vc, "void main()\n{\n");
    sb_append_cstr(&vc, "  v_color     = color;\n");
    sb_append_cstr(&vc, "  gl_Position = pv * model * vec4(position, 1.0f);\n");
    sb_append_cstr(&vc, "}\n\n");

    sb_term(&vc);

    return true;
}

BENEATH_API int beneath_opengl_shader_compile(char *shaderCode, unsigned int shaderType, beneath_api_io_print print)
{
    unsigned int shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId);

    int success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shaderId, 1024, NULL, infoLog);
        print(__FILE__, __LINE__, infoLog);
        return -1;
    }

    return (int)shaderId;
}

BENEATH_API beneath_bool beneath_opengl_shader_load(
    beneath_opengl_context *ctx,
    beneath_draw_call *draw_call,
    beneath_api_io_print print)
{
    unsigned int draw_call_hash = beneath_draw_call_hash(draw_call);

    /* Check if shader already exists */
    /* TODO: load from hashmap in the future */
    {
        unsigned int i;

        for (i = 0; i < ctx->shaders_size; ++i)
        {
            if (ctx->shaders[i].hash == draw_call_hash)
            {
                ctx->shaders_active_index = i;
                return true;
            }
        }
    }

    beneath_opengl_shader shader = {0};
    /* shader.program_id = -1; */
    shader.hash = draw_call_hash;

    /* Generate shader code */
    if (!beneath_opengl_shader_generate(
            draw_call,
            shader.code_vertex, sizeof(shader.code_vertex),
            shader.code_fragment, sizeof(shader.code_fragment)))
    {
        return false;
    }

    /* Compile shader */
    int vertex_shader_id = beneath_opengl_shader_compile(shader.code_vertex, GL_VERTEX_SHADER, print);
    int fragment_shader_id = beneath_opengl_shader_compile(shader.code_fragment, GL_FRAGMENT_SHADER, print);

    if (vertex_shader_id == -1 || fragment_shader_id == -1)
    {
        return false;
    }

    /* Load shader */
    {
        int success;

        shader.program_id = glCreateProgram();
        glAttachShader(shader.program_id, (unsigned int)vertex_shader_id);
        glAttachShader(shader.program_id, (unsigned int)fragment_shader_id);
        glLinkProgram(shader.program_id);
        glGetProgramiv(shader.program_id, GL_LINK_STATUS, &success);
        glDeleteShader((unsigned int)vertex_shader_id);
        glDeleteShader((unsigned int)fragment_shader_id);

        if (!success)
        {
            char infoLog[1024];
            glGetProgramInfoLog(shader.program_id, 1024, NULL, infoLog);
            print(__FILE__, __LINE__, infoLog);
            return false;
        }
    }

    /* Load shader uniform locations */
    {
        glUseProgram(shader.program_id);

        unsigned int i;
        for (i = 0; i < BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_COUNT; ++i)
        {
            shader.uniform_locations[i] = -1;
            shader.uniform_locations[i] = glGetUniformLocation(shader.program_id, beneath_opengl_shader_uniform_names[i]);
        }
    }

    ctx->shaders[ctx->shaders_size] = shader;
    ctx->shaders_active_index = ctx->shaders_size;
    ctx->shaders_size++;

    return true;
}

/******************************/
/* Pixelation Functions       */
/******************************/
BENEATH_API beneath_bool beneath_opengl_framebuffer_initialize(beneath_opengl_context *ctx, beneath_api_io_print print, int fbo_width, int fbo_height)
{
    /* --- Pixelation setup --- */
    ctx->fbo_width = fbo_width;   /* low-res target width */
    ctx->fbo_height = fbo_height; /* low-res target height */

    glGenFramebuffers(1, &ctx->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ctx->fbo);

    /* Color texture */
    glGenTextures(1, &ctx->fbo_color);
    glBindTexture(GL_TEXTURE_2D, ctx->fbo_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ctx->fbo_width, ctx->fbo_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctx->fbo_color, 0);

    /* Depth buffer */
    glGenRenderbuffers(1, &ctx->fbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, ctx->fbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ctx->fbo_width, ctx->fbo_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ctx->fbo_depth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        print(__FILE__, __LINE__, "FBO incomplete!\n");
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

BENEATH_API beneath_bool beneath_opengl_framebuffer_shader_load(beneath_opengl_context *ctx, beneath_api_io_print print)
{
    char *vertex_shader =
        "#version 330 core                       \n"
        "layout (location = 0) in vec2 aPos;     \n"
        "layout (location = 1) in vec2 aUV;      \n"
        "out vec2 vUV;                           \n"
        "void main() {                           \n"
        "    vUV = aUV;                          \n"
        "    gl_Position = vec4(aPos, 0.0, 1.0); \n"
        "}                                       \n";

    /* "    FragColor = texture(screen_texture, vUV); \n" */
    char *fragment_shader =
        "#version 330 core                        \n"
        "const float colors_per_channel = 16.0f;  \n"
        "in vec2 vUV;                             \n"
        "out vec4 FragColor;                      \n"
        "uniform sampler2D screen_texture;        \n"
        "void main() {                            \n"
        "    vec3 color = texture(screen_texture, vUV).rgb; \n"
        "    vec3 quantized = floor(color * colors_per_channel) / colors_per_channel; \n"
        "    FragColor = vec4(quantized, 1.0);    \n"
        "}                                        \n";

    int vertex_shader_id = beneath_opengl_shader_compile(vertex_shader, GL_VERTEX_SHADER, print);
    int fragment_shader_id = beneath_opengl_shader_compile(fragment_shader, GL_FRAGMENT_SHADER, print);

    /* Load shader */
    {
        int success;

        ctx->blit_program = glCreateProgram();
        glAttachShader(ctx->blit_program, (unsigned int)vertex_shader_id);
        glAttachShader(ctx->blit_program, (unsigned int)fragment_shader_id);
        glLinkProgram(ctx->blit_program);
        ctx->blit_tex_uniform = glGetUniformLocation(ctx->blit_program, "screen_texture");

        glGetProgramiv(ctx->blit_program, GL_LINK_STATUS, &success);
        glDeleteShader((unsigned int)vertex_shader_id);
        glDeleteShader((unsigned int)fragment_shader_id);

        if (!success)
        {
            char infoLog[1024];
            glGetProgramInfoLog(ctx->blit_program, 1024, NULL, infoLog);
            print(__FILE__, __LINE__, infoLog);
            return false;
        }
    }

    return true;
}

/******************************/
/* Render Functions           */
/******************************/
static beneath_opengl_context ctx = {0};

BENEATH_API beneath_bool beneath_opengl_draw(
    beneath_state *state,         /* The state */
    beneath_draw_call *draw_call, /* The draw call instanced objects */
    float projection_view[16],    /* The projection view matrix */
    float camera_position[3],      /* The camera x,y,z position */
    beneath_api_io_print print)
{
    (void)draw_call;
    (void)projection_view;
    (void)state;

    if (!draw_call || draw_call->models_count == 0 || !draw_call->mesh)
    {
        return false;
    }

    if (!ctx.initialized)
    {
        if (!beneath_opengl_shader_load(&ctx, draw_call, print))
        {

            print(__FILE__, __LINE__, "cannot load shaders!!!\n");
            return false;
        }

        print(__FILE__, __LINE__, "Vertex Shader Code:\n");
        print(__FILE__, __LINE__, ctx.shaders[ctx.shaders_active_index].code_vertex);
        print(__FILE__, __LINE__, "Fragment Shader Code:\n");
        print(__FILE__, __LINE__, ctx.shaders[ctx.shaders_active_index].code_fragment);

        glGenVertexArrays(BENEATH_OPENGL_MESHES_MAX, ctx.storage_vertex_array);
        glGenBuffers(BENEATH_OPENGL_MESHES_MAX * BENEATH_OPENGL_SHADER_LAYOUT_COUNT, ctx.storage_buffer_object);

        ctx.initialized = true;

        /* Pixel */
        {
            if (!beneath_opengl_framebuffer_shader_load(&ctx, print))
            {
                print(__FILE__, __LINE__, "cannot compile framebuffer shaders !!!\n");
                return false;
            }
            /*
                        if (!beneath_opengl_framebuffer_initialize(&ctx, print, (int)state->window_width / 2, (int)state->window_height / 2))
              */
            if (!beneath_opengl_framebuffer_initialize(&ctx, print, (int)300, (int)200))
            {
                print(__FILE__, __LINE__, "cannot initialize framebuffers!!!\n");
                return false;
            }

            float quad_vertices[] = {
                /* pos, uv */
                -1.0f, -1.0f, 0.0f, 0.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f};

            glGenVertexArrays(1, &ctx.fbo_vao);
            glGenBuffers(1, &ctx.fbo_vbo);

            glBindVertexArray(ctx.fbo_vao);
            glBindBuffer(GL_ARRAY_BUFFER, ctx.fbo_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        }
    }

    beneath_mesh *mesh = draw_call->mesh;
    beneath_opengl_shader shader_active = ctx.shaders[ctx.shaders_active_index];

    if (mesh->changed)
    {
        int sizeM4x4 = sizeof(float) * 16;
        int i;
        char buffer[1024];
        sb tmp = {0};

        unsigned int buffer_index = mesh->id * BENEATH_OPENGL_SHADER_LAYOUT_COUNT;

        sb_init(&tmp, buffer, 1024);
        sb_append_cstr(&tmp, "\n");
        sb_append_cstr(&tmp, "+----------------------------------------------+\n");
        sb_append_cstr(&tmp, "| Mesh & Draw Call Information                 |\n");
        sb_append_cstr(&tmp, "+----------------------------------------------+\n");
        sb_append_cstr(&tmp, "| mesh->vertices_count            : ");
        sb_append_ulong_direct(&tmp, mesh->vertices_count);
        sb_append_cstr(&tmp, "\n");
        sb_append_cstr(&tmp, "| mesh->indices_count             : ");
        sb_append_ulong_direct(&tmp, mesh->indices_count);
        sb_append_cstr(&tmp, "\n");
        sb_append_cstr(&tmp, "| mesh->uvs_count                 : ");
        sb_append_ulong_direct(&tmp, mesh->uvs_count);
        sb_append_cstr(&tmp, "\n");
        sb_append_cstr(&tmp, "| mesh->normals_count             : ");
        sb_append_ulong_direct(&tmp, mesh->normals_count);
        sb_append_cstr(&tmp, "\n");
        sb_append_cstr(&tmp, "| mesh->tangents_count            : ");
        sb_append_ulong_direct(&tmp, mesh->tangents_count);
        sb_append_cstr(&tmp, "\n");
        sb_append_cstr(&tmp, "| mesh->bitangents_count          : ");
        sb_append_ulong_direct(&tmp, mesh->bitangents_count);
        sb_append_cstr(&tmp, "\n");
        sb_append_cstr(&tmp, "| mesh->colors_count              : ");
        sb_append_ulong_direct(&tmp, mesh->colors_count);
        sb_append_cstr(&tmp, "\n");
        sb_append_cstr(&tmp, "| draw_call->models_count         : ");
        sb_append_ulong_direct(&tmp, draw_call->models_count);
        sb_append_cstr(&tmp, "\n");
        sb_append_cstr(&tmp, "| draw_call->colors_count         : ");
        sb_append_ulong_direct(&tmp, draw_call->colors_count);
        sb_append_cstr(&tmp, "\n");
        sb_append_cstr(&tmp, "| draw_call->texture_indices_count: ");
        sb_append_ulong_direct(&tmp, draw_call->texture_indices_count);
        sb_append_cstr(&tmp, "\n");

        for (i = 0; i < BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_COUNT; ++i)
        {
            sb_append_cstr(&tmp, "| shader->uniform->");
            sb_printf1(&tmp, "%15s: ", beneath_opengl_shader_uniform_names[i]);
            sb_append_ulong_direct(&tmp, (unsigned long)shader_active.uniform_locations[i]);
            sb_append_cstr(&tmp, "\n");
        }
        sb_append_cstr(&tmp, "+-----------------------------------------------\n\n");
        sb_term(&tmp);

        print(__FILE__, __LINE__, buffer);

        glBindVertexArray(ctx.storage_vertex_array[mesh->id]);

        /* Vertex data */
        glBindBuffer(GL_ARRAY_BUFFER, ctx.storage_buffer_object[buffer_index]);
        glBufferData(GL_ARRAY_BUFFER, (int)mesh->vertices_count * (int)sizeof(float), mesh->vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(BENEATH_OPENGL_SHADER_LAYOUT_POSITION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(BENEATH_OPENGL_SHADER_LAYOUT_POSITION);

        /* Index data */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx.storage_buffer_object[buffer_index + 1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (int)mesh->indices_count * (int)sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

        /* UV data */
        if (mesh->uvs_count > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, ctx.storage_buffer_object[buffer_index + 2]);
            glBufferData(GL_ARRAY_BUFFER, (int)mesh->uvs_count * (int)sizeof(float), mesh->uvs, GL_STATIC_DRAW);
            glVertexAttribPointer(BENEATH_OPENGL_SHADER_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(BENEATH_OPENGL_SHADER_LAYOUT_UV);
        }

        /* Normals data */
        if (mesh->normals_count > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, ctx.storage_buffer_object[buffer_index + 3]);
            glBufferData(GL_ARRAY_BUFFER, (int)mesh->normals_count * (int)sizeof(float), mesh->normals, GL_STATIC_DRAW);
            glVertexAttribPointer(BENEATH_OPENGL_SHADER_LAYOUT_NORMAL, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(BENEATH_OPENGL_SHADER_LAYOUT_NORMAL);
        }

        /* Tangent data */
        if (mesh->tangents_count > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, ctx.storage_buffer_object[buffer_index + 4]);
            glBufferData(GL_ARRAY_BUFFER, (int)mesh->tangents_count * (int)sizeof(float), mesh->tangents, GL_STATIC_DRAW);
            glVertexAttribPointer(BENEATH_OPENGL_SHADER_LAYOUT_TANGENT, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(BENEATH_OPENGL_SHADER_LAYOUT_TANGENT);
        }

        /* Bitangent data */
        if (mesh->bitangents_count > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, ctx.storage_buffer_object[buffer_index + 5]);
            glBufferData(GL_ARRAY_BUFFER, (int)mesh->bitangents_count * (int)sizeof(float), mesh->bitangents, GL_STATIC_DRAW);
            glVertexAttribPointer(BENEATH_OPENGL_SHADER_LAYOUT_BITANGENT, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(BENEATH_OPENGL_SHADER_LAYOUT_BITANGENT);
        }

        /* Color data */
        if (mesh->colors_count > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, ctx.storage_buffer_object[buffer_index + 6]);
            glBufferData(GL_ARRAY_BUFFER, (int)mesh->colors_count * (int)sizeof(float), mesh->colors, GL_STATIC_DRAW);
            glVertexAttribPointer(BENEATH_OPENGL_SHADER_LAYOUT_COLOR, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(BENEATH_OPENGL_SHADER_LAYOUT_COLOR);
        }

        /* Instanced model */
        glBindBuffer(GL_ARRAY_BUFFER, ctx.storage_buffer_object[buffer_index + 7]);
        glBufferData(GL_ARRAY_BUFFER, (int)draw_call->models_count * sizeM4x4, &draw_call->models[0], GL_STATIC_DRAW);

        /* set attribute pointers 6 - 9 for model matrix (4 times vec4) */
        for (i = 0; i < 4; ++i)
        {
            int model_location = BENEATH_OPENGL_SHADER_LAYOUT_INSTANCE_MODEL;
            glEnableVertexAttribArray((unsigned int)(model_location + i));
            glVertexAttribPointer((unsigned int)(model_location + i), 4, GL_FLOAT, GL_FALSE, sizeM4x4, (void *)((unsigned long long)i * sizeof(float) * 4));
            glVertexAttribDivisor((unsigned int)(model_location + i), 1);
        }

        glBindVertexArray(0);
    }

    if (draw_call->pixelize)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ctx.fbo);
        glViewport(0, 0, ctx.fbo_width, ctx.fbo_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    {

        /*
        if (ctx.shaders_active_index != ctx.shaders_previous_index)
        {

            ctx.shaders_previous_index = ctx.shaders_active_index;
        }
        */

        glUseProgram(shader_active.program_id);
        glBindVertexArray(ctx.storage_vertex_array[mesh->id]);
        glUniformMatrix4fv(shader_active.uniform_locations[BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_PROJECTION_VIEW], 1, GL_FALSE, projection_view);
        glUniform3f(shader_active.uniform_locations[BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_CAMERA_POSITION], camera_position[0], camera_position[1], camera_position[2]);

        if (draw_call->changed)
        {
            glUniform1f(shader_active.uniform_locations[BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_TIME], (float)state->time);
            glUniform1f(shader_active.uniform_locations[BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_DELTA_TIME], (float)state->delta_time);
            glUniform2f(shader_active.uniform_locations[BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_RESOLUTION], (float)state->window_width, (float)state->window_height);

            if (draw_call->colors_count > 0)
            {
                glUniform3f(shader_active.uniform_locations[BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_INSTANCE_COLOR], draw_call->colors[0], draw_call->colors[1], draw_call->colors[2]);
            }
        }

        glDrawElementsInstanced(GL_TRIANGLES, (int)mesh->indices_count, GL_UNSIGNED_INT, 0, (int)draw_call->models_count);
        glBindVertexArray(0);
    }

    if (draw_call->pixelize)
    {
        /* --- Post-process pixelated upscale --- */
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, (int)state->window_width, (int)state->window_height);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(ctx.blit_program);
        glBindVertexArray(ctx.fbo_vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ctx.fbo_color);
        glUniform1i(ctx.blit_tex_uniform, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    draw_call->changed = false;
    draw_call->mesh->changed = false;

    return true;
}

#endif /* WIN32_BENEATH_OPENGL */