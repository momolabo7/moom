@echo off
setlocal

SET me=%~dp0

SET root_dir=%me%..
SET code_dir=%root_dir%\code
SET build_dir=%root_dir%\build
SET output_name="w32_game.exe"
SET is_release_version="0"

if not exist %build_dir% mkdir %build_dir%

for %%a in (%*) do (
  rem Momo: The fact that 'c' set here almost made me want
  rem       to write my own build script...
  for /f "tokens=1,2 delims=:" %%b in ("%%a") do (
    if "%%b" == "/o" (
      SET is_release_version="1"
    )

    if "%%b" == "/n" (
      SET output_name=%%c
    )

  )
)


SET CompilerFlags=-MT -WX -W4 -wd4706 -wd4189 -wd4702 -wd4201 -wd4505 -wd4996 -wd4100 -Zi  -GR -EHa  -std:c++17 

echo ******************* 

if %is_release_version%=="1" (
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
