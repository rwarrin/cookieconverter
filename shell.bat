@echo off

SUBST Y: /D
SUBST Y: %cd%

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
start D:\Development\Vim\vim80\gvim.exe
