@echo off
setlocal

set ThisDir=%~dp0
set FBuild=%ThisDir%FASTBuild\FBuild.exe

pushd "%ThisDir%.."
  "%FBuild%" -noprogress BackboneTests
popd

endlocal
