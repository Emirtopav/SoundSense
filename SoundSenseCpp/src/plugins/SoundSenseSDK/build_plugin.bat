@echo off
echo =========================================
echo SoundSense Plugin Builder
echo =========================================

if "%~1"=="" (
    echo Error: Please drag and drop your .cpp file onto this bat file.
    pause
    exit /b
)

set FILENAME=%~n1
set SOURCE=%~1
set OUTDIR=..\plugins
set OUTFILE=%OUTDIR%\%FILENAME%.dll

echo Compiling %FILENAME%.cpp...
if not exist "%OUTDIR%" mkdir "%OUTDIR%"

g++ -shared -o "%OUTFILE%" "%SOURCE%"

if %ERRORLEVEL% EQU 0 (
    echo Success! Plugin compiled to %OUTFILE%
) else (
    echo Compilation failed. Check your code.
)
pause
