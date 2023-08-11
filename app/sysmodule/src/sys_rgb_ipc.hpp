#ifndef SYS_RGB_IPC_HPP
#define SYS_RGB_IPC_HPP

#include <atomic>
#include <stratosphere.hpp>

#include "sysrgb.h"
#include "config_impl.hpp"
#include "cpu_mon.hpp"
#include "gen_mon.hpp"
#include "sysrgb/ipc.h"

#define READ_BUFFER_SIZE 0x1000

enum I2CConfigState
{
    I2CConfigState_None,
    I2CConfigState_Init,
    I2CConfigState_Update,
    I2CConfigState_Refresh
};

enum I2CSendState
{
    I2CSendState_none,
    I2CSendState_num_pixels,
    I2CSendState_brightness,
    I2CSendState_scale,
    I2CSendState_speed,
    I2CSendState_hue,
    I2CSendState_mode,
    I2CSendState_done,
};

#define I_SYSRGB_INTERFACE_INFO(C, H)                                                                                                                           \
    AMS_SF_METHOD_INFO(C, H, SysRgbIpcCmd_GetApiVersion,        Result, GetApiVersion,      (ams::sf::Out<u32> version),                        (version))      \
    AMS_SF_METHOD_INFO(C, H, SysRgbIpcCmd_GetCurrentContext,    Result, GetCurrentContext,  (ams::sf::OutBuffer &buffer),                       (buffer))       \
    AMS_SF_METHOD_INFO(C, H, SysRgbIpcCmd_GetConfigValues,      Result, GetConfigValues,    (ams::sf::OutBuffer &buffer),                       (buffer))       \
    AMS_SF_METHOD_INFO(C, H, SysRgbIpcCmd_SetConfigValues,      Result, SetConfigValues,    (ams::sf::InBuffer &buffer),                        (buffer))       \
    AMS_SF_METHOD_INFO(C, H, SysRgbIpcCmd_SendI2c,              Result, SendI2c,            (ams::sf::InBuffer &buffer),                        (buffer))

AMS_SF_DEFINE_INTERFACE(ams::sys::rgb, ISysRgbIpc, I_SYSRGB_INTERFACE_INFO, 0xFEEDCAFE)

namespace ams::sys::rgb
{
    constexpr inline const sm::ServiceName SysRgbServiceName = sm::ServiceName::Encode("sys:rgb");
    constexpr inline const psc::PmModuleId SysRgbPmModuleId = psc::PmModuleId(0x420);

    void InitializeIpcServer();
    void LoopIpcServer();
    void FinalizeIpcServer();

    void HandleConfig(CfgSet* = nullptr);

    void HandleSleep(bool bSleep);

    Result SendI2C(uint8_t addr, const uint8_t* pData, size_t len, bool bLog = false);
    Result SendI2C(const uint8_t* pData, size_t len, bool bLog = false);

    Result SendI2C(I2cSession& session, uint8_t addr, const uint8_t* pData, size_t len, bool bLog = false);
    Result SendI2C(I2cSession& session, const uint8_t* pData, size_t len, bool bLog = false);

    class SysRgbIpc
    {
    public:
        Result GetApiVersion(ams::sf::Out<u32> version);
        Result GetVersionString(ams::sf::OutBuffer &buffer);
        Result GetCurrentContext(ams::sf::OutBuffer &buffer);
        Result GetConfigValues(ams::sf::OutBuffer &buffer);

        Result SetConfigValues(ams::sf::InBuffer &buffer);
        Result SendI2c(ams::sf::InBuffer &buffer);
    private:
    };
} // ams::sys::rgb

#endif //SYS_RGB_IPC_HPP
