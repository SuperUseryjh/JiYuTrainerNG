// inject.cpp
// DLL 注入实现（CreateRemoteThread + LoadLibraryW）
#include "inject.h"

namespace jiyu {

bool InjectDll(DWORD pid, const std::wstring& dllPath) {
    if (pid == 0 || dllPath.empty()) return false;

    HANDLE hProc = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE, pid);
    if (!hProc) return false;

    size_t dllSize = (dllPath.size() + 1) * sizeof(wchar_t);
    void* remoteMem = VirtualAllocEx(hProc, NULL, dllSize,
                                     MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteMem) {
        CloseHandle(hProc);
        return false;
    }

    bool ok = false;
    if (WriteProcessMemory(hProc, remoteMem, dllPath.c_str(), dllSize, NULL)) {
        // kernel32.dll 已加载到目标进程，且 32/64 位同架构下 LoadLibraryW 地址一致
        HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
        LPTHREAD_START_ROUTINE loadLib =
            (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");

        if (loadLib) {
            HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, loadLib, remoteMem, 0, NULL);
            if (hThread) {
                WaitForSingleObject(hThread, INFINITE);
                CloseHandle(hThread);
                ok = true;
            }
        }
    }

    VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProc);
    return ok;
}

} // namespace jiyu
