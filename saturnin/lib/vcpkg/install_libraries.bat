@echo off
rem checking parameter
if "%1"=="" (
  echo "Please specify library type (dynamic/static)"
  goto :EOF
) 
set "parameter_is_valid="
if "%1"=="dynamic" (
  set library_type=
  set "parameter_is_valid=y"
)
if "%1"=="static" (
  set library_type=-static
  set "parameter_is_valid=y"
)
if not defined parameter_is_valid (
  echo "Unknown parameter"
  goto :EOF
)

rem getting vcpkg path
for /f "delims=" %%x in (vcpkg.cfg) do (set "%%x")

set "libslist=libraries.lst"
setlocal enabledelayedexpansion
set command=%vcpkg_full_path% install
for /f %%l in (%libslist%) do (
    set command=!command! %%l:x86-windows%library_type% --editable
    set command=!command! %%l:x64-windows%library_type% --editable
)
%command%