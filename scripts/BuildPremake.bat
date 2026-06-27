@echo off
setlocal

:: This script:
:: - Looks if premake submodule is present or not
:: - If present, builds premake using the custom build script: depbuildscripts/Premake.Bootstrap.mak
:: - Copies premake.exe from the build directory to the root of the project

:: Config: ------------------------------------------------------------------------------------------------------------------------------
set "SUBMODULE_PATH=deps/premake"
set "CUSTOM_MAKEFILE=depbuildscripts/Premake.Bootstrap.mak"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
:: --------------------------------------------------------------------------------------------------------------------------------------

if "%~1"==""  (
    call :ColoredText "ERROR: Specify the VS tools version: vs2022 or vs2026" Red
    exit /b
)

if /i "%~1%"=="vs2022" goto ok
if /i "%~1%"=="vs2026" goto ok

call :ColoredText "Invalid option. Use vs2022 or vs2026" Red
exit /b 1

:ok

if not exist %SUBMODULE_PATH% (
    call :ColoredText "ERROR: Premake submdule not found" Red
    call :ColoredText "ERROR: Clone all the submodules using git submodule init followed by git submodule update" Red

    exit /b 1
)

call :ColoredText "Building Premake..." Yellow

set "CURRENT_DIR=%cd%"
set "MSDEV=%1"

for /f "usebackq delims=" %%i in (`
    "%VSWHERE%" -latest -prerelease -products * ^
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
    -find Common7\Tools\VsDevCmd.bat
`) do (
    call "%%i"
    goto FoundNMake
)

call :ColoredText "ERROR: Cannot find nmake.exe" Red
exit /b 1

: Change the directory the the premake's submodule, build using custom makefile
:FoundNMake
cd %SUBMODULE_PATH%
nmake -f %CURRENT_DIR%/%CUSTOM_MAKEFILE% windows

if errorlevel 1 (
    call :ColoredText "ERROR: Failed to build premake!" Red
    exit /b 1
)

call :ColoredText "Copying premake5.exe..." Yellow
copy "bin\release\premake5.exe" "%CURRENT_DIR%\premake5.exe"

cd %CURRENT_DIR%

call :ColoredText "Sucessfully built premake. Use premake5 --help to get started." Green
exit /b

:ColoredText
powershell -NoProfile -Command "Write-Host '%~1' -ForegroundColor %~2"
exit /b

endlocal