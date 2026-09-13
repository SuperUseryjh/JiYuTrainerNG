// knock.h
// Knock1 密码破解
#pragma once
#include <string>

namespace jiyu {

// 读取并解密极域卸载/解锁密码（Knock1）。
// 注册表：HKLM\SOFTWARE\TopDomain\e-Learning Class\Student\Knock1（REG_BINARY）
// 需要管理员权限。
// 返回 true 成功，out 填明文密码。
bool ReadKnock1Password(std::wstring& out);

} // namespace jiyu
