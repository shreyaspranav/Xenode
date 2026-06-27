@echo off
setlocal

:: This script:
:: - Detects VS installation and validates it.
:: - Builds premake and copies premake5.exe to the root.
:: - Generates .sln/.slnx/.vcxproj files, VSCode files if required and compile_commands.json if required.  

:: Config: ------------------------------------------------------------------------------------------------------------------------------
set "BUILD_PREMAKE=false"
set "GENERATE_VSCODE_FILES=true"
set "GENERATE_COMPILE_COMMANDS=true"
:: --------------------------------------------------------------------------------------------------------------------------------------

if "%~1"==""  (
    call :ColoredText "Specify the VS tools version: vs2022 or vs2026" Red
    exit /b 1
)

if /i "%~1%"=="vs2022" goto ok
if /i "%~1%"=="vs2026" goto ok

call :ColoredText "Invalid option. Use vs2022 or vs2026" Red
exit /b 1

:ok

if /i "%~2"=="-p" (
    set "BUILD_PREMAKE=true"
)

:: Check if vswhere exists
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    call :ColoredText "vswhere.exe not found. Cannot detect Visual Studio." Red
    exit /b 1
)

"%VSWHERE%" -version [%VS_MIN_VERSION%,) -products * -requires Microsoft.Component.MSBuild -property installationPath >nul 2>&1

if errorlevel 1 (
    call :ColoredText "Visual Studio 2022/2026 or Build Tools NOT found." Red
    exit /b 1
)

call :ColoredText "Visual Studio 2022+ detected." Green

:: Build Premake using BuildPremake.bat
if "%BUILD_PREMAKE%"=="true" (
    call "scripts/BuildPremake.bat" %~1%
)

call :ColoredText "Generating Solution and Project files..." Cyan
premake5 %~1% 

if "%GENERATE_VSCODE_FILES%"=="true" (
    call :ColoredText "Generating tasks.json and launch.json..." Cyan
    premake5 vscode --config=Debug
)

if "%GENERATE_COMPILE_COMMANDS%"=="true" (
    call :ColoredText "Generating compile_commands.json" Cyan

    if "%~1%"=="vs2022" premake5 compilecommands --cc-config=Debug --cc=msc-v143
    if "%~1%"=="vs2026" premake5 compilecommands --cc-config=Debug --cc=msc-v145
)
exit /b

:: Function to print a particular color
:ColoredText
powershell -NoProfile -Command "Write-Host '%~1' -ForegroundColor %~2"
exit /b

endlocal