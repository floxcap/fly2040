#!/bin/bash
kikit panelize -p jlcpcb.kikit ../fly2040/fly2040.kicad_pcb panel_4_3.kicad_pcb
kikit fab jlcpcb --no-drc --assembly --schematic ../fly2040/fly2040.kicad_sch panel_4_3.kicad_pcb jlcpcb_4_3
