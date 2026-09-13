// jiyu_common.h
// 公共头文件：类型定义与工具函数声明
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>

namespace jiyu {

// 从极域学生端进程定位安装目录
// 返回值：成功 true，out 填安装目录（不含尾随反斜杠）
bool FindStudentMainDir(std::wstring& out);

// 按名字加载 DLL（先系统目录，再极域目录）
HMODULE LoadDll(const wchar_t* name, const std::wstring& dir);

// 遍历进程，找到指定进程名的 PID（返回 0 表示未找到）
DWORD FindProcessId(const wchar_t* exeName);

// 卸载驱动服务（停止 + 删除）
bool UnloadDriverService(const wchar_t* serviceName);

} // namespace jiyu
