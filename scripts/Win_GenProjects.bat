@echo off
pushd %~dp0\..\
call third-party\premake\bin\premake5.exe vs2022
popd
PAUSE