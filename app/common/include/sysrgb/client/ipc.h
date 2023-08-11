/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#pragma once

#include "../../types.h"
#include "../../constants.h"
#include "../../cfg_item.h"
#include "../ipc.h"

#ifdef __cplusplus
extern "C" {
#endif

bool sysrgbIpcRunning();
Result sysrgbIpcInitialize(void);
void sysrgbIpcExit(void);

Result sysrgbIpcGetAPIVersion(u32* out_ver);
Result sysrgbIpcGetCurrentContext(SysRgbContext* out_context);
Result sysrgbIpcSetEnabled(bool enabled);
Result sysrgbIpcExitCmd();
Result sysrgbIpcGetConfigValues(CfgSet* out);
Result sysrgbIpcSetConfigValues(CfgSet* in);
Result sysrgbIpcSendI2c(void* data, size_t len);

#ifdef __cplusplus
}
#endif