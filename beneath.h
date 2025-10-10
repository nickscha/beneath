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
 * # Beneath Platform Controller state
 * #############################################################################
 */
typedef enum beneath_key
{
  /* Mouse buttons */
  BENEATH_KEY_MOUSE_LEFT = 0,
  BENEATH_KEY_MOUSE_RIGHT,
  BENEATH_KEY_MOUSE_MIDDLE,

  /* Function keys */
  BENEATH_KEY_F1,
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
  beneath_controller_state keys[BENEATH_KEY_COUNT];

  /* Mouse movement */
  beneath_bool mouse_attached;
  float mouse_offset_scroll;
  float mouse_offset_x;
  float mouse_offset_y;
  int mouse_position_x;
  int mouse_position_y;

} beneath_controller_input;

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

typedef beneath_bool (*beneath_api_time_sleep)(
    unsigned int milliseconds /* The number of milliseconds to sleep */
);

typedef unsigned int (*beneath_api_perf_cycle_count)(void);
typedef double (*beneath_api_perf_time_nanoseconds)(void);

typedef struct beneath_api
{
  /* Platform IO */
  beneath_api_io_print io_print;           /* Prints the specified string to console */
  beneath_api_io_file_size io_file_size;   /* Returns the file size */
  beneath_api_io_file_read io_file_read;   /* Reads the specified file into the buffer */
  beneath_api_io_file_write io_file_write; /* Writes the specified buffer to a file */

  /* Time */
  beneath_api_time_sleep time_sleep; /* Sleeps for the specified amound of milliseconds */

  /* Platform Performance Metrics */
  beneath_api_perf_cycle_count perf_cycle_count;           /* The current cpu cycle count  */
  beneath_api_perf_time_nanoseconds perf_time_nanoseconds; /* The curent nanoseconds epoch */

  /* Platform Graphics */
  /* graphics_toggle_wireframe  */
  /* graphics_draw              */

} beneath_api;

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

  char window_title[64];
  unsigned int window_width;
  unsigned int window_height;
  float window_clear_color_r;
  float window_clear_color_g;
  float window_clear_color_b;
  float window_clear_color_a;

  int frames_per_second_target; /* < 0 = VSYNC, 0 = unlimited, > 0 = Target FPS set to amount */

  unsigned int frames_per_second;
  double delta_time;

} beneath_state;

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
