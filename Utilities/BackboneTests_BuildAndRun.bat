@echo off
setlocal

set ThisDir=%~dp0

call "%ThisDir%BackboneTests_BuildOnly.bat"
if %errorlevel% neq 0 goto :eof

pushd "%ThisDir%..\Build"
  BackboneTests.exe
popd

endlocal
