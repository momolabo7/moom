@echo off

set app_name=%~1

:label_parse_next
shift
IF "%~1"=="" (
  GOTO label_parse_end
)
IF "%~1"=="-r" (
  CALL buildo w32_eden -run 
  GOTO label_parse_next
)
IF "%~1"=="-a" (
  CALL buildo app_%app_name% -app
  GOTO label_parse_next
)
IF "%~1"=="-ao" (
  CALL buildo app_%app_name% -app -o
  GOTO label_parse_next
)
IF "%~1"=="-p" (
  CALL buildo pass_%app_name%
  GOTO label_parse_next
)
IF "%~1"=="-pr" (
  CALL buildo pass_%app_name% -bun
  GOTO label_parse_next
)
IF "%~1"=="-w" (  
  CALL buildo w32_eden -w32 
  GOTO label_parse_next
)
IF "%~1"=="-all" (  
  CALL buildo pass_%app_name% -bun
  CALL buildo app_%app_name% -app
  CALL buildo w32_eden -w32 
  GOTO label_parse_next
)
GOTO label_parse_next

:label_parse_end


