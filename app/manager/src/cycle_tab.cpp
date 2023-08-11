
#include "cycle_tab.hpp"
#include "config_impl.hpp"

CycleTab::CycleTab()
{
    this->inflateFromXMLRes("xml/tabs/cycle.xml");

    brightness->setProgress(ConfigImpl::get().cycle.brightness);
    brightness_lbl->setText(std::to_string((int)(brightness->getProgress() * 100)));
    brightness->getProgressEvent()->subscribe([this](float progress) {
        ConfigImpl::set([=](CfgSet& set){set.cycle.brightness = progress;});
        this->brightness_lbl->setText(std::to_string((int)(progress * 100)));
    });

    speed->setProgress(ConfigImpl::speedToProgress(ConfigImpl::get().cycle.speed));
    speed_lbl->setText(std::to_string((int)(ConfigImpl::progressToSpeed(speed->getProgress()))));
    speed->getProgressEvent()->subscribe([this](float progress) {
            ConfigImpl::set([=](CfgSet& set){set.cycle.speed = ConfigImpl::progressToSpeed(progress);});
            this->speed_lbl->setText(std::to_string((int)(ConfigImpl::progressToSpeed(progress))));
        });
}

brls::View* CycleTab::create()
{
    return new CycleTab();
}
