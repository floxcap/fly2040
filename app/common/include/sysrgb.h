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

#include "sysrgb/ipc.h"
#include "cfg_item.h"
#include "sysrgb/errors.h"
#include <sys/_stdint.h>

#define MAX_LOG 512

typedef struct
{
    uint8_t cpumon;
    uint8_t genmon;
    uint8_t config;
    uint8_t i2c;
    float cpumonValues[4];
    float cpumonAverage;
    float genmonValue;
#if MAX_LOG > 0
    uint32_t put;
    uint32_t get;
    char log[MAX_LOG];
#endif
} SysRgbContext;

#define SYSRGB_ENUM_VALID(n, v) ((v) < n##_EnumMax)
