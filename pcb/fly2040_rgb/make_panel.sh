#!/bin/bash
kikit panelize -p ../jlcpcb.kikit --layout 'rows: 3; cols: 3' --tooling '3hole; hoffset: 2.5mm; voffset: 2.5mm; size: 1.152mm' fly2040_rgb.kicad_pcb panel_3_3.kicad_pcb
kikit fab jlcpcb --no-drc --assembly --schematic fly2040_rgb.kicad_sch panel_3_3.kicad_pcb jlcpcb_3_3
