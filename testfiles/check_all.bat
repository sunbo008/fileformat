@echo off
chcp 65001 >nul 2>&1

echo ========================================
echo    File Format Detection - Batch Test
echo ========================================
echo.

set "TEST_EXE=..\single\test_single.exe"

if not exist "%TEST_EXE%" (
    echo Error: test_single.exe not found
    echo Please compile first:
    echo   cd ..\single
    echo   cl /EHsc /std:c++17 /O2 test_single.cpp /Fe:test_single.exe
    pause
    exit /b 1
)

echo Results:
echo ----------------------------------------

for %%f in (*.*) do (
    if /i not "%%~nxf"=="check_all.bat" (
        if /i not "%%~nxf"=="create_test_files.cpp" (
            "%TEST_EXE%" "%%f"
        )
    )
)

echo ----------------------------------------
echo Done!
pause
