##Firmware for thermostat boards on AVR mcu's##
Software for simple bang bang temperature digital controler. Supports numeric 7-segment LED display with multiplexing, 2 buttons, 2 temparature measure points (if needed) buzzer and rotary encoder. Look into other repositories to build board. 

Navigation is achieved via two buttons: start/stop button and change mode button. Pressing start button starts the timer and temperature control, pressing it again stops timer and turns off controlled element. Pressing mode button changes the displaed value between: current temperature > desired temperature > timer seconds (when idling) or current temperature > desired temperature > remaining seconds (when running). By rotating the encoder displayed parameter (desired temparature or timer seconds) can be adjusted. When time is out, buzzer is turned on for 5 seconds.

Configurable values, ports and pins are stored in definitions.h file. By default it turns on heater element (ex. ovens), for cooling (ex. freezer or other chiller), changes are needed in `temp_reg_bangbang` function in firmware.c file. 
For thermistor table calculation I used [this](http://aterlux.ru/article/ntcresistor-en#__h2_6), use it as well if you want to connect different thermistor. Table in repository source code works with EPCOS 100k B:3950 NTC thermistor (popular among reprap 3d printers).

Edit makefile for different AVR microcontroller or different programmer (for chip flashing) and its port. Default mcu is ATmega8 and avrisp programmer.

- **make** - builds source into flashable binary.
- **make fuse** - sets fuse bits on chip (atmega8) to use crystal as external clock signal.
- **make flash** - flashes chip