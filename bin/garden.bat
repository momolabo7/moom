
@echo off

setlocal EnableDelayedExpansion

set app_name=%1
set option=%2

IF %option%==-ship (
  echo "ship!"
)

IF %option%==-build (
  cmd /c build pass_%app_name% -bun
  cmd /c build app_%app_name% -app
  cmd /c build w32_eden -w32 
)

IF %option%==-hot (
  cmd /c build app_%app_name% -app
)

IF %option%==-run (
  cmd /c build w32_eden -run
)

IF %option%==-pass (
  cmd /c build pass_%app_name% -bun
)

