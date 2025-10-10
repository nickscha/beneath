/* win32_opengl.h - v0.1 - public domain data structures - nickscha 2025

A C89 standard compliant, single header, nostdlib (no C Standard Library) win32 and opengl api function prototypes without using windows.h.

LICENSE

  Placed in the public domain and also MIT licensed.
  See end of file for detailed license information.

*/
#ifndef WIN32_OPENGL_H
#define WIN32_OPENGL_H

/* NVIDIA Force to use GPU instead of discrete GCPU*/
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;

/* AMD Force discrete GPU */
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#include "win32_api.h"

/* OPENGL 1.X forward version calls which are no longer part of windows shipped libs*/
typedef struct __GLsync *GLsync;
typedef int (*PFNWGLCHOOSEPIXELFORMATARBPROC)(void *hdc, int *piAttribIList, float *pfAttribFList, unsigned int nMaxFormats, int *piFormats, unsigned int *nNumFormats);
typedef void *(*PFNWGLCREATECONTEXTATTRIBSARBPROC)(void *hDC, void *hShareContext, int *attribList);
typedef int (*PFNWGLSWAPINTERVALEXTPROC)(int interval);
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

static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
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

int win32_opengl_init_gl_functions(void)
{
#ifdef _MSC_VER
#pragma warning(disable : 4068)
#endif
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"

  wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
  wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
  wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

  glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
  glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
  glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
  glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
  glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
  glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
  glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
  glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
  glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
  glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
  glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
  glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
  glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
  glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
  glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
  glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
  glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
  glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
  glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
  glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
  glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
  glDrawArrays = (PFNGLDRAWARRAYSPROC)wglGetProcAddress("glDrawArrays");
  glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
  glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
  glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
  glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
  glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
  glGenQueries = (PFNGLGENQUERIESPROC)wglGetProcAddress("glGenQueries");
  glBeginQuery = (PFNGLBEGINQUERYPROC)wglGetProcAddress("glBeginQuery");
  glEndQuery = (PFNGLENDQUERYPROC)wglGetProcAddress("glEndQuery");
  glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)wglGetProcAddress("glGetQueryObjectiv");
  glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)wglGetProcAddress("glGetQueryObjectuiv");
  glDeleteQueries = (PFNGLDELETEQUERIESPROC)wglGetProcAddress("glDeleteQueries");
  glFenceSync = (PFNGLFENCESYNCPROC)wglGetProcAddress("glFenceSync");
  glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
  glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
  glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
  glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
  glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
  glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
  glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
  glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
  glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
  glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)wglGetProcAddress("glDrawElementsInstanced");
  glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)wglGetProcAddress("glVertexAttribDivisor");
  glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)wglGetProcAddress("glVertexAttribIPointer");
  glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
  glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");

  WIN32_API_ASSERT(wglChoosePixelFormatARB);
  WIN32_API_ASSERT(wglCreateContextAttribsARB);
  WIN32_API_ASSERT(wglSwapIntervalEXT);
  WIN32_API_ASSERT(glCreateShader);
  WIN32_API_ASSERT(glCreateProgram);
  WIN32_API_ASSERT(glAttachShader);
  WIN32_API_ASSERT(glShaderSource);
  WIN32_API_ASSERT(glCompileShader);
  WIN32_API_ASSERT(glGetShaderiv);
  WIN32_API_ASSERT(glGetShaderInfoLog);
  WIN32_API_ASSERT(glLinkProgram);
  WIN32_API_ASSERT(glGetProgramiv);
  WIN32_API_ASSERT(glGetProgramInfoLog);
  WIN32_API_ASSERT(glDeleteShader);
  WIN32_API_ASSERT(glGenVertexArrays);
  WIN32_API_ASSERT(glGenBuffers);
  WIN32_API_ASSERT(glBindVertexArray);
  WIN32_API_ASSERT(glBindBuffer);
  WIN32_API_ASSERT(glBufferData);
  WIN32_API_ASSERT(glBufferSubData);
  WIN32_API_ASSERT(glVertexAttribPointer);
  WIN32_API_ASSERT(glEnableVertexAttribArray);
  WIN32_API_ASSERT(glDeleteProgram);
  WIN32_API_ASSERT(glUseProgram);
  WIN32_API_ASSERT(glDrawArrays);
  WIN32_API_ASSERT(glDeleteVertexArrays);
  WIN32_API_ASSERT(glDeleteBuffers);
  WIN32_API_ASSERT(glUniformMatrix4fv);
  WIN32_API_ASSERT(glGetUniformLocation);
  WIN32_API_ASSERT(glUniform3f);
  WIN32_API_ASSERT(glGenQueries);
  WIN32_API_ASSERT(glBeginQuery);
  WIN32_API_ASSERT(glEndQuery);
  WIN32_API_ASSERT(glGetQueryObjectiv);
  WIN32_API_ASSERT(glGetQueryObjectuiv);
  WIN32_API_ASSERT(glDeleteQueries);
  WIN32_API_ASSERT(glFenceSync);
  WIN32_API_ASSERT(glGenFramebuffers);
  WIN32_API_ASSERT(glBindFramebuffer);
  WIN32_API_ASSERT(glFramebufferTexture2D);
  WIN32_API_ASSERT(glGenRenderbuffers);
  WIN32_API_ASSERT(glBindRenderbuffer);
  WIN32_API_ASSERT(glRenderbufferStorage);
  WIN32_API_ASSERT(glFramebufferRenderbuffer);
  WIN32_API_ASSERT(glCheckFramebufferStatus);
  WIN32_API_ASSERT(glBindFramebuffer);
  WIN32_API_ASSERT(glDrawElementsInstanced);
  WIN32_API_ASSERT(glVertexAttribDivisor);
  WIN32_API_ASSERT(glVertexAttribIPointer);
  WIN32_API_ASSERT(glUniform1i);
  WIN32_API_ASSERT(glActiveTexture);

#pragma GCC diagnostic pop

  return 1;
}

#endif /* WIN32_OPENGL_H */

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
