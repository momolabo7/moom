@echo off
rem Usage: build [cpp filename] [-r]
rem   -r: if specificed, will run the executable


SETLOCAL

SET me=%~dp0
SET root_dir=%me%..
SET code_dir=%root_dir%\code
SET build_dir=%root_dir%\build
SET run=0
SET c_file=%~1
SHIFT

:label_parse 
IF "%~1"=="" GOTO label_end_parse
IF "%~1"=="-r" SET run=1
SHIFT
GOTO label_parse

:label_end_parse




IF not exist %build_dir% mkdir %build_dir%

SET CommonCompilerFlags=-MT -WX -W4 -wd4189 -wd4702 -wd4201 -wd4505 -wd4996 -wd4100 -Zi  -GR -EHa  -std:c++17
SET CommonCompilerFlags=-DENABLE_ASSERTS=1 -DINTERNAL=1  %CommonCompilerFlags%

pushd %build_dir%
cl %CommonCompilerFlags% %code_dir%\%c_file%.cpp 

IF %run%==1 CALL %c_file%.exe

popd
