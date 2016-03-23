Reference FX2 Source code for OPT8241-based systems.
====================================================

Version: 0.17

This repository contains reference source code to the firmware for the Cypress CY7C68053 (FX2) microcontroller for use with an OPT8241-based system.
This firmware uses the LGPL 2.1 licensed fx2lib library from [1].

[1] https://github.com/djmuhlestein/fx2lib/


Disclaimer
==========
This code is for use only on custom-built boards. Please take care to not use the compiled version of this code on the official OPT8241-CDK-EVM from TI as it may void laser safety.


Build Requirements
==================
The source code is built with SDCC 2.9.0 (#5416) and GNU Make 3.81 on Windows.

SDCC 2.9.0 can be downloaded from here:
http://sourceforge.net/projects/sdcc/files/sdcc-win32/2.9.0/


Build Instructions
==================

1) cd lib
2) make
3) cd ../examples/opt8241-reference-fw
4) Edit Makefile and set FX2_DIR to the top-level source directory
5) make

The .hex and .iic files will be generated in the release subdirectory.

