#ifndef USK_LEDS_H
#define USK_LEDS_H

#include "../../app/common/include/constants.h"
/*
enum RgbMode
{
    RgbMode_Off,
    RgbMode_Pulse,
    RgbMode_Cycle,
    RgbMode_Rainbow,
    RgbMode_CpuLoad,
    RgbMode_GpuLoad,
    RgbMode_Temperature
};

// Register mapping for i2c device.
typedef struct __attribute__((__packed__)) {
    float hue;
    float brightness;
    float scale;
    float speed;
    uint8_t mode;
} RgbState;

typedef union
{
    RgbState state;
    uint8_t mem[256];
} RgbRegister;
*/

#define PIX_blu 0x00003F
#define PIX_yel 0x151500
#define PIX_whi 0x111111

#define PIX_b 0x00000F

#define PIX_off 0x000000

#ifndef MAX_PIXELS
#define MAX_PIXELS 50
#endif
void init_pixels();
bool enable_pixels();
void disable_pixels(bool);
void put_pixel(uint32_t pixel_grb);

uint32_t HSVtoRGB(float fH, float fS, float fV);

void leds_mode();

#endif //USK_LEDS_H
