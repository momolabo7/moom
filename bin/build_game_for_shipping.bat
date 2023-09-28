@echo off

setlocal

set me=%~dp0
set root_dir=%me%..
set code_dir=%root_dir%\code
set build_dir=%root_dir%\build

set filename=%~1
shift




set compiler_flags=-MT -WX -W4 -wd4189 -wd4702 -wd4201 -wd4505 -wd4996 -wd4100 -Zi  -GR -EHa  -std:c++17
set linker_flags=-link -incremental:no -opt:ref

echo ******************* 


rem Optimization
if %optimize%==1 (
  echo Version  : Release
  set compiler_flags=-O2 -DASSERTIVE=0 %compiler_flags%
) else (
  echo Version  : Internal 
  set compiler_flags=-DASSERTIVE=1 %compiler_flags%
)

rem Build Type
if %game%==1 (
  echo Build    : Game
  set compiler_flags=%compiler_flags% -LD
  set linker_flags=%linker_flags% -out:game:dll 
  goto end_build_type
)
if %w32%==1 (
  echo Build    : Win32
  set linker_flags=%linker_flags% user32.lib opengl32.lib gdi32.lib winmm.lib ole32.lib imm32.lib shell32.lib 
  goto end_build_type
)

echo Build    : Normal 

:end_build_type

echo ******************* 

pushd %build_dir%

cl %compiler_flags% %code_dir%\%filename%.cpp %linker_flags% 

if %run%==1 CALL %filename%.exe

popd

