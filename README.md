# MCP6S91-serial-programmable-gain-amplifier
This project is compatible with MCP6S91/2/3 but to switch between channel CH1 and CH2 you must create some new commands. Chaining multiple ic is not implemented yet. The program provides a simple plug and play device with an interactive serial user interface to set the gain of the PGA. The code is compiled for an ATMEGA328P and is compatible with an Arduino UNO developing board. To set the gain you only need to type the numerical value in the serial terminal/monitor; the interpreter expects all commands to be terminated with the new line character '\n'.

### Valid gain commands  entry are:
* "0" No gain, PGA off
* "1" Set a gain of +1
* "2" Set a gain of +2
* "4" Set a gain of +4
* "5" Set a gain of +5
* "8" Set a gain of +8
* "10" Set a gain of +10
* "16" Set a gain of +16
* "32" Set a gain of +32

### MCP6S91 pinout
1 Vout -> Output signal<br />
2 CH0  -> Input signal<br />
3 Vref -> GND<br />
4 Vss  -> GND<br />
5 CS   -> CS   Atmega328 PB2 (Arduino pin 10)<br />
6 SI   -> DATA Atmega328 PB0 (Arduino pin 8)<br />
7 SCK  -> SCK  Atmega328 PB1 (Arduino pin 9)<br />
Vdd    -> 5V<br />
