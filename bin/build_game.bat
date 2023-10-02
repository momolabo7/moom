
@echo off
rem Usage: build [game cpp filename] [-o]
rem   -o: if specificed, will build with optimized flags


SETLOCAL

SET me=%~dp0
SET root_dir=%me%..
SET code_dir=%root_dir%\code
SET build_dir=%root_dir%\build
SET optimize=0
SET c_file=%~1
SHIFT

:label_parse 
IF "%~1"=="" GOTO label_end_parse
IF "%~1"=="-o" SET optimize=1
SHIFT
GOTO label_parse
:label_end_parse


IF not exist %build_dir% mkdir %build_dir%

SET CompilerFlags=-MT -WX -W4 -wd4189 -wd4702 -wd4201 -wd4505 -wd4996 -wd4100 -Zi  -GR -EHa  -std:c++17

echo ******************* 

if %optimize%==1 (
  echo Version  : Release
  SET CompilerFlags=-O2 -DASSERTIVE=0 %CompilerFlags%
) else (
  echo Version  : Internal 
  SET CompilerFlags=-DASSERTIVE=1 %CompilerFlags%
)
echo ******************* 


pushd %build_dir%
call cl %CompilerFlags% %code_dir%\%c_file%.cpp -LD -link -incremental:no -opt:ref -out:game.dll 


rem We do this because for some reason, compiling will result in the DLL being modified twice and cus our hot reloading feature to be loaded twice. This *should* prevent it.
rem
popd
