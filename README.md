This repository contains the source code, schematic, and PCB layout for the electronic badge developed as part of my thesis project. 

Getting Started: Reprogramming the E-Badge:

Install the Arduino IDE: If you haven't already, download and install the Arduino IDE, which is available at https://www.arduino.cc/en/software.

Add Raspberry Pi Pico Support: In the Arduino IDE, you’ll need to add support for the Raspberry Pi Pico board (the microcontroller used in this badge). Follow these steps:
- Open the Arduino IDE and go to File > Preferences.
- In the "Additional Boards Manager URLs" field, add the following URL: https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json


Install the Raspberry Pi Pico Package:

- Go to Tools > Board > Boards Manager.
- Search for "Raspberry Pi Pico" in the Boards Manager.
- Install the package named Raspberry Pi Pico/RP2040 by Earle Philhower.


Select the Board and Port:

- Connect your badge (Raspberry Pi Pico) to your computer via USB.
- In the Arduino IDE, go to Tools > Board and select Raspberry Pi Pico.
- Then, under Tools > Port, select the port that corresponds to your badge.

  Upload Code to the Badge: Now you can open any of the .ino files in this repository, modify the code if desired, and click Upload to program the badge.
