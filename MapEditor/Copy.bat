@echo off
setlocal

set BASE=%~dp0

REM 생성될 때까지 기다리는 함수
call :WaitForFile "%BASE%Engine\Bin\Engine.dll" 10
call :WaitForFile "%BASE%Engine\Bin\Engine.lib" 10

REM 실제 복사
xcopy /y "%BASE%Engine\Header\*.*"         "%BASE%Reference\Header\"
xcopy /y "%BASE%Engine\Bin\Engine.dll"     "%BASE%Client\Bin\"
xcopy /y "%BASE%Engine\Bin\Engine.lib"     "%BASE%Reference\Library\"

goto :eof

:WaitForFile
set file=%~1
set retry=%~2
set count=0

:loop
if exist %file% (
    echo Found %file%
    goto :eof
)

set /a count+=1
if %count% GEQ %retry% (
    echo [ERROR] %file% not found after %retry% seconds.
    exit /b 1
)

echo Waiting for %file% ...
timeout /t 1 > nul
goto loop
