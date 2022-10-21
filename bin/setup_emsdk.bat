@echo off

SET me=%~dp0
SET RootDir=%me%..
SET EMSDK=%RootDir%\emsdk


if not exist %EMSDK% GOTO begin_clone_emsdk
:end_clone_emsdk


pushd%EMSDK%
git pull
call emsdk install latest
call emsdk activate latest
call emsdk_env.bat
popd
GOTO done

:begin_clone_emsdk
pushd %RootDir%
git clone https://github.com/emscripten-core/emsdk.git
popd
GOTO end_clone_emsdk

:done
