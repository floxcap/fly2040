#include "config_impl.hpp"
#include <algorithm>
#include <cstring>
#include "context.hpp"
#include "file_utils.h"
#include "json.hpp"

#define FILE_CONFIG_FILE_PATH FILE_CONFIG_DIR "/config.json"

ConfigImpl::~ConfigImpl()
{
    Close();
}

bool ConfigImpl::Load()
{
    Close();

    bool bFound = false;
    ams::fs::FileHandle cfgFile = {};
    if (R_SUCCEEDED(ams::fs::HasFile(&bFound, FILE_CONFIG_FILE_PATH)) && bFound)
    {
        if (R_SUCCEEDED(ams::fs::OpenFile(&cfgFile, FILE_CONFIG_FILE_PATH, ams::fs::OpenMode_Read)))
        {
            ON_SCOPE_EXIT
            { ams::fs::CloseFile(cfgFile); };

            s64 size = 0;
            if (Context::r_ok("cfg ld sz", ams::fs::GetFileSize(&size, cfgFile)) && size > 0 && size < CFG_BUF_MAX-1)
            {
                //FileUtils::LogLine("[cfg] Sz:%ld", size);

                if (Context::r_ok("cfg fs rd", ams::fs::ReadFile(cfgFile, 0, _buf, size)))
                {
                    //FileUtils::LogLine("[cfg] parse");

                    std::lock_guard lock(_mutex);
                    _buf[size] = 0;
                    get() = parse(_buf, size);
                    _loaded = true;
                    return true;
                }
                else
                {
                    FileUtils::LogLine("[cfg] Rd Err");
                }
            }
            else
            {
                FileUtils::LogLine("[cfg] Sz Err");
            }
        }
        else
        {
            FileUtils::LogLine("[cfg] Open Err");
        }
    }

    return false;
}

void ConfigImpl::Close()
{
    _loaded = false;
    get() = CfgSet();
}

bool ConfigImpl::Refresh()
{
    uint64_t now = armTicksToNs(armGetSystemTick());
    if ((now - _last_check) < FILE_CFG_CHECK_INTERVAL_NS)
    {
        return false;
    }

    _last_check = now;
    s64 mod_time = this->CheckModificationTime();
    if (!_loaded || _mtime != mod_time)
    {
        if (Load())
        {
            _mtime = mod_time;
            return true;
        }
    }
    return false;
}

s64 ConfigImpl::CheckModificationTime()
{
    std::lock_guard lock(_mutex);
    ams::fs::FileTimeStamp ts;
    if (R_SUCCEEDED(GetFileTimeStamp(&ts, FILE_CONFIG_FILE_PATH)))
    {
        return ts.modify.value;
    }
    return 0;
}

void ConfigImpl::SetEnabled(bool enabled)
{
    _enabled = enabled;
}

bool ConfigImpl::Enabled()
{
    return _enabled;
}

void ConfigImpl::GetConfigValues(CfgSet* out_configValues)
{
    *out_configValues = get();
}

bool ConfigImpl::SetConfigValues(CfgSet* configValues, bool immediate)
{
    get() = *configValues;
    return true;
}

bool ConfigImpl::SaveConfig()
{
    if (Save())
    {
        _mtime = CheckModificationTime();
        _loaded = true;
        return true;
    }

    return false;
}

bool ConfigImpl::Save()
{
    try
    {
        std::lock_guard lock(_mutex);
        ams::fs::FileHandle cfgFile;
        ams::fs::DeleteFile(FILE_CONFIG_FILE_PATH);
        ams::fs::CreateFile(FILE_CONFIG_FILE_PATH, 0);
        if (Context::r_ok("cfg sv", ams::fs::OpenFile(&cfgFile, FILE_CONFIG_FILE_PATH, ams::fs::OpenMode_All)))
        {
            ON_SCOPE_EXIT { ams::fs::CloseFile(cfgFile); };

            toStream(_stream, 4);
            if (Context::r_ok("cfg wr", ams::fs::WriteFile(cfgFile, 0, _buf, _stream.available(), ams::fs::WriteOption::Flush)))
            {
                return true;
            }
        }
    }
    catch (nlohmann::json::exception& exception)
    {
        FileUtils::LogLine("[cfg] json error");
    }

    return false;
}
