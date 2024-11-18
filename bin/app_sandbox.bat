@echo off

setlocal EnableDelayedExpansion

rem 0 means build
rem 1 means run
rem 2 means hot reload
rem 3 means ship
set run=0

:label_parse_next
IF "%~1"=="-run" (
  set run=1
  GOTO label_parse_end
)
IF "%~1"=="-hot" (
  set run=2
  GOTO label_parse_end
)
IF "%~1"=="-ship" (
  set run=3
  GOTO label_parse_end
)
IF "%~1"=="" (
  GOTO label_parse_end
)
:label_parse_end

IF %run%==0 (
  cmd /c build pass_sandbox -bun
  cmd /c build app_sandbox -app
  cmd /c build w32_eden -w32 
)
IF %run%==1 ( 
  cmd /c build pass_sandbox -bun
  cmd /c build app_sandbox -app
  cmd /c build w32_eden -w32 -bun
) 
IF %run%==2 (
  cmd /c build app_sandbox -app
)

IF %run%==3 (
  cmd /c build app_sandbox -ship -o
)

