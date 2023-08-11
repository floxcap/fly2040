
#include "rainbow_tab.hpp"
#include "config_impl.hpp"

RainbowTab::RainbowTab()
{
    this->inflateFromXMLRes("xml/tabs/rainbow.xml");

    brightness->setProgress(ConfigImpl::get().rainbow.brightness);
    brightness_lbl->setText(std::to_string((int)(brightness->getProgress() * 100)));
    brightness->getProgressEvent()->subscribe([this](float progress) {
        ConfigImpl::set([=](CfgSet& set){set.rainbow.brightness = progress;});
        this->brightness_lbl->setText(std::to_string((int)(progress * 100)));
    });

    speed->setProgress(ConfigImpl::speedToProgress(ConfigImpl::get().rainbow.speed));
    speed_lbl->setText(std::to_string((int)(ConfigImpl::progressToSpeed(speed->getProgress()))));
    speed->getProgressEvent()->subscribe([this](float progress) {
            ConfigImpl::set([=](CfgSet& set){set.rainbow.speed = ConfigImpl::progressToSpeed(progress);});
            this->speed_lbl->setText(std::to_string((int)(ConfigImpl::progressToSpeed(progress))));
        });
}

brls::View* RainbowTab::create()
{
    return new RainbowTab();
}
