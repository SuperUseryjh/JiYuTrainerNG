// knock.cpp
// Knock1 密码破解实现
//
// 算法逆向自 JiYuTrainer UnDecryptJiyuKnock + 极域 unins000.exe @0x13c9：
//   1. 整个数据每 4 字节 XOR 0x50434C45 ("ELCP")
//   2. 再每 4 字节 XOR 0x454C4350 ("PCLE")
//      （两步等价于单次 XOR 0x15030F15）
//   3. 解密后 buf[0] 为密码字符串偏移，从 buf[buf[0]] 读取 UTF-16 明文
#include "knock.h"
#include <windows.h>

namespace jiyu {

namespace {

// 解密 Knock1 数据，out 填明文密码
bool DecryptKnock(BYTE* data, DWORD size, std::wstring& out) {
    DWORD n = size / 4;
    // 第 1 轮：XOR 0x50434C45 ("ELCP")
    for (DWORD i = 0; i < n; i++) {
        *(DWORD*)(data + i * 4) ^= 0x50434C45u;
    }
    // 第 2 轮：XOR 0x454C4350 ("PCLE")
    for (DWORD i = 0; i < n; i++) {
        *(DWORD*)(data + i * 4) ^= 0x454C4350u;
    }

    // buf[0] 是密码字符串的字节偏移
    BYTE off = data[0];
    if (off >= size) return false;

    const wchar_t* pw = (const wchar_t*)(data + off);
    out.assign(pw, 32);  // 最多 32 个字符
    size_t nul = out.find(L'\0');
    if (nul != std::wstring::npos) out.resize(nul);

    return !out.empty();
}

} // namespace

bool ReadKnock1Password(std::wstring& out) {
    HKEY hKey = NULL;
    // 先尝试 64 位视图，再回退 32 位视图
    LONG r = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Wow6432Node\\TopDomain\\e-Learning Class\\Student",
        0, KEY_READ, &hKey);
    if (r != ERROR_SUCCESS) {
        r = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\TopDomain\\e-Learning Class\\Student",
            0, KEY_READ, &hKey);
    }
    if (r != ERROR_SUCCESS) return false;

    BYTE data[120] = { 0 };
    DWORD type = REG_BINARY;
    DWORD size = sizeof(data);
    r = RegQueryValueExW(hKey, L"Knock1", NULL, &type, data, &size);
    RegCloseKey(hKey);
    if (r != ERROR_SUCCESS) return false;

    return DecryptKnock(data, size, out);
}

} // namespace jiyu
