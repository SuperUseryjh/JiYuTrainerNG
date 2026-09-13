// jiyu_common.cpp
// 工具函数实现
#include "jiyu_common.h"
#include <tlhelp32.h>

namespace jiyu {

bool FindStudentMainDir(std::wstring& out) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    bool found = false;

    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, L"StudentMain.exe") == 0 ||
                _wcsicmp(pe.szExeFile, L"Student.exe") == 0) {
                HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pe.th32ProcessID);
                if (h) {
                    wchar_t full[MAX_PATH] = { 0 };
                    DWORD len = MAX_PATH;
                    if (QueryFullProcessImageNameW(h, 0, full, &len)) {
                        wchar_t* p = wcsrchr(full, L'\\');
                        if (p) *p = 0;  // 去掉文件名，只留目录
                        out = full;
                        found = true;
                    }
                    CloseHandle(h);
                }
                break;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return found;
}

HMODULE LoadDll(const wchar_t* name, const std::wstring& dir) {
    HMODULE h = LoadLibraryW(name);
    if (h) return h;
    if (!dir.empty()) {
        std::wstring full = dir + L"\\" + name;
        h = LoadLibraryW(full.c_str());
    }
    return h;
}

DWORD FindProcessId(const wchar_t* exeName) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    DWORD pid = 0;

    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, exeName) == 0) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return pid;
}

bool UnloadDriverService(const wchar_t* serviceName) {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!scm) return false;

    bool result = false;
    SC_HANDLE svc = OpenServiceW(scm, serviceName, SERVICE_ALL_ACCESS);
    if (svc) {
        SERVICE_STATUS status;
        // 先尝试停止服务
        ControlService(svc, SERVICE_CONTROL_STOP, &status);
        // 再删除服务（即使停止失败也尝试删除）
        result = DeleteService(svc) ? true : false;
        CloseServiceHandle(svc);
    } else {
        // 服务不存在，视为已卸载
        result = true;
    }

    CloseServiceHandle(scm);
    return result;
}

} // namespace jiyu
