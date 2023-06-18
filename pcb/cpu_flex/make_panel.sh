#!/bin/bash
kikit panelize -p ../jlcpcb.kikit --layout 'rows: 3; cols: 3' fly2040_min.kicad_pcb panel_3_3.kicad_pcb
kikit fab jlcpcb --no-drc --assembly --schematic fly2040_min.kicad_sch panel_3_3.kicad_pcb jlcpcb_3_3
