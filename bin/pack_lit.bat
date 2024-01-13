@echo off
setlocal

SET me=%~dp0
SET root_dir=%me%..
SET build_dir=%root_dir%\build
SET icon_dir=%root_dir%\icons
SET pack_dir=%root_dir%\pack\lit

call build pass_lit -run
call build lit -ship -o 

if not exist %build_dir% goto :eof
if not exist %pack_dir% mkdir %pack_dir%

move %build_dir%\lit.dat %pack_dir% >nul
move %build_dir%\lit.exe %pack_dir% >nul
move %icon_dir%\lit.ico %pack_dir%\window.ico

:eof






