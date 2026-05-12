@echo off
title SoundSense Firmware Flasher
color 0b
echo.
echo  #########################################
echo  #                                       #
echo  #       SOUNDSENSE FIRMWARE FLASH       #
echo  #                                       #
echo  #########################################
echo.

:: Check for esptool
python -m esptool version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] esptool not found. Please install it with: pip install esptool
    pause
    exit /b
)

:: Auto-detect port or ask
set /p PORT="Enter ESP32 COM Port (e.g. COM3): "

if not exist "firmware.bin" (
    echo [ERROR] firmware.bin not found in this directory.
    pause
    exit /b
)

echo.
echo [INFO] Flashing firmware to %PORT%...
python -m esptool --chip esp32 --port %PORT% --baud 921600 write_flash -z 0x1000 firmware.bin

if %errorlevel% equ 0 (
    echo.
    echo [SUCCESS] Firmware updated successfully!
) else (
    echo.
    echo [ERROR] Flashing failed. Check your connection and port.
)

pause
