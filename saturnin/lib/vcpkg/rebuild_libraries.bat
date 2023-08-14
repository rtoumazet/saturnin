rem @echo off
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

set command_remove=%vcpkg_full_path% remove
for /f %%l in (%libslist%) do (
	for /f "tokens=1,2 delims=[" %%a in ("%%l") do (
		%command_remove% %%a:x86-windows%library_type% --recurse
		%command_remove% %%a:x64-windows%library_type% --recurse
	) 
)

set command_install=%vcpkg_full_path% install
for /f %%l in (%libslist%) do (
    set command_install=!command_install! %%l:x86-windows%library_type% 
    set command_install=!command_install! %%l:x64-windows%library_type% 
)

%command_install% --editable
