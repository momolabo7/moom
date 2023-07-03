
echo off
setlocal

SET me=%~dp0

SET root_dir=%me%..
SET code_dir=%root_dir%\code
SET build_dir=%root_dir%\build
SET is_release_version="0"

if not exist %build_dir% mkdir %build_dir%

for %%a in (%*) do (
  rem Momo: The fact that 'c' set here almost made me want
  rem       to write my own build script...
  for /f "tokens=1,2 delims=:" %%b in ("%%a") do (
    if "%%b" == "/o" (
      SET is_release_version="1"
    )

  )
)

SET CompilerFlags=-MT -WX -W4 -wd4706 -wd4189 -wd4702 -wd4201 -wd4505 -wd4996 -wd4100 -Zi  -GR -EHa  -std:c++17 

echo ******************* 

if %is_release_version%=="1" (
  echo Version  : Release
  SET CompilerFlags=-O2 -DASSERTIVE=0 %CompilerFlags%
) else (
  echo Version  : Internal 
  SET CompilerFlags=-DASSERTIVE=1 %CompilerFlags%
)
echo ******************* 

pushd %build_dir%
call cl %CompilerFlags% %code_dir%\lit.cpp -LD -link -out:game.dll 

rem We do this because for some reason, compiling will result in the DLL being modified twice and cus our hot reloading feature to be loaded twice. This *should* prevent it.

popd

