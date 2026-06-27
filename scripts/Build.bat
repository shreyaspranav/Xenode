@echo off
setlocal

:: Args: Configuration - [Debug, Release_Debug, Production]

:: This script:
:: - Finds MSBuild.exe
:: - Builds Xenode with its dependencies

:: Config: ---------------------------------------------------------------------------------------------------------
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
:: -----------------------------------------------------------------------------------------------------------------

if not exist "%VSWHERE%" (
    call :ColoredText "ERROR: vswhere.exe not found." Red
    exit /b 1
)

if "%~1"==""  (
    call :ColoredText "ERROR: Specify the build configuration" Red
    exit /b
)

:: Try to find MSBuild.exe.
for /f "usebackq delims=" %%i in (`
    "%VSWHERE%" -latest -prerelease -products * ^
    -requires Microsoft.Component.MSBuild ^
    -find MSBuild\**\Bin\MSBuild.exe
`) do (
    set "MSBUILD=%%i"
    goto BuildDirect
)

call :ColoredText "ERROR: Could not locate MSBuild.exe." Red
exit /b 1

:BuildDirect

:: Build using MSBuild.exe
"%MSBUILD%" /p:Configuration=%~1

exit /b 0

:ColoredText
powershell -NoProfile -Command "Write-Host '%~1' -ForegroundColor %~2"
exit /b

endlocal
