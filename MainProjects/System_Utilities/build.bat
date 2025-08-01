@echo off
setlocal

echo Building the app...
echo.

gcc --version >nul 2>&1
if errorlevel 1 (
    echo Error: GCC compiler not found!
    echo Please intall MinGW-w64 or MSYS2 or some other compiler
    pause
    exit /b 1
)

if exist *.o del *.off
if exist sysmon.exe del sysmon.exe

echo Compiling files...
gcc -Wall -Wextra -std=c99 -02 -c main.c -o main.o
if errorlevel 1 goto :error

gcc -Wall -Wextra -std=c99 -02 -c system_info.c -o system_info.o
if errorlevel 1 goto :error

gcc -Wall -Wextra -std=c99 -02 -c process_info.c -o process_info.o
if errorlevel 1 goto :error

gcc -Wall -Wextra -std=c99 -02 -c ui.c -o ui.o
if errorlevel 1 goto :error

echo Linking executable...
gcc main.o system_info.o process_info.o ui.o -o sysmon.exe -lpsapi
if errorlevel 1 goto :error

del *.o

echo.
echo Build complete!
echo Executable: sysmon.exe
echo.
echo To run the application, open sysmon.exe or type sysmon.exe
echo.
pause
exit /b 0 

:error
echo.
echo Compiling failed.
pause
exit /b 1