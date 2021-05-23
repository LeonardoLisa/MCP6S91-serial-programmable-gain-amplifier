# MCP6S91-serial-programmable-gain-amplifier

The program provides a simple plug and play device with an interactive serial user interface to set the gain of the PGA. The code is compiled for an ATMEGA328P and is compatible with an Arduino UNO developing board. To set the gain you only need to type the numerical value, the interpreter expects all commands to be terminated with the new line character '\n'. Valid gain commands  entry are:
* "0" No gain, PGA off
* "1" Set a gain of +1
* "2" Set a gain of +2
* "4" Set a gain of +4
* "5" Set a gain of +5
* "8" Set a gain of +8
* "10" Set a gain of +10
* "16" Set a gain of +16
* "32" Set a gain of +32

This project is compatible with MCP6S91/2/3 but in order to switch between channel CH1 and CH2 you must create some new commands. Chain configuration is not implemented yet. 
