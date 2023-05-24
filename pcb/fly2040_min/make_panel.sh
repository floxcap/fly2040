#!/bin/bash
kikit panelize -p ../jlcpcb.kikit --layout 'rows: 2; cols: 1' fly2040_min.kicad_pcb panel_2_1.kicad_pcb
kikit fab jlcpcb --no-drc --assembly --schematic fly2040_min.kicad_sch panel_2_1.kicad_pcb jlcpcb_2_1
