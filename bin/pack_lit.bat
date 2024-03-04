@echo off
setlocal

SET me=%~dp0
SET root_dir=%me%..
SET build_dir=%root_dir%\build
SET icon_dir=%root_dir%\icons
SET pack_dir=%root_dir%\pack\lit

call build pass_lit -run
call build app_lit -ship -o 

if not exist %build_dir% goto :eof
if not exist %pack_dir% mkdir %pack_dir%

xcopy /q /y %build_dir%\lit.dat %pack_dir% >nul
xcopy /q /y %build_dir%\app_lit.exe %pack_dir%\lit.exe >nul
xcopy /q /y %icon_dir%\lit.ico %pack_dir%\window.ico 

:eof






