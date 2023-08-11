#pragma once
#include <atomic>
#include <ctime>
#include <map>
#include <mutex>
#include <initializer_list>
#include <stratosphere.hpp>
#include <switch.h>
#include <sysrgb.h>
#include "config.hpp"

#define FILE_CFG_CHECK_INTERVAL_NS 5000000000ULL

class ConfigImpl : public Config
{
  public:
    ConfigImpl(ConfigImpl const&) = delete;

    ConfigImpl& operator=(ConfigImpl const&) = delete;

    virtual ~ConfigImpl();

    bool Refresh();

    void SetEnabled(bool enabled);
    bool Enabled();

    bool SaveConfig();

    void GetConfigValues(CfgSet* out_configValues);
    bool SetConfigValues(CfgSet* configValues, bool immediate);

    static CfgSet& get()
    {
        return get_instance()->Config::get();
    }

    static ConfigImpl* get_instance()
    {
        // Static local variable initialization is thread-safe
        // and will be initialized only once.
        static ConfigImpl instance{};
        return &instance;
    }

private:
    explicit ConfigImpl() : Config(), _enabled(false), _mutex(false), _last_check(0), _loaded(false), _mtime(0)
    {}

  protected:
    bool Load();
    void Close();
    bool Save();
    s64 CheckModificationTime();

    std::atomic_bool _enabled;
    ams::os::Mutex _mutex;
    uint64_t _last_check;
    bool _loaded;
    s64 _mtime;

};
