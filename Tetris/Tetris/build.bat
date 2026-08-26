@echo off
cls
echo ==========================================
echo    COMPILING GEMSTONE GUARDIAN TETRIS
echo ==========================================
echo.

C:/msys64/ucrt64/bin/g++.exe tetris.cpp -o tetris.exe ^
-IC:/msys64/ucrt64/include ^
-IC:/msys64/ucrt64/include/freetype2 ^
-IC:/msys64/ucrt64/include/libpng16 ^
-IC:/msys64/ucrt64/include/harfbuzz ^
-IC:/msys64/ucrt64/include/glib-2.0 ^
-IC:/msys64/ucrt64/lib/glib-2.0/include ^
-LC:/msys64/ucrt64/lib ^
-lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system

if %errorlevel% equ 0 (
    echo.
    echo ==========================================
    echo    COMPILATION SUCCESSFUL!
    echo ==========================================
    echo.
    echo Starting game...
    echo.
    tetris.exe
) else (
    echo.
    echo ==========================================
    echo    COMPILATION FAILED!
    echo    Check the errors above.
    echo ==========================================
    echo.
    pause
)