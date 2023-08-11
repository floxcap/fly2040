/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include "sysrgb/ipc.h"
//TODO: do we really need this? #define NX_SERVICE_ASSUME_NON_DOMAIN
// https://discordapp.com/channels/269333940928512010/383368936466546698/730608325258903632
#include <sysrgb/client/ipc.h>
#include <switch.h>
#include <atomic>

static Service g_sysrgbSrv;
static std::atomic<size_t> g_refCnt;

bool sysrgbIpcRunning()
{
    Handle handle;
    bool running = R_FAILED(smRegisterService(&handle, smEncodeName(SYSRGB_IPC_SERVICE_NAME), false, 1));

    if (!running)
    {
        smUnregisterService(smEncodeName(SYSRGB_IPC_SERVICE_NAME));
    }

  return running;
}

Result sysrgbIpcInitialize(void)
{
    Result rc = 0;

    g_refCnt++;

    if (serviceIsActive(&g_sysrgbSrv))
        return 0;

    rc = smGetService(&g_sysrgbSrv, SYSRGB_IPC_SERVICE_NAME);

    if (R_FAILED(rc)) sysrgbIpcExit();

    return rc;
}

void sysrgbIpcExit(void)
{
    if (--g_refCnt == 0)
    {
        serviceClose(&g_sysrgbSrv);
    }
}

Result sysrgbIpcGetAPIVersion(u32* out_ver)
{
    return serviceDispatchOut(&g_sysrgbSrv, SysRgbIpcCmd_GetApiVersion, *out_ver);
}

Result sysrgbIpcGetCurrentContext(SysRgbContext* out_context)
{
    return serviceDispatch(&g_sysrgbSrv, SysRgbIpcCmd_GetCurrentContext,
                           .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcMapAlias },
                           .buffers = {{out_context, sizeof(SysRgbContext) }},
    );
}

Result sysrgbIpcGetConfigValues(CfgSet* out)
{
    return serviceDispatch(&g_sysrgbSrv, SysRgbIpcCmd_GetConfigValues,
      .buffer_attrs = { SfBufferAttr_Out | SfBufferAttr_HipcMapAlias },
      .buffers = {{out, sizeof(CfgSet)}}
      );
}

Result sysrgbIpcSetConfigValues(CfgSet* in)
{
    return serviceDispatch(&g_sysrgbSrv, SysRgbIpcCmd_SetConfigValues,
                           .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcMapAlias },
                           .buffers = { { in, sizeof(CfgSet)}});
}

Result sysrgbIpcSendI2c(void* data, size_t len)
{
    return serviceDispatch(&g_sysrgbSrv, SysRgbIpcCmd_SendI2c,
                           .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcMapAlias },
                           .buffers = { { data, len } });
}
