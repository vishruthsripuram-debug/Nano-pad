# Nano pad
A 4 key macro pad that you can program actions to.
<img width="836" height="574" alt="Screenshot 2026-05-01 at 12 41 59 pm" src="https://github.com/user-attachments/assets/5d858c60-c8d6-4d94-99de-d2a331fe4b3d" />

## Design
My inspiration for this macro pad was the printed pad by prota design but i wanted to make something smaller and cheaper than both of their offering the printed pad and glyff. The printed pad being $68 seemed quite unreasonable to me seeing as it was only a screen a few key swicthes and a microcontroller.

I wanted the project to be simple and easily accessible and customisable by many people so all of the PCB design files are included in the rep for download and printing from a pcb service. The case is 3d printable in any colour you like and customisable to include a knob or other features as step and other cad files are included. The model has also been published on makerworld for the ease of use with Bambulab printers. The whole idea is to have a simple good looking macro pad that anyone can own for under $15 USD, for 25 USD you can own 5 macro pads. 

**The design is completely original and simply takes some inspiration fro the prota designs macropad**

## Bill of Materials — Macro Pad

| Item # | Designator | Qty | Manufacturer | Mfg Part # | Description / Value | Package/Footprint | Type | Price Range (AUD / Unit) | Verified Store Links | Notes |
|---|---|---|---|---|---|---|---|---|---|---|
| **1** | SW1, SW2, SW3, SW4 | 4 | Cherry | MX1A-G1NW | MX Brown, tactile, PCB mount, 5-pin | SW_Cherry_MX_1.00u_PCB | THT | $0.40 – $0.88 | [Cafege](https://cafege.com.au "Cherry MX1A-G1NW") <br> [NetNest](https://netnest.com.au "NetNest MX1A-G1NW") <br> [Robot Gear](https://robotgear.com.au "Robot Gear Cherry MX2A Brown") | Sold in packs or individually. Newer MX2A variants are drop-in compatible 5-pin tactile substitutes if the hyperglide variant lacks stock. |
| **2** | U1 | 1 | DFRobot | DFR0648 | Fermion 0.91" 128x32 SSD1306 OLED, I2C, top-mounted pin header (OLED-B) | Module, pin header, THT | THT | $9.90 – $14.50 | [DFRobot Official](https://dfrobot.com "DFRobot DFR0648") <br> [DigiKey](https://digikey.com "DigiKey DFR0648") <br> [Element14 AU](https://element14.com "Element14 DFR0648") | **Double-check pinout prior to soldering**: ensure pins sit on the short top border. |
| **3** | U2 | 1 | Seeed Studio | 101991470 | XIAO ESP32-C6 module (Tape and Reel, SMD variant) | SMD castellated module | SMD | $12.08 – $15.50 | [Pakronics](https://pakronics.com.au "Pakronics Seeed 101991470") <br> [Seeed Studio](https://seeedstudio.com "Seeed Studio 101991470") <br> [RobotShop](https://robotshop.com "RobotShop Seeed 101991470") | Specifically the tape-and-reel edition designed for automated SMT layout without any pre-attached pins. |
| **4** | PCB1 | 1 | Custom | N/A | Main Board PCB | Custom | Board | $1.80 | N/A | Cost per individual board based on production run. |


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
2. connect your board
3. select the desired functions for your macropad
4. save and uppload
5. exit
6. Youre all done :)

## Schematic and PCB images
 <img width="417" height="658" alt="Screenshot 2026-09-15 at 11 06 41 AM" src="https://github.com/user-attachments/assets/1df46572-4710-4aa7-a7d8-e131ff9e807e" />
<img width="358" height="389" alt="Screenshot 2026-09-15 at 11 06 30 AM" src="https://github.com/user-attachments/assets/36d4f93e-4843-4420-9e06-77fce34c9ceb" />

## Collaboration
In collaboration with PCBWay
<img width="2500" height="860" alt="image" src="https://github.com/user-attachments/assets/535d4313-0928-4a1c-ba73-91cac2d6da16" />
**PCBWay has financially sponsored the project by providin the parts and assembly free of cost**
