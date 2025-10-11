#define BENEATH_PLATFORM_LAYER
#define BENEATH_PLATFORM_LAYER_NAME "win32_beneath"
#include "beneath.h"

#include "win32_api.h"    /* windows.h replacement   */
#include "win32_opengl.h" /* opengl loading function */

FILETIME win32_beneath_file_modification_time(char *file)
{
    static FILETIME empty = {0, 0};

    WIN32_FILE_ATTRIBUTE_DATA fad;

    return GetFileAttributesExA(file, GetFileExInfoStandard, &fad) ? fad.ftLastWriteTime : empty;
}

BENEATH_API BENEATH_INLINE unsigned int win32_beneath_api_strlen(char *str)
{
    unsigned int length = 0;
    while (str[length] != '\0')
    {
        length++;
    }
    return length;
}

BENEATH_API BENEATH_INLINE void win32_beneath_api_io_print(
    char *filename, /* The current compilation unit filename (usualy __FILE__)*/
    int line,       /* The current compilation unit line number (usually __LINE__)*/
    char *string    /* The string to print to the console */
)
{
    unsigned long written;
    void *hConsole;
    char output[256];
    unsigned int len;

    /* Write filename:line prefix */
    wsprintfA(output, "%s:%d ", filename, line);
    len = win32_beneath_api_strlen(output);

    /* Copy string safely, truncating if needed */
    {
        unsigned int i;
        for (i = 0; i + len < sizeof(output) - 1 && string[i]; i++)
        {
            output[len + i] = string[i];
        }
        output[len + i] = '\0';
    }

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleA(hConsole, output, win32_beneath_api_strlen(output), &written, NULL);
}

BENEATH_API BENEATH_INLINE beneath_bool win32_beneath_api_io_file_size(
    char *filename,         /* The filename/path of which to return the file size */
    unsigned int *file_size /* The gathered file size in bytes */
)
{
    void *hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE)
    {
        return false;
    }

    *file_size = GetFileSize(hFile, 0);

    if (!CloseHandle(hFile) || *file_size == INVALID_FILE_SIZE)
    {
        return false;
    }

    return true;
}

BENEATH_API BENEATH_INLINE beneath_bool win32_beneath_api_io_file_read(
    char *filename,                    /* The filename/path to read into the file_buffer */
    unsigned char *file_buffer,        /* The user provided file_buffer large enough to hold the file contents */
    unsigned int file_buffer_capacity, /* The capacity/max site of the file_buffer */
    unsigned int *file_buffer_size     /* The total number of bytes read by this function */
)
{
    void *hFile;
    unsigned long fileSize;
    unsigned long bytesRead;

    hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE)
    {
        return false;
    }

    fileSize = GetFileSize(hFile, 0);

    if (fileSize == INVALID_FILE_SIZE || file_buffer_capacity < fileSize + 1 || !ReadFile(hFile, file_buffer, fileSize, &bytesRead, 0) || bytesRead != fileSize)
    {
        CloseHandle(hFile);
        return false;
    }

    file_buffer[fileSize] = '\0';
    *file_buffer_size = fileSize;

    CloseHandle(hFile);

    return true;
}

BENEATH_API BENEATH_INLINE beneath_bool win32_beneath_api_io_file_write(
    char *filename,          /* The filename/path to write the file_buffer to */
    unsigned char *buffer,   /* The file content to be written */
    unsigned int buffer_size /* The size of the file content buffer */
)
{
    void *hFile;
    unsigned long bytes_written;
    int success;

    hFile = CreateFileA(filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE)
    {
        return false;
    }

    success = WriteFile(hFile, buffer, buffer_size, &bytes_written, 0);
    success = CloseHandle(hFile);

    return (success && (bytes_written == buffer_size));
}

BENEATH_API BENEATH_INLINE beneath_bool win32_beneath_api_time_sleep(unsigned int milliseconds)
{
    Sleep(milliseconds);
    return true;
}

BENEATH_API BENEATH_INLINE unsigned int win32_beneath_api_perf_cycle_count(void)
{
    unsigned int low_part = 0;
    unsigned int high_part = 0;
    __asm __volatile("rdtsc" : "=a"(low_part), "=d"(high_part));
    return ((unsigned int)((double)high_part * 4294967296.0 + (double)low_part));
}

BENEATH_API BENEATH_INLINE double win32_beneath_api_perf_time_nanoseconds(void)
{
    static double frequencyValue;
    static int perf_count_frequency_initialized = 0;

    LARGE_INTEGER counter;

    if (!perf_count_frequency_initialized)
    {
        LARGE_INTEGER perf_count_frequency;
        QueryPerformanceFrequency(&perf_count_frequency);

        /* Convert the 64-bit frequency value into a double */
        frequencyValue = win32_ll_to_double(perf_count_frequency.LowPart, perf_count_frequency.HighPart);

        perf_count_frequency_initialized = 1;
    }

    QueryPerformanceCounter(&counter);

    /* Convert the 64-bit counter value into a double for precision */
    return (win32_ll_to_double(counter.LowPart, counter.HighPart) * 1000000000.0) / frequencyValue;
}

/* Dynamically load application dll */
#ifdef BENEATH_LIB
typedef struct beneath_application
{
    void *hDLL;
    FILETIME lastWriteTime;
    char *dllName;
} beneath_application;

static beneath_application app;

BENEATH_API BENEATH_INLINE beneath_bool win32_beneath_load_application(void)
{
    char *dllName = BENEATH_STRINGIZE(BENEATH_APPLICATION_LAYER_NAME.dll);
    char *dllTempName = BENEATH_STRINGIZE(BENEATH_APPLICATION_LAYER_NAME.temp.dll);

    if (app.hDLL != NULL)
    {
        if (!FreeLibrary(app.hDLL))
        {
            return false;
        }
        app.hDLL = NULL;
    }

    if (!CopyFileA(dllName, dllTempName, false))
    {
        return false;
    }

    app.hDLL = LoadLibraryA(dllTempName);
    app.dllName = dllName;
    app.lastWriteTime = win32_beneath_file_modification_time(dllName);

    if (!app.hDLL)
    {
        return false;
    }

    /* FIX for ERROR: ISO C forbids conversion of object pointer to function pointer type*/
    /* https://pubs.opengroup.org/onlinepubs/009695399/functions/dlsym.html */
    *(void **)(&beneath_update) = GetProcAddress(app.hDLL, "beneath_update");

    if (!beneath_update)
    {
        return false;
    }

    return true;
}
#endif

BENEATH_API BENEATH_INLINE void win32_beneath_enable_dpi_awareness(void)
{

    void *shcore = LoadLibraryA("Shcore.dll");

    if (shcore)
    {
        typedef long(__stdcall * SetProcessDpiAwarenessProc)(int);
        SetProcessDpiAwarenessProc setDpiAwareness;

        *(void **)(&setDpiAwareness) = GetProcAddress(shcore, "SetProcessDpiAwareness");

        if (setDpiAwareness)
        {
            setDpiAwareness(2); /* PROCESS_PER_MONITOR_DPI_AWARE */
        }

        FreeLibrary(shcore);
    }
    else
    {
        SetProcessDPIAware();
    }
}

BENEATH_API BENEATH_INLINE void win32_precise_sleep(void **timer, double seconds)
{
    LARGE_INTEGER li;
    long long val = -(long long)(seconds * 10000000.0); /* relative 100ns intervals */
    li.LowPart = (unsigned long)(val & 0xFFFFFFFF);
    li.HighPart = (long)((val >> 32) & 0xFFFFFFFF);

    SetWaitableTimer(*timer, &li, 0, NULL, NULL, false);
    WaitForSingleObject(*timer, INFINITE);
}

BENEATH_API BENEATH_INLINE beneath_key win32_beneath_input_map_virtual_key(unsigned short vKey)
{
    switch (vKey)
    {
    case VK_F1:
        return BENEATH_KEY_F1;
    case VK_F2:
        return BENEATH_KEY_F2;
    case VK_F3:
        return BENEATH_KEY_F3;
    case VK_F4:
        return BENEATH_KEY_F4;
    case VK_F5:
        return BENEATH_KEY_F5;
    case VK_F6:
        return BENEATH_KEY_F6;
    case VK_F7:
        return BENEATH_KEY_F7;
    case VK_F8:
        return BENEATH_KEY_F8;
    case VK_F9:
        return BENEATH_KEY_F9;
    case VK_F10:
        return BENEATH_KEY_F10;
    case VK_F11:
        return BENEATH_KEY_F11;
    case VK_F12:
        return BENEATH_KEY_F12;
    case VK_BACK:
        return BENEATH_KEY_BACKSPACE;
    case VK_TAB:
        return BENEATH_KEY_TAB;
    case VK_RETURN:
        return BENEATH_KEY_RETURN;
    case VK_SHIFT:
        return BENEATH_KEY_SHIFT;
    case VK_CONTROL:
        return BENEATH_KEY_CONTROL;
    case VK_MENU:
        return BENEATH_KEY_ALT;
    case VK_CAPITAL:
        return BENEATH_KEY_CAPSLOCK;
    case VK_SPACE:
        return BENEATH_KEY_SPACE;
    case VK_LEFT:
        return BENEATH_KEY_ARROW_LEFT;
    case VK_UP:
        return BENEATH_KEY_ARROW_UP;
    case VK_RIGHT:
        return BENEATH_KEY_ARROW_RIGHT;
    case VK_DOWN:
        return BENEATH_KEY_ARROW_DOWN;
    case '0':
        return BENEATH_KEY_0;
    case '1':
        return BENEATH_KEY_1;
    case '2':
        return BENEATH_KEY_2;
    case '3':
        return BENEATH_KEY_3;
    case '4':
        return BENEATH_KEY_4;
    case '5':
        return BENEATH_KEY_5;
    case '6':
        return BENEATH_KEY_6;
    case '7':
        return BENEATH_KEY_7;
    case '8':
        return BENEATH_KEY_8;
    case '9':
        return BENEATH_KEY_9;
    case 'A':
        return BENEATH_KEY_A;
    case 'B':
        return BENEATH_KEY_B;
    case 'C':
        return BENEATH_KEY_C;
    case 'D':
        return BENEATH_KEY_D;
    case 'E':
        return BENEATH_KEY_E;
    case 'F':
        return BENEATH_KEY_F;
    case 'G':
        return BENEATH_KEY_G;
    case 'H':
        return BENEATH_KEY_H;
    case 'I':
        return BENEATH_KEY_I;
    case 'J':
        return BENEATH_KEY_J;
    case 'K':
        return BENEATH_KEY_K;
    case 'L':
        return BENEATH_KEY_L;
    case 'M':
        return BENEATH_KEY_M;
    case 'N':
        return BENEATH_KEY_N;
    case 'O':
        return BENEATH_KEY_O;
    case 'P':
        return BENEATH_KEY_P;
    case 'Q':
        return BENEATH_KEY_Q;
    case 'R':
        return BENEATH_KEY_R;
    case 'S':
        return BENEATH_KEY_S;
    case 'T':
        return BENEATH_KEY_T;
    case 'U':
        return BENEATH_KEY_U;
    case 'V':
        return BENEATH_KEY_V;
    case 'W':
        return BENEATH_KEY_W;
    case 'X':
        return BENEATH_KEY_X;
    case 'Y':
        return BENEATH_KEY_Y;
    case 'Z':
        return BENEATH_KEY_Z;

    default:
        return BENEATH_KEY_COUNT;
    }
}

BENEATH_API BENEATH_INLINE beneath_bool win32_beneath_input_register_raw_input(void *window_handle)
{
    RAWINPUTDEVICE rid[2] = {0};

    rid[0].usUsagePage = 0x01;
    rid[0].usUsage = 0x06;            /* Keyboard */
    rid[0].dwFlags = RIDEV_INPUTSINK; /* Receive input even when not focused */
    rid[0].hwndTarget = window_handle;

    rid[1].usUsagePage = 0x01;
    rid[1].usUsage = 0x02; /* Mouse */
    rid[1].dwFlags = RIDEV_INPUTSINK;
    rid[1].hwndTarget = window_handle;

    if (!RegisterRawInputDevices(rid, 2, sizeof(rid[0])))
    {
        win32_beneath_api_io_print(__FILE__, __LINE__, "Failed to register RAWINPUT device\n");
        return false;
    }

    return true;
}

typedef struct win32_beneath_state
{
    beneath_state *state;
    beneath_controller_input *input;

} win32_beneath_state;

BENEATH_API BENEATH_INLINE LONG_PTR WIN32_API_CALLBACK win32_beneath_window_callback(void *window, unsigned int message, UINT_PTR wParam, LONG_PTR lParam)
{
    win32_beneath_state *win32_state = (win32_beneath_state *)GetWindowLongPtrA(window, GWLP_USERDATA);
    LONG_PTR result = 0;

    switch (message)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_CREATE:
    {
        CREATESTRUCTA *cs = (CREATESTRUCTA *)lParam;
        win32_state = (win32_beneath_state *)cs->lpCreateParams;
        SetWindowLongPtrA(window, GWLP_USERDATA, (LONG_PTR)win32_state);

        win32_beneath_input_register_raw_input(window);
    }
    break;
    case WM_INPUT:
    {
        unsigned int dwSize = 0;

        GetRawInputData((RAWINPUT *)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

#define WIN32_BENEATH_RAWINPUT_BUFFER_SIZE 128
        static unsigned char rawBuffer[WIN32_BENEATH_RAWINPUT_BUFFER_SIZE];
        RAWINPUT *raw = (RAWINPUT *)rawBuffer;

        if (dwSize <= sizeof(rawBuffer) &&
            GetRawInputData((RAWINPUT *)lParam, RID_INPUT, raw, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize)
        {
            beneath_controller_input *input = win32_state->input;

            if (raw->header.dwType == RIM_TYPEKEYBOARD)
            {
                unsigned short vKey = raw->data.keyboard.VKey;
                unsigned short flags = raw->data.keyboard.Flags;

                beneath_key bKey = win32_beneath_input_map_virtual_key(vKey);

                if (bKey != BENEATH_KEY_COUNT)
                {
                    beneath_controller_state *keyState = &input->keys[bKey];

                    beneath_bool isDown = !(flags & RI_KEY_BREAK);

                    if (keyState->ended_down != isDown)
                    {
                        keyState->half_transition_count++;
                        keyState->ended_down = isDown;
                        keyState->pressed = isDown ? 1 : 0;

                        if (isDown)
                        {
                            keyState->active = !keyState->active;
                        }
                    }
                }
            }
            else if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                RAWMOUSE *mouse = &raw->data.mouse;
                POINT p;

                input->mouse_attached = true;

                /* Movement (raw, unaccelerated) */
                input->mouse_offset_x += (float)mouse->lLastX;
                input->mouse_offset_y += (float)mouse->lLastY;

                /* Update absolute position for convenience (optional) */
                if (GetCursorPos(&p))
                {
                    ScreenToClient((void *)window, &p);
                    input->mouse_position_x = p.x;
                    input->mouse_position_y = p.y;
                }

                /* Scroll wheel */
                if (mouse->usButtonFlags & RI_MOUSE_WHEEL)
                {
                    short wheelDelta = (short)mouse->usButtonData;
                    input->mouse_offset_scroll += (float)wheelDelta / (float)WHEEL_DELTA;
                }

                /* --- Left button --- */
                if (mouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
                {
                    beneath_controller_state *b = &input->mouse_left;
                    b->half_transition_count++;
                    b->ended_down = 1;
                    b->pressed = 1;
                    b->active = !b->active;
                }
                else if (mouse->usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
                {
                    beneath_controller_state *b = &input->mouse_left;
                    b->half_transition_count++;
                    b->ended_down = 0;
                    b->pressed = 0;
                }

                /* --- Right button --- */
                if (mouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
                {
                    beneath_controller_state *b = &input->mouse_right;
                    b->half_transition_count++;
                    b->ended_down = 1;
                    b->pressed = 1;
                    b->active = !b->active;
                }
                else if (mouse->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
                {
                    beneath_controller_state *b = &input->mouse_right;
                    b->half_transition_count++;
                    b->ended_down = 0;
                    b->pressed = 0;
                }

                /* --- Middle button --- */
                if (mouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
                {
                    beneath_controller_state *b = &input->mouse_middle;
                    b->half_transition_count++;
                    b->ended_down = 1;
                    b->pressed = 1;
                    b->active = !b->active;
                }
                else if (mouse->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
                {
                    beneath_controller_state *b = &input->mouse_middle;
                    b->half_transition_count++;
                    b->ended_down = 0;
                    b->pressed = 0;
                }
            }
        }
    }
    break;
    case WM_CLOSE:
    {
        win32_state->state->running = false;
    }
    break;
    case WM_SIZE:
    {
        win32_state->state->window_width = (unsigned int)LOWORD((unsigned long)lParam);
        win32_state->state->window_height = (unsigned int)HIWORD((unsigned long)lParam);

        glViewport(0, 0, (int)win32_state->state->window_width, (int)win32_state->state->window_height);
    }
    break;
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
    }
    break;
    default:
    {
        result = DefWindowProcA(window, message, wParam, lParam);
    }
    break;
    }

    return (result);
}

BENEATH_API BENEATH_INLINE beneath_bool win32_beneath_initialize_opengl(win32_beneath_state *win32_state, void **window_handle, void **dc)
{
    void *instance = GetModuleHandleA(NULL);
    WNDCLASSA windowClass = {0};
    beneath_state *state = win32_state->state;

    void *fakeWND;
    void *fakeDC;
    int fakePFDID;
    void *fakeRC;

    unsigned long windowStyle;

    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = win32_beneath_window_callback;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorA(0, IDC_ARROW);
    windowClass.lpszClassName = BENEATH_STRINGIZE(BENEATH_APPLICATION_LAYER_NAME);

    if (!RegisterClassA(&windowClass))
    {
        return false;
    }

    /***********************/
    /*OPENGL INITIALIZATION*/
    /***********************/
    fakeWND = CreateWindowExA(
        WS_EX_TOOLWINDOW,
        windowClass.lpszClassName,
        windowClass.lpszClassName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0,
        1, 1,
        0, 0,
        instance, 0);

    if (!fakeWND)
    {
        return false;
    }

    fakeDC = GetDC(fakeWND);

    PIXELFORMATDESCRIPTOR fakePFD = {0};
    fakePFD.nSize = sizeof(fakePFD);
    fakePFD.nVersion = 1;
    fakePFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    fakePFD.iPixelType = PFD_TYPE_RGBA;
    fakePFD.cColorBits = 32;
    fakePFD.cAlphaBits = 8;
    fakePFD.cDepthBits = 24;

    fakePFDID = ChoosePixelFormat(fakeDC, &fakePFD);

    if (!fakePFDID || !SetPixelFormat(fakeDC, fakePFDID, &fakePFD))
    {
        return false;
    }

    fakeRC = wglCreateContext(fakeDC);

    if (!fakeRC || !wglMakeCurrent(fakeDC, fakeRC) || !win32_opengl_init_gl_functions())
    {
        return false;
    }

    /* Find out center location of the window*/
    windowStyle = WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;

    RECT rect = {0, 0, (long)state->window_width, (long)state->window_height};
    AdjustWindowRect(&rect, windowStyle, false);
    state->window_width = (unsigned int)(rect.right - rect.left);
    state->window_height = (unsigned int)(rect.bottom - rect.top);

    *window_handle = CreateWindowExA(
        0,
        windowClass.lpszClassName,
        windowClass.lpszClassName,
        windowStyle,
        0, 0,
        (int)state->window_width, (int)state->window_height,
        0, 0,
        instance,
        win32_state /* Pass pointer to user data to the window callback */
    );

    /* Modal window
    SetWindowPos(*window_handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    */

    *dc = GetDC(*window_handle);

    int pixelAttribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0};

    int pixelFormatID;
    unsigned int numFormats;
    int status = wglChoosePixelFormatARB(*dc, pixelAttribs, 0, 1, &pixelFormatID, &numFormats);

    if (!status || !numFormats)
    {
        return false;
    }

    SetPixelFormat(*dc, pixelFormatID, NULL);

    /* Open GL 3.3 specification */
    int contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0};

    void *rc = wglCreateContextAttribsARB(*dc, 0, contextAttribs);

    if (!rc)
    {
        return false;
    }

    wglMakeCurrent(0, 0);
    wglDeleteContext(fakeRC);
    ReleaseDC(fakeWND, fakeDC);
    DestroyWindow(fakeWND);

    if (!wglMakeCurrent(*dc, rc))
    {
        return false;
    }

    if (wglSwapIntervalEXT)
    {
        wglSwapIntervalEXT(state->frames_per_second_target < 0 ? 1 : 0);
    }

    /* Avoid clear color flickering */
    glClearColor(state->window_clear_color_r, state->window_clear_color_g, state->window_clear_color_b, state->window_clear_color_a);
    glClear(GL_COLOR_BUFFER_BIT);
    SwapBuffers(*dc);

    /* Make the window visible */
    ShowWindow(*window_handle, SW_SHOW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glViewport(0, 0, (int)state->window_width, (int)state->window_height);

    return true;
}

BENEATH_API BENEATH_INLINE void win32_beneath_update_state(beneath_state *state, void **window_handle)
{
    static WINDOWPLACEMENT g_wpPrev = {0};
    static int last_swap_interval = -2; /* invalid initial value to force first update */

    if (!window_handle || !*window_handle || !state)
    {
        return;
    }

    /****************************/
    /* Window change handling   */
    /****************************/
    if (state->changed_flags & BENEATH_STATE_CHANGED_FLAG_WINDOW)
    {
        void *hwnd = *window_handle;
        long dwStyle = GetWindowLongA(hwnd, GWL_STYLE);

        /****************************/
        /* Window mode handling     */
        /****************************/
        MONITORINFO mi;
        mi.cbSize = sizeof(mi);
        GetMonitorInfoA(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);

        RECT rect;

        switch (state->window_mode)
        {
        case BENEATH_WINDOW_MODE_FULLSCREEN:
            if (dwStyle & WS_OVERLAPPEDWINDOW)
            {
                if (GetWindowPlacement(hwnd, &g_wpPrev))
                {
                    SetWindowLongA(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                    SetWindowPos(hwnd, HWND_TOP,
                                 mi.rcMonitor.left, mi.rcMonitor.top,
                                 mi.rcMonitor.right - mi.rcMonitor.left,
                                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

                    state->window_width = (unsigned int)(mi.rcMonitor.right - mi.rcMonitor.left);
                    state->window_height = (unsigned int)(mi.rcMonitor.bottom - mi.rcMonitor.top);
                }
            }
            break;

        case BENEATH_WINDOW_MODE_BORDERLESS:
        {
            if (GetWindowPlacement(hwnd, &g_wpPrev))
            {
                SetWindowLongA(hwnd, GWL_STYLE, (long)WS_POPUP);
                SetWindowPos(hwnd, HWND_TOP,
                             mi.rcMonitor.left, mi.rcMonitor.top,
                             mi.rcMonitor.right - mi.rcMonitor.left,
                             mi.rcMonitor.bottom - mi.rcMonitor.top,
                             SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

                state->window_width = (unsigned int)(mi.rcMonitor.right - mi.rcMonitor.left);
                state->window_height = (unsigned int)(mi.rcMonitor.bottom - mi.rcMonitor.top);
            }
        }
        break;

        case BENEATH_WINDOW_MODE_WINDOWED:
        default:
            if (!(dwStyle & WS_OVERLAPPEDWINDOW))
            {
                SetWindowLongA(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                SetWindowPlacement(hwnd, &g_wpPrev);

                /* restore placement */
                SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

                GetClientRect(hwnd, &rect);
                state->window_width = (unsigned int)(rect.right - rect.left);
                state->window_height = (unsigned int)(rect.bottom - rect.top);
            }
            else
            {
                /* resize windowed mode */
                RECT rect = {0, 0, (long)state->window_width, (long)state->window_height};
                AdjustWindowRect(&rect, (unsigned long)GetWindowLongA(hwnd, GWL_STYLE), false);
                SetWindowPos(hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            }
            break;
        }

        /****************************/
        /* Viewport update          */
        /****************************/
        glViewport(0, 0, (int)state->window_width, (int)state->window_height);

        /****************************/
        /* Cursor clipping          */
        /****************************/
        if (state->window_clip_cursor && GetForegroundWindow() == hwnd)
        {
            RECT rect;
            GetClientRect(hwnd, &rect);
            MapWindowPoints(hwnd, NULL, (POINT *)&rect, 2);
            ClipCursor(&rect);
        }
        else
        {
            ClipCursor(NULL);
        }

        /****************************/
        /* Window title             */
        /****************************/
        SetWindowTextA(hwnd, state->window_title);

        /****************************/
        /* Clear color              */
        /****************************/
        glClearColor(state->window_clear_color_r,
                     state->window_clear_color_g,
                     state->window_clear_color_b,
                     state->window_clear_color_a);

        state->changed_flags &= ~(unsigned int)BENEATH_STATE_CHANGED_FLAG_WINDOW;
    }

    /****************************/
    /* VSync / FPS handling     */
    /****************************/
    if (state->changed_flags & BENEATH_STATE_CHANGED_FLAG_FRAMES_PER_SECOND_TARGET)
    {
        if (wglSwapIntervalEXT)
        {
            int swap_interval = (state->frames_per_second_target < 0) ? 1 : 0; /* VSYNC if negative */

            if (swap_interval != last_swap_interval)
            {
                wglSwapIntervalEXT(swap_interval);
                last_swap_interval = swap_interval;
            }
        }

        state->changed_flags &= ~(unsigned int)BENEATH_STATE_CHANGED_FLAG_FRAMES_PER_SECOND_TARGET;
    }
}

BENEATH_API BENEATH_INLINE void win32_beneath_process_input(beneath_state *state, beneath_controller_input *input)
{
    MSG message;

    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
    {
        switch (message.message)
        {
        case WM_QUIT:
            state->running = false;
            break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        {
            beneath_controller_state *b = &input->mouse_left;
            beneath_bool isDown = (message.message == WM_LBUTTONDOWN);

            if (b->ended_down != isDown)
            {
                b->half_transition_count++;
                b->ended_down = isDown;
                b->pressed = isDown ? 1 : 0;
                b->active = !b->active;
            }
        }
        break;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        {
            beneath_controller_state *b = &input->mouse_middle;
            beneath_bool isDown = (message.message == WM_MBUTTONDOWN);

            if (b->ended_down != isDown)
            {
                b->half_transition_count++;
                b->ended_down = isDown;
                b->pressed = isDown ? 1 : 0;
                b->active = !b->active;
            }
        }
        break;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        {
            beneath_controller_state *b = &input->mouse_right;
            beneath_bool isDown = (message.message == WM_RBUTTONDOWN);

            if (b->ended_down != isDown)
            {
                b->half_transition_count++;
                b->ended_down = isDown;
                b->pressed = isDown ? 1 : 0;
                b->active = !b->active;
            }
        }
        break;
        default:
            TranslateMessage(&message);
            DispatchMessageA(&message);
            break;
        }
    }
}

#ifdef __clang__
#elif __GNUC__
__attribute((externally_visible))
#endif
#ifdef __i686__
__attribute((force_align_arg_pointer))
#endif
int mainCRTStartup(void)
{
    double last_time;
    unsigned int memory_size = 1024 * 1024 * 1; /* 1 MB */

    beneath_memory memory = {0};
    beneath_api api = {0};
    beneath_state *state;
    beneath_controller_input input = {0};
    win32_beneath_state win32_state = {0};

    /* Make this process high priority and set timer resolution */
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);                        /* Set process to high priority */
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);                    /* Set thread to high priority */
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED); /* Prevent Windows sleep during running application */
    win32_beneath_enable_dpi_awareness();                                              /* Make App DPI-Aware */
    timeBeginPeriod(1);                                                                /* Set timer resolution to 1 millisecond */

    memory.memory_offset = sizeof(beneath_state);
    memory.memory = VirtualAlloc(0, memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (!memory.memory)
    {
        return 1;
    }

    /* Default state before application call */
    state = (beneath_state *)memory.memory;
    state->window_width = 800;
    state->window_height = 600;
    state->window_clear_color_r = 0.157f;
    state->window_clear_color_g = 0.157f;
    state->window_clear_color_b = 0.157f;
    state->window_clear_color_a = 1.0f;
    state->running = true;
    state->frames_per_second_target = BENEATH_STATE_FRAMES_PER_SECOND_VSYNC;

    win32_state.state = state;
    win32_state.input = &input;

    /* Assign platform specific api function exposed to the application */
    api.io_print = win32_beneath_api_io_print;
    api.io_file_size = win32_beneath_api_io_file_size;
    api.io_file_read = win32_beneath_api_io_file_read;
    api.io_file_write = win32_beneath_api_io_file_write;
    api.time_sleep = win32_beneath_api_time_sleep;
    api.perf_cycle_count = win32_beneath_api_perf_cycle_count;
    api.perf_time_nanoseconds = win32_beneath_api_perf_time_nanoseconds;

    /* Load window and initialize opengl 3.3 */
    void *window_handle = (void *)0;
    void *dc = (void *)0;
    void *timer = CreateWaitableTimerA(NULL, true, NULL);

    if (!timer)
    {
        return 1;
    }

    if (!win32_beneath_initialize_opengl(&win32_state, &window_handle, &dc))
    {
        return 1;
    }

#ifdef BENEATH_LIB
    win32_beneath_load_application();
#endif

    last_time = win32_beneath_api_perf_time_nanoseconds();

    while (state->running)
    {
        /******************************/
        /* Delta & Timing Metrics     */
        /******************************/
        double now = win32_beneath_api_perf_time_nanoseconds();
        double delta = now - last_time;

        last_time = now;
        state->delta_time = delta * 1e-9;
        state->frames_per_second = (unsigned int)(1.0 / state->delta_time);

#ifdef BENEATH_LIB
        /******************************/
        /*  HOT-Reload Code           */
        /******************************/
        FILETIME ddlFtCurrent = win32_beneath_file_modification_time(app.dllName);

        if (CompareFileTime(&ddlFtCurrent, &app.lastWriteTime) != 0 && win32_beneath_load_application())
        {
            win32_beneath_api_io_print(__FILE__, __LINE__, "Hot reloaded application dll!\n");
        }

#endif

        /******************************/
        /* Update State if Needed    */
        /******************************/
        if (state->changed_flags)
        {
            win32_beneath_update_state(state, &window_handle);
        }

        /******************************/
        /* Input Processing           */
        /******************************/
        win32_beneath_process_input(state, &input);

        /******************************/
        /* Rendering                  */
        /******************************/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /******************************/
        /* Call Application           */
        /******************************/
        beneath_update(
            &memory, /* Memory From Platform     */
            &input,  /* Keyboard/Mouse/etc input */
            &api     /* Platform API calls       */
        );

        SwapBuffers(dc);

        /******************************/
        /* Frame Rate Limiting        */
        /******************************/
        if (state->frames_per_second_target > 0)
        {
            double targetFrameTime = 1.0 / (double)state->frames_per_second_target;
            win32_precise_sleep(&timer, targetFrameTime);
        }
    }

    win32_beneath_api_io_print(__FILE__, __LINE__, "[win32] ended\n");

    timeEndPeriod(1);
    SetThreadExecutionState(ES_CONTINUOUS);

    ExitProcess(0);

    return 0;
}
