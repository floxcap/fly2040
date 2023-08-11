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

#include <stdint.h>
#include "sysrgb.h"

#define SYSRGB_IPC_API_VERSION 0
#define SYSRGB_IPC_SERVICE_NAME "sys:rgb"
#define SYSRGB_IPC(id) (SysRgbIpcCmd_##id)

enum SysRgbIpcCmd
{
    SysRgbIpcCmd_GetApiVersion = 0,
    SysRgbIpcCmd_GetCurrentContext,
    SysRgbIpcCmd_GetConfigValues,
    SysRgbIpcCmd_SetConfigValues,
    SysRgbIpcCmd_SendI2c,
};

