#ifndef WIN32_BENEATH_OPENGL
#define WIN32_BENEATH_OPENGL

#include "beneath.h"
#include "win32_opengl.h"
#include "sb.h" /* Temporary for prototype: String builder */
#include "vm.h" /* Temporary for prototype: Vector math */

static char beneath_opengl_shader_shadow_vertex[] = {
    " /* Beneath ShadowMap Vertex Shader */                     \n"
    " #version 330 core                                         \n"
    "                                                           \n"
    " layout (location = 0) in vec3 position;                   \n"
    " layout (location = 6) in mat4 model; /* Instanced data */ \n"
    "                                                           \n"
    " uniform mat4 pv;                                          \n"
    "                                                           \n"
    " void main()                                               \n"
    " {                                                         \n"
    "     gl_Position = pv * model * vec4(position, 1.0);       \n"
    " }                                                         \n"};

static char beneath_opengl_shader_shadow_fragment[] = {
    "/* Beneath ShadowMap Fragment Shader */ \n"
    "#version 330 core                       \n"
    "void main()                             \n"
    "{                                       \n"
    "  /* Only writes depth automatically */ \n"
    "}                                       \n"};

static char beneath_opengl_shader_post_process_base_vertex[] = {
    "#version 330 core                       \n"
    "layout (location = 0) in vec2 aPos;     \n"
    "layout (location = 1) in vec2 aUV;      \n"
    "out vec2 vUV;                           \n"
    "void main() {                           \n"
    "    vUV = aUV;                          \n"
    "    gl_Position = vec4(aPos, 0.0, 1.0); \n"
    "}                                       \n"};

static char beneath_opengl_shader_post_process_base_fragment[] = {
    "#version 330 core                        \n"
    "in vec2 vUV;                             \n"
    "out vec4 FragColor;                      \n"
    "uniform sampler2D screen_texture;        \n"
    "void main() {                            \n"
    "    vec3 color = texture(screen_texture, vUV).rgb;\n"
    "    FragColor = vec4(color, 1.0);\n"
    "}                                        \n"};

static char beneath_opengl_shader_pixel_fragment[] = {
    "#version 330 core                        \n"
    "const float colors_per_channel = 16.0f;  \n"
    "const float gamma = 2.2f;                \n"
    "in vec2 vUV;                             \n"
    "out vec4 FragColor;                      \n"
    "uniform sampler2D screen_texture;        \n"
    "uniform vec2 texel_size;                 \n" // 1.0 / FBO size
    "void main() {                            \n"
    "    vec3 color = texture(screen_texture, vUV).rgb;\n"
    "    color = pow(color, vec3(1.0 / gamma));\n"
    "    vec3 quantized = (floor(color * colors_per_channel) + 0.5) / colors_per_channel;\n"
    "    quantized = pow(quantized, vec3(gamma));\n"
    "\n"
    "    // --- Edge detection ---\n"
    "    vec3 north = texture(screen_texture, vUV + vec2(0.0, texel_size.y)).rgb;\n"
    "    vec3 south = texture(screen_texture, vUV - vec2(0.0, texel_size.y)).rgb;\n"
    "    vec3 east  = texture(screen_texture, vUV + vec2(texel_size.x, 0.0)).rgb;\n"
    "    vec3 west  = texture(screen_texture, vUV - vec2(texel_size.x, 0.0)).rgb;\n"
    "\n"
    "    float edge = length(north - south) + length(east - west);\n"
    "    edge = clamp(edge * 0.5, 0.0, 1.0); // tweak strength\n"
    "\n"
    "    // --- Combine quantized color and edge ---\n"
    "    vec3 edge_color = vec3(0.2, 0.2, 0.2);\n"
    "    FragColor = vec4(mix(quantized, edge_color, edge), 1.0);\n"
    "}                                        \n"};

static char beneath_opengl_shader_volumetric_fragment[] = {
    "#version 330 core                        \n"
    "in vec2 vUV;                             \n"
    "out vec4 FragColor;                      \n"
    "uniform sampler2D screen_texture; /* Color texture from camera view */\n"
    "uniform sampler2D shadow_map;     /* Shadow Map from light position view */\n"
    "void main() {                            \n"
    "    vec3 color = texture(screen_texture, vUV).rgb;\n"
    "    FragColor = vec4(color, 1.0);\n"
    "}                                        \n"};

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

static char *beneath_opengl_shader_uniform_names[7] = {
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
    char code_vertex[8192];
    char code_fragment[8192];
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

    /* Screen FBO */
    unsigned int fbo_screen;
    unsigned int fbo_screen_color;
    unsigned int fbo_screen_depth;
    unsigned int fbo_screen_vao;
    unsigned int fbo_screen_vbo;
    int fbo_screen_width;
    int fbo_screen_height;

    /* Shadow Shader */
    unsigned int shadow_program;
    int shadow_uniform_pv;
    unsigned int shadow_texture_depth;
    m4x4 shadow_uniform_pv_data;
    unsigned int shadow_fbo;

    /* Post processing starts here */
    unsigned int post_process_base_program;
    int post_process_base_uniform_screen_texture;

    /* Pixelation Shader */
    unsigned int blit_program;
    int blit_tex_uniform;
    int blit_texel_uniform;

    /* Volumetric raymarch */
    unsigned int volumetric_program;
    int volumetric_uniform_screen_texture;
    int volumetric_uniform_shadow_map;
    int volumetric_uniform_light_pos;
    int volumetric_uniform_camera_pos;
    int volumetric_uniform_scattering;
    int volumetric_uniform_exposure;

} beneath_opengl_context;

/******************************/
/* Shader Functions           */
/******************************/

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

BENEATH_API beneath_bool beneath_opengl_shader_create(unsigned int *shader_program, char *shader_vertex_code, char *shader_fragment_code, beneath_api_io_print print)
{
    int vertex_shader_id;
    int fragment_shader_id;
    int success;

    vertex_shader_id = beneath_opengl_shader_compile(shader_vertex_code, GL_VERTEX_SHADER, print);

    if (vertex_shader_id == -1)
    {
        print(__FILE__, __LINE__, "[opengl] vertex shader compilation failed!\n");
        return false;
    }

    fragment_shader_id = beneath_opengl_shader_compile(shader_fragment_code, GL_FRAGMENT_SHADER, print);

    if (vertex_shader_id == -1)
    {
        print(__FILE__, __LINE__, "[opengl] fragment shader compilation failed!\n");
        return false;
    }

    *shader_program = glCreateProgram();
    glAttachShader(*shader_program, (unsigned int)vertex_shader_id);
    glAttachShader(*shader_program, (unsigned int)fragment_shader_id);
    glLinkProgram(*shader_program);
    glGetProgramiv(*shader_program, GL_LINK_STATUS, &success);
    glDeleteShader((unsigned int)vertex_shader_id);
    glDeleteShader((unsigned int)fragment_shader_id);

    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(*shader_program, 1024, NULL, infoLog);

        print(__FILE__, __LINE__, "[opengl] shader linking failed!\n");
        print(__FILE__, __LINE__, infoLog);

        return false;
    }

    return true;
}

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
    sb_append_cstr(&fc, "in vec3 v_color;\n");

    if (draw_call->lightning)
    {
        sb_append_cstr(&fc, "in vec3 v_frag_pos;\n");
        sb_append_cstr(&fc, "in vec3 v_normal;\n");
    }

    if (draw_call->shadow)
    {
        sb_append_cstr(&fc, "in vec4 v_frag_pos_light_space;\n");
    }

    sb_append_cstr(&fc, "\n");

    /* Uniforms */
    sb_append_cstr(&fc, "/* Uniforms */\n");
    sb_append_cstr(&fc, "uniform float time;            /* Global Elapsed seconds             */\n");
    sb_append_cstr(&fc, "uniform float delta_time;      /* Global Seconds since last frame    */\n");
    sb_append_cstr(&fc, "uniform vec2  resolution;      /* Global Screen width and height     */\n");
    sb_append_cstr(&fc, "uniform vec3  camera_position; /* Global World space camera position */\n");
    sb_append_cstr(&fc, "uniform mat4  pv;              /* Global Projection View Matrix      */\n");

    if (draw_call->shadow)
    {
        sb_append_cstr(&fc, "uniform sampler2D shadow_map;\n");
    }

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

    if (draw_call->shadow)
    {
        sb_append_cstr(
            &fc,
            "float rand(vec2 co)\n"
            "{\n"
            "    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);\n"
            "}\n"
            "\n"
            "float ShadowCalculation(vec4 frag_pos_light_space, vec3 normal, vec3 light_dir)\n"
            "{\n"
            "    // Transform to [0,1] range\n"
            "    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;\n"
            "    proj_coords = proj_coords * 0.5 + 0.5;\n"
            "\n"
            "    if (proj_coords.z > 1.0)\n"
            "        return 0.0;\n"
            "\n"
            "    float bias = max(0.005 * (1.0 - dot(normalize(normal), normalize(-light_dir))), 0.001);\n"
            "\n"
            "    float shadow = 0.0;\n"
            "    vec2 texel_size = 1.0 / textureSize(shadow_map, 0);\n"
            "\n"
            "    // Generate a pseudo-random rotation per fragment\n"
            "    float angle = rand(proj_coords.xy) * 6.2831853; // 0..2PI\n"
            "    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));\n"
            "\n"
            "    // 3x3 PCF kernel with randomized rotation\n"
            "    for (int x = -1; x <= 1; ++x)\n"
            "    {\n"
            "        for (int y = -1; y <= 1; ++y)\n"
            "        {\n"
            "            vec2 offset = rot * vec2(x, y) * texel_size;\n"
            "            float pcf_depth = texture(shadow_map, proj_coords.xy + offset).r;\n"
            "            shadow += (proj_coords.z - bias > pcf_depth ? 1.0 : 0.0);\n"
            "        }\n"
            "    }\n"
            "\n"
            "    shadow /= 9.0;\n"
            "    return shadow;\n"
            "}\n\n");
    }

    if (draw_call->lightning)
    {
        sb_append_cstr(&fc, "/* Lighting structures */ \n");
        sb_append_cstr(&fc, "struct DirectionalLight { \n");
        sb_append_cstr(&fc, "    vec3 direction;       \n");
        sb_append_cstr(&fc, "    vec3 ambient;         \n");
        sb_append_cstr(&fc, "    vec3 diffuse;         \n");
        sb_append_cstr(&fc, "    vec3 specular;        \n");
        sb_append_cstr(&fc, "};                        \n");
        sb_append_cstr(&fc, "\n");
        sb_append_cstr(&fc, "uniform DirectionalLight dir_light;\n");
        sb_append_cstr(&fc, "\n");

        sb_append_cstr(&fc, "/* Function to calculate Blinn-Phong lighting from a directional light */     \n");
        sb_append_cstr(&fc, "vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir, float shadow) \n");
        sb_append_cstr(&fc, "{                                                                             \n");
        sb_append_cstr(&fc, "    vec3 light_dir = normalize(-light.direction);                             \n");
        sb_append_cstr(&fc, "                                                                              \n");
        sb_append_cstr(&fc, "    /* Diffuse term */                                                        \n");
        sb_append_cstr(&fc, "    float diff = max(dot(normal, light_dir), 0.0);                            \n");
        sb_append_cstr(&fc, "                                                                              \n");
        sb_append_cstr(&fc, "    /* Specular term (Blinn-Phong) */                                         \n");
        sb_append_cstr(&fc, "    vec3 halfway_dir = normalize(light_dir + view_dir);                       \n");
        sb_append_cstr(&fc, "    float spec       = pow(max(dot(normal, halfway_dir), 0.0), 32.0);         \n");
        sb_append_cstr(&fc, "    vec3 ambient     = light.ambient * v_color;                               \n");
        sb_append_cstr(&fc, "    vec3 diffuse     = light.diffuse * diff * v_color;                        \n");
        sb_append_cstr(&fc, "    vec3 specular    = light.specular * spec;                                 \n");
        sb_append_cstr(&fc, "                                                                              \n");
        sb_append_cstr(&fc, "    return ambient + (1.0 - shadow) * (diffuse + specular);                   \n");
        sb_append_cstr(&fc, "}                                                                             \n");
        sb_append_cstr(&fc, "\n");

        sb_append_cstr(&fc, "out vec4 FragColor;\n\n");
        sb_append_cstr(&fc, "void main()                                                          \n");
        sb_append_cstr(&fc, "{                                                                    \n");
        sb_append_cstr(&fc, "    vec3 norm     = normalize(v_normal);                             \n");
        sb_append_cstr(&fc, "    vec3 view_dir = normalize(camera_position - v_frag_pos);         \n");
        sb_append_cstr(&fc, "    float shadow  = ShadowCalculation(v_frag_pos_light_space,norm, dir_light.direction);       \n");
        sb_append_cstr(&fc, "    vec3 result   = CalcDirectionalLight(dir_light, norm, view_dir, shadow); \n");
        sb_append_cstr(&fc, "    FragColor     = vec4(result, 1.0);                               \n");
        sb_append_cstr(&fc, "}\n\n");
    }
    else
    {
        sb_append_cstr(&fc, "out vec4 FragColor;\n\n");
        sb_append_cstr(&fc, "void main()\n{\n");
        sb_append_cstr(&fc, "  FragColor = vec4(v_color, 1.0f);\n");
        sb_append_cstr(&fc, "}\n\n");
    }

    sb_term(&fc);

    /* Vertex Shader */
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

    if (draw_call->shadow)
    {
        sb_append_cstr(&vc, "uniform mat4 light_space_matrix; \n");
    }

    sb_append_cstr(&vc, "\n");

    /* Outputs */
    sb_append_cstr(&vc, "/* Outputs */\n");
    sb_append_cstr(&vc, "out vec3 v_color;\n");

    if (draw_call->lightning)
    {
        sb_append_cstr(&vc, "out vec3 v_normal;\n");
        sb_append_cstr(&vc, "out vec3 v_frag_pos;\n");
    }

    if (draw_call->shadow)
    {
        sb_append_cstr(&vc, "out vec4 v_frag_pos_light_space; \n");
    }

    sb_append_cstr(&vc, "\n");

    /* Main */
    sb_append_cstr(&vc, "void main()\n{\n");

    if (draw_call->lightning)
    {
        sb_append_cstr(&vc, "  vec4 world_pos = model * vec4(position, 1.0);\n\n");
        sb_append_cstr(&vc, "  v_frag_pos     = world_pos.xyz;\n");
        sb_append_cstr(&vc, "  v_normal       = mat3(transpose(inverse(model))) * normal;\n");
        sb_append_cstr(&vc, "  v_color        = color;\n");
        if (draw_call->shadow)
        {
            sb_append_cstr(&vc, "  v_frag_pos_light_space = light_space_matrix * world_pos;\n");
        }
        sb_append_cstr(&vc, "  gl_Position    = pv * world_pos;\n");
    }
    else
    {
        sb_append_cstr(&vc, "  v_color     = color;\n");
        sb_append_cstr(&vc, "  gl_Position = pv * model * vec4(position, 1.0f);\n");
    }

    sb_append_cstr(&vc, "}\n\n");

    sb_term(&vc);

    return true;
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

    if (!beneath_opengl_shader_create(&shader.program_id, shader.code_vertex, shader.code_fragment, print))
    {
        return false;
    }

    /* Load shader uniform locations */
    {
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
BENEATH_API beneath_bool beneath_opengl_framebuffer_screen_initialize(beneath_opengl_context *ctx, beneath_api_io_print print, int fbo_width, int fbo_height)
{
    /* --- Pixelation setup --- */
    ctx->fbo_screen_width = fbo_width;   /* low-res target width */
    ctx->fbo_screen_height = fbo_height; /* low-res target height */

    glGenFramebuffers(1, &ctx->fbo_screen);
    glBindFramebuffer(GL_FRAMEBUFFER, ctx->fbo_screen);

    /* Color texture */
    glGenTextures(1, &ctx->fbo_screen_color);
    glBindTexture(GL_TEXTURE_2D, ctx->fbo_screen_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ctx->fbo_screen_width, ctx->fbo_screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctx->fbo_screen_color, 0);

    /* Depth buffer */
    glGenRenderbuffers(1, &ctx->fbo_screen_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, ctx->fbo_screen_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ctx->fbo_screen_width, ctx->fbo_screen_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ctx->fbo_screen_depth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        print(__FILE__, __LINE__, "FBO incomplete!\n");
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

/******************************/
/* Render Functions           */
/******************************/
static beneath_opengl_context ctx = {0};

BENEATH_API void beneath_opengl_draw_call_print(beneath_draw_call *draw_call, beneath_api_io_print print)
{
    int i;
    char buffer[1024];
    sb tmp = {0};

    beneath_mesh *mesh = draw_call->mesh;
    beneath_opengl_shader shader_active = ctx.shaders[ctx.shaders_active_index];

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
}

static m4x4 shadow_pv;
static v3 shadow_light_position;

BENEATH_API beneath_bool beneath_opengl_draw(
    beneath_state *state,         /* The state */
    beneath_draw_call *draw_call, /* The draw call instanced objects */
    float projection_view[16],    /* The projection view matrix */
    float camera_position[3],     /* The camera x,y,z position */
    beneath_api_io_print print)
{

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

        /*
        print(__FILE__, __LINE__, "Vertex Shader Code:\n");
        print(__FILE__, __LINE__, ctx.shaders[ctx.shaders_active_index].code_vertex);
        print(__FILE__, __LINE__, "Fragment Shader Code:\n");
        print(__FILE__, __LINE__, ctx.shaders[ctx.shaders_active_index].code_fragment);
        */

        glGenVertexArrays(BENEATH_OPENGL_MESHES_MAX, ctx.storage_vertex_array);
        glGenBuffers(BENEATH_OPENGL_MESHES_MAX * BENEATH_OPENGL_SHADER_LAYOUT_COUNT, ctx.storage_buffer_object);

        ctx.initialized = true;

        /* Setup Screen Framebuffer and VAO,VBO */
        {
            if (!beneath_opengl_framebuffer_screen_initialize(
                    &ctx, print,
                    (int)(draw_call->pixelize ? 300 : state->window_width),
                    (int)(draw_call->pixelize ? 200 : state->window_height)))
            {
                print(__FILE__, __LINE__, "cannot initialize screen framebuffers!!!\n");
                return false;
            }

            float quad_vertices[] = {
                /* pos, uv */
                -1.0f, -1.0f, 0.0f, 0.0f,
                1.0f, -1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f};

            glGenVertexArrays(1, &ctx.fbo_screen_vao);
            glGenBuffers(1, &ctx.fbo_screen_vbo);

            glBindVertexArray(ctx.fbo_screen_vao);
            glBindBuffer(GL_ARRAY_BUFFER, ctx.fbo_screen_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

            glBindVertexArray(0);
        }

        /* Post processing shader */
        {
            if (!beneath_opengl_shader_create(
                    &ctx.post_process_base_program,
                    beneath_opengl_shader_post_process_base_vertex,
                    beneath_opengl_shader_post_process_base_fragment,
                    print))
            {
                print(__FILE__, __LINE__, "cannot compile post processing shader !!!\n");
                return false;
            }

            ctx.post_process_base_uniform_screen_texture = glGetUniformLocation(ctx.shadow_program, "screen_texture");
        }

        /* Volumetric Shader */
        {
            if (!beneath_opengl_shader_create(
                    &ctx.volumetric_program,
                    beneath_opengl_shader_post_process_base_vertex,
                    beneath_opengl_shader_volumetric_fragment,
                    print))
            {
                print(__FILE__, __LINE__, "cannot compile volumetric shader !!!\n");
                return false;
            }

            ctx.volumetric_uniform_screen_texture = glGetUniformLocation(ctx.volumetric_program, "screen_texture");
            ctx.volumetric_uniform_shadow_map = glGetUniformLocation(ctx.volumetric_program, "shadow_map");
            ctx.volumetric_uniform_light_pos = glGetUniformLocation(ctx.volumetric_program, "ligth_pos");
            ctx.volumetric_uniform_camera_pos = glGetUniformLocation(ctx.volumetric_program, "camera_pos");
            ctx.volumetric_uniform_scattering = glGetUniformLocation(ctx.volumetric_program, "scattering");
            ctx.volumetric_uniform_exposure = glGetUniformLocation(ctx.volumetric_program, "exposure");
        }

        /* Pixel */
        {
            if (!beneath_opengl_shader_create(
                    &ctx.blit_program,
                    beneath_opengl_shader_post_process_base_vertex,
                    beneath_opengl_shader_pixel_fragment,
                    print))
            {
                print(__FILE__, __LINE__, "cannot compile pixel shaders !!!\n");
                return false;
            }

            ctx.blit_tex_uniform = glGetUniformLocation(ctx.blit_program, "screen_texture");
            ctx.blit_texel_uniform = glGetUniformLocation(ctx.blit_program, "texel_size");
        }

        /* Shadow Map */
        {
#define SHADOW_SIZE 1024
            float border_color[] = {1.0, 1.0, 1.0, 1.0};

            if (!beneath_opengl_shader_create(
                    &ctx.shadow_program,
                    beneath_opengl_shader_shadow_vertex,
                    beneath_opengl_shader_shadow_fragment,
                    print))
            {
                print(__FILE__, __LINE__, "cannot compile shadow shaders !!!\n");
                return false;
            }

            ctx.shadow_uniform_pv = glGetUniformLocation(ctx.shadow_program, "pv");

            glGenTextures(1, &ctx.shadow_texture_depth);
            glBindTexture(GL_TEXTURE_2D, ctx.shadow_texture_depth);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

            glGenFramebuffers(1, &ctx.shadow_fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, ctx.shadow_fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ctx.shadow_texture_depth, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            v3 light_direction = vm_v3_normalize(vm_v3(
                draw_call->lightning->directional.direction[0],
                draw_call->lightning->directional.direction[1],
                draw_call->lightning->directional.direction[2]));

            v3 center = vm_v3_zero;
            float distance = 10.0f;

            shadow_light_position = vm_v3_sub(center, vm_v3_mulf(light_direction, distance));

            /*v3 shadow_light_position = vm_v3(-5.0f, 7.0f, 4.0f);*/
            m4x4 shadow_projection = vm_m4x4_orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);
            m4x4 shadow_view = vm_m4x4_lookAt(shadow_light_position, vm_v3_zero, vm_v3_up);
            shadow_pv = vm_m4x4_mul(shadow_projection, shadow_view);
        }
    }

    beneath_mesh *mesh = draw_call->mesh;
    beneath_opengl_shader shader_active = ctx.shaders[ctx.shaders_active_index];

    if (mesh->changed)
    {
        int sizeM4x4 = sizeof(float) * 16;
        int i;

        unsigned int buffer_index = mesh->id * BENEATH_OPENGL_SHADER_LAYOUT_COUNT;

        beneath_opengl_draw_call_print(draw_call, print);

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

    /* (1) Shadow Map Render Pass */
    if (draw_call->shadow)
    {
        glCullFace(GL_FRONT);
        glBindFramebuffer(GL_FRAMEBUFFER, ctx.shadow_fbo);
        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
        glClear(GL_DEPTH_BUFFER_BIT);

        glUseProgram(ctx.shadow_program);
        glUniformMatrix4fv(ctx.shadow_uniform_pv, 1, GL_FALSE, shadow_pv.e);
        glBindVertexArray(ctx.storage_vertex_array[mesh->id]);
        glDrawElementsInstanced(GL_TRIANGLES, (int)mesh->indices_count, GL_UNSIGNED_INT, 0, (int)draw_call->models_count);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, (int)state->window_width, (int)state->window_height);
        glCullFace(GL_BACK);
    }

    /* Post processing enabled. Render to fbo_screen */
    if (draw_call->pixelize || draw_call->volumetric)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ctx.fbo_screen);
        glViewport(0, 0, ctx.fbo_screen_width, ctx.fbo_screen_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    {
        glUseProgram(shader_active.program_id);
        glBindVertexArray(ctx.storage_vertex_array[mesh->id]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, ctx.shadow_texture_depth);
        glUniform1i(glGetUniformLocation(shader_active.program_id, "shadow_map"), 1);
        glUniformMatrix4fv(shader_active.uniform_locations[BENEATH_OPENGL_SHADER_UNIFORM_LOCATION_PROJECTION_VIEW], 1, GL_FALSE, projection_view);
        glUniformMatrix4fv(glGetUniformLocation(shader_active.program_id, "light_space_matrix"), 1, GL_FALSE, shadow_pv.e);
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

            if (draw_call->lightning)
            {
                beneath_light_directional *dl = &draw_call->lightning->directional;
                glUniform3f(glGetUniformLocation(shader_active.program_id, "dir_light.direction"), dl->direction[0], dl->direction[1], dl->direction[2]);
                glUniform3f(glGetUniformLocation(shader_active.program_id, "dir_light.ambient"), dl->ambient[0], dl->ambient[1], dl->ambient[2]);
                glUniform3f(glGetUniformLocation(shader_active.program_id, "dir_light.diffuse"), dl->diffuse[0], dl->diffuse[1], dl->diffuse[2]);
                glUniform3f(glGetUniformLocation(shader_active.program_id, "dir_light.specular"), dl->specular[0], dl->specular[1], dl->specular[2]);
            }
        }

        glDrawElementsInstanced(GL_TRIANGLES, (int)mesh->indices_count, GL_UNSIGNED_INT, 0, (int)draw_call->models_count);
        glBindVertexArray(0);
    }

    /* --- Post-processing --- */
    if (draw_call->pixelize || draw_call->volumetric)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, (int)state->window_width, (int)state->window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (draw_call->pixelize)
        {
            /* Use the pixel shader program */
            glUseProgram(ctx.blit_program);
            glBindVertexArray(ctx.fbo_screen_vao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ctx.fbo_screen_color);
            glUniform1i(ctx.blit_tex_uniform, 0);
            glUniform2f(ctx.blit_texel_uniform, 1.0f / (float)ctx.fbo_screen_width, 1.0f / (float)ctx.fbo_screen_height);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        else if (draw_call->volumetric)
        {
            int i;
            m4x4 camera_view_projection;

            for (i = 0; i < 16; ++i)
            {
                camera_view_projection.e[i] = projection_view[i];
            }

            v3 cam_pos = vm_v3(camera_position[0], camera_position[1], camera_position[2]);
            v3 light_dir = vm_v3_normalize(vm_v3(
                draw_call->lightning->directional.direction[0],
                draw_call->lightning->directional.direction[1],
                draw_call->lightning->directional.direction[2]));

            // Move “back” along the light direction
            v3 light_point = vm_v3_sub(cam_pos, vm_v3_mulf(light_dir, 20.0f));

            v4 clip = vm_m4x4_mul_v4(camera_view_projection, vm_v4(light_point.x, light_point.y, light_point.z, 1.0f));
            clip.x /= clip.w;
            clip.y /= clip.w;

            float light_x = clip.x * 0.5f + 0.5f;
            float light_y = clip.y * 0.5f + 0.5f;

            glUniform2f(ctx.volumetric_uniform_light_pos, light_x, light_y);

            /* Use volumetric program */
            glUseProgram(ctx.volumetric_program);
            glBindVertexArray(ctx.fbo_screen_vao);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ctx.fbo_screen_color);
            glUniform1i(ctx.volumetric_uniform_screen_texture, 0);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, ctx.shadow_texture_depth);
            glUniform1i(ctx.volumetric_uniform_shadow_map, 1);

            glUniform2f(ctx.volumetric_uniform_light_pos, light_x, light_y);

            glUniform3f(ctx.volumetric_uniform_camera_pos, camera_position[0], camera_position[1], camera_position[2]);

            /*
            glUniform1f(ctx.volumetric_uniform_scattering, 0.08f);
            glUniform1f(ctx.volumetric_uniform_exposure, 1.5f);
*/
            glUniform1f(ctx.volumetric_uniform_scattering, 0.04f);
            glUniform1f(ctx.volumetric_uniform_exposure, 1.0f);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        else
        {
            /* Use base post processing shader */
            glUseProgram(ctx.post_process_base_program);
            glBindVertexArray(ctx.fbo_screen_vao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ctx.fbo_screen_color);
            glUniform1i(ctx.post_process_base_uniform_screen_texture, 0);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

    draw_call->changed = false;
    draw_call->mesh->changed = false;

    return true;
}

#endif /* WIN32_BENEATH_OPENGL */