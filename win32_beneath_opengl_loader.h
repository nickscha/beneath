#ifndef WIN32_BENEATH_OPENGL_LOADER_H
#define WIN32_BENEATH_OPENGL_LOADER_H

__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001; /* NVIDIA Force discrete GPU */
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;   /* AMD Force discrete GPU    */

/* #############################################################################
 * # [Section] OpenGL Functions
 * #############################################################################
 */
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_TYPE_RGBA_ARB 0x202B
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_ALPHA_BITS_ARB 0x201B
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_SAMPLE_BUFFERS_ARB 0x2041
#define WGL_SAMPLES_ARB 0x2042
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001

#define GL_DEPTH_COMPONENT 0x1902
#define GL_ALPHA 0x1906
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_FRAMEBUFFER_UNDEFINED 0x8219
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_SAMPLES_PASSED 0x8914
#define GL_ANY_SAMPLES_PASSED 0x8C2F
#define GL_QUERY_RESULT 0x8866
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define GL_COMPILE_STATUS 0x8B81
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_LINK_STATUS 0x8B82
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_INT 0x1404
#define GL_FLOAT 0x1406
#define GL_TRUE 1
#define GL_FALSE 0
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_DEPTH_TEST 0x0B71
#define GL_MULTISAMPLE 0x809D
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_FRONT_AND_BACK 0x0408
#define GL_LINE 0x1B01
#define GL_FILL 0x1B02
#define GL_CULL_FACE 0x0B44
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_CCW 0x0901
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_UNSIGNED_INT 0x1405
#define GL_SYNC_GPU_COMMANDS_COMPLETE 0x9117
#define GL_FRAMEBUFFER 0x8D40
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_DEPTH_COMPONENT 0x1902
#define GL_TEXTURE_2D 0x0DE1
#define GL_RGBA 0x1908
#define GL_RED 0x1903
#define GL_UNSIGNED_BYTE 0x1401
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_LINEAR 0x2601
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_RENDERBUFFER 0x8D41
#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_NEAREST 0x2600
#define GL_NONE 0
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_TEXTURE_BORDER_COLOR 0x1004
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_BLEND 0x0BE2
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303

typedef void *(*PFNWGLCREATECONTEXTPROC)(void *unnamedParam1);
typedef void *(*PFNWGLGETCURRENTCONTEXTPROC)(void);
typedef void *(*PFNWGLGETCURRENTDCPROC)(void);
typedef int (*PFNWGLDELETECONTEXTPROC)(void *unnamedParam1);
typedef int (*PFNWGLMAKECURRENTPROC)(void *unnamedParam1, void *unnamedParam2);
typedef PROC (*PFNWGLGETPROCADDRESSPROC)(char *unnamedParam1);
typedef int (*PFNWGLCHOOSEPIXELFORMATARBPROC)(void *hdc, int *piAttribIList, float *pfAttribFList, unsigned int nMaxFormats, int *piFormats, unsigned int *nNumFormats);
typedef void *(*PFNWGLCREATECONTEXTATTRIBSARBPROC)(void *hDC, void *hShareContext, int *attribList);
typedef int (*PFNWGLSWAPINTERVALEXTPROC)(int interval);

typedef unsigned char *(*PFNGLGETSTRINGPROC)(unsigned int name);
typedef void (*PFNGLCLEARCOLORPROC)(float red, float green, float blue, float alpha);
typedef void (*PFNGLCLEARPROC)(unsigned int mask);
typedef unsigned int (*PFNGLGETERRORPROC)(void);
typedef void (*PFNGLENABLEPROC)(unsigned int cap);
typedef void (*PFNGLDISABLEPROC)(unsigned int cap);
typedef void (*PFNGLBLENDFUNCPROC)(unsigned int sfactor, unsigned int dfactor);
typedef void (*PFNGLPOLYGONMODEPROC)(unsigned int face, unsigned int mode);
typedef void (*PFNGLCULLFACEPROC)(unsigned int mode);
typedef void (*PFNGLFRONTFACEPROC)(unsigned int mode);
typedef void (*PFNGLVIEWPORTPROC)(int x, int y, int width, int height);
typedef void (*PFNGLDRAWELEMENTSPROC)(unsigned int mode, int count, unsigned int type, void *indices);
typedef void (*PFNGLCOLORMASKPROC)(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
typedef void (*PFNGLDEPTHMASKPROC)(unsigned char flag);
typedef void (*PFNGLREADBUFFERPROC)(unsigned int mode);
typedef void (*PFNGLDRAWBUFFERPROC)(unsigned int mode);
typedef void (*PFNGLREADPIXELSPROC)(int x, int y, int width, int height, unsigned int format, unsigned int type, void *pixels);
typedef void (*PFNGLGENTEXTURESPROC)(int n, unsigned int *textures);
typedef void (*PFNGLBINDTEXTUREPROC)(unsigned int target, unsigned int texture);
typedef void (*PFNGLTEXIMAGE2DPROC)(unsigned int target, int level, int internalformat, int width, int height, int border, int format, unsigned int type, void *pixels);
typedef void (*PFNGLTEXPARAMETERIPROC)(unsigned int target, unsigned int pname, int param);
typedef void (*PFNGLTEXPARAMETERFVPROC)(unsigned int target, unsigned int pname, float *params);
typedef struct __GLsync *GLsync;
typedef unsigned int (*PFNGLCREATESHADERPROC)(unsigned int shaderType);
typedef unsigned int (*PFNGLCREATEPROGRAMPROC)(void);
typedef void (*PFNGLATTACHSHADERPROC)(unsigned int program, unsigned int shader);
typedef void (*PFNGLSHADERSOURCEPROC)(unsigned int shader, int count, char **string, int *length);
typedef void (*PFNGLCOMPILESHADERPROC)(unsigned int shader);
typedef void (*PFNGLGETSHADERIVPROC)(unsigned int shader, unsigned int pname, int *params);
typedef void (*PFNGLGETSHADERINFOLOGPROC)(unsigned int shader, int maxLength, int *length, char *infoLog);
typedef void (*PFNGLLINKPROGRAMPROC)(unsigned int program);
typedef void (*PFNGLGETPROGRAMIVPROC)(unsigned int program, unsigned int pname, int *params);
typedef void (*PFNGLGETPROGRAMINFOLOGPROC)(unsigned int program, int maxLength, int *length, char *infoLog);
typedef void (*PFNGLDELETESHADERPROC)(unsigned int shader);
typedef void (*PFNGLGENVERTEXARRAYSPROC)(int n, unsigned int *arrays);
typedef void (*PFNGLGENBUFFERSPROC)(int n, unsigned int *buffers);
typedef void (*PFNGLBINDVERTEXARRAYPROC)(unsigned int array);
typedef void (*PFNGLBINDBUFFERPROC)(unsigned int target, unsigned int buffer);
typedef void (*PFNGLBUFFERDATAPROC)(unsigned int target, int size, void *data, unsigned int usage);
typedef void (*PFNGLBUFFERSUBDATAPROC)(unsigned int target, int offset, int size, void *data);
typedef void (*PFNGLVERTEXATTRIBPOINTERPROC)(unsigned int index, int size, unsigned int type, unsigned char normalized, int stride, void *pointer);
typedef void (*PFNGLENABLEVERTEXATTRIBARRAYPROC)(unsigned int index);
typedef void (*PFNGLDELETEPROGRAMPROC)(unsigned int program);
typedef void (*PFNGLUSEPROGRAMPROC)(unsigned int program);
typedef void (*PFNGLDRAWARRAYSPROC)(unsigned int mode, int first, int count);
typedef void (*PFNGLDELETEVERTEXARRAYSPROC)(int n, unsigned int *arrays);
typedef void (*PFNGLDELETEBUFFERSPROC)(int n, unsigned int *buffers);
typedef int (*PFNGLGETUNIFORMLOCATIONPROC)(unsigned int program, char *name);
typedef void (*PFNGLUNIFORMMATRIX4FVPROC)(int location, int count, unsigned char transpose, float *value);
typedef void (*PFNGLUNIFORM1FPROC)(int location, float v0);
typedef void (*PFNGLUNIFORM2FPROC)(int location, float v0, float v1);
typedef void (*PFNGLUNIFORM3FPROC)(int location, float v0, float v1, float v2);
typedef void (*PFNGLGENQUERIESPROC)(int n, unsigned int *ids);
typedef void (*PFNGLBEGINQUERYPROC)(unsigned int target, unsigned int id);
typedef void (*PFNGLENDQUERYPROC)(unsigned int target);
typedef void (*PFNGLGETQUERYOBJECTIVPROC)(unsigned int id, unsigned int pname, int *params);
typedef void (*PFNGLGETQUERYOBJECTUIVPROC)(unsigned int id, unsigned int pname, unsigned int *params);
typedef void (*PFNGLDELETEQUERIESPROC)(int n, unsigned int *ids);
typedef GLsync (*PFNGLFENCESYNCPROC)(unsigned int condition, unsigned int flags);
typedef void (*PFNGLGENFRAMEBUFFERSPROC)(int n, unsigned int *ids);
typedef void (*PFNGLBINDFRAMEBUFFERPROC)(unsigned int target, unsigned int framebuffer);
typedef void (*PFNGLFRAMEBUFFERTEXTURE2DPROC)(unsigned int target, unsigned int attachment, unsigned int textarget, unsigned int texture, int level);
typedef void (*PFNGLGENRENDERBUFFERSPROC)(int n, unsigned int *renderbuffers);
typedef void (*PFNGLBINDRENDERBUFFERPROC)(unsigned int target, unsigned int renderbuffer);
typedef void (*PFNGLRENDERBUFFERSTORAGEPROC)(unsigned int target, unsigned int internalformat, int width, int height);
typedef void (*PFNGLFRAMEBUFFERRENDERBUFFERPROC)(unsigned int target, unsigned int attachment, unsigned int renderbuffertarget, unsigned int renderbuffer);
typedef unsigned int (*PFNGLCHECKFRAMEBUFFERSTATUSPROC)(unsigned int target);
typedef void (*PFNGLDRAWELEMENTSINSTANCEDPROC)(unsigned int mode, int count, unsigned int type, void *indices, int primcount);
typedef void (*PFNGLVERTEXATTRIBDIVISORPROC)(unsigned int index, unsigned int divisor);
typedef void (*PFNGLVERTEXATTRIBIPOINTERPROC)(unsigned int index, int size, unsigned int type, int stride, void *pointer);
typedef void (*PFNGLUNIFORM1IPROC)(int location, int v0);
typedef void (*PFNGLACTIVETEXTUREPROC)(unsigned int texture);

static PFNWGLCREATECONTEXTPROC wglCreateContext;
static PFNWGLGETCURRENTCONTEXTPROC wglGetCurrentContext;
static PFNWGLGETCURRENTDCPROC wglGetCurrentDC;
static PFNWGLDELETECONTEXTPROC wglDeleteContext;
static PFNWGLMAKECURRENTPROC wglMakeCurrent;
static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

static PFNGLGETSTRINGPROC glGetString;
static PFNGLCLEARCOLORPROC glClearColor;
static PFNGLCLEARPROC glClear;
static PFNGLGETERRORPROC glGetError;
static PFNGLENABLEPROC glEnable;
static PFNGLDISABLEPROC glDisable;
static PFNGLBLENDFUNCPROC glBlendFunc;
static PFNGLPOLYGONMODEPROC glPolygonMode;
static PFNGLCULLFACEPROC glCullFace;
static PFNGLFRONTFACEPROC glFrontFace;
static PFNGLVIEWPORTPROC glViewport;
static PFNGLDRAWELEMENTSPROC glDrawElements;
static PFNGLCOLORMASKPROC glColorMask;
static PFNGLDEPTHMASKPROC glDepthMask;
static PFNGLREADBUFFERPROC glReadBuffer;
static PFNGLDRAWBUFFERPROC glDrawBuffer;
static PFNGLREADPIXELSPROC glReadPixels;
static PFNGLGENTEXTURESPROC glGenTextures;
static PFNGLBINDTEXTUREPROC glBindTexture;
static PFNGLTEXIMAGE2DPROC glTexImage2D;
static PFNGLTEXPARAMETERIPROC glTexParameteri;
static PFNGLTEXPARAMETERFVPROC glTexParameterfv;
static PFNGLCREATESHADERPROC glCreateShader;
static PFNGLCREATEPROGRAMPROC glCreateProgram;
static PFNGLATTACHSHADERPROC glAttachShader;
static PFNGLSHADERSOURCEPROC glShaderSource;
static PFNGLCOMPILESHADERPROC glCompileShader;
static PFNGLGETSHADERIVPROC glGetShaderiv;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
static PFNGLLINKPROGRAMPROC glLinkProgram;
static PFNGLGETPROGRAMIVPROC glGetProgramiv;
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
static PFNGLDELETESHADERPROC glDeleteShader;
static PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
static PFNGLGENBUFFERSPROC glGenBuffers;
static PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
static PFNGLBINDBUFFERPROC glBindBuffer;
static PFNGLBUFFERDATAPROC glBufferData;
static PFNGLBUFFERSUBDATAPROC glBufferSubData;
static PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
static PFNGLDELETEPROGRAMPROC glDeleteProgram;
static PFNGLUSEPROGRAMPROC glUseProgram;
static PFNGLDRAWARRAYSPROC glDrawArrays;
static PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
static PFNGLDELETEBUFFERSPROC glDeleteBuffers;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
static PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
static PFNGLUNIFORM1FPROC glUniform1f;
static PFNGLUNIFORM2FPROC glUniform2f;
static PFNGLUNIFORM3FPROC glUniform3f;
static PFNGLGENQUERIESPROC glGenQueries;
static PFNGLBEGINQUERYPROC glBeginQuery;
static PFNGLENDQUERYPROC glEndQuery;
static PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
static PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
static PFNGLDELETEQUERIESPROC glDeleteQueries;
static PFNGLFENCESYNCPROC glFenceSync;
static PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
static PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
static PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
static PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
static PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
static PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
static PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
static PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
static PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
static PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
static PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
static PFNGLUNIFORM1IPROC glUniform1i;
static PFNGLACTIVETEXTUREPROC glActiveTexture;

/* #############################################################################
 * # [Section] Loader Implementation
 * #############################################################################
 */
#include "win32_api.h"

/* Global OpenGL library handle */
static void *win32_beneath_opengl_lib;

/* Function pointer types */
typedef void *(__stdcall *wglGetProcAddressProc)(char *);
static wglGetProcAddressProc wglGetProcAddress;

/* Load a single OpenGL function */
static void *win32_beneath_opengl_load_function(char *gl_function_name)
{
    void *function;

    /* Load opengl32.dll and wglGetProcAddress if not done yet */
    if (!win32_beneath_opengl_lib)
    {
        win32_beneath_opengl_lib = LoadLibraryA("opengl32.dll");

        if (!win32_beneath_opengl_lib)
        {
            return (void *)0;
        }

        *(void **)(&wglGetProcAddress) = GetProcAddress(win32_beneath_opengl_lib, "wglGetProcAddress");

        if (!wglGetProcAddress)
        {
            FreeLibrary(win32_beneath_opengl_lib);
            win32_beneath_opengl_lib = (void *)0;
            return (void *)0;
        }
    }

    /* Try wglGetProcAddress first (modern functions) */
    function = (void *)wglGetProcAddress(gl_function_name);

    /* Invalid function checks */
    if (!function ||
        function == (void *)1 ||
        function == (void *)2 ||
        function == (void *)3 ||
        function == (void *)-1)
    {
        /* Fallback to GetProcAddress for OpenGL 1.1 functions */
        function = (void *)GetProcAddress(win32_beneath_opengl_lib, gl_function_name);
    }

    return function;
}

#define BENEATH_FUNC_FROM_PTR(type, p) ((union { void *obj; type fn; }){(p)}.fn)
#define BENEATH_OPENGL_MAX_REPORTED_FAILURES 8

static char *beneath_opengl_failed_loads[BENEATH_OPENGL_MAX_REPORTED_FAILURES + 1];
static unsigned int beneath_opengl_failed_loads_count = 0;

#define BENEATH_OPENGL_FUNCTION(type, name)                                                \
    name = BENEATH_FUNC_FROM_PTR(type, win32_beneath_opengl_load_function(#name));         \
    if (!name && beneath_opengl_failed_loads_count < BENEATH_OPENGL_MAX_REPORTED_FAILURES) \
    {                                                                                      \
        beneath_opengl_failed_loads[beneath_opengl_failed_loads_count++] = #name;          \
    }

static int win32_beneath_opengl_load_wgl_functions(void)
{
    BENEATH_OPENGL_FUNCTION(PFNWGLCREATECONTEXTPROC, wglCreateContext);
    BENEATH_OPENGL_FUNCTION(PFNWGLGETCURRENTCONTEXTPROC, wglGetCurrentContext);
    BENEATH_OPENGL_FUNCTION(PFNWGLGETCURRENTDCPROC, wglGetCurrentDC);
    BENEATH_OPENGL_FUNCTION(PFNWGLDELETECONTEXTPROC, wglDeleteContext);
    BENEATH_OPENGL_FUNCTION(PFNWGLMAKECURRENTPROC, wglMakeCurrent);

    return beneath_opengl_failed_loads_count < 1;
}

static int win32_beneath_opengl_load_functions(void)
{
    BENEATH_OPENGL_FUNCTION(PFNWGLCHOOSEPIXELFORMATARBPROC, wglChoosePixelFormatARB);
    BENEATH_OPENGL_FUNCTION(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);
    BENEATH_OPENGL_FUNCTION(PFNWGLSWAPINTERVALEXTPROC, wglSwapIntervalEXT);

    /* OpenGL until 1.1 */
    BENEATH_OPENGL_FUNCTION(PFNGLGETSTRINGPROC, glGetString);
    BENEATH_OPENGL_FUNCTION(PFNGLCLEARCOLORPROC, glClearColor);
    BENEATH_OPENGL_FUNCTION(PFNGLCLEARPROC, glClear);
    BENEATH_OPENGL_FUNCTION(PFNGLGETERRORPROC, glGetError);
    BENEATH_OPENGL_FUNCTION(PFNGLENABLEPROC, glEnable);
    BENEATH_OPENGL_FUNCTION(PFNGLDISABLEPROC, glDisable);
    BENEATH_OPENGL_FUNCTION(PFNGLBLENDFUNCPROC, glBlendFunc);
    BENEATH_OPENGL_FUNCTION(PFNGLPOLYGONMODEPROC, glPolygonMode);
    BENEATH_OPENGL_FUNCTION(PFNGLCULLFACEPROC, glCullFace);
    BENEATH_OPENGL_FUNCTION(PFNGLFRONTFACEPROC, glFrontFace);
    BENEATH_OPENGL_FUNCTION(PFNGLVIEWPORTPROC, glViewport);
    BENEATH_OPENGL_FUNCTION(PFNGLDRAWELEMENTSPROC, glDrawElements);
    BENEATH_OPENGL_FUNCTION(PFNGLCOLORMASKPROC, glColorMask);
    BENEATH_OPENGL_FUNCTION(PFNGLDEPTHMASKPROC, glDepthMask);
    BENEATH_OPENGL_FUNCTION(PFNGLREADBUFFERPROC, glReadBuffer);
    BENEATH_OPENGL_FUNCTION(PFNGLDRAWBUFFERPROC, glDrawBuffer);
    BENEATH_OPENGL_FUNCTION(PFNGLREADPIXELSPROC, glReadPixels);
    BENEATH_OPENGL_FUNCTION(PFNGLGENTEXTURESPROC, glGenTextures);
    BENEATH_OPENGL_FUNCTION(PFNGLBINDTEXTUREPROC, glBindTexture);
    BENEATH_OPENGL_FUNCTION(PFNGLTEXIMAGE2DPROC, glTexImage2D);
    BENEATH_OPENGL_FUNCTION(PFNGLTEXPARAMETERIPROC, glTexParameteri);
    BENEATH_OPENGL_FUNCTION(PFNGLTEXPARAMETERFVPROC, glTexParameterfv);

    /* OpenGL 1.1 forward */
    BENEATH_OPENGL_FUNCTION(PFNGLCREATESHADERPROC, glCreateShader);
    BENEATH_OPENGL_FUNCTION(PFNGLCREATEPROGRAMPROC, glCreateProgram);
    BENEATH_OPENGL_FUNCTION(PFNGLATTACHSHADERPROC, glAttachShader);
    BENEATH_OPENGL_FUNCTION(PFNGLSHADERSOURCEPROC, glShaderSource);
    BENEATH_OPENGL_FUNCTION(PFNGLCOMPILESHADERPROC, glCompileShader);
    BENEATH_OPENGL_FUNCTION(PFNGLGETSHADERIVPROC, glGetShaderiv);
    BENEATH_OPENGL_FUNCTION(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
    BENEATH_OPENGL_FUNCTION(PFNGLLINKPROGRAMPROC, glLinkProgram);
    BENEATH_OPENGL_FUNCTION(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
    BENEATH_OPENGL_FUNCTION(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
    BENEATH_OPENGL_FUNCTION(PFNGLDELETESHADERPROC, glDeleteShader);
    BENEATH_OPENGL_FUNCTION(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
    BENEATH_OPENGL_FUNCTION(PFNGLGENBUFFERSPROC, glGenBuffers);
    BENEATH_OPENGL_FUNCTION(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
    BENEATH_OPENGL_FUNCTION(PFNGLBINDBUFFERPROC, glBindBuffer);
    BENEATH_OPENGL_FUNCTION(PFNGLBUFFERDATAPROC, glBufferData);
    BENEATH_OPENGL_FUNCTION(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
    BENEATH_OPENGL_FUNCTION(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
    BENEATH_OPENGL_FUNCTION(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
    BENEATH_OPENGL_FUNCTION(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
    BENEATH_OPENGL_FUNCTION(PFNGLUSEPROGRAMPROC, glUseProgram);
    BENEATH_OPENGL_FUNCTION(PFNGLDRAWARRAYSPROC, glDrawArrays);
    BENEATH_OPENGL_FUNCTION(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
    BENEATH_OPENGL_FUNCTION(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
    BENEATH_OPENGL_FUNCTION(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
    BENEATH_OPENGL_FUNCTION(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
    BENEATH_OPENGL_FUNCTION(PFNGLUNIFORM1FPROC, glUniform1f);
    BENEATH_OPENGL_FUNCTION(PFNGLUNIFORM2FPROC, glUniform2f);
    BENEATH_OPENGL_FUNCTION(PFNGLUNIFORM3FPROC, glUniform3f);
    BENEATH_OPENGL_FUNCTION(PFNGLGENQUERIESPROC, glGenQueries);
    BENEATH_OPENGL_FUNCTION(PFNGLBEGINQUERYPROC, glBeginQuery);
    BENEATH_OPENGL_FUNCTION(PFNGLENDQUERYPROC, glEndQuery);
    BENEATH_OPENGL_FUNCTION(PFNGLGETQUERYOBJECTIVPROC, glGetQueryObjectiv);
    BENEATH_OPENGL_FUNCTION(PFNGLGETQUERYOBJECTUIVPROC, glGetQueryObjectuiv);
    BENEATH_OPENGL_FUNCTION(PFNGLDELETEQUERIESPROC, glDeleteQueries);
    BENEATH_OPENGL_FUNCTION(PFNGLFENCESYNCPROC, glFenceSync);
    BENEATH_OPENGL_FUNCTION(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
    BENEATH_OPENGL_FUNCTION(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
    BENEATH_OPENGL_FUNCTION(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
    BENEATH_OPENGL_FUNCTION(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers);
    BENEATH_OPENGL_FUNCTION(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer);
    BENEATH_OPENGL_FUNCTION(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage);
    BENEATH_OPENGL_FUNCTION(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer);
    BENEATH_OPENGL_FUNCTION(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
    BENEATH_OPENGL_FUNCTION(PFNGLDRAWELEMENTSINSTANCEDPROC, glDrawElementsInstanced);
    BENEATH_OPENGL_FUNCTION(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor);
    BENEATH_OPENGL_FUNCTION(PFNGLVERTEXATTRIBIPOINTERPROC, glVertexAttribIPointer);
    BENEATH_OPENGL_FUNCTION(PFNGLUNIFORM1IPROC, glUniform1i);
    BENEATH_OPENGL_FUNCTION(PFNGLACTIVETEXTUREPROC, glActiveTexture);

    return beneath_opengl_failed_loads_count < 1;
}

#endif /* WIN32_BENEATH_OPENGL_LOADER_H */