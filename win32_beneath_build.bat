@echo off
REM Compiles the program without the C standard library

set PLATFORM_NAME=win32_beneath
set APP_NAME=beneath_application
set DIST_DIR=dist

set DEF_COMPILER_FLAGS=-mconsole -march=native -mtune=native -std=c89 -pedantic -nodefaultlibs -nostdlib -mno-stack-arg-probe -Xlinker /STACK:0x100000,0x100000 ^
-fno-builtin -ffreestanding -fno-asynchronous-unwind-tables -fuse-ld=lld ^
-Wall -Wextra -Werror -Wvla -Wconversion -Wdouble-promotion -Wsign-conversion -Wmissing-field-initializers -Wuninitialized -Winit-self -Wunused -Wunused-macros -Wunused-local-typedefs

set DEF_FLAGS_LINKER=-lkernel32 -luser32 -lgdi32 -lopengl32 -lwinmm

mkdir %DIST_DIR%

REM "[beneath] Static Builds"
REM cc -g3 -DBENEATH_APPLICATION_LAYER_NAME=%APP_NAME% %DEF_COMPILER_FLAGS% %PLATFORM_NAME%.c -o %DIST_DIR%/%PLATFORM_NAME%_static_debug.exe %DEF_FLAGS_LINKER%
REM cc -s -O2 -DBENEATH_APPLICATION_LAYER_NAME=%APP_NAME% %DEF_COMPILER_FLAGS% %PLATFORM_NAME%.c -o %DIST_DIR%/%PLATFORM_NAME%_static_release.exe %DEF_FLAGS_LINKER%

REM "[beneath] Dynamic Builds"
cc -g3 -DBENEATH_LIB -DBENEATH_APPLICATION_LAYER_NAME=%APP_NAME%_dynamic_debug %DEF_COMPILER_FLAGS% -std=c99 %PLATFORM_NAME%.c -o %DIST_DIR%/%PLATFORM_NAME%_dynamic_debug.exe %DEF_FLAGS_LINKER%
cc -g3 -shared -DBENEATH_LIB %DEF_COMPILER_FLAGS% %APP_NAME%.c -o %DIST_DIR%/%APP_NAME%_dynamic_debug.dll
REM cc -s -O2 -DBENEATH_LIB -DBENEATH_APPLICATION_LAYER_NAME=%APP_NAME%_dynamic_release %DEF_COMPILER_FLAGS% %PLATFORM_NAME%.c -o %DIST_DIR%/%PLATFORM_NAME%_dynamic_release.exe %DEF_FLAGS_LINKER%
REM cc -s -O2 -shared -DBENEATH_LIB %DEF_COMPILER_FLAGS% %APP_NAME%.c -o %DIST_DIR%/%APP_NAME%_dynamic_release.dll

cd %DIST_DIR%
REM %PLATFORM_NAME%_static_debug.exe
REM %PLATFORM_NAME%_static_release.exe
%PLATFORM_NAME%_dynamic_debug.exe
REM %PLATFORM_NAME%_dynamic_release.exe
cd ..


