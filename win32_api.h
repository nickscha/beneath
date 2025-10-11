#ifndef WIN32_API_H
#define WIN32_API_H

#define WIN32_API_STATIC_ASSERT(c, m) typedef char types_assert_##m[(c) ? 1 : -1]

#define WIN32_API(r) __declspec(dllimport) r __stdcall

#define WIN32_API_CALLBACK __stdcall

#define WIN32_API_ASSERT(expression) \
    if (!(expression))               \
    {                                \
        *(volatile int *)0 = 0;      \
    }

typedef unsigned long long UINT_PTR;
typedef long long LONG_PTR;

typedef LONG_PTR(WIN32_API_CALLBACK *WNDPROC)(void *, unsigned int, UINT_PTR, LONG_PTR);
typedef void *(*PROC)(void);

typedef struct tagPOINT
{
    long x;
    long y;

} POINT;

typedef struct tagMSG
{
    void *hwnd;
    unsigned int message;
    UINT_PTR wParam;
    LONG_PTR lParam;
    unsigned long time;
    POINT pt;
    unsigned long lPrivate;
} MSG, *LPMSG;

typedef struct WNDCLASSA
{
    unsigned int style;
    WNDPROC lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    void *hInstance;
    void *hIcon;
    void *hCursor;
    void *hbrBackground;
    char *lpszMenuName;
    char *lpszClassName;
} WNDCLASSA;

typedef struct FILETIME
{
    unsigned long dwLowDateTime;
    unsigned long dwHighDateTime;

} FILETIME;

