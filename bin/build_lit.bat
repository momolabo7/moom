
echo off
setlocal

SET me=%~dp0

SET RootDir=%me%..
SET CodeDir=%RootDir%\code
SET BuildDir=%RootDir%\build
SET Opt=%1

if not exist %BuildDir% mkdir %BuildDir%

for %%a in (%*) do (
  echo %%a
  set "args[%%a]=1"
)

SET CompilerFlags=-MT -WX -W4 -wd4706 -wd4189 -wd4702 -wd4201 -wd4505 -wd4996 -wd4100 -Zi  -GR -EHa  -std:c++17 

if defined args[/r] (
  echo /release
  SET CompilerFlags=-O2 -DASSERTIVE=0 %CompilerFlags%
) else (
  echo /internal
  SET CompilerFlags=-O2 -DASSERTIVE=1 %CompilerFlags%
)

pushd %BuildDir%
call cl %CompilerFlags% %CodeDir%\lit.cpp -LD -link -out:game.dll 

rem We do this because for some reason, compiling will result in the DLL being modified twice and cus our hot reloading feature to be loaded twice. This *should* prevent it.


popd
