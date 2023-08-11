/*
    Copyright 2021 natinusala

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "settings_tab.hpp"
#include "config_impl.hpp"
#include "memlog.hpp"

#ifndef MAX_PIXELS
#define MAX_PIXELS 50
#endif

using namespace brls::literals; // for _i18n

SettingsTab::SettingsTab()
{
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/tabs/settings.xml");

    mode->init("main/mode/title"_i18n, {
                   "main/mode/off"_i18n, "main/mode/pulse"_i18n, "main/mode/cycle"_i18n, "main/mode/rainbow"_i18n, "main/mode/load"_i18n,
                   "main/mode/temp"_i18n
               },
               ConfigImpl::get().mode,
               [&](int selected)
               {
                   switch (selected)
                   {
                       default:
                       case 0:
                           ConfigImpl::set([](CfgSet& set){set.mode = RgbMode::RgbMode_Off;});
                           break;
                       case 1:
                           ConfigImpl::set([](CfgSet& set){set.mode = RgbMode::RgbMode_Pulse;});
                           break;
                       case 2:
                           ConfigImpl::set([](CfgSet& set){set.mode = RgbMode::RgbMode_Cycle;});
                           break;
                       case 3:
                           ConfigImpl::set([](CfgSet& set){set.mode = RgbMode::RgbMode_Rainbow;});
                           break;
                       case 4:
                           ConfigImpl::set([](CfgSet& set){set.mode = RgbMode::RgbMode_Load;});
                           break;
                       case 5:
                           ConfigImpl::set([](CfgSet& set){set.mode = RgbMode::RgbMode_Temperature;});
                           break;
                   }
               });

    int selected = 0;
    switch (ConfigImpl::get().update_ms)
    {
        default:
        case 100:
            selected = 0;
            break;

        case 250:
            selected = 1;
            break;

        case 500:
            selected = 2;
            break;

        case 1000:
            selected = 3;
            break;
    }
    update->init("main/update/title"_i18n,
                 {"main/update/s100"_i18n, "main/update/s250"_i18n, "main/update/s500"_i18n, "main/update/s1000"_i18n},
                 selected,
                 [](int selected)
                 {
                     switch (selected)
                     {
                         case 0:
                             ConfigImpl::set([](CfgSet& set){set.update_ms = 200;});
                             break;
                         case 1:
                             ConfigImpl::set([](CfgSet& set){set.update_ms = 250;});
                             break;
                         case 2:
                             ConfigImpl::set([](CfgSet& set){set.update_ms = 500;});
                             break;
                         case 3:
                             ConfigImpl::set([](CfgSet& set){set.update_ms = 1000;});
                             break;
                     }
                 });

    pixels->setProgress((float)ConfigImpl::get().num_pixels/MAX_PIXELS);
    pixels_lbl->setText(std::to_string((int) (pixels->getProgress() * MAX_PIXELS)));
    pixels->getProgressEvent()->subscribe(
        [this](float progress)
        {
            ConfigImpl::set([=](CfgSet& set){set.num_pixels = (int) (progress * MAX_PIXELS);});
            this->pixels_lbl->setText(std::to_string((int) (progress * MAX_PIXELS)));
        });
}

brls::View* SettingsTab::create()
{
    return new SettingsTab();
}
