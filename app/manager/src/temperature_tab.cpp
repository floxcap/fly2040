
#include "temperature_tab.hpp"
#include "config_impl.hpp"

TemperatureTab::TemperatureTab()
{
    this->inflateFromXMLRes("xml/tabs/temperature.xml");

    brightness->setProgress(ConfigImpl::get().temperature.brightness);
    brightness_lbl->setText(std::to_string((int)(brightness->getProgress() * 100)));
    brightness->getProgressEvent()->subscribe([this](float progress) {
        ConfigImpl::set([=](CfgSet& set){set.temperature.brightness = progress;});
        this->brightness_lbl->setText(std::to_string((int)(progress * 100)));
    });
}

brls::View* TemperatureTab::create()
{
    return new TemperatureTab();
}
