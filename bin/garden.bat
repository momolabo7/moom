
@echo off

setlocal EnableDelayedExpansion

set app_name=%1
set option=%2

IF %option%==-ship (
  echo "ship!"
)

IF %option%==-build (
  build pass_%app_name% -bun
  build app_%app_name% -app
  build w32_eden -w32 
)

IF %option%==-hot (
  build app_%app_name% -app
)

IF %option%==-run (
  build w32_eden -run
)

IF %option%==-pass (
  build pass_%app_name% -bun
)



