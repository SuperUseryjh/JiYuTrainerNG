@echo off
setlocal
REM ============================================================
REM 编译 32 位注入 DLL (jiyu_inject.dll)
REM
REM 前提：在 "x86 Native Tools Command Prompt for VS" 中运行本脚本
REM   （开始菜单 -> Visual Studio 20xx -> x86 Native Tools Command Prompt）
REM   或先执行：call "...\Microsoft Visual Studio\...\VC\Auxiliary\Build\vcvarsall.bat" x86
REM
REM 说明：注入 DLL 必须为 32 位，因为 StudentMain.exe 是 x86。
REM ============================================================

if not exist build mkdir build

cl /nologo /LD /MT /O2 /EHsc /utf-8 /DUNICODE /D_UNICODE ^
   inject_dll\dllmain.cpp ^
   native\monitor.cpp ^
   native\unlock.cpp ^
   native\jiyu_common.cpp ^
   /I native ^
   /link /OUT:build\jiyu_inject.dll ^
   advapi32.lib user32.lib

if errorlevel 1 (
    echo [ERROR] 编译失败，请确认在 x86 Native Tools 命令行中运行
    exit /b 1
)

echo.
echo [OK] 输出: build\jiyu_inject.dll (32位)
endlocal
