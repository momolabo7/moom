@echo off


rem USAGE
rem
rem   build [-run] [-w32] [-o] [-game] [cpp files]
rem
rem ARGUMENTS
rem
rem   -run 
rem     Run the output, whatever it is.
rem
rem   -o    
rem     Run with my favourite optimization flags 
rem
rem   -game
rem     Build as game.dll. Expects only 1 .cpp file
rem
rem   -w32
rem     Build with common win32 stuff. Expects only 1 .cpp file
rem   
rem   -ship
rem     Special build for shipping a game. Expects 1 .cpp file. 
rem     Will build w32_game.cpp and the given 1.cpp file.
rem     
rem   TODO: Maybe allow an output flag?
rem
rem TODO
rem - output flag to specify output?
rem - When building, place all intermiediately file elsewhere
rem

setlocal EnableDelayedExpansion

set me=%~dp0
set root_dir=%me%..
set code_dir=%root_dir%\code
set build_dir=%root_dir%\build
set ship_file= 

set run=0
set w32=0
set optimize=0 
set game=0
set ship=0
set filename=%~1
shift

:label_parse 
IF "%~1"=="" GOTO label_end_parse
IF "%~1"=="-run"   set run=1
IF "%~1"=="-w32"   set w32=1
IF "%~1"=="-o"     set optimize=1
IF "%~1"=="-game"  set game=1
IF "%~1"=="-ship"  set ship=1
shift
GOTO label_parse
:label_end_parse



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

rem Different build types
rem TODO: we should really have better names, maybe with prefixes like %build_game%
if %game%==1 (
  echo Build    : Game
  echo Out      : game.dll
  set compiler_flags=%compiler_flags% -LD
  set linker_flags=%linker_flags% -out:game:dll 
  goto end_build_type
)

if %w32%==1 (
  echo Build    : Win32
  set linker_flags=%linker_flags% user32.lib opengl32.lib gdi32.lib winmm.lib ole32.lib imm32.lib shell32.lib 
  goto end_build_type
)

if %ship%==1 (
  echo Build    : Ship
  echo Out      : %filename%.exe
  set compiler_flags=%compiler_flags% -DHOT_RELOAD=0 
  set linker_flags=%linker_flags% user32.lib opengl32.lib gdi32.lib winmm.lib ole32.lib imm32.lib shell32.lib -out:%filename%.exe
  set ship_file=%code_dir%\%filename%_ship.cpp 
  goto end_build_type
)

echo Build    : Normal 
echo Out      : %filename%.exe

:end_build_type

echo ******************* 


pushd %build_dir%

if %ship%==1 (

  echo #include "%filename%.cpp" > %ship_file% 
  type %code_dir%\w32_game.cpp >>  %ship_file%  
  cl %compiler_flags% %ship_file% %linker_flags% 
  del %ship_file% 
) else (
  cl %compiler_flags% %code_dir%\%filename%.cpp %linker_flags% 
)

if %run%==1 CALL %filename%.exe


popd
