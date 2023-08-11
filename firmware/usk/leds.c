#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/vreg.h"
#include "hardware/i2c.h"
#include "hardware/sync.h"
#include "pico/i2c_slave.h"
#include <string.h>
#include <math.h>
#include "leds.h"
#include "pins.h"
#include "ws2812.pio.h"
#include "board_detect.h"
#include "misc.h"
#include "config.h"

extern int ws_pio_offset;

// The slave implements a 256 byte memory. To write a series of bytes, the master first
// writes the memory address, followed by the data. The address is automatically incremented
// for each byte transferred, looping back to 0 upon reaching the end. Reading is done
// sequentially from the current memory address.
static struct
{
    volatile RgbRegister reg;

    volatile uint8_t mem_address;
    volatile bool mem_address_written;
    volatile bool update_ready;
    volatile bool write;
} i2c_context;

RgbState _state;
RgbState _i2c_state;

uint32_t pixel_buf[MAX_PIXELS];
bool leds_enabled = false;
bool led_pwr_enabled = false;
spin_lock_t* _lock;
int _lock_nr;

void init_pixels()
{
    memset(&i2c_context, 0, sizeof(i2c_context));
    i2c_context.mem_address_written = false;
    i2c_context.write = false;
    i2c_context.reg.state.num_pixels = 9;
    i2c_context.reg.state.speed = F2U32(5000);
    i2c_context.reg.state.hue = F2U32(0.0f);
    i2c_context.reg.state.brightness = F2U32(1.0f);
    i2c_context.reg.state.scale = F2U32(0.0f);
    if (is_i2c_configured())
    {
        // Once initial i2c received default to off.
        i2c_context.reg.state.mode = RgbMode_Off;
    }
    else
    {
        // Unconfigured - default to cycle.
        i2c_context.reg.state.mode = RgbMode_Cycle;
    }
    _i2c_state = i2c_context.reg.state;
    _state = i2c_context.reg.state;
    _lock_nr = spin_lock_claim_unused(true);
    _lock = spin_lock_init(_lock_nr);
}

bool enable_pixels()
{
    if (!leds_enabled)
    {
        leds_enabled = true;
        ws2812_program_init(pio0, 3, ws_pio_offset, led_pin(), 800000, false);
        if (!led_pwr_enabled && pwr_pin() != 31)
        {
            led_pwr_enabled = true;
            gpio_init(pwr_pin());
            gpio_set_drive_strength(pwr_pin(), GPIO_DRIVE_STRENGTH_12MA);
            gpio_set_dir(pwr_pin(), true);
            gpio_put(pwr_pin(), 1);
            sleep_us(200);
        }
        return true;
    }
    return false;
}

void disable_pixels(bool wasDisabled)
{
    if (leds_enabled)
    {
        leds_enabled = false;
        sleep_us(wasDisabled?50:200);
        pio_sm_set_enabled(pio0, 3, false);
        gpio_init(led_pin());
    }
}

uint32_t HSVtoRGB(float fH, float fS, float fV)
{
    double      p, q, t, ff;
    long        i;

    if(fS <= 0.0)
    {
        return ((uint32_t)(fV * 255) << 16) | ((uint32_t)(fV * 255) << 8) | (uint32_t)(fV * 255);
    }
    if(fH >= 360.0) fH = 0.0;
    fH /= 60.0;
    i = (long)fH;
    ff = fH - i;
    p = fV * (1.0 - fS);
    q = fV * (1.0 - (fS * ff));
    t = fV * (1.0 - (fS * (1.0 - ff)));

    switch(i) {
        case 0:
            return ((uint32_t)(fV * 255) << 16) | ((uint32_t)(t * 255) << 8) | (uint32_t)(p * 255);
        case 1:
            return ((uint32_t)(q * 255) << 16) | ((uint32_t)(fV * 255) << 8) | (uint32_t)(p * 255);
        case 2:
            return ((uint32_t)(p * 255) << 16) | ((uint32_t)(fV * 255) << 8) | (uint32_t)(t * 255);
        case 3:
            return ((uint32_t)(p * 255) << 16) | ((uint32_t)(q * 255) << 8) | (uint32_t)(fV * 255);
        case 4:
            return ((uint32_t)(t * 255) << 16) | ((uint32_t)(p * 255) << 8) | (uint32_t)(fV * 255);
        case 5:
        default:
            return ((uint32_t)(fV * 255) << 16) | ((uint32_t)(p * 255) << 8) | (uint32_t)(q * 255);
    }
}

void put_pixel(uint32_t pixel_grb)
{
    static bool led_enabled = false;
    if (is_pico())
    {
        gpio_init(led_pin());
        if (pixel_grb) {
            gpio_set_dir(led_pin(), true);
            gpio_put(led_pin(), 1);
        }
        return;
    }
    ws2812_program_init(pio0, 3, ws_pio_offset, led_pin(), 800000, true);
    if (!led_enabled && pwr_pin() != 31)
    {
        led_enabled = true;
        gpio_init(pwr_pin());
        gpio_set_drive_strength(pwr_pin(), GPIO_DRIVE_STRENGTH_12MA);
        gpio_set_dir(pwr_pin(), true);
        gpio_put(pwr_pin(), 1);
        sleep_us(200);
    }
    pio_sm_put_blocking(pio0, 3, pixel_grb << 8u);
    sleep_us(50);
    pio_sm_set_enabled(pio0, 3, false);
    gpio_init(led_pin());
}

void set_pixels(uint32_t* pixel_grb, uint len)
{
    for (uint i = 0; i < len; ++i) {
        pixel_buf[i] = (pixel_grb[i] << 8u);
    }
}

void set_pixel(uint32_t pixel_grb, int index)
{
    if (index < 0) {
        for (uint i = 0; i < _state.num_pixels; ++i) {
            pixel_buf[i] = (pixel_grb << 8u);
        }
    } else if (index < _state.num_pixels) {
        pixel_buf[index] = (pixel_grb << 8u);
    }
}

void update_pixels()
{
    for (uint i = 0; i < _state.num_pixels; ++i) {
        pio_sm_put_blocking(pio0, 3, pixel_buf[i]);
    }
}


// Our handler is called from the I2C ISR, so it must complete quickly. Blocking calls
// printing to stdio may interfere with interrupt handling.
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    switch (event) {
        case I2C_SLAVE_RECEIVE: // master has written some data
            if (!i2c_context.mem_address_written) {
                // writes always start with the memory address
                i2c_context.mem_address = i2c_read_byte_raw(i2c);
                i2c_context.mem_address_written = true;
            } else {
                // save into memory
                i2c_context.reg.mem[i2c_context.mem_address] = i2c_read_byte_raw(i2c);
                i2c_context.mem_address++;
                i2c_context.write = true;
            }
            break;
        case I2C_SLAVE_REQUEST: // master is requesting data
            // load from memory
            i2c_write_byte_raw(i2c, i2c_context.reg.mem[i2c_context.mem_address]);
            i2c_context.mem_address++;
            i2c_context.write = false;
            break;
        case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
            i2c_context.mem_address_written = false;
            if (i2c_context.write)
            {
                uint32_t saved_irq = spin_lock_blocking(_lock);
                _i2c_state = i2c_context.reg.state;
                i2c_context.update_ready = true;
                spin_unlock(_lock, saved_irq);
            }
            break;
        default:
            break;
    }
}

static void setup_slave()
{
    gpio_init(sda_pin());
    gpio_set_function(sda_pin(), GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin());

    gpio_init(scl_pin());
    gpio_set_function(scl_pin(), GPIO_FUNC_I2C);
    gpio_pull_up(scl_pin());

    i2c_init(i2c0, 400000);//I2C_BAUDRATE);
    // configure I2C0 for slave mode
    i2c_slave_init(i2c0, I2C_SLAVE_ADDRESS, &i2c_slave_handler);
}

void leds_mode()
{
    setup_slave();

    enable_pixels();

    while (1)
    {
        uint32_t ms = to_ms_since_boot(get_absolute_time());
        bool state_updated = false;

        if (i2c_context.update_ready)
        {
            uint32_t saved_irq = spin_lock_blocking(_lock);
            i2c_context.update_ready = false;
            _state = _i2c_state;
            spin_unlock(_lock, saved_irq);

            state_updated = true;
        }

        if (state_updated)
        {
            // Scale from 0.0 - 1.0
            _state.scale = CLAMP(_state.scale, 0, U32F_SCALE);
            // Brightness from 0.0 - 1.0
            _state.brightness = CLAMP(_state.brightness, 0, U32F_SCALE);
            // Hue from 0.0 - 1.0
            _state.hue = CLAMP(_state.hue, 0, U32F_SCALE);
            // Speed from 1 sec - 1 hour
            _state.speed = CLAMP(_state.speed, 1000, U32F_SCALE*60*60);
            // Limit max leds
            _state.num_pixels = MIN(MAX_PIXELS, _state.num_pixels);
            // Save i2c configured flag if not already set.
            set_i2c_configured();
        }

        switch (_state.mode)
        {
            case RgbMode_Off:
                set_pixel(PIX_off, -1);
                break;

            case RgbMode_Pulse:
                {
                    float s = U322F(_state.speed);
                    float p = fabs(sin((s/2.0f - (ms % (uint32_t)s))/s)*2.0f);
                    uint32_t c = HSVtoRGB(U322F(_state.hue) * 360, 1, p);
                    set_pixel(c, -1);
                }
                break;

            default:
            case RgbMode_Cycle:
                {
                    float p = (ms % (uint32_t)U322F(_state.speed))/U322F(_state.speed);
                    uint32_t c = HSVtoRGB(p * 360, 1, U322F(_state.brightness));
                    set_pixel(c, -1);
                }
                break;

            case RgbMode_Rainbow:
                {
                    float p = (ms % (uint32_t)U322F(_state.speed))/U322F(_state.speed);
                    uint step = 360/_state.num_pixels;
                    for (uint i=0; i<_state.num_pixels; i++)
                    {
                        pixel_buf[i] = (HSVtoRGB((int)(p*360+step*i) % 360, 1, U322F(_state.brightness)) << 8u);
                    }
                }
                break;

            case RgbMode_Load:
            case RgbMode_Temperature:
                {
                    uint32_t c = HSVtoRGB(U322F(_state.scale) * 360, 1, U322F(_state.brightness));
                    set_pixel(c, -1);
                }
                break;
        }

        update_pixels();

        sleep_ms(50);
    }
}
