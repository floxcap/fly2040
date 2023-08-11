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

#include "debug_tab.hpp"
#include "context.hpp"
#include "buffer.hpp"
#include "common.hpp"
#include "file_utils.hpp"

bool i2c_send(std::vector<uint8_t> bytes)
{
    //return (0 == sysrgbIpcSendI2c(pData, len));
    std::ostringstream ss;
    for (uint8_t b : bytes)
    {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)b << " ";
    }
    brls::Logger::debug("i2c send:{}", ss.str());

    sysrgbIpcSendI2c(bytes.data(), bytes.size());
    return true;
}

std::vector<uint8_t> HexToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;

    for (unsigned int i = 0; i < hex.length(); i += 2) {
        while (' ' == hex.at(i) && i < hex.length()) {
            i++;
        }
        if (i < hex.length()) {
            std::string byteString = hex.substr(i, 2);
            uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
            bytes.push_back(byte);
        }
    }

    return bytes;
}

using namespace brls::literals; // for _i18n

DebugTab::DebugTab() : _timer()
{
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/tabs/debug.xml");

    input->init(
        "I2C (hex)", "00 ", [](std::string text) {
            std::vector<uint8_t> bytes = HexToBytes(text);
            if (bytes.size() > 1) {
                i2c_send(bytes);
            }
        },
        "00 ", "hex");

    refresh();
}

void DebugTab::willAppear(bool resetState)
{
    Box::willAppear(resetState);
    _timer.setPeriod(brls::Time(1000));
    _timer.setCallback([&]{
        refresh();
    });
    _timer.start();
}

void DebugTab::willDisappear(bool resetState)
{
    Box::willDisappear(resetState);
    _timer.stop();
}

void DebugTab::refresh()
{
    SysRgbContext* buf = reinterpret_cast<SysRgbContext*>(Buffer::reserve());
    Result rc = sysrgbIpcGetCurrentContext(buf);

    if (R_SUCCEEDED(rc))
    {
        Context::get() = *buf;
        Context::set_mark();

        //FileUtils::LogLine("ctx put:%u get:%u avail:%lu", Context::get().put, Context::get().get, Context::stream().available());

        std::ostringstream ss;
        ss << "Cpu: " << (Context::get().cpumon ? "true" : "false") << " avg val: " << roundp(Context::get().cpumonAverage, 2) << std::endl;
        for (int i=0; i<4; i++)
        {
            ss << "Cpu(" << i << ") " << " val: " << Context::get().cpumonValues[i] << std::endl;
        }
        ss << "Gen: " << (Context::get().genmon ? "true" : "false") << " val: " << Context::get().genmonValue << std::endl;
        ss << "I2C: " << (Context::get().i2c ? "true" : "false") << std::endl;

        ss << Context::stream().str();
        status->setText(ss.str());
    }
    else
    {
        brls::Application::notify("Failed to get status.");
        status->setText("Failed to get status.");
    }
    Buffer::release();
}

brls::View* DebugTab::create()
{
    return new DebugTab();
}
