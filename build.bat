@echo off
echo ============================================================
echo   Building Music Playlist Manager (C11 / GCC)
echo ============================================================

gcc -std=c11 -Wall -Wextra src/song.c src/library.c src/singly_linked_list.c src/doubly_linked_list.c src/circular_linked_list.c src/stack.c src/queue.c src/priority_queue.c src/hash_table.c src/bst.c src/graph.c src/file_handler.c src/interface.c src/main.c -Iinclude -o music_manager.exe

if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Compilation finished without errors!
    echo Running music_manager.exe...
    echo ============================================================
    music_manager.exe
) else (
    echo [ERROR] Compilation failed. Please ensure GCC is installed and on your PATH.
)
pause
