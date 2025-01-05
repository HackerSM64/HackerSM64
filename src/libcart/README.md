# libcart - Nintendo 64 flash cartridge library
Copyright (C) 2022 - 2023 devwizard

## About
This library provides an interface to several Nintendo 64 flash cartridges.  It
automatically detects the hardware in use, and uses the appropriate protocol for
the hardware.

## Supported hardware
* 64Drive HW1 and HW2
* EverDrive-64 V1, V2, V2.5, V3, X7 and X5
* ED64Plus / Super 64
* SummerCart64

[Testing result spreadsheet](https://docs.google.com/spreadsheets/d/19opOv4e_4ABqgOmqRBSREtXLdGy08VzMsymtxWky0NU/edit?usp=sharing)

## Features
* Unlocks the cartridge ROM, making it rewritable.
* Uses optimised bus timing for faster DMA (EverDrive-64).
* Memory card access, with I/O between RDRAM and cartridge ROM.

## Credits
Special thanks to Reonu and Meeq for their extensive testing on ED64Plus and
EverDrive-64 V2.  Without them, this project would not be possible.
