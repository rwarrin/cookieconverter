@echo off

IF NOT EXIST build mkdir build
pushd build

set CompilerFlags=-nologo -Z7 -Od -Oi -fp:fast -FC
set LinkerFlags=

cl.exe %CompilerFlags% ..\code\main.cpp /link %LinkerFlags%

popd
