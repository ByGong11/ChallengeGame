@echo off
setlocal EnableExtensions

set "PROJECT_ROOT=%~dp0"
set "PROJECT_FILE=%PROJECT_ROOT%ChallengeGame.uproject"
set "ENGINE_BIN=D:\Epic Games\UE_5.7\Engine\Binaries\Win64"
set "EDITOR_CMD=%ENGINE_BIN%\UnrealEditor-Cmd.exe"
set "REPORT_DIR=%PROJECT_ROOT%Saved\AutomationReports\AITesting"

if not exist "%PROJECT_FILE%" goto ProjectMissing
if exist "%EDITOR_CMD%" goto RunTests

set "EDITOR_CMD=%ENGINE_BIN%\UnrealEditor.exe"
if exist "%EDITOR_CMD%" goto RunTests
goto EditorMissing

:RunTests
if not exist "%REPORT_DIR%" mkdir "%REPORT_DIR%"
echo Running all AI-generated ChallengeGame automation tests...
call "%EDITOR_CMD%" "%PROJECT_FILE%" -unattended -nop4 -nosplash -NullRHI -ExecCmds="Automation RunTest ChallengeGame.AI.DeathRespawn.RespawnState;Quit" -ReportExportPath="%REPORT_DIR%"
set "EXIT_CODE=%ERRORLEVEL%"
echo.
echo Test run finished.
echo Report directory: %REPORT_DIR%
echo Current expected baseline: AI-UT-002 fails and documents AI-DEF-001.
echo Unreal Editor exit code: %EXIT_CODE%
pause
exit /b %EXIT_CODE%

:ProjectMissing
echo ERROR: ChallengeGame.uproject was not found beside this script.
echo Place this file in the ChallengeGame project root and run it again.
pause
exit /b 1

:EditorMissing
echo ERROR: UnrealEditor-Cmd.exe and UnrealEditor.exe were not found.
echo Checked directory: %ENGINE_BIN%
echo Edit ENGINE_BIN in this file if Unreal Engine is installed elsewhere.
pause
exit /b 1
