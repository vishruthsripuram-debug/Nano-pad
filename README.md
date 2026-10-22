# Nano pad
A 4 key macro pad that you can program actions to.
<img width="836" height="574" alt="Screenshot 2026-05-01 at 12 41 59 pm" src="https://github.com/user-attachments/assets/5d858c60-c8d6-4d94-99de-d2a331fe4b3d" />

## Design
My inspiration for this macro pad was the printed pad by prota design but i wanted to make something smaller and cheaper than both of their offering the printed pad and glyff. The printed pad being $68 seemed quite unreasonable to me seeing as it was only a screen a few key swicthes and a microcontroller.

I wanted the project to be simple and easily accessible and customisable by many people so all of the PCB design files are included in the rep for download and printing from a pcb service. The case is 3d printable in any colour you like and customisable to include a knob or other features as step and other cad files are included. The model has also been published on makerworld for the ease of use with Bambulab printers. The whole idea is to have a simple good looking macro pad that anyone can own for under $15 USD, for 25 USD you can own 5 macro pads. 

**The design is completely original and simply takes some inspiration fro the prota designs macropad**

## Bill of Materials

| # | Designator | Qty | Part # | Description | Price | Links | Notes |
|---|---|---|---|---|---|---|---|
| 1 | SW1–SW4 | 4 | Cherry MX1A-G1NW | MX Brown, tactile, PCB mount, 5-pin | n/a | [Little Bird (MX Brown)](https://littlebirdelectronics.com.au/products/cherry-mx-switch-brown) · [Little Bird (MX2A Brown)](https://littlebirdelectronics.com.au/products/cherry-mx2a-keyswitch-brown) · [MechanicalKeyboards.com](https://mechanicalkeyboards.com/products/cherry-mx-brown-hyperglide-45g-tactile) | MX2A is a drop-in substitute. Little Bird's page doesn't list the exact part number, so confirm it before ordering. |
| 2 | U1 | 1 | DFRobot DFR0648 | 0.91" 128x32 SSD1306 OLED, I2C | US$9.90 / A$13.84 ex GST | [DFRobot](https://dfrobot.com/product-2018.html) · [Element14 AU](https://au.element14.com/dfrobot/dfr0648/oled-display-module-0-91-128x32/dp/4308185) | Check the pinout before soldering. Element14 AU may be out of stock. |
| 3 | U2 | 1 | Seeed 101991470 | XIAO ESP32-C6, Tape & Reel (SMD) | US$6.54 | [RobotShop (T&R)](https://www.robotshop.com/es/products/placa-seeedstudio-seeed-studio-xiao-esp32-c6-tape-reel) · [Core Electronics (AU)](https://core-electronics.com.au/catalog/product/view/sku/SS113991254) · [Pakronics (AU)](https://www.pakronics.com.au/products/seeed-studio-xiao-esp32c6-ss113991254) | The AU stores sell the standard part, 113991254, for A$12.80. Only RobotShop lists the T&R variant. |
| 4 | PCB1 | 1 | Custom | Main board PCB | $1.80 | n/a | Per-board cost for the production run. |

## Software
<img width="1171" height="697" alt="software" src="https://github.com/user-attachments/assets/c2d497b5-6434-459d-b071-70ca37bc7aba" />

## Instructions
### Assembly
1. Order the PCB, I ordered with PCBWay and had assembly included.
2. Print out the case.
3. Assemble the PCB, put the OLED into the pin headers and solder it through, Ideally the SMD ESP32 is assembled by PCBWay, but if you can solder SMD then go for it. The keyswitches are through hole solders as well and thats everything.
4. for assembly, insert the board into the case and use m3x 4 BHCS screws to attach the board to the case, the close the lid of the cad, it is a press fit.
5. Click on your keycaps and plug in the macropad

### Flashing
1. Once the board is plugged into your PC, downlaod the code from the github repository, specifically the INO file and open it in Arduino IDE
2. Add ESP32 board support:
File → Preferences → Additional Boards Manager URLs → add:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
Tools → Board → Boards Manager → search "esp32" → install "esp32 by Espressif Systems"
3. Select the board
4. Install required libraries (Library Manager, Tools → Manage Libraries):
Adafruit GFX / relevant OLED driver library for your DFRobot module (name it specifically once confirmed)
Any keyboard/HID library
5. Press the upload button and then your done

### Setup
1. Downlaod and open the html file
2. connect your board through bluetooth settings on your laptop
3. select the desired functions for your macropad
4. save and uppload
5. exit
6. Youre all done :)

## Schematic and PCB images
<img width="417" height="658" alt="Screenshot 2026-09-15 at 11 06 41 AM" src="https://github.com/user-attachments/assets/1df46572-4710-4aa7-a7d8-e131ff9e807e" />
<img width="686" height="442" alt="image" src="https://github.com/user-attachments/assets/83a4f3b8-4cd0-4035-b739-ddac2e9adbed" />

## Device images
<img width="4032" height="3024" alt="IMG_4889" src="https://github.com/user-attachments/assets/61af93a4-0cf8-4b1c-8d66-70e51410d232" />
<img width="4032" height="3024" alt="IMG_4890" src="https://github.com/user-attachments/assets/a9ebc1e8-b2ae-429e-ae32-f85e04df1d8c" />
<img width="5712" height="4284" alt="IMG_4888" src="https://github.com/user-attachments/assets/fe687be3-8442-44e3-be45-4a7d9885c123" />


## Credits
In collaboration with PCBWay
<img width="2500" height="860" alt="image" src="https://github.com/user-attachments/assets/535d4313-0928-4a1c-ba73-91cac2d6da16" />
**PCBWay has financially sponsored the project by providin the parts and assembly free of cost**

The keycaps used in this project are the simple cherry mx keycaps by 3dnerdcave on printables
https://www.printables.com/model/118708-simple-cherry-mx-keycap/files
