
#pragma once

#include <borealis.hpp>

class TemperatureTab : public brls::Box
{
  public:
    TemperatureTab();

    static brls::View* create();

  private:
    BRLS_BIND(brls::Slider, brightness, "brightness");
    BRLS_BIND(brls::Label, brightness_lbl, "brightness_lbl");
};
