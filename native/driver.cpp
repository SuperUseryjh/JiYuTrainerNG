// driver.cpp
// 极域内核驱动卸载实现
#include "driver.h"
#include "jiyu_common.h"

namespace jiyu {

bool UnloadFileFilter() {
    return UnloadDriverService(L"TDFileFilter");
}

bool UnloadNetFilter() {
    return UnloadDriverService(L"TDNetFilter");
}

} // namespace jiyu
