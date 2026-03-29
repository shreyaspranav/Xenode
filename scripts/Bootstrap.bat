@echo off
setlocal

:: This script downloads 
:: - Detects VS installation and validates it
:: - Downloads premake 
:: - Generates compile_commands.json if required 

:: Config: ------------------------------------------------------------------------------------------------------------------------------
set "VS_MIN_VERSION=17.0"
set "PREMAKE_VERSION=5.0.0-beta8"
set "PREMAKE_URL=https://github.com/premake/premake-core/releases/download/v5.0.0-beta8/premake-%PREMAKE_VERSION%-windows.zip"
set "TMP_DIR=scripts\tmp"
set "ZIP_FILE=%TMP_DIR%\premake.zip"
set "EXTRACT_DIR=."
set "GENERATE_COMPILE_COMMANDS=false"
:: --------------------------------------------------------------------------------------------------------------------------------------

:: Check if vswhere exists
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    call :ColoredText "vswhere.exe not found. Cannot detect Visual Studio." Red
    exit /b 1
)

"%VSWHERE%" -version [%VS_MIN_VERSION%,) -products * -requires Microsoft.Component.MSBuild -property installationPath >nul 2>&1

if errorlevel 1 (
    call :ColoredText "Visual Studio 2022 or Build Tools NOT found." Red
    exit /b 1
)

call :ColoredText "Visual Studio 2022+ detected." Green

if not exist "%TMP_DIR%" mkdir "%TMP_DIR%"

call :ColoredText "Downloading Premake v%PREMAKE_VERSION%..." Yellow
curl -L --fail -o "%ZIP_FILE%" "%PREMAKE_URL%"
if errorlevel 1 (
    call :ColoredText "Failed to download Premake v%PREMAKE_VERSION%" Red 
    exit /b 1
)

call :ColoredText "Extracting Premake..." Yellow
powershell -NoProfile -Command "Expand-Archive -Path '%ZIP_FILE%' -DestinationPath '%EXTRACT_DIR%' -Force"
if errorlevel 1 (
    call :ColoredText "Failed to extract Premake." Red
    exit /b 1
)

call :ColoredText "Cleaning up..." Yellow
rmdir /s /q "%TMP_DIR%"

if "%GENERATE_COMPILE_COMMANDS%"=="true" (
    call :ColoredText "Generating compile_commands.json" Cyan
    premake5 export-compile-commands
)
exit /b

:: Function to print a particular color
:ColoredText
powershell -NoProfile -Command "Write-Host '%~1' -ForegroundColor %~2"
exit /b

endlocal