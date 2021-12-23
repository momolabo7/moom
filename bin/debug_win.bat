@echo off
SET me=%~dp0
SET RootDir=%me%\..
SET BuildDir=%RootDir%\build
SET Opt=%1%



pushd %BuildDir%


if exist win.rdbg (
	call remedybg win.rdbg
) else (
	call remedybg win.exe
)

popd
