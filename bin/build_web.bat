@echo off
SET me=%~dp0

SET RootDir=%me%..
SET CodeDir=%RootDir%\code
SET BuildDir=%RootDir%\build
SET Opt=%1

if not exist %BuildDir% mkdir %BuildDir%

pushd %BuildDir%

call emcc %CodeDir%\web.cpp -o web.html -sSINGLE_FILE 

popd
