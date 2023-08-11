
#include <switch.h>
#include <vapours.hpp>
#include <stratosphere.hpp>

#include "errors.h"
#include "file_utils.h"
#include "sys_rgb_ipc.hpp"
#include "context.hpp"

extern "C"
{
    // We are a sysmodule, so don't use more FS sessions than needed.
    u32 __nx_fs_num_sessions = 1;
    // We don't need to reserve memory for fsdev, so don't use it.
    u32 __nx_fsdev_direntry_cache_size = 1;
}

namespace ams {

    namespace result
    {
        //bool CallFatalOnResultAssertion = true;
    }

    namespace
    {
        alignas(os::MemoryPageSize) constinit u8 g_heap_memory[os::MemoryPageSize*18];//128_KB];//[
    } // namespace

    namespace init {
        void InitializeSystemModule()
        {
            init::InitializeAllocator(g_heap_memory, sizeof(g_heap_memory));
            R_ABORT_UNLESS(sm::Initialize());
            if (R_SUCCEEDED(setsysInitialize()))
            {
                SetSysFirmwareVersion fw;
                if (R_SUCCEEDED(setsysGetFirmwareVersion(&fw)))
                    hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
                setsysExit();
            }

            Context::r_ok("i2c init", i2cInitialize());

            fs::InitializeForSystem();
            fs::SetEnabledAutoAbort(false);
            R_ABORT_UNLESS(::pscmInitialize());
            Context::r_ok("sd", fs::MountSdCard("sdmc"));
        }

        void FinalizeSystemModule()
        {
        }

        void Startup() {
            os::SetThreadNamePointer(os::GetCurrentThread(), "sys.rgb.Main");
        }
    }

    void Exit(int rc)
    {
        AMS_UNUSED(rc);
    }

    void Main() {
        try
        {
            FileUtils::Initialize();
            FileUtils::LogLine("=== " TARGET " " TARGET_VERSION " ===");
            ams::sys::rgb::InitializeIpcServer();
            ams::sys::rgb::LoopIpcServer();
            ams::sys::rgb::FinalizeIpcServer();
        }
        catch (const std::exception &ex)
        {
        }
        catch (...)
        {
            //std::exception_ptr p = std::current_exception();
            //FileUtils::LogLine("[!?] %s", p ? p.__cxa_exception_type()->name() : "...");
        }

        FileUtils::LogLine("Exit");
        svcSleepThread(1000000ULL);
        FileUtils::Exit();
    }
}
