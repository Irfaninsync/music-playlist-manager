@echo off
echo ============================================================
echo   Building Music Playlist Manager (C11 / GCC)
echo ============================================================

gcc -std=c11 -Wall -Wextra main.c -o music_manager.exe

if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Compilation finished without errors!
    echo Running music_manager.exe...
    echo ============================================================
    music_manager.exe
) else (
    echo [ERROR] Compilation failed. Please ensure GCC is installed on your PATH.
)
pause
