// unlock.cpp
// DCS 键盘/鼠标解锁实现
//
// 逆向自 StudentMain.exe：
//   锁: LoadLibraryW("DcsVKbd.dll") -> DCS_DisableKeys(0x8f)
//   解: LoadLibraryW("DcsVKbd.dll") -> DCS_EnableKeys(0x8f)
//   DCS_EnableKeys / DCS_DisableKeys 都带参数 0x8f (143)
#include "unlock.h"
#include "jiyu_common.h"

namespace jiyu {

// DCS 导出函数签名（__stdcall，返回 0 表示成功）
typedef int (WINAPI *DCS_Keys_t)(int);

// 锁和解锁共用的参数
static const int DCS_KEY_PARAM = 0x8f;

int UnlockDCS(const std::wstring& dir) {
    // BlockInput 与极域无关，顺手解除以防万一
    BlockInput(FALSE);

    HMODULE hVKbd = LoadDll(L"DcsVKbd.dll", dir);
    if (!hVKbd) return -1;

    DCS_Keys_t pEnable = (DCS_Keys_t)GetProcAddress(hVKbd, "DCS_EnableKeys");
    if (!pEnable) {
        FreeLibrary(hVKbd);
        return -2;
    }

    // 直接解锁键盘/鼠标，不终止任何进程
    int r = pEnable(DCS_KEY_PARAM);

    FreeLibrary(hVKbd);
    return r;
}

} // namespace jiyu
