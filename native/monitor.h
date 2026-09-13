// monitor.h
// 反监视：关闭屏幕捕获
#pragma once
#include <string>

namespace jiyu {

// 在当前进程内调用 LibDispCap20.dll!DispSetNonCapturer() 关闭屏幕捕获。
// 该接口操作 LibDispCap20.dll 的进程内全局函数指针表，因此
// 必须在 StudentMain.exe 进程内调用才有效（通常配合注入 DLL 使用）。
// 返回 1 表示成功，<0 表示失败。
int SetNonCapturer(const std::wstring& dir);

} // namespace jiyu
