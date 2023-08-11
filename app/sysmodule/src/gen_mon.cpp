//
// Created by adam on 13/07/23.
//

#include <algorithm>
#include "gen_mon.hpp"
#include "file_utils.h"
#include "context.hpp"

#define THREAD_STACK    0x1000
#define NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD 0x80044715
namespace
{
    alignas(0x1000) u8 gen_thread_stack[THREAD_STACK];
    static Service g_monSrv;
}

extern "C"
{
Result fnvOpen(u32 *fd, const char *devicepath) {
    struct {
        u32 fd;
        u32 error;
    } out;

    Result rc = serviceDispatchOut(&g_monSrv, 0, out,
        .buffer_attrs = { SfBufferAttr_In | SfBufferAttr_HipcMapAlias },
    .buffers = { { devicepath, strlen(devicepath) } },
    );

    if (R_SUCCEEDED(rc))
        rc = nvConvertError(out.error);

    if (R_SUCCEEDED(rc) && fd)
        *fd = out.fd;

    return rc;
}

Result fnvIoctl(u32 fd, u32 request, void* argp) {
    size_t bufsize = _NV_IOC_SIZE(request);
    u32 dir = _NV_IOC_DIR(request);

    void *buf_send = NULL, *buf_recv = NULL;
    size_t buf_send_size = 0, buf_recv_size = 0;

    if (dir & _NV_IOC_WRITE) {
        buf_send = argp;
        buf_send_size = bufsize;
    }

    if (dir & _NV_IOC_READ) {
        buf_recv = argp;
        buf_recv_size = bufsize;
    }

    const struct {
        u32 fd;
        u32 request;
    } in = { fd, request };

    u32 error = 0;
    Result rc = serviceDispatchInOut(&g_monSrv, 1, in, error,
        .buffer_attrs = {
        SfBufferAttr_HipcAutoSelect | SfBufferAttr_In,
        SfBufferAttr_HipcAutoSelect | SfBufferAttr_Out,
    },
    .buffers = {
        { buf_send, buf_send_size },
        { buf_recv, buf_recv_size },
    },
    );

    if (R_SUCCEEDED(rc))
        rc = nvConvertError(error);

    return rc;
}
}

void GenMon::setMode(GenMonMode mode)
{
    _mode = mode;
}

ams::Result GenMon::start()
{
    Context::logs("gmon");

    if (!_hosGt14)
    {
        _hosGt14 = hosversionAtLeast(14, 0, 0);
    }

    //Result rc = smGetService(&g_monSrv, "nvdrv:t");


    /*
    Context::logs("gmon nv");
    if (!_nvCheck)
    {
        Result rc = nvInitialize();
        FileUtils::LogIfError("nvInitialize", rc);
        if (Context::r_ok(rc))
        {
            rc = nvOpen(&_nvh, "/dev/nvhost-ctrl-gpu");
            FileUtils::LogIfError("nvOpen", rc);
            _nvCheck = Context::r_ok(rc);
        }
    }
    */

    if (!_tsCheck)
    {
        _tsCheck = Context::r_ok("tsinit", tsInitialize());
    }

    if (!_tcCheck)
    {
        if (hosversionAtLeast(5, 0, 0))
            _tcCheck = Context::r_ok("tcinit", tcInitialize());
    }

    if (!_running)
    {
        _running = true;
        Result rc = threadCreate(&_threadh, &GenMon::_thread, this, gen_thread_stack, THREAD_STACK, ams::os::LowestSystemThreadPriority+1, -2);
        if (Context::r_err("gen thr", rc))
        {
            return rc;
        }
        rc = threadStart(&_threadh);
        if (Context::r_err("gen st", rc))
        {
            return rc;
        }
    }

    return ::ams::ResultSuccess();
}

void GenMon::stop()
{
    if (_running)
    {
        _running = false;
        threadWaitForExit(&_threadh);
        threadClose(&_threadh);
        if (_nvCheck)
        {
            nvClose(_nvh);
        }
    }
}

bool GenMon::running()
{
    return _running;
}

void GenMon::thread()
{
    while (_running)
    {
        switch (_mode)
        {
            case GenMonMode_GPU:
                if (_nvCheck)
                {
                    nvIoctl(_nvh, NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD, &_GPULoad);
                }
                break;

            case GenMonMode_SOCTemperature:
                if (_tsCheck)
                {
                    if (_hosGt14)
                    {
                        tsGetTemperature(TsLocation_External, &_temperature);
                    }
                    else
                    {
                        tsGetTemperatureMilliC(TsLocation_External, &_temperature);
                    }
                }
                break;

            case GenMonMode_PCBTemperature:
                if (_tsCheck)
                {
                    if (_hosGt14)
                    {
                        tsGetTemperature(TsLocation_Internal, &_temperature);
                    }
                    else
                    {
                        tsGetTemperatureMilliC(TsLocation_Internal, &_temperature);
                    }
                }
                break;

            case GenMonMode_SKINTemperature:
                if (_tcCheck)
                {
                    tcGetSkinTemperatureMilliC(&_temperature);
                }
                break;
        }
        svcSleepThread(1000000 * _refreshRate);
    }
}

float GenMon::get()
{
    switch (_mode)
    {
        case GenMonMode_GPU:
            return std::clamp((float)_GPULoad/1000.0f, 0.0f, 1.0f);

        case GenMonMode_SOCTemperature:
        case GenMonMode_PCBTemperature:
        case GenMonMode_SKINTemperature:
            // Display range 21C - 60C -> 0.0 - 1.0
            return std::clamp(((((float)_temperature/100000.0f - 0.21f)) / (0.6f - 0.21f)) + 0.21f, 0.0f, 1.0f);
    }

    return 0.0f;
}

void GenMon::_thread(void* pthis)
{
    ((GenMon*)pthis)->thread();
}
