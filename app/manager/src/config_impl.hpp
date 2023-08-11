#ifndef _CONFIG_IMPL_HPP
#define _CONFIG_IMPL_HPP

#include "config.hpp"
#include "../../common/include/sysrgb/client/ipc.h"

class ConfigImpl : public Config
{
public:
    ConfigImpl(ConfigImpl const&) = delete;

    ConfigImpl& operator=(ConfigImpl const&) = delete;

    ~ConfigImpl()
    {}

    static CfgSet& get()
    {
        return get_instance()->Config::get();
    }

    static void set(std::function<void(CfgSet&)> fp)
    {
        CfgSet config = get();
        fp(config);
        uint8_t* src = (uint8_t*) &config;
        uint8_t* dst = (uint8_t*) &get();
        bool modified = false;
        for (size_t i = 0; i < sizeof(CfgSet); i++)
        {
            modified = modified | (*src++ != *dst++);
            if (modified)
                break;
        }
        get() = config;

        if (modified)
        {
            get_instance()->setUpdated(true);
        }
    }

    static bool updated()
    {
        return get_instance()->_updated;
    }

    static float speedToProgress(float speed)
    {
        return (speed - 1000)/10000;
    }

    static float progressToSpeed(float progress)
    {
        return progress*10000 + 1000;
    }

private:
    static ConfigImpl* get_instance()
    {
        // Static local variable initialization is thread-safe
        // and will be initialized only once.
        static ConfigImpl instance{};
        return &instance;
    }

    void setUpdated(bool bUpdated)
    {
        if (bUpdated && !_updated)
        {
            _action = brls::Application::getActivitiesStack()[0]->registerAction(
                "Apply",
                brls::ControllerButton::BUTTON_Y,
                [&](brls::View* v)
                {
                    if (0 == (sysrgbIpcSetConfigValues(&get())))
                    {
                        _updated = false;
                        brls::Logger::debug("Apply success");
                        brls::Application::getActivitiesStack()[0]->unregisterAction(_action);
                        brls::Application::getActivitiesStack()[0]->getContentView()->invalidate();
                    }
                    else
                    {
                        brls::Logger::debug("Apply failed");
                        brls::Application::notify("Failed to set config.");
                    }

                    return true;
                });
            brls::Application::getActivitiesStack()[0]->getContentView()->invalidate();
        }
        _updated = bUpdated;
    }

    explicit ConfigImpl() : Config()
    {}

    bool _updated = false;
    brls::ActionIdentifier _action = ACTION_NONE;
};

#endif // _CONFIG_IMPL_HPP