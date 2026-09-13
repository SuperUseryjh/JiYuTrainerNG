// unlock.h
// DCS 键盘/鼠标解锁
#pragma once
#include <string>

namespace jiyu {

// 调用 DcsVKbd.dll!DCS_EnableKeys(0x8f) 解锁键盘/鼠标。
// 返回 0 表示成功，<0 表示失败（-1 加载 DLL 失败，-2 未找到导出函数）。
// 需要管理员权限。
int UnlockDCS(const std::wstring& dir);

} // namespace jiyu
