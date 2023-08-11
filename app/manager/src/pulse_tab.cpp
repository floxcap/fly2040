
#include "pulse_tab.hpp"
#include "config_impl.hpp"

PulseTab::PulseTab()
{
    this->inflateFromXMLRes("xml/tabs/pulse.xml");

    brightness->setProgress(ConfigImpl::get().pulse.brightness);
    brightness_lbl->setText(std::to_string((int)(brightness->getProgress() * 100)));
    brightness->getProgressEvent()->subscribe([this](float progress) {
        ConfigImpl::set([=](CfgSet& set){set.pulse.brightness = progress;});
        this->brightness_lbl->setText(std::to_string((int)(progress * 100)));
        this->hue_lbl->setBorderColor(nvgHSL(hue->getProgress(), 1.0f, brightness->getProgress()));
    });

    speed->setProgress(ConfigImpl::speedToProgress(ConfigImpl::get().pulse.speed));
    speed_lbl->setText(std::to_string((int)(ConfigImpl::progressToSpeed(speed->getProgress()))));
    speed->getProgressEvent()->subscribe([this](float progress) {
            ConfigImpl::set([=](CfgSet& set){set.pulse.speed = ConfigImpl::progressToSpeed(progress);});
            this->speed_lbl->setText(std::to_string((int)(ConfigImpl::progressToSpeed(progress))));
        });

    hue->setProgress(ConfigImpl::get().pulse.hue);
    hue_lbl->setText(std::to_string((int)(hue->getProgress() * 360)));
    hue_lbl->setBorderThickness(3);
    hue_lbl->setBorderColor(nvgHSL(hue->getProgress(), 1.0f, brightness->getProgress()));
    hue->getProgressEvent()->subscribe([this](float progress) {
            ConfigImpl::set([=](CfgSet& set){set.pulse.hue = progress;});
            this->hue_lbl->setText(std::to_string((int)(progress * 360)));
            this->hue_lbl->setBorderColor(nvgHSL(hue->getProgress(), 1.0f, brightness->getProgress()));
        });
}

brls::View* PulseTab::create()
{
    return new PulseTab();
}
