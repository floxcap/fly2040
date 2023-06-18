# fly2040
Basic RP2040 board.

Note - most of these boards require a 3.3V power supply when programming.
A simple programming PCB is being designed and will be released shortly.
The intention is to be able to program the boards with a 1.27mm pitch test clip / pogo pins.

The v1 fly2040 board has been tested and is working except for the led/5V boost circuit - don't use.
The v2 fly2040 board is work in progress - don't use.
The fly2040_min board is going through assembly and testing now - no issues expected.
The fly2040_ds is not yet tested and going through updates.

There are currently 3 boards.

The fly2040 is an experimental board with line isolation and USB isolation.
The intention here is to be able to disconnect the lines when not in use.

The fly2040_min board is an attempt at making a single sided board as small
as possible.

The fly2040_ds is a tiny double sided board. The double sided board is tiny!! 
15mm x 12mm (0.59in x 0.47in)

Panels provided for fabrication.
![Double sided PCB](pcb/fly2040_ds/fly2040_ds_pcb.jpg)

The mini board is a little bigger but cheaper to get assembled.
![Mini board 3D Back](pcb/fly2040_min/fly2040_min_back.jpg)
![Mini board 3D Front](pcb/fly2040_min/fly2040_min_front.jpg)
![Mini board PCB](pcb/fly2040_min/fly2040_min_pcb.jpg)
