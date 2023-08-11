/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2020  natinusala
    Copyright (C) 2019  p-sam

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdlib.h>

#include <borealis.hpp>
#include <string>

#include "context.hpp"
#include "buffer.hpp"
#include "main_activity.hpp"
#include "settings_tab.hpp"
#include "pulse_tab.hpp"
#include "cycle_tab.hpp"
#include "rainbow_tab.hpp"
#include "load_tab.hpp"
#include "temperature_tab.hpp"
#include "about_tab.hpp"
#include "debug_tab.hpp"
#include "config_impl.hpp"
#include "file_utils.hpp"

using namespace brls::literals; // for _i18n

int main(int argc, char* argv[])
{
    // Init the app
    brls::Logger::setLogLevel(brls::LogLevel::LOG_DEBUG);

    FileUtils::LogLine("App Launch");

    uint32_t apiVersion;
    bool ipcEnabled = false;

    // Init the app and i18n
    if (!brls::Application::init())
    {
        brls::Logger::error("Unable to init application");
        return EXIT_FAILURE;
    }

    // Check that sys-rgb is running
    if (!sysrgbIpcRunning())
    {
        brls::Logger::error("sys-rgb is not running");
        brls::Application::notify("sys-rgb is not running");
        //brls::Application::crash("sys-rgb does not seem to be running, please check that it is correctly installed and enabled.");
    }
        // Initialize sys-rgb IPC client
    else if (R_FAILED(sysrgbIpcInitialize()) || R_FAILED(sysrgbIpcGetAPIVersion(&apiVersion)))
    {
        brls::Logger::error("Unable to initialize sys-rgb IPC client");
        brls::Application::notify("Unable to initialize sys-rgb IPC client");
        //brls::Application::crash("Could not connect to sys-rgb, please check that it is correctly installed and enabled.");
    }
    else if (SYSRGB_IPC_API_VERSION != apiVersion)
    {
        brls::Logger::error("sys-rgb IPC API version mismatch (expected: %u; actual: %u)", SYSRGB_IPC_API_VERSION,
                            apiVersion);
        brls::Application::notify("sys-rgb IPC API version mismatch");
        //brls::Application::crash("The manager is not compatible with the currently running sysmodule of sys-rgb, please check that you have correctly installed the latest version (reboot?).");
    }
    else
    {
        brls::Logger::debug("IPC connected.");
        ipcEnabled = true;
    }

    brls::Application::createWindow("main/title"_i18n);
    brls::Application::setGlobalQuit(true);

    if (ipcEnabled)
    {
        Result rc = sysrgbIpcGetConfigValues(&ConfigImpl::get());
        if (R_FAILED(rc))
        {
            brls::Logger::info("Get config failed:{}", rc);
        }
    }

    // Register custom views (including tabs, which are views)
    brls::Application::registerXMLView("SettingsTab", SettingsTab::create);
    brls::Application::registerXMLView("PulseTab", PulseTab::create);
    brls::Application::registerXMLView("CycleTab", CycleTab::create);
    brls::Application::registerXMLView("RainbowTab", RainbowTab::create);
    brls::Application::registerXMLView("LoadTab", LoadTab::create);
    brls::Application::registerXMLView("TemperatureTab", TemperatureTab::create);
    brls::Application::registerXMLView("AboutTab", AboutTab::create);
    brls::Application::registerXMLView("DebugTab", DebugTab::create);

    // Add custom values to the style
    brls::getStyle().addMetric("about/padding_top_bottom", 50);
    brls::getStyle().addMetric("about/padding_sides", 75);
    brls::getStyle().addMetric("about/description_margin", 50);

    // Create and push the main activity to the stack
    brls::Application::pushActivity(new MainActivity());

    // Run the app
    while (brls::Application::mainLoop());

    sysrgbIpcExit();

    // Exit
    return EXIT_SUCCESS;
}
