@echo off
setlocal
REM ============================================================
REM 编译 native addon (jiyu_native.node)，供 Electron 调用
REM
REM 前提：已安装 Node.js、npm、Visual Studio（含 C++ 桌面开发负载）
REM 说明：native addon 必须针对 Electron 的 ABI 编译，不能直接用系统 Node 编译。
REM ============================================================

REM 1. 安装依赖（node-addon-api、electron、node-gyp）
call npm install
if errorlevel 1 (
    echo [ERROR] npm install 失败
    exit /b 1
)

REM 2. 针对 Electron ABI 重新编译 native addon
call npx electron-rebuild -f -w jiyu_native
if errorlevel 1 (
    echo [ERROR] electron-rebuild 失败
    exit /b 1
)

echo.
echo [OK] 输出: build\Release\jiyu_native.node
endlocal
