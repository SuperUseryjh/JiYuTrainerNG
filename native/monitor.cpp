// monitor.cpp
// 反监视实现：DispSetNonCapturer
//
// 逆向自 LibDispCap20.dll：
//   DispSetNonCapturer() 将捕获相关函数指针全部切换为 stub，
//   并置全局标志，从而关闭整个进程的屏幕捕获。
#include "monitor.h"
#include "jiyu_common.h"

namespace jiyu {

typedef int (WINAPI *DispSetNonCapturer_t)();

int SetNonCapturer(const std::wstring& dir) {
    HMODULE hDisp = LoadDll(L"LibDispCap20.dll", dir);
    if (!hDisp) return -1;

    DispSetNonCapturer_t pSet = (DispSetNonCapturer_t)GetProcAddress(hDisp, "DispSetNonCapturer");
    if (!pSet) {
        FreeLibrary(hDisp);
        return -2;
    }

    int r = pSet();
    FreeLibrary(hDisp);
    return r;
}

} // namespace jiyu
