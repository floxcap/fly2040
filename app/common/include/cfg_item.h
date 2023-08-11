#ifndef _CFG_ITEM_H
#define _CFG_ITEM_H

#include "json.hpp"
#include "constants.h"

NLOHMANN_JSON_SERIALIZE_ENUM( RgbMode, {
    {RgbMode_Off, "off"},
    {RgbMode_Pulse, "pulse"},
    {RgbMode_Cycle, "cycle"},
    {RgbMode_Rainbow, "rainbow"},
    {RgbMode_Load, "load"},
    {RgbMode_Temperature, "temperature"},
})

struct CfgItem
{
    float brightness = 1.0f;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CfgItem, brightness)
};

struct CfgItemSpd : CfgItem
{
    float speed = 5000.0f;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CfgItemSpd, brightness, speed)
};

struct CfgItemHue : CfgItemSpd
{
    float hue = 0.5f;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CfgItemHue, hue, brightness, speed)
};

struct CfgSet
{
    RgbMode mode = RgbMode::RgbMode_Off;
    bool load_cpu = true;
    bool load_gpu = false;
    uint update_ms = 200;
    uint num_pixels = 9;
    CfgItem load;
    CfgItem temperature;
    CfgItemSpd cycle;
    CfgItemSpd rainbow;
    CfgItemHue pulse;

    bool operator!=(const CfgSet& other)
    {
        return (0 != memcmp(this, &other, sizeof(CfgSet)));
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(CfgSet, mode, load_cpu, load_gpu, update_ms, num_pixels, load, temperature, cycle, rainbow, pulse)
};

#endif //_CFG_ITEM_H
