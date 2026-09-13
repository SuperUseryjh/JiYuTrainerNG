// inject.h
// DLL 注入
#pragma once
#include <string>

namespace jiyu {

// 通过 CreateRemoteThread + LoadLibraryW 将 DLL 注入目标进程。
// 返回 true 表示注入线程成功创建（不保证 DLL 加载逻辑成功）。
bool InjectDll(DWORD pid, const std::wstring& dllPath);

} // namespace jiyu
