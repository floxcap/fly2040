# fly2040 pcbs
KiCad designs for a RP2040 board and supporting boards.

Note - The main board requires a 3.3V power supply when programming.<br>
The intention is to be able to program the boards with a 1.27mm pitch test clip / pogo pins.
(See: [usb_programmer](usb_programmer)).

The [fly2040_rgb](fly2040_rgb) board is the main board and has been tested and working
on V1/V2/Lite devices.

The [fly2040_leds](fly2040_leds) adds support for a flex led strip.

The [cpu_flex](cpu_flex) is untested.

## Installation
See: [guide](../doc/PicoFlyGuideV.6.2-1.pdf) or [guide-thread](https://gbatemp.net/download/a-definitive-picofly-install-guide.37968/)

The usb-programmer board is designed to accept the P50-E2 (shaft diameter 0.68mm) which should be soldered in
to the (programmer) board.
![Pogo pins](usb_programmer/p50_pogo.jpg)

