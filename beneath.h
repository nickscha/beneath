/* beneath.h - v0.1 - public domain data structures - nickscha 2025

A C89 standard compliant, single header, nostdlib (no C Standard Library) Beneath Graphics Engine (BENEATH).

LICENSE

  Placed in the public domain and also MIT licensed.
  See end of file for detailed license information.

*/
#ifndef BENEATH_H
#define BENEATH_H

/* #############################################################################
 * # COMPILER SETTINGS
 * #############################################################################
 */
/* Check if using C99 or later (inline is supported) */
#if __STDC_VERSION__ >= 199901L
#define BENEATH_INLINE inline
#elif defined(__GNUC__) || defined(__clang__)
#define BENEATH_INLINE __inline__
#elif defined(_MSC_VER)
#define BENEATH_INLINE __inline
#else
#define BENEATH_INLINE
#endif

#define BENEATH_API static

#ifndef BENEATH_PLATFORM_LAYER
#define BENEATH_APPLICATION_LAYER
#else
/* Platform layer checks */
#ifndef BENEATH_PLATFORM_LAYER_NAME
#error "A platform layer needs to specifiy its name via: 'BENEATH_PLATFORM_LAYER_NAME' or -DBENEATH_PLATFORM_LAYER_NAME=...!"
#endif /* BENEATH_PLATFORM_LAYER_NAME */
#ifndef BENEATH_APPLICATION_LAYER_NAME
#error "A platform layer needs to specifiy the application name (WITHOUT QUOTES!): 'BENEATH_APPLICATION_LAYER_NAME' or -DBENEATH_APPLICATION_LAYER_NAME=...!"
#endif /* BENEATH_APPLICATION_LAYER_NAME */
#endif

/* This macros are needed for dyanmically #include BENEATH_PLATFORM_LAYER_NAME and appending '.c' to it  */
#define BENEATH_STRINGIZE_DETAIL(x) #x
#define BENEATH_STRINGIZE(x) BENEATH_STRINGIZE_DETAIL(x)

/* Helper Macros */
#define BENEATH_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#ifdef _MSC_VER
#pragma function(memset)
#endif
void *memset(void *dest, int c, unsigned int count)
{
  char *bytes = (char *)dest;
  while (count--)
  {
    *bytes++ = (char)c;
  }
  return dest;
}

#ifdef _MSC_VER
#pragma function(memcpy)
#endif
void *memcpy(void *dest, const void *src, unsigned int count)
{
  char *dest8 = (char *)dest;
  const char *src8 = (const char *)src;
  while (count--)
  {
    *dest8++ = *src8++;
  }
  return dest;
}

void beneath_strcpy(char *dest, char *src, int dest_size)
{
  int i;

  if (dest_size == 0)
  {
    return;
  }

  for (i = 0; i < dest_size - 1 && src[i] != '\0'; i++)
  {
    dest[i] = src[i];
  }

  dest[i] = '\0';
}

/* #############################################################################
 * # Beneath Types
 * #############################################################################
 */
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

typedef unsigned char beneath_bool;

/* #############################################################################
 * # Beneath Memory Block
 * #############################################################################
 */
typedef struct beneath_memory
{
  beneath_bool memory_initialized;
  unsigned int memory_size;
  unsigned int memory_offset; /* By default the first entry of the memory is the beneath_state struct */
  void *memory;

} beneath_memory;

/* #############################################################################
 * # Beneath Rendering
 * #############################################################################
 */
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
  unsigned int *indices;

} beneath_mesh;

/* SoA style draw call */
typedef struct beneath_draw_call
{
  unsigned int id;
  unsigned int data_capacity; /* How many instances can be added to the buffers */
  beneath_bool changed;       /* Did the draw call change? If yes we may need to resend buffer data */

  beneath_mesh *mesh; /* The mesh data */

  unsigned int models_count;          /* Number of models == Number of instances */
  unsigned int colors_count;          /* Per model color */
  unsigned int texture_indices_count; /* Per model texture_index. If a texture index is specified we ignore colors */

  float *models;        /* Instance data model matrices (Matrix 4x4 = 16 floats) */
  float *colors;        /* Instance data model colors (Vec3 = 3 floats) */
  int *texture_indices; /* Instance data texture indices (1 int) */

  beneath_bool pixelize; /* Temporary */

} beneath_draw_call;

BENEATH_API BENEATH_INLINE beneath_bool beneath_draw_call_append(
    beneath_draw_call *draw_call,
    float model[16],
    float color[3],
    int texture_index)
{
  if (!draw_call || draw_call->data_capacity < 1 || (!model && !color && texture_index < 0))
  {
    return false;
  }

  if (model)
  {
    unsigned int i;
    unsigned int base_index = draw_call->models_count * 16;

    /* Not enough memory allocated to store the data */
    if (draw_call->models_count + 1 > draw_call->data_capacity)
    {
      return false;
    }

    for (i = 0; i < 16; ++i)
    {
      draw_call->models[base_index + i] = model[i];
    }

    draw_call->models_count++;
  }

  if (color)
  {
    unsigned int i;
    unsigned int base_index = draw_call->colors_count * 3;

    /* Not enough memory allocated to store the data */
    if (draw_call->colors_count + 1 > draw_call->data_capacity)
    {
      return false;
    }

    for (i = 0; i < 3; ++i)
    {
      draw_call->colors[base_index + i] = color[i];
    }

    draw_call->colors_count++;
  }

  if (texture_index >= 0)
  {
    /* Not enough memory allocated to store the data */
    if (draw_call->texture_indices_count + 1 > draw_call->data_capacity)
    {
      return false;
    }

    draw_call->texture_indices[draw_call->texture_indices_count] = texture_index;
    draw_call->texture_indices_count++;
  }

  return true;
}

BENEATH_API BENEATH_INLINE unsigned int beneath_draw_call_hash(beneath_draw_call *dc)
{
  unsigned int hash = 2166136261u; /* FNV-1a offset basis */
  unsigned int prime = 16777619u;  /* FNV-1a prime */

  /* Draw call fields */
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
    beneath_bool use_mesh_color;

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
    use_mesh_color = (m->colors_count > 0 && dc->colors_count == 0 && dc->texture_indices_count == 0);
    hash ^= (use_mesh_color ? 1 : 0);
    hash *= prime;

    /* Indices count probably doesn't affect shader layout, but we can include it for safety */
    hash ^= (m->indices_count > 0 ? 1 : 0);
    hash *= prime;
  }

  return hash;
}

/* #############################################################################
 * # Beneath Platform Controller state
 * #############################################################################
 */
typedef enum beneath_key
{
  /* Function keys */
  BENEATH_KEY_F1 = 0,
  BENEATH_KEY_F2,
  BENEATH_KEY_F3,
  BENEATH_KEY_F4,
  BENEATH_KEY_F5,
  BENEATH_KEY_F6,
  BENEATH_KEY_F7,
  BENEATH_KEY_F8,
  BENEATH_KEY_F9,
  BENEATH_KEY_F10,
  BENEATH_KEY_F11,
  BENEATH_KEY_F12,

  /* Special keys */
  BENEATH_KEY_BACKSPACE,
  BENEATH_KEY_TAB,
  BENEATH_KEY_RETURN,
  BENEATH_KEY_SHIFT,
  BENEATH_KEY_CONTROL,
  BENEATH_KEY_ALT,
  BENEATH_KEY_CAPSLOCK,
  BENEATH_KEY_SPACE,
  BENEATH_KEY_ARROW_LEFT,
  BENEATH_KEY_ARROW_UP,
  BENEATH_KEY_ARROW_RIGHT,
  BENEATH_KEY_ARROW_DOWN,

  /* Numeric keys */
  BENEATH_KEY_0,
  BENEATH_KEY_1,
  BENEATH_KEY_2,
  BENEATH_KEY_3,
  BENEATH_KEY_4,
  BENEATH_KEY_5,
  BENEATH_KEY_6,
  BENEATH_KEY_7,
  BENEATH_KEY_8,
  BENEATH_KEY_9,

  /* Alphanumeric keys */
  BENEATH_KEY_A,
  BENEATH_KEY_B,
  BENEATH_KEY_C,
  BENEATH_KEY_D,
  BENEATH_KEY_E,
  BENEATH_KEY_F,
  BENEATH_KEY_G,
  BENEATH_KEY_H,
  BENEATH_KEY_I,
  BENEATH_KEY_J,
  BENEATH_KEY_K,
  BENEATH_KEY_L,
  BENEATH_KEY_M,
  BENEATH_KEY_N,
  BENEATH_KEY_O,
  BENEATH_KEY_P,
  BENEATH_KEY_Q,
  BENEATH_KEY_R,
  BENEATH_KEY_S,
  BENEATH_KEY_T,
  BENEATH_KEY_U,
  BENEATH_KEY_V,
  BENEATH_KEY_W,
  BENEATH_KEY_X,
  BENEATH_KEY_Y,
  BENEATH_KEY_Z,

  /* Total key count */
  BENEATH_KEY_COUNT

} beneath_key;

typedef struct beneath_controller_state
{
  unsigned int half_transition_count; /* How often a key was pressed between two frames */
  beneath_bool ended_down;            /* Key hold down at end of frame */
  beneath_bool active;                /* Toggles on/off when key pressed again */
  beneath_bool pressed;               /* Key was pressed this frame */

} beneath_controller_state;

typedef struct beneath_controller_input
{
  /* Keyboard Events */
  beneath_controller_state keys[BENEATH_KEY_COUNT];

  /* Mouse movement */
  beneath_bool mouse_attached;
  float mouse_offset_scroll;
  float mouse_offset_x;
  float mouse_offset_y;
  int mouse_position_x;
  int mouse_position_y;
  beneath_controller_state mouse_left;
  beneath_controller_state mouse_right;
  beneath_controller_state mouse_middle;

} beneath_controller_input;

/* #############################################################################
 * # Beneath Platform State
 * #############################################################################
 *
 * These information are exchanged between the platform and application state.
 */
#define BENEATH_STATE_FRAMES_PER_SECOND_VSYNC -1
#define BENEATH_STATE_FRAMES_PER_SECOND_UNLIMITED 0

typedef enum beneath_state_changed_flags
{
  BENEATH_STATE_CHANGED_FLAG_NOTHING = 0u,
  BENEATH_STATE_CHANGED_FLAG_WINDOW = 1u << 0,
  BENEATH_STATE_CHANGED_FLAG_FRAMES_PER_SECOND_TARGET = 1u << 1

} beneath_state_changed_flags;

typedef enum beneath_window_mode
{
  BENEATH_WINDOW_MODE_WINDOWED = 0,
  BENEATH_WINDOW_MODE_FULLSCREEN,
  BENEATH_WINDOW_MODE_BORDERLESS

} beneath_window_mode;

typedef struct beneath_state
{
  unsigned int changed_flags; /* bitmask of beneath_state_changed_flags */

  beneath_bool running;
  beneath_bool wireframe;
  beneath_window_mode window_mode;
  beneath_bool window_clip_cursor;

  char window_title[128];
  unsigned int window_width;
  unsigned int window_height;
  float window_clear_color_r;
  float window_clear_color_g;
  float window_clear_color_b;
  float window_clear_color_a;

  int frames_per_second_target; /* < 0 = VSYNC, 0 = unlimited, > 0 = Target FPS set to amount */

  unsigned int frames_per_second;
  double time; /* Total elapsed time in seconds */
  double delta_time;

} beneath_state;

/* #############################################################################
 * # Beneath Platform provided api calls
 * #############################################################################
 */
typedef void (*beneath_api_io_print)(
    char *filename, /* The current compilation unit filename (usualy __FILE__)*/
    int line,       /* The current compilation unit line number (usually __LINE__)*/
    char *string    /* The string to print to the console */
);

typedef beneath_bool (*beneath_api_io_file_size)(
    char *filename,         /* The filename/path of which to return the file size */
    unsigned int *file_size /* The gathered file size in bytes */
);

typedef beneath_bool (*beneath_api_io_file_read)(
    char *filename,                    /* The filename/path to read into the file_buffer */
    unsigned char *file_buffer,        /* The user provided file_buffer large enough to hold the file contents */
    unsigned int file_buffer_capacity, /* The capacity/max site of the file_buffer */
    unsigned int *file_buffer_size     /* The total number of bytes read by this function */
);

typedef beneath_bool (*beneath_api_io_file_write)(
    char *filename,          /* The filename/path to write the file_buffer to */
    unsigned char *buffer,   /* The file content to be written */
    unsigned int buffer_size /* The size of the file content buffer */
);

/* Platform Performance Metrics */
typedef unsigned int (*beneath_api_perf_cycle_count)(void);
typedef double (*beneath_api_perf_time_nanoseconds)(void);

/* Platform Graphics */
typedef beneath_bool (*beneath_api_graphics_draw)(
    beneath_state *state,         /* The state */
    beneath_draw_call *draw_call, /* The draw call instanced objects */
    float projection_view[16],    /* The projection view matrix */
    float camera_position[3]      /* The camera x,y,z position */
);

typedef struct beneath_api
{
  /* Platform IO */
  beneath_api_io_print io_print;           /* Prints the specified string to console */
  beneath_api_io_file_size io_file_size;   /* Returns the file size */
  beneath_api_io_file_read io_file_read;   /* Reads the specified file into the buffer */
  beneath_api_io_file_write io_file_write; /* Writes the specified buffer to a file */

  /* Platform Performance Metrics */
  beneath_api_perf_cycle_count perf_cycle_count;           /* The current cpu cycle count  */
  beneath_api_perf_time_nanoseconds perf_time_nanoseconds; /* The curent nanoseconds epoch */

  /* Platform Graphics */
  beneath_api_graphics_draw graphics_draw;
  /* graphics_toggle_wireframe  */

} beneath_api;

/* #############################################################################
 * # Beneath Application entry/update point
 * #############################################################################
 */
#ifdef BENEATH_LIB /* Dynamically linking shared library with platform layer */

#ifdef BENEATH_PLATFORM_LAYER
void beneath_update_stub(
    beneath_memory *memory,          /* The total block of memory handed to the application */
    beneath_controller_input *input, /* The input (keyboard, mouse, joystick) state */
    beneath_api *api                 /* Platform specific api calls that made accessible for the application */
)
{
  beneath_state *state = (beneath_state *)memory->memory;

  if (!memory->memory_initialized)
  {
    state->window_width = 800;
    state->window_width = 600;
    state->window_clear_color_r = 1.0f;
    state->window_clear_color_g = 0.0f;
    state->window_clear_color_b = 0.0f;
    state->window_clear_color_a = 1.0f;

    api->io_print(__FILE__, __LINE__, "[beneath][error]\n");
    api->io_print(__FILE__, __LINE__, "[beneath][error] Expected to find '" BENEATH_STRINGIZE(BENEATH_APPLICATION_LAYER_NAME) ".dll' since 'BENEATH_LIB' was defined (shared library linking)\n");
    api->io_print(__FILE__, __LINE__, "[beneath][error] If you want to statically link your application with '" BENEATH_PLATFORM_LAYER_NAME "':\n");
    api->io_print(__FILE__, __LINE__, "[beneath][error] - Undefine 'BENEATH_LIB'\n");
    api->io_print(__FILE__, __LINE__, "[beneath][error] - This will include your '.c' file directly with the platform file\n");
    api->io_print(__FILE__, __LINE__, "[beneath][error]\n");

    memory->memory_initialized = true;
  }

  (void)input;

  /* Immediatly stop running */
  state->running = false;
}

typedef void (*beneath_update_function)(
    beneath_memory *memory,          /* The total block of memory handed to the application */
    beneath_controller_input *input, /* The input (keyboard, mouse, joystick) state */
    beneath_api *api                 /* Platform specific api calls that made accessible for the application */
);
static beneath_update_function beneath_update = beneath_update_stub;
#endif /* BENEATH_PLATFORM_LAYER */

/* If we are not statically linking we need at least a Dll Entry Point in WIN32 for the application */
#ifdef BENEATH_APPLICATION_LAYER
#ifdef _WIN32
#ifdef __clang__
#elif __GNUC__
__attribute((externally_visible))
#endif
#ifdef __i686__
__attribute((force_align_arg_pointer))
#endif
int DllMainCRTStartup(void)
{
  return 1;
}
#endif /* _WIN32 */
#endif /* BENEATH_APPLICATION_LAYER */

#else /* Statically linking the application with the beneath platform layer */
#if defined(BENEATH_PLATFORM_LAYER) && defined(BENEATH_APPLICATION_LAYER_NAME)
#include BENEATH_STRINGIZE(BENEATH_APPLICATION_LAYER_NAME.c)
#endif
#endif /* BENEATH_LIB */
#endif /* BENEATH_H */

/*
   ------------------------------------------------------------------------------
   This software is available under 2 licenses -- choose whichever you prefer.
   ------------------------------------------------------------------------------
   ALTERNATIVE A - MIT License
   Copyright (c) 2025 nickscha
   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is furnished to do
   so, subject to the following conditions:
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
   ------------------------------------------------------------------------------
   ALTERNATIVE B - Public Domain (www.unlicense.org)
   This is free and unencumbered software released into the public domain.
   Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
   software, either in source code form or as a compiled binary, for any purpose,
   commercial or non-commercial, and by any means.
   In jurisdictions that recognize copyright laws, the author or authors of this
   software dedicate any and all copyright interest in the software to the public
   domain. We make this dedication for the benefit of the public at large and to
   the detriment of our heirs and successors. We intend this dedication to be an
   overt act of relinquishment in perpetuity of all present and future rights to
   this software under copyright law.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   ------------------------------------------------------------------------------
*/
