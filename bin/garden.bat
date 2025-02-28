@echo off

set app_name=%~1

:label_parse_next
shift
IF "%~1"=="" (
  GOTO label_parse_end
)
IF "%~1"=="-r" (
  CALL build w32_eden -run 
  GOTO label_parse_next
)
IF "%~1"=="-a" (
  CALL build app_%app_name% -app
  GOTO label_parse_next
)
IF "%~1"=="-ao" (
  CALL build app_%app_name% -app -o
  GOTO label_parse_next
)
IF "%~1"=="-p" (
  CALL build pass_%app_name%
  GOTO label_parse_next
)
IF "%~1"=="-pr" (
  CALL build pass_%app_name% -bun
  GOTO label_parse_next
)
IF "%~1"=="-w" (  
  CALL build w32_eden -w32 
  GOTO label_parse_next
)
IF "%~1"=="-all" (  
  CALL build pass_%app_name% -bun
  CALL build app_%app_name% -app
  CALL build w32_eden -w32 
  GOTO label_parse_next
)
GOTO label_parse_next

:label_parse_end


