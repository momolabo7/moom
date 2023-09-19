@echo off
setlocal

SET me=%~dp0

SET root_dir=%me%..
SET code_dir=%root_dir%\code
SET build_dir=%root_dir%\build
SET output_name="w32_game.exe"
SET optimize=0

:label_parse 
IF "%~1"=="" GOTO label_end_parse
IF "%~1"=="-o" SET optimize=1
SHIFT
GOTO label_parse
:label_end_parse



if not exist %build_dir% mkdir %build_dir%



SET CompilerFlags=-MT -WX -W4 -wd4706 -wd4189 -wd4702 -wd4201 -wd4505 -wd4996 -wd4100 -Zi  -GR -EHa  -std:c++17 

echo ******************* 

if optimize==1 (
  echo Version  : Release
  SET CompilerFlags=-O2 -DASSERTIVE=0 -DHOT_RELOADABLE=0 %CompilerFlags%
) else  (
  echo Version  : Internal 
  SET CompilerFlags=-DASSERTIVE=1 -DHOT_RELOADABLE=1 %CompilerFlags%
)

SET LinkerFlags=-incremental:no -opt:ref
SET LinkerFlags=user32.lib opengl32.lib gdi32.lib winmm.lib ole32.lib imm32.lib shell32.lib %LinkerFlags%

echo Output   : %output_name%

echo ******************* 

pushd %build_dir%

call cl %CompilerFlags% %code_dir%\w32_game.cpp -link %LinkerFlags% -out:%output_name%

rem Copy icon for window ----------------------
xcopy %root_dir%\icons\window.ico  %build_dir% /Q /Y

popd
