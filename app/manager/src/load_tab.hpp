
#pragma once

#include <borealis.hpp>

class LoadTab : public brls::Box
{
  public:
    LoadTab();

    static brls::View* create();

  private:
    BRLS_BIND(brls::BooleanCell, cpu, "cpu");
    BRLS_BIND(brls::BooleanCell, gpu, "gpu");

    BRLS_BIND(brls::Slider, brightness, "brightness");
    BRLS_BIND(brls::Label, brightness_lbl, "brightness_lbl");
};
