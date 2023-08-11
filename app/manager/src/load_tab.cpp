
#include "load_tab.hpp"
#include "config_impl.hpp"

using namespace brls::literals; // for _i18n

LoadTab::LoadTab()
{
    this->inflateFromXMLRes("xml/tabs/load.xml");

    cpu->title->setText("main/load/cpu"_i18n);
    cpu->setOn(ConfigImpl::get().load_cpu, false);

    gpu->title->setText("main/load/gpu"_i18n);
    gpu->setOn(ConfigImpl::get().load_gpu, false);

    brightness->setProgress(ConfigImpl::get().load.brightness);
    brightness_lbl->setText(std::to_string((int)(brightness->getProgress() * 100)));
    brightness->getProgressEvent()->subscribe([this](float progress) {
        ConfigImpl::set([=](CfgSet& set){set.load.brightness = progress;});
        this->brightness_lbl->setText(std::to_string((int)(progress * 100)));
    });
}

brls::View* LoadTab::create()
{
    return new LoadTab();
}
