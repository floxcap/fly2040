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

#define SYSRGB_ERROR_MODULE 420
#define SYSRGB_ERROR(desc) ((SYSRGB_ERROR_MODULE & 0x1FF) | (SysRgbError_##desc & 0x1FFF)<<9)

typedef enum
{
    SysRgbError_Generic,
    SysRgbError_IpcData,
    SysRgbError_InvalidSize,
    SysRgbError_ConfigNotLoaded,
    SysRgbError_ConfigSaveFailed,
    SysRgbError_I2CNullService,
    SysRgbError_I2CSendFailed,
} SysRgbError;
