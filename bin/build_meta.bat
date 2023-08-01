@echo off

SET me=%~dp0

SET RootDir=%me%..
SET CodeDir=%RootDir%\code
SET BuildDir=%RootDir%\build
SET Opt=%1

if not exist %BuildDir% mkdir %BuildDir%

SET CommonCompilerFlags=-MT -WX -W4 -wd4189 -wd4702 -wd4201 -wd4505 -wd4996 -wd4100 -Zi  -GR -EHa  -std:c++17
SET CommonCompilerFlags=-DENABLE_ASSERTS=1 -DINTERNAL=1  %CommonCompilerFlags%

pushd %BuildDir%
cl %CommonCompilerFlags% %CodeDir%\meta.cpp 

call meta.exe

popd
