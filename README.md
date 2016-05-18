# ledTimer
* Small battery powered device to try nanoWatt Technology from Microchip.
* By pressing of tactile switch we put LED on for ~30min.

Two different ways to wake up from sleep mode are used: by extINT and WDT.

## HW/SW
- PIC12F683
- MPLAB® X IDE v3.30 + MPLAB® XC8 Compiler v1.37

### Pin connections
- GP0 pin7 NC
- GP1 pin6 NC
- GP2 pin5 tactile switch to the Vss
- GP3 pin4 Vdd
- GP4 pin3 LED + resistor to the Vdd
- GP5 pin2 LED + resistor to the Vdd# ledTimer

# Enjoy !