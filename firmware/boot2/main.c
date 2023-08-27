#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/multicore.h"

#include "hardware/vreg.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

#include "pico_hal.h"
#include "wren.h"
#include "cbor.h"

void core1_main()
{
    while(1)
    {
        sleep(1000);
    }
}

int main(void)
{
    board_init();

    // init host stack on configured roothub port
    tuh_init(BOARD_TUH_RHPORT);

    if (pico_mount(true) != LFS_ERR_OK)
    {
        //WrenConfiguration config;
        //wrenInitConfiguration(&config);

    }

    while(1)
    {
        tud_task();
    }

	return 0;
}