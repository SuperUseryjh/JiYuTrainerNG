// driver.h
// 极域内核驱动卸载
#pragma once

namespace jiyu {

// 卸载文件过滤驱动 TDFileFilter（解除 USB/文件访问限制）
bool UnloadFileFilter();

// 卸载网络过滤驱动 TDNetFilter（解除网络访问限制）
bool UnloadNetFilter();

} // namespace jiyu