typedef struct WIN32_FILE_ATTRIBUTE_DATA
{
    unsigned long dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    unsigned long nFileSizeHigh;
    unsigned long nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA;

typedef enum GET_FILEEX_INFO_LEVELS
{
    GetFileExInfoStandard,
    GetFileExMaxInfoLevel
} GET_FILEEX_INFO_LEVELS;

typedef struct tagRECT
{
    long left;
    long top;
    long right;
    long bottom;
} RECT, *PRECT, *LPRECT;

typedef struct LARGE_INTEGER
{
    unsigned long LowPart;
    long HighPart;
} LARGE_INTEGER;

static double win32_ll_to_double(unsigned long low_part, long high_part)
{
    /* The value of 2 to the power of 32, as a double constant. */
    double two_power_32 = 4294967296.0;

    return ((double)high_part * two_power_32) + (double)low_part;
}

typedef struct tagPIXELFORMATDESCRIPTOR
{
    unsigned short nSize;
    unsigned short nVersion;
    unsigned long dwFlags;
    unsigned char iPixelType;
    unsigned char cColorBits;
    unsigned char cRedBits;
    unsigned char cRedShift;
    unsigned char cGreenBits;
    unsigned char cGreenShift;
    unsigned char cBlueBits;
    unsigned char cBlueShift;
    unsigned char cAlphaBits;
    unsigned char cAlphaShift;
    unsigned char cAccumBits;
    unsigned char cAccumRedBits;
    unsigned char cAccumGreenBits;
    unsigned char cAccumBlueBits;
    unsigned char cAccumAlphaBits;
    unsigned char cDepthBits;
    unsigned char cStencilBits;
    unsigned char cAuxBuffers;
    unsigned char iLayerType;
    unsigned char bReserved;
    unsigned long dwLayerMask;
    unsigned long dwVisibleMask;
    unsigned long dwDamageMask;
} PIXELFORMATDESCRIPTOR, *LPPIXELFORMATDESCRIPTOR;

typedef struct RAWINPUTDEVICE
{
    unsigned short usUsagePage;
    unsigned short usUsage;
    unsigned long dwFlags;
    void *hwndTarget;
} RAWINPUTDEVICE;

typedef struct RAWINPUTHEADER
{
    unsigned long dwType;
    unsigned long dwSize;
    void *hDevice;
    UINT_PTR wParam;
} RAWINPUTHEADER;

typedef struct RAWKEYBOARD
{
    unsigned short MakeCode;
    unsigned short Flags;
    unsigned short Reserved;
    unsigned short VKey;
    unsigned int Message;
    unsigned long ExtraInformation;

} RAWKEYBOARD;

typedef struct RAWMOUSE
{
    unsigned short usFlags;
    unsigned long ulButtons;
    unsigned long ulRawButtons;
    long lLastX;
    long lLastY;
    unsigned long ulExtraInformation;

} RAWMOUSE;

typedef struct RAWHID
{
    unsigned long dwSizeHid;
    unsigned long dwCount;
    unsigned char bRawData[1];
} RAWHID;

typedef struct RAWINPUT
{
    RAWINPUTHEADER header;
    union
    {
        RAWMOUSE mouse;
        RAWKEYBOARD keyboard;
        RAWHID hid;
    } data;
} RAWINPUT;

typedef struct _PROCESS_MEMORY_COUNTERS_EX
{
    unsigned long cb;
    unsigned long PageFaultCount;
    UINT_PTR PeakWorkingSetSize;
    UINT_PTR WorkingSetSize;
    UINT_PTR QuotaPeakPagedPoolUsage;
    UINT_PTR QuotaPagedPoolUsage;
    UINT_PTR QuotaPeakNonPagedPoolUsage;
    UINT_PTR QuotaNonPagedPoolUsage;
    UINT_PTR PagefileUsage;
    UINT_PTR PeakPagefileUsage;
    UINT_PTR PrivateUsage;
} PROCESS_MEMORY_COUNTERS_EX, *PPROCESS_MEMORY_COUNTERS_EX;

typedef struct tagWINDOWPLACEMENT
{
    unsigned int length;
    unsigned int flags;
    unsigned int showCmd;
    POINT ptMinPosition;
    POINT ptMaxPosition;
    RECT rcNormalPosition;
    RECT rcDevice;
} WINDOWPLACEMENT;

typedef struct tagMONITORINFO
{
    unsigned long cbSize;
    RECT rcMonitor;
    RECT rcWork;
    unsigned long dwFlags;
} MONITORINFO, *LPMONITORINFO;

typedef RAWINPUTDEVICE *PCRAWINPUTDEVICE;

typedef struct tagCREATESTRUCTA
{
    void *lpCreateParams;
    void *hInstance;
    void *hMenu;
    void *hwndParent;
    int cy;
    int cx;
    int y;
    int x;
    long style;
    const char *lpszName;
    const char *lpszClass;
    unsigned long dwExStyle;
} CREATESTRUCTA, *LPCREATESTRUCTA;

#define ES_SYSTEM_REQUIRED ((unsigned long)0x00000001)
#define ES_DISPLAY_REQUIRED ((unsigned long)0x00000002)
#define ES_CONTINUOUS ((unsigned long)0x80000000)

#define RIM_TYPEMOUSE 0
#define RIM_TYPEKEYBOARD 1
#define RIM_TYPEHID 2
#define RIM_TYPEMAX 2
#define RI_KEY_BREAK 1
#define RIDEV_INPUTSINK 0x00000100

#define WM_ERASEBKGND 0x0014
#define THREAD_BASE_PRIORITY_MAX 2
#define THREAD_PRIORITY_HIGHEST THREAD_BASE_PRIORITY_MAX
#define HIGH_PRIORITY_CLASS 0x80
#define GWL_EXSTYLE (-20)
#define WS_POPUP 0x80000000L
#define GWLP_USERDATA -21
#define WS_EX_TOOLWINDOW 0x00000080L
#define INVALID_HANDLE ((void *)-1)
#define GENERIC_WRITE (0x40000000L)
#define CREATE_ALWAYS 2
#define MEM_COMMIT 0x00001000
#define MEM_RESERVE 0x00002000
#define PAGE_READWRITE 0x04
#define INVALID_HANDLE_VALUE ((void *)(LONG_PTR) - 1)
#define GENERIC_READ (0x80000000L)
#define FILE_SHARE_READ 0x00000001
#define OPEN_EXISTING 3
#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_FLAG_OVERLAPPED 0x40000000
#define INVALID_FILE_SIZE ((unsigned long)0xFFFFFFFF)
#define STD_OUTPUT_HANDLE ((unsigned long)-11)
#define PM_REMOVE 0x0001
#define WM_CREATE 0x0001
#define WM_INPUT 0x00FF
#define WM_CLOSE 0x0010
#define WM_DESTROY 0x0002
#define WM_QUIT 0x0012
#define WM_SIZE 0x0005
#define WM_SYSKEYDOWN 0x0104
#define WM_SYSKEYUP 0x0105
#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202
#define WM_RBUTTONDOWN 0x0204
#define WM_RBUTTONUP 0x0205
#define WM_MBUTTONDOWN 0x0207
#define WM_MBUTTONUP 0x0208
#define WM_MOUSEMOVE 0x0200
#define WM_MOUSEWHEEL 0x020A
#define VK_ESCAPE 0x1B
#define VK_RETURN 0x0D
#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_SHIFT 0x10
#define VK_MENU 0x12
#define VK_CAPITAL 0x14
#define VK_SPACE 0x20
#define VK_CONTROL 0x11
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_F1 0x70
#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F8 0x77
#define VK_F9 0x78
#define VK_F10 0x79
#define VK_F11 0x7A
#define VK_F12 0x7B
#define CS_VREDRAW 0x0001
#define CS_HREDRAW 0x0002
#define WS_OVERLAPPED 0x00000000L
#define WS_EX_TOPMOST 0x00000008L
#define WS_CAPTION 0x00C00000L
#define WS_SYSMENU 0x00080000L
#define WS_THICKFRAME 0x00040000L
#define WS_MINIMIZEBOX 0x00020000L
#define WS_MAXIMIZEBOX 0x00010000L
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_VISIBLE 0x10000000L
#define CW_USEDEFAULT ((int)0x80000000)
#define WS_CLIPSIBLINGS 0x04000000
#define WS_CLIPCHILDREN 0x02000000
#define MAKEINTRESOURCEA(i) ((char *)((unsigned long)((unsigned short)(i))))
#define IDC_ARROW MAKEINTRESOURCEA(32512)
#define HWND_TOP ((void *)0)
#define HWND_TOPMOST ((void *)-1)
#define SWP_SHOWWINDOW 0x0040
#define SWP_NOSIZE 0x0001
#define SWP_NOMOVE 0x0002
#define SWP_NOOWNERZORDER 0x0200
#define SWP_FRAMECHANGED 0x0020
#define SWP_NOZORDER 0x0004
#define GWL_STYLE -16
#define MONITOR_DEFAULTTONULL 0x00000000
#define MONITOR_DEFAULTTOPRIMARY 0x00000001
#define MONITOR_DEFAULTTONEAREST 0x00000002
#define SPI_GETWORKAREA 0x0030
#define LOWORD(l) ((unsigned short)(((UINT_PTR)(l)) & 0xffff))
#define HIWORD(l) ((unsigned short)((((UINT_PTR)(l)) >> 16) & 0xffff))
#define RID_INPUT 0x10000003
#define RIM_TYPEMOUSE 0
#define CS_OWNDC 0x0020
#define GL_TRUE 1
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
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define SW_SHOW 5
#define SW_SHOWDEFAULT 10
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_SAMPLES_PASSED 0x8914
#define GL_ANY_SAMPLES_PASSED 0x8C2F
#define GL_QUERY_RESULT 0x8866
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define PFD_DOUBLEBUFFER 0x00000001
#define PFD_SUPPORT_OPENGL 0x00000020
#define PFD_DRAW_TO_WINDOW 0x00000004
#define PFD_TYPE_RGBA 0
#define NULL ((void *)0)
#define GL_COMPILE_STATUS 0x8B81
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_LINK_STATUS 0x8B82
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_INT 0x1404
#define GL_FLOAT 0x1406
#define GL_FALSE 0
#define GL_TRIANGLES 0x0004
#define GL_DEPTH_TEST 0x0B71
#define GL_MULTISAMPLE 0x809D
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_FRONT_AND_BACK 0x0408
#define GL_LINE 0x1B01
#define GL_FILL 0x1B02
#define GL_CULL_FACE 0x0B44
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

/* WIN32 Function prototyes */
#ifndef _WINDOWS_

WIN32_API(int)
SetProcessDPIAware(void);

WIN32_API(unsigned long)
SetThreadExecutionState(unsigned long esFlags);

WIN32_API(void *)
GetModuleHandleA(const char *lpModuleName);

WIN32_API(LONG_PTR)
GetWindowLongPtrA(void *hWnd, int nIndex);
WIN32_API(LONG_PTR)
SetWindowLongPtrA(void *hWnd, int nIndex, LONG_PTR dwNewLong);

WIN32_API(unsigned int)
timeBeginPeriod(unsigned int uPeriod);
WIN32_API(unsigned int)
timeEndPeriod(unsigned int uPeriod);
WIN32_API(void *)
GetForegroundWindow(void);
WIN32_API(void *)
GetCurrentThread(void);
WIN32_API(int)
SetThreadPriority(void *hThread, int nPriority);
WIN32_API(int)
WriteFile(void *hFile, void *lpBuffer, unsigned long nNumberOfBytesToWrite, unsigned long *lpNumberOfBytesWritten, void *lpOverlapped);
WIN32_API(int)
PeekMessageA(LPMSG lpMsg, void *hWnd, unsigned int wMsgFilterMin, unsigned int wMsgFilterMax, unsigned int wRemoveMsg);
WIN32_API(int)
TranslateMessage(MSG *lpMsg);
WIN32_API(LONG_PTR)
DispatchMessageA(MSG *lpMsg);
WIN32_API(int)
ShowWindow(void *hWnd, int nCmdShow);
WIN32_API(int)
DestroyWindow(void *hWnd);
WIN32_API(LONG_PTR)
DefWindowProcA(void *hWnd, unsigned int Msg, UINT_PTR wParam, LONG_PTR lParam);
WIN32_API(void *)
LoadCursorA(void *hInstance, char *lpCursorName);
WIN32_API(int)
ShowCursor(int bShow);
WIN32_API(unsigned short)
RegisterClassA(WNDCLASSA *lpWndClass);
WIN32_API(void *)
CreateWindowExA(unsigned long dwExStyle, char *lpClassName, char *lpWindowName, unsigned long dwStyle, int X, int Y, int nWidth, int nHeight, void *hWndParent, void *hMenu, void *hInstance, void *lpParam);
WIN32_API(int)
SetWindowTextA(void *hWnd, char *lpString);
WIN32_API(int)
SetWindowPos(void *hWnd, void *hWndInsertAfter, int X, int Y, int cx, int cy, unsigned int uFlags);
WIN32_API(int)
AdjustWindowRect(LPRECT lpRect, unsigned long dwStyle, int bMenu);
WIN32_API(void)
Sleep(unsigned long dwMilliseconds);
WIN32_API(void)
ExitProcess(unsigned int uExitCode);
WIN32_API(void)
PostQuitMessage(int nExitCode);
WIN32_API(int)
wsprintfA(char *unnamedParam1, char *unnamedParam2, ...);
WIN32_API(int)
wvsprintfA(char *unnamedParam1, char *unnamedParam2, char *arglist);
WIN32_API(long)
CompareFileTime(FILETIME *lpFileTime1, FILETIME *lpFileTime2);
WIN32_API(int)
GetFileAttributesExA(char *lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, void *lpFileInformation);
WIN32_API(int)
CopyFileA(char *lpExistingFileName, char *lpNewFileName, int bFailIfExists);
WIN32_API(void *)
LoadLibraryA(char *lpLibFileName);
WIN32_API(int)
FreeLibrary(void *hLibModule);
WIN32_API(void *)
GetProcAddress(void *hModule, char *lpProcName);
WIN32_API(int)
CloseHandle(void *hObject);
WIN32_API(int)
GetClientRect(void *hWnd, LPRECT lpRect);
WIN32_API(int)
MapWindowPoints(void *hWndFrom, void *hWndTo, POINT *lpPoints, unsigned int cPoints);
WIN32_API(int)
ClipCursor(RECT *lpRect);
WIN32_API(int)
GetCursorPos(POINT *lpPoint);
WIN32_API(int)
SetCursorPos(int X, int Y);
WIN32_API(int)
ScreenToClient(void *hWnd, POINT *lpPoint);
WIN32_API(int)
ClientToScreen(void *hWnd, POINT *lpPoint);
WIN32_API(int)
QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount);
WIN32_API(int)
QueryPerformanceFrequency(LARGE_INTEGER *lpFrequency);
WIN32_API(int)
SystemParametersInfoA(unsigned int uiAction, unsigned int uiParam, void *pvParam, unsigned int fWinIni);
WIN32_API(int)
SwapBuffers(void *unnamedParam1);
WIN32_API(void *)
GetDC(void *hWnd);
WIN32_API(int)
ReleaseDC(void *hWnd, void *hDC);
WIN32_API(int)
TextOutA(void *hdc, int x, int y, char *lpString, int c);
WIN32_API(int)
ChoosePixelFormat(void *hdc, PIXELFORMATDESCRIPTOR *ppfd);
WIN32_API(int)
SetPixelFormat(void *hdc, int format, PIXELFORMATDESCRIPTOR *ppfd);
WIN32_API(int)
DescribePixelFormat(void *hdc, int iPixelFormat, unsigned int nBytes, LPPIXELFORMATDESCRIPTOR ppfd);
WIN32_API(void *)
CreateFileA(char *lpFileName, unsigned long dwDesiredAccess, unsigned long dwShareMode, void *, unsigned long dwCreationDisposition, unsigned long dwFlagsAndAttributes, void *hTemplateFile);
WIN32_API(int)
ReadFile(void *hFile, void *lpBuffer, unsigned long nNumberOfBytesToRead, unsigned long *lpNumberOfBytesRead, void *lpOverlapped);
WIN32_API(unsigned long)
GetFileSize(void *hFile, unsigned long *lpFileSizeHigh);
WIN32_API(void *)
VirtualAlloc(void *lpAddress, UINT_PTR dwSize, unsigned longflAllocationType, unsigned longflProtect);
WIN32_API(void *)
GetStdHandle(unsigned long nStdHandle);
WIN32_API(int)
WriteConsoleA(void *hConsoleOutput, void *lpBuffer, unsigned longnNumberOfCharsToWrite, unsigned long *lpNumberOfCharsWritten, void *lpReserved);
WIN32_API(void *)
wglCreateContext(void *unnamedParam1);
WIN32_API(void *)
wglGetCurrentContext(void);
WIN32_API(void *)
wglGetCurrentDC(void);
WIN32_API(int)
wglDeleteContext(void *unnamedParam1);
WIN32_API(int)
wglMakeCurrent(void *unnamedParam1, void *unnamedParam2);
WIN32_API(PROC)
wglGetProcAddress(char *unnamedParam1);
/* Windows provided OPEN GL 1.X functions */
WIN32_API(unsigned char) * glGetString(unsigned int name);
WIN32_API(void)
glClearColor(float red, float green, float blue, float alpha);
WIN32_API(void)
glClear(unsigned int mask);
WIN32_API(unsigned int)
glGetError(void);
WIN32_API(void)
glEnable(unsigned int cap);
WIN32_API(void)
glDisable(unsigned int cap);
WIN32_API(void)
glBlendFunc(unsigned int sfactor, unsigned int dfactor);
WIN32_API(void)
glPolygonMode(unsigned int face, unsigned int mode);
WIN32_API(void)
glCullFace(unsigned int mode);
WIN32_API(void)
glFrontFace(unsigned int mode);
WIN32_API(void)
glViewport(int x, int y, int width, int height);
WIN32_API(void)
glDrawElements(unsigned int mode, int count, unsigned int type, void *indices);
WIN32_API(void)
glColorMask(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
WIN32_API(void)
glDepthMask(unsigned char flag);
WIN32_API(void)
glReadBuffer(unsigned int mode);
WIN32_API(void)
glDrawBuffer(unsigned int mode);
WIN32_API(void)
glReadPixels(int x, int y, int width, int height, unsigned int format, unsigned int type, void *pixels);
WIN32_API(void)
glGenTextures(int n, unsigned int *textures);
WIN32_API(void)
glBindTexture(unsigned int target, unsigned int texture);
WIN32_API(void)
glTexImage2D(unsigned int target, int level, int internalformat, int width, int height, int border, int format, unsigned int type, void *pixels);
WIN32_API(void)
glTexParameteri(unsigned int target, unsigned int pname, int param);
WIN32_API(void)
glTexParameterfv(unsigned int target, unsigned int pname, float *params);
WIN32_API(int)
RegisterRawInputDevices(PCRAWINPUTDEVICE pRawInputDevices, unsigned int uiNumDevices, unsigned int cbSize);
WIN32_API(unsigned int)
GetRawInputData(void *hRawInput, unsigned int uiCommand, void *pData, unsigned int *pcbSize, unsigned int cbSizeHeader);
WIN32_API(void *)
GetCurrentProcess(void);
WIN32_API(int)
SetPriorityClass(void *hProcess, unsigned long dwPriorityClass);
WIN32_API(int)
GetProcessHandleCount(void *hProcess, unsigned long *pdwHandleCount);
WIN32_API(int)
K32GetProcessMemoryInfo(void *Process, PPROCESS_MEMORY_COUNTERS_EX ppsmemCounters, unsigned long cb);
WIN32_API(long)
GetWindowLongA(void *hWnd, int nIndex);
WIN32_API(int)
GetWindowPlacement(void *hWnd, WINDOWPLACEMENT *lpwndpl);
WIN32_API(int)
GetMonitorInfoA(void *hMonitor, LPMONITORINFO lpmi);
WIN32_API(void *)
MonitorFromWindow(void *hwnd, unsigned long dwFlags);
WIN32_API(long)
SetWindowLongA(void *hWnd, int nIndex, long dwNewLong);
WIN32_API(int)
SetWindowPos(void *hWnd, void *hWndInsertAfter, int X, int Y, int cx, int cy, unsigned int uFlags);
WIN32_API(int)
SetWindowPlacement(void *hWnd, WINDOWPLACEMENT *lpwndpl);
#endif

#endif /* WIN32_API_H */

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
