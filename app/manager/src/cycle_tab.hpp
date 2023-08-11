
#pragma once

#include <borealis.hpp>

class CycleTab : public brls::Box
{
  public:
    CycleTab();

    static brls::View* create();

  private:
    BRLS_BIND(brls::Slider, brightness, "brightness");
    BRLS_BIND(brls::Slider, speed, "speed");

    BRLS_BIND(brls::Label, brightness_lbl, "brightness_lbl");
    BRLS_BIND(brls::Label, speed_lbl, "speed_lbl");
};
