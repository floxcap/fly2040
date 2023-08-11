#!/bin/bash
#kikit panelize -p ../jlcpcb.kikit --layout 'rows: 50; cols: 1' --cuts none --tooling '3hole; hoffset: 2.5mm; voffset: 2.5mm; size: 1.152mm' --framing 'railstb' fly2040_leds.kicad_pcb panel_50_1.kicad_pcb
#kikit fab jlcpcb --no-drc --assembly --schematic fly2040_leds.kicad_sch panel_50_1.kicad_pcb jlcpcb_50_1
kikit panelize -p ../jlcpcb.kikit --layout 'rows: 10; cols: 1' --cuts none --tooling '3hole; hoffset: 2.5mm; voffset: 2.5mm; size: 1.152mm' --framing 'railstb' fly2040_leds.kicad_pcb panel_10_1.kicad_pcb
kikit fab jlcpcb --no-drc --assembly --schematic fly2040_leds.kicad_sch panel_10_1.kicad_pcb jlcpcb_10_1
