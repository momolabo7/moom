@echo off
setlocal

SET me=%~dp0

SET RootDir=%me%..
SET CodeDir=%RootDir%\code
SET BuildDir=%RootDir%\build
SET Opt=%1

if not exist %BuildDir% mkdir %BuildDir%

for %%a in (%*) do set "args[%%a]=1"

SET CompilerFlags=-MT -WX -W4 -wd4706 -wd4189 -wd4702 -wd4201 -wd4505 -wd4996 -wd4100 -Zi  -GR -EHa  -std:c++17 


if defined args[/r] (
  echo /release
  SET CompilerFlags=-O2 -DASSERTIVE=0 %CompilerFlags%
) else  (
  echo /internal
  SET CompilerFlags=-O2 -DASSERTIVE=1 %CompilerFlags%
)

SET LinkerFlags=-incremental:no -opt:ref
SET LinkerFlags=user32.lib opengl32.lib gdi32.lib winmm.lib ole32.lib imm32.lib shell32.lib %LinkerFlags%

pushd %BuildDir%

call cl %CompilerFlags% %CodeDir%\w32_game.cpp -link %LinkerFlags%

rem Copy icon for window ----------------------
xcopy %RootDir%\icons\window.ico  %BuildDir% /Q /Y

popd
