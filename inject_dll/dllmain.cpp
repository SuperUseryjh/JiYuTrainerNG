// dllmain.cpp
// 注入到 StudentMain.exe 的 DLL：在进程内关闭屏幕捕获 + 解锁输入。
//
// 编译成 32 位 DLL（StudentMain.exe 为 x86）。
#include <windows.h>
#include "../native/monitor.h"
#include "../native/unlock.h"
#include "../native/jiyu_common.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        std::wstring dir;
        jiyu::FindStudentMainDir(dir);

        // 反监视：把 LibDispCap20.dll 捕获函数指针切为 stub，关闭屏幕捕获
        jiyu::SetNonCapturer(dir);

        // 解锁键盘/鼠标（DCS_EnableKeys，作为兜底）
        jiyu::UnlockDCS(dir);
    }
    return TRUE;
}
