// addon.cpp
// N-API 绑定：把 C++ 核心库暴露给 Electron/Node
#include <napi.h>
#include "jiyu_common.h"
#include "unlock.h"
#include "monitor.h"
#include "udp.h"
#include "knock.h"
#include "driver.h"
#include "inject.h"

namespace {

std::wstring ToWString(const Napi::String& s) {
    std::u16string u16 = s.Utf16Value();
    return std::wstring(u16.begin(), u16.end());
}

std::string ToString(const Napi::String& s) {
    return s.Utf8Value();
}

// unlockDCS(): 解锁键盘/鼠标，返回 0 成功
Napi::Value UnlockDCS(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::wstring dir;
    jiyu::FindStudentMainDir(dir);
    int r = jiyu::UnlockDCS(dir);
    return Napi::Number::New(env, r);
}

// injectToStudentMain(dllPath): 注入 DLL 到 StudentMain.exe
Napi::Value InjectToStudentMain(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "expected dllPath string").ThrowAsJavaScriptException();
        return env.Null();
    }
    std::wstring dllPath = ToWString(info[0].As<Napi::String>());
    DWORD pid = jiyu::FindProcessId(L"StudentMain.exe");
    if (pid == 0) pid = jiyu::FindProcessId(L"Student.exe");
    bool ok = jiyu::InjectDll(pid, dllPath);
    return Napi::Boolean::New(env, ok);
}

// sendShutdown(ip, port, delaySeconds, msg)
Napi::Value SendShutdown(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 4) {
        Napi::TypeError::New(env, "expected (ip, port, delaySeconds, msg)").ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string ip = ToString(info[0].As<Napi::String>());
    uint16_t port = (uint16_t)info[1].As<Napi::Number>().Uint32Value();
    uint32_t delay = info[2].As<Napi::Number>().Uint32Value();
    std::wstring msg = ToWString(info[3].As<Napi::String>());
    return Napi::Boolean::New(env, jiyu::SendShutdown(ip, port, delay, msg));
}

// sendReboot(ip, port, delaySeconds, msg)
Napi::Value SendReboot(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 4) {
        Napi::TypeError::New(env, "expected (ip, port, delaySeconds, msg)").ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string ip = ToString(info[0].As<Napi::String>());
    uint16_t port = (uint16_t)info[1].As<Napi::Number>().Uint32Value();
    uint32_t delay = info[2].As<Napi::Number>().Uint32Value();
    std::wstring msg = ToWString(info[3].As<Napi::String>());
    return Napi::Boolean::New(env, jiyu::SendReboot(ip, port, delay, msg));
}

// sendMessage(ip, port, text)
Napi::Value SendMessage(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 3) {
        Napi::TypeError::New(env, "expected (ip, port, text)").ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string ip = ToString(info[0].As<Napi::String>());
    uint16_t port = (uint16_t)info[1].As<Napi::Number>().Uint32Value();
    std::wstring text = ToWString(info[2].As<Napi::String>());
    return Napi::Boolean::New(env, jiyu::SendMessage(ip, port, text));
}

// sendExecCommand(ip, port, cmdLine)
Napi::Value SendExecCommand(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 3) {
        Napi::TypeError::New(env, "expected (ip, port, cmdLine)").ThrowAsJavaScriptException();
        return env.Null();
    }
    std::string ip = ToString(info[0].As<Napi::String>());
    uint16_t port = (uint16_t)info[1].As<Napi::Number>().Uint32Value();
    std::wstring cmdLine = ToWString(info[2].As<Napi::String>());
    return Napi::Boolean::New(env, jiyu::SendExecCommand(ip, port, cmdLine));
}

// readKnock1Password(): 返回明文密码字符串，失败返回空串
Napi::Value ReadKnock1Password(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::wstring pw;
    if (jiyu::ReadKnock1Password(pw)) {
        std::u16string u16(pw.begin(), pw.end());
        return Napi::String::New(env, u16);
    }
    return Napi::String::New(env, u"");
}

// unloadFileFilter(): 卸载 TDFileFilter
Napi::Value UnloadFileFilter(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), jiyu::UnloadFileFilter());
}

// unloadNetFilter(): 卸载 TDNetFilter
Napi::Value UnloadNetFilter(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), jiyu::UnloadNetFilter());
}

// findStudentMainDir(): 返回 StudentMain.exe 安装目录
Napi::Value FindStudentMainDir(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::wstring dir;
    if (jiyu::FindStudentMainDir(dir)) {
        std::u16string u16(dir.begin(), dir.end());
        return Napi::String::New(env, u16);
    }
    return Napi::String::New(env, u"");
}

} // namespace

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("unlockDCS", Napi::Function::New(env, UnlockDCS));
    exports.Set("injectToStudentMain", Napi::Function::New(env, InjectToStudentMain));
    exports.Set("sendShutdown", Napi::Function::New(env, SendShutdown));
    exports.Set("sendReboot", Napi::Function::New(env, SendReboot));
    exports.Set("sendMessage", Napi::Function::New(env, SendMessage));
    exports.Set("sendExecCommand", Napi::Function::New(env, SendExecCommand));
    exports.Set("readKnock1Password", Napi::Function::New(env, ReadKnock1Password));
    exports.Set("unloadFileFilter", Napi::Function::New(env, UnloadFileFilter));
    exports.Set("unloadNetFilter", Napi::Function::New(env, UnloadNetFilter));
    exports.Set("findStudentMainDir", Napi::Function::New(env, FindStudentMainDir));
    return exports;
}

NODE_API_MODULE(jiyu_native, Init)
