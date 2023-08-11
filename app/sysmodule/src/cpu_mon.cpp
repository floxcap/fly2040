#include <cmath>
#include <algorithm>
#include <stratosphere.hpp>
#include "cpu_mon.hpp"
#include "file_utils.h"
#include "context.hpp"

#define SYSTEM_TICK_FREQUENCY 19200000.0f

// Libnx limits alignment & min size ->
// if (((uintptr_t)stack_mem & 0xFFF) || (stack_sz & 0xFFF))
// return MAKERESULT(Module_Libnx, LibnxError_BadInput);
#define THREAD_STACK    0x1000

namespace
{
    alignas(0x1000) u8 g_thread_stacks[NUM_CORES][THREAD_STACK];
}

ams::Result CpuMon::start()
{
    if (!_running)
    {
        _running = true;
        for (int i = 0; i < NUM_CORES; i++)
        {
            _ctx[i]._id = i;
            _ctx[i]._mon = this;
            Result rc = threadCreate(&_ctx[i]._thread, &CpuMon::_thread, &_ctx[i], g_thread_stacks[i], THREAD_STACK,
                                     ams::os::LowestSystemThreadPriority+1, i);
            if (Context::r_err("cpu thr", rc))
            {
                return rc;
            }
            rc = threadStart(&_ctx[i]._thread);
            if (Context::r_err("cpu st", rc))
            {
                return rc;
            }
        }
    }

    return ::ams::ResultSuccess();
}

void CpuMon::stop()
{
    if (_running)
    {
        _running = false;
        for (int i = 0; i < NUM_CORES; i++)
        {
            threadWaitForExit(&_ctx[i]._thread);
            threadClose(&_ctx[i]._thread);
        }
    }
}

bool CpuMon::running()
{
    return _running;
}

float CpuMon::get(int id)
{
    float result = 0.0f;

    if (id >= 0 && id < NUM_CORES)
    {
        result += (float)_ctx[id]._idle;
    }
    else
    {
        for (int i = 0; i < NUM_CORES; i++)
        {
            result += (float)_ctx[i]._idle;
        }
        result /= 4.0f;
    }

    return result;
}

float CpuMon::avg(int id)
{
    float result = 0.0f;

    if (id >= 0 && id < NUM_CORES)
    {
        result += (float)_ctx[id]._idleAvg;
    }
    else
    {
        for (int i = 0; i < NUM_CORES; i++)
        {
            result += (float)_ctx[i]._idleAvg;
        }
        result /= 4.0f;
    }

    return result;
}

void CpuMon::thread(CpuMonCtx& ctx)
{
    uint64_t diff_idletick;
    uint64_t diff_systick;

    while (_running)
    {
        ctx._sysA = armGetSystemTick();
        ctx._idleA = ams::os::GetIdleTickCount().GetInt64Value();
        svcSleepThread(1000000 * _refreshRate);
        ctx._sysB = armGetSystemTick();
        ctx._idleB = ams::os::GetIdleTickCount().GetInt64Value();
        diff_idletick = ctx._idleB - ctx._idleA;
        diff_systick  = ctx._sysB - ctx._sysA;
        ctx._idle = std::round(std::clamp((diff_idletick * 100 / diff_systick)/100.0f, 0.0f, 1.0f)*1000.0f)/1000.0f;
        ctx._idleAvg = std::round((ctx._idleAvg + ctx._idle)*500.0f)/1000.0f;
    }
}

void CpuMon::_thread(void* pctx)
{
    ((CpuMonCtx*)pctx)->_mon->thread(*((CpuMonCtx*)pctx));
}
