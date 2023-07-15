@echo off
SET me=%~dp0

SET RootDir=%me%..
SET CodeDir=%RootDir%\code
SET BuildDir=%RootDir%\build
SET Opt=%1%

call bin\setup_cl_x64.bat
IF NOT "%PLATFORM%" == "X64" IF NOT "%PLATFORM%" == "x64" (EXIT /b)


SET CommonCompilerFlags=-MT -WX -W4 -wd4533 -wd4189 -wd4702 -wd4201 -wd4505 -wd4996 -wd4100 -Zi -Oi -GR- -EHa -Gm- -std:c++17
SET CommonCompilerFlags=%CommonCompilerFlags%

pushd %BuildDir%

call cl %CommonCompilerFlags% %CodeDir%\pass_lit.cpp -link -OUT:pass_lit.exe

call pass_lit.exe

popd
