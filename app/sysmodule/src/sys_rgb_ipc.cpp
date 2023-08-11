
#include "sys_rgb_ipc.hpp"
#include "context.hpp"
#include "file_utils.h"
#include <span>

namespace ams::sys::rgb
{
    namespace
    {
        constexpr inline size_t SessionCountMax = 42;

        constexpr inline size_t NumServers = 1;

        struct ServerManagerOptions
        {
            static constexpr size_t PointerBufferSize = 0x400;
            static constexpr size_t MaxDomains = 31;
            static constexpr size_t MaxDomainObjects = 61;
            static constexpr bool CanDeferInvokeRequest = false;
            static constexpr bool CanManageMitmServers = false;
        };

        using ServerManager = sf::hipc::ServerManager<NumServers, ServerManagerOptions, SessionCountMax>;

        constinit util::TypedStorage<ServerManager> g_server_manager_storage = {};

        constinit ServerManager* g_server_manager = nullptr;

        constinit util::TypedStorage<psc::PmModule> g_pm_module_storage = {};

        constinit psc::PmModule* g_pm_module = nullptr;

        constinit os::MultiWaitHolderType g_pm_module_holder = {};
        constinit os::MultiWaitHolderType g_tmr_module_holder = {};

        constexpr const psc::PmModuleId PmModuleDependencies[] = {psc::PmModuleId_Fs, psc::PmModuleId_I2c};

        /* Service objects. */
        constinit sf::UnmanagedServiceObject<ams::sys::rgb::ISysRgbIpc, ams::sys::rgb::SysRgbIpc> g_service_object;

        ams::os::TimerEventType g_timer_event = {};
        I2CConfigState _cfgState = I2CConfigState_None;
        I2CSendState _sendState = I2CSendState_none;
        uint8_t _buf[MAX(sizeof(uint32_t)+1, 8)];
        bool _cfgChanged = false;
        bool _sleeping = false;
        RgbRegister _reg;
        CfgSet _curCfg;
        CpuMon _cpuMon;
        GenMon _genMon;
    }

    void InitializeIpcServer()
    {
        /* Check that we're not already initialized. */
        AMS_ABORT_UNLESS(g_server_manager == nullptr);
        AMS_ABORT_UNLESS(g_pm_module == nullptr);

        // Create the server manager.
        g_server_manager = util::ConstructAt(g_server_manager_storage);

        // Set some defaults.
        _reg.state.mode = RgbMode_Off;
        _reg.state.num_pixels = 9;
        _reg.state.speed = F2U32(5000);
        _reg.state.hue = F2U32(0.0f);
        _reg.state.brightness = F2U32(1.0f);
        _reg.state.scale = F2U32(1.0f);
        Context::get().i2c = true;

        // Create and initialize the psc module.
        g_pm_module = util::ConstructAt(g_pm_module_storage);
        R_ABORT_UNLESS(g_pm_module->Initialize(SysRgbPmModuleId, PmModuleDependencies, util::size(PmModuleDependencies),
                                               os::EventClearMode_ManualClear));

        // Create the psc module multi wait holder.
        os::InitializeMultiWaitHolder(std::addressof(g_pm_module_holder), g_pm_module->GetEventPointer()->GetBase());
        os::SetMultiWaitHolderUserData(std::addressof(g_pm_module_holder), SysRgbPmModuleId);

        // Add the pm module holder.
        g_server_manager->AddUserMultiWaitHolder(std::addressof(g_pm_module_holder));

        // Create a periodic timer event and add it to the multi wait list.
        os::InitializeTimerEvent(std::addressof(g_timer_event), os::EventClearMode::EventClearMode_ManualClear);
        os::InitializeMultiWaitHolder(std::addressof(g_tmr_module_holder), std::addressof(g_timer_event));
        os::SetMultiWaitHolderUserData(std::addressof(g_tmr_module_holder), SysRgbPmModuleId);
        g_server_manager->AddUserMultiWaitHolder(std::addressof(g_tmr_module_holder));

        // Create services.
        R_ABORT_UNLESS(g_server_manager->RegisterObjectForServer(g_service_object.GetShared(), SysRgbServiceName,
                                                                 SessionCountMax));

        // Start the server manager.
        g_server_manager->ResumeProcessing();
    }

    void LoopIpcServer()
    {
        /* Check that we're initialized. */
        AMS_ABORT_UNLESS(g_server_manager != nullptr);

        TimeSpan ts = TimeSpan::FromMilliSeconds(ConfigImpl::get().update_ms);
        os::StartPeriodicTimerEvent(std::addressof(g_timer_event), ts, ts);

        // Loop forever, servicing the server.
        while (true)
        {
            // Get the next signaled holder.
            auto* signaled_holder = g_server_manager->WaitSignaled();
            if (signaled_holder == std::addressof(g_pm_module_holder))
            {
                // If pm module, clear the event.
                g_pm_module->GetEventPointer()->Clear();

                // Get the power state.
                psc::PmState pm_state;
                psc::PmFlagSet pm_flags;
                if (R_SUCCEEDED(g_pm_module->GetRequest(std::addressof(pm_state), std::addressof(pm_flags))))
                {
                    switch (pm_state) {
                        case psc::PmState_FullAwake:
                        {
                            if (_sleeping)
                            {
                                HandleSleep(false);
                                ts = TimeSpan::FromMilliSeconds(ConfigImpl::get().update_ms);
                                os::StartPeriodicTimerEvent(std::addressof(g_timer_event), ts, ts);
                            }
                        }
                        break;

                        case psc::PmState_ShutdownReady:
                        case psc::PmState_SleepReady:
                        {
                            if (!_sleeping)
                            {
                                HandleSleep(true);
                                os::StopTimerEvent(std::addressof(g_timer_event));
                                os::ClearTimerEvent(std::addressof(g_timer_event));
                            }
                        }
                        break;
                        default:
                            break;
                    }

                    // Acknowledge the state transition.
                    R_ABORT_UNLESS(g_pm_module->Acknowledge(pm_state, ResultSuccess()));
                }
                g_server_manager->AddUserMultiWaitHolder(signaled_holder);
            }
            else if (signaled_holder == std::addressof(g_tmr_module_holder))
            {
                os::ClearTimerEvent(std::addressof(g_timer_event));
                g_server_manager->AddUserMultiWaitHolder(signaled_holder);
                if (_cfgChanged)
                {
                    ConfigImpl::get_instance()->SaveConfig();
                    _cfgChanged = false;
                }

                HandleConfig();
            }
            else
            {
                /* If ipc, process. */
                R_ABORT_UNLESS(g_server_manager->Process(signaled_holder));
            }
        }
    }

    void StopIpcServer()
    {
        /* Check that we're initialized. */
        AMS_ABORT_UNLESS(g_server_manager != nullptr);

        /* Stop the server manager. */
        g_server_manager->RequestStopProcessing();
    }

    void FinalizeIpcServer()
    {
        /* Check that we're initialized. */
        AMS_ABORT_UNLESS(g_server_manager != nullptr);

        /* Destroy the server manager. */
        std::destroy_at(g_server_manager);
        g_server_manager = nullptr;
    }

    Result SysRgbIpc::GetApiVersion(ams::sf::Out<u32> version)
    {
        version.SetValue(SYSRGB_IPC_API_VERSION);
        return ::ams::ResultSuccess();
    }

    Result SysRgbIpc::GetCurrentContext(ams::sf::OutBuffer& buffer)
    {
        if (buffer.GetSize() < sizeof(SysRgbContext))
            return SYSRGB_ERROR(InvalidSize);

        Context::get_mark();
        SysRgbContext& ctx = Context::get();

        ctx.cpumon = _cpuMon.running();
        ctx.genmon = _genMon.running();

        for (int i = 0; i < 4; i++)
        {
            ctx.cpumonValues[i] = _cpuMon.get(i);
        }
        ctx.cpumonAverage = _cpuMon.avg();
        ctx.genmonValue = _genMon.get();

        *reinterpret_cast<SysRgbContext*>(buffer.GetPointer()) = ctx;

        return ::ams::ResultSuccess();
    }

    Result SysRgbIpc::GetConfigValues(ams::sf::OutBuffer& buffer)
    {
        ConfigImpl::get_instance()->GetConfigValues(reinterpret_cast<CfgSet*>(buffer.GetPointer()));
        return ::ams::ResultSuccess();
    }

    Result SysRgbIpc::SetConfigValues(ams::sf::InBuffer& buffer)
    {
        ConfigImpl::get_instance()->GetConfigValues(&_curCfg);
        ConfigImpl::get_instance()->get() = *(reinterpret_cast<const CfgSet*>(buffer.GetPointer()));
        if (ConfigImpl::get_instance()->get() != _curCfg)
        {
            _cfgChanged = true;
        }
        HandleConfig(&_curCfg);
        return ::ams::ResultSuccess();
    }

    Result SysRgbIpc::SendI2c(ams::sf::InBuffer& buffer)
    {
        return SendI2C(reinterpret_cast<const uint8_t *>(buffer.GetPointer()), buffer.GetSize(), true);
    }

    void HandleSleep(bool bSleep)
    {
        if (bSleep && _sleeping != bSleep)
        {
            _cfgState = I2CConfigState_Init;
            _reg.state.mode = RgbMode_Off;
            _genMon.stop();
            _cpuMon.stop();
            SendI2C(RGB_OFFSET(mode), &_reg.mem[RGB_OFFSET(mode)], RGB_SIZEOF(mode));
        }
        _sleeping = bSleep;
    }

    void HandleConfig(CfgSet* oldCfg)
    {
        // Check file if we're not handling a config change.
        if (nullptr == oldCfg && ConfigImpl::get_instance()->Refresh())
        {
            Context::get().i2c = true;
            _cfgState = I2CConfigState_Init;
        }

        if (_cfgState == I2CConfigState_Init || nullptr != oldCfg)
        {
            if (nullptr != oldCfg)
            {
                if (oldCfg->mode != ConfigImpl::get().mode)
                {
                    // deactivate old mode
                    switch (oldCfg->mode)
                    {
                        case RgbMode_Off:
                        case RgbMode_Pulse:
                        case RgbMode_Cycle:
                        case RgbMode_Rainbow:
                            // Autonomous mode - nothing to stop.
                            break;

                        case RgbMode_Load:
                            _cpuMon.stop();
                            if (oldCfg->load_gpu && ConfigImpl::get().mode != RgbMode_Temperature)
                            {
                                _genMon.stop();
                            }
                            break;

                        case RgbMode_Temperature:
                            if (ConfigImpl::get().mode != RgbMode_Load && !ConfigImpl::get().load_gpu)
                            {
                                _genMon.stop();
                            }
                            break;
                    }
                }
                else
                {
                    if (oldCfg->load_gpu != ConfigImpl::get().load_gpu)
                    {
                        if (!ConfigImpl::get().load_gpu)
                        {
                            _genMon.stop();
                        }
                    }

                    if (oldCfg->load_cpu != ConfigImpl::get().load_cpu)
                    {
                        if (!ConfigImpl::get().load_cpu)
                        {
                            _cpuMon.stop();
                        }
                    }
                }
            }

            // Setup new mode.
            _reg.state.mode = ConfigImpl::get().mode;
            // Set num pixels.
            _reg.state.num_pixels = ConfigImpl::get().num_pixels;

            switch (ConfigImpl::get().mode)
            {
                case RgbMode_Off:
                    break;

                case RgbMode_Pulse:
                    _reg.state.hue = F2U32(ConfigImpl::get().pulse.hue);
                    _reg.state.speed = F2U32(ConfigImpl::get().pulse.speed);
                    _reg.state.brightness = F2U32(ConfigImpl::get().pulse.brightness);
                    break;

                case RgbMode_Cycle:
                    _reg.state.speed = F2U32(ConfigImpl::get().cycle.speed);
                    _reg.state.brightness = F2U32(ConfigImpl::get().cycle.brightness);
                    break;

                case RgbMode_Rainbow:
                    _reg.state.speed = F2U32(ConfigImpl::get().rainbow.speed);
                    _reg.state.brightness = F2U32(ConfigImpl::get().rainbow.brightness);
                    break;

                case RgbMode_Load:
                    _reg.state.brightness = F2U32(ConfigImpl::get().load.brightness);
                    if (ConfigImpl::get().load_gpu)
                    {
                        _genMon.setMode(GenMonMode_GPU);
                        _genMon.start();
                    }
                    if (ConfigImpl::get().load_cpu)
                    {
                        _cpuMon.start();
                    }
                    break;

                case RgbMode_Temperature:
                    _reg.state.brightness = F2U32(ConfigImpl::get().temperature.brightness);
                    //TODO: set other Temperatures?
                    _genMon.setMode(GenMonMode_SOCTemperature);
                    _genMon.start();
                    break;
            }

            // Wait for next cycle before setting new i2c values.
            _cfgState = I2CConfigState_Update;
            _sendState = I2CSendState_none;
            return;
        }

        // Don't apply changes if currently sleeping.
        if (_sleeping)
            return;

        switch (_cfgState)
        {
            case I2CConfigState_Refresh:
            {
                bool bChanged = false;

                // Send 'scale' value only (if required).
                switch (ConfigImpl::get().mode)
                {
                    case RgbMode_Off:
                    case RgbMode_Pulse:
                    case RgbMode_Cycle:
                    case RgbMode_Rainbow:
                        break;

                    case RgbMode_Load:
                    {
                        if (ConfigImpl::get().load_cpu && ConfigImpl::get().load_gpu)
                        {
                            float val = (_cpuMon.get() + _genMon.get()) / 2.0f;
                            if (val != _reg.state.scale)
                            {
                                _reg.state.scale = F2U32(val);
                                bChanged = true;
                            }
                        }
                        else if (ConfigImpl::get().load_gpu)
                        {
                            float val = _genMon.get();
                            if (val != _reg.state.scale)
                            {
                                _reg.state.scale = F2U32(val);
                                bChanged = true;
                            }
                        }
                        else if (ConfigImpl::get().load_cpu)
                        {
                            float val = _cpuMon.get();
                            if (val != _reg.state.scale)
                            {
                                _reg.state.scale = F2U32(val);
                                bChanged = true;
                            }
                        }
                        else
                        {
                            if (_reg.state.scale != 0)
                            {
                                _reg.state.scale = 0;
                                bChanged = true;
                            }
                        }
                    }
                    break;

                    case RgbMode_Temperature:
                    {
                        float val = _genMon.get();
                        if (val != _reg.state.scale)
                        {
                            _reg.state.scale = val;
                            bChanged = true;
                        }
                    }
                    break;
                }

                if (bChanged)
                {
                    SendI2C(RGB_OFFSET(scale), &_reg.mem[RGB_OFFSET(scale)], RGB_SIZEOF(scale));
                }
            }
            break;

            case I2CConfigState_Init:
            case I2CConfigState_Update:
            {
                I2cSession session;
                if (Context::r_ok("i2c sess", i2cOpenSession(&session, I2cDevice::I2cDevice_DebugPad)))
                {
                    ON_SCOPE_EXIT { i2csessionClose(&session); };

                    while (_sendState < I2CSendState_done)
                    {
                        // Send next I2C update part.
                        switch (_sendState)
                        {
                            case I2CSendState_none:
                                _sendState = I2CSendState_num_pixels;
                                break;

                            case I2CSendState_num_pixels:
                                _sendState = I2CSendState_brightness;
                                SendI2C(session, RGB_OFFSET(num_pixels), &_reg.mem[RGB_OFFSET(num_pixels)], RGB_SIZEOF(num_pixels), true);
                                break;

                            case I2CSendState_brightness:
                                _sendState = I2CSendState_scale;
                                SendI2C(session, RGB_OFFSET(brightness), &_reg.mem[RGB_OFFSET(brightness)], RGB_SIZEOF(brightness), true);
                                break;

                            case I2CSendState_scale:
                                _sendState = I2CSendState_speed;
                                SendI2C(session, RGB_OFFSET(scale), &_reg.mem[RGB_OFFSET(scale)], RGB_SIZEOF(scale), true);
                                break;

                            case I2CSendState_speed:
                                _sendState = I2CSendState_hue;
                                SendI2C(session, RGB_OFFSET(speed), &_reg.mem[RGB_OFFSET(speed)], RGB_SIZEOF(speed), true);
                                break;

                            case I2CSendState_hue:
                                _sendState = I2CSendState_mode;
                                SendI2C(session, RGB_OFFSET(hue), &_reg.mem[RGB_OFFSET(hue)], RGB_SIZEOF(hue), true);
                                break;

                            case I2CSendState_mode:
                                _sendState = I2CSendState_done;
                                _cfgState = I2CConfigState_Refresh;
                                SendI2C(session, RGB_OFFSET(mode), &_reg.mem[RGB_OFFSET(mode)], RGB_SIZEOF(mode), true);
                                break;

                            case I2CSendState_done:
                                _cfgState = I2CConfigState_Refresh;
                                break;
                        }
                    }
                }
            }
            break;

            case I2CConfigState_None:
            default:
            // Do nothing.
            break;
        }
    }

    Result SendI2C(uint8_t addr, const uint8_t* pData, size_t len, bool bLog)
    {
        I2cSession session;
        if (Context::r_ok("i2c sess", i2cOpenSession(&session, I2cDevice::I2cDevice_DebugPad)))
        {
            ON_SCOPE_EXIT { i2csessionClose(&session); };
            return SendI2C(session, addr, pData, len, bLog);
        }
        return 0;
    }

    Result SendI2C(const uint8_t* pData, size_t len, bool bLog)
    {
        I2cSession session;
        if (Context::r_ok("i2c sess", i2cOpenSession(&session, I2cDevice::I2cDevice_DebugPad)))
        {
            ON_SCOPE_EXIT { i2csessionClose(&session); };
            return SendI2C(session, pData, len, bLog);
        }
        return 0;
    }

    Result SendI2C(I2cSession& session, uint8_t addr, const uint8_t* pData, size_t len, bool bLog)
    {
        // Set address.
        _buf[0] = addr;
        // Set data.
        memcpy(&_buf[1], pData, len);
        return SendI2C(session, _buf, len+1, bLog);
    }

    Result SendI2C(I2cSession& session, const uint8_t* pData, size_t len, bool bLog)
    {
        if (bLog)
        {
            Context::logd("i2c", pData, len);
        }
        return i2csessionSendAuto(&session, pData, len, I2cTransactionOption::I2cTransactionOption_All);
    }

} // ams::sys::rgb