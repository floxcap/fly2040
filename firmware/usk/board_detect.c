#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pins.h"
#include "ws2812.pio.h"
#include "misc.h"
#include "board_detect.h"

extern int ws_pio_offset;

enum board_type {
    BOARD_WS = 0,
    BOARD_XO,
    BOARD_IB,
    BOARD_PI,
    BOARD_SQ
};

enum board_type cur_board = BOARD_WS;
bool _jmp_lv = false;
bool _jmp_leds = false;

bool detect_by_pull_up(int frc_pin, int det_pin)
{
    bool result = false;
    if (frc_pin >= 0)
        gpio_init(frc_pin);
    gpio_init(det_pin);
    if (frc_pin >= 0)
        gpio_set_dir(frc_pin, true);
    gpio_pull_up(det_pin);
    sleep_us(15);
    result = !gpio_get(det_pin);
    gpio_deinit(det_pin);
    if (frc_pin >= 0)
        gpio_deinit(frc_pin);
    gpio_disable_pulls(det_pin);
    return result;
}

bool test_xiao()
{
    return detect_by_pull_up(1, 2);
}

bool test_itsy()
{
    return detect_by_pull_up(3, 2);
}

bool test_pico()
{
    return detect_by_pull_up(-1, 22);
}

bool test_ws()
{
    return detect_by_pull_up(-1, 25);
}

bool test_sqc()
{
    return detect_by_pull_up(-1, 17);
}

// Test low voltage (I2C) jumper.
// Default to true for boards without jumper.
bool test_jmp_lv()
{
    int pin = jmp_lv_pin();
    if (pin < 0)
        return true;

    return detect_by_pull_up(-1, pin);
}

// Test for I2C slave mode for RGB leds.
// Default to false for boards without jumper.
bool test_jmp_leds()
{
    int pin = jmp_leds_pin();
    if (pin < 0)
        return false;

    return detect_by_pull_up(-1, pin);
}

void detect_board()
{
    gpio_pull_down(PIN_GLI_WS);
    gpio_pull_down(PIN_GLI_PICO);
    gpio_pull_down(PIN_GLI_XIAO);
    gpio_pull_down(PIN_GLI_ITSY);
    gpio_disable_input_output(PIN_RST);
    if (test_ws()) {
        cur_board = BOARD_WS;
    } else if (test_xiao()) {
        cur_board = BOARD_XO;
    } else if (test_itsy()) {
        cur_board = BOARD_IB;
    } else if (test_pico()) {
        cur_board = BOARD_PI;
    } else if (test_sqc()) {
        cur_board = BOARD_SQ;
    } else {
        cur_board = BOARD_WS;
    }

    _jmp_lv = test_jmp_lv();
    _jmp_leds = test_jmp_leds();
}

int led_pin()
{
    switch(cur_board){
        case BOARD_XO:
            return PIN_LED_XIAO;
        case BOARD_PI:
            return PIN_LED_PICO;
        case BOARD_IB:
            return PIN_LED_ITSY;
        default:
            return PIN_LED_WS;
    }; 
}

int pwr_pin()
{
    switch(cur_board){
        case BOARD_XO:
            return PIN_LED_PWR_XIAO;
        case BOARD_IB:
            return PIN_LED_PWR_ITSY;
        default:
            return 31;
    }; 
}

int scl_pin()
{
    switch(cur_board){
        case BOARD_XO:
            return PIN_SCL_XIAO;
        case BOARD_IB:
            return PIN_SCL_ITSY;
        case BOARD_PI:
            return PIN_SCL_PICO;
        case BOARD_SQ:
            return PIN_SCL_SQC;
        default:
            return PIN_SCL_WS;
    }; 
}

int sda_pin()
{
    switch(cur_board){
        case BOARD_XO:
            return PIN_SDA_XIAO;
        case BOARD_IB:
            return PIN_SDA_ITSY;
        case BOARD_PI:
            return PIN_SDA_PICO;
        case BOARD_SQ:
            return PIN_SDA_SQC;
        default:
            return PIN_SDA_WS;
    }; 
}

int gli_pin()
{
    switch(cur_board){
        case BOARD_XO:
            return PIN_GLI_XIAO;
        case BOARD_IB:
            return PIN_GLI_ITSY;
        case BOARD_PI:
            return PIN_GLI_PICO;
        default:
            return PIN_GLI_WS;
    }; 
}

int jmp_lv_pin()
{
    switch(cur_board) {
        case BOARD_WS:
            return PIN_I2C_MODE_LV;

        case BOARD_XO:
        case BOARD_IB:
        case BOARD_PI:
        default:
            return -1;
    }
}

int jmp_leds_pin()
{
    switch(cur_board) {
        case BOARD_WS:
            return PIN_I2C_MODE_LEDS;

        case BOARD_XO:
        case BOARD_IB:
        case BOARD_PI:
        default:
            return -1;
    }
}

bool is_jmp_lv()
{
    return _jmp_lv;
}

bool is_jmp_leds()
{
    return _jmp_leds;
}

bool is_pico()
{
    return cur_board == BOARD_PI;
}
