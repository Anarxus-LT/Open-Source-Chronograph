# Open-Source-Chronograph
 An open source chronograph used to measure the speed and energy of projectiles. Mainly intended for airsoft.

[!CAUTION]
SOME DISPLAYS HAVE INVERTED GND AND VCC!!!\
I have added jumpers for both versions

ToDo: upload photos with examples
= FOR PIN 1 GND X FOR PIN 1 VCC

## Donations :smiley:
Did you find the project useful ? feel free to buy me a cofee :coffee:

[![](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/donate/?hosted_button_id=XSP59UAQV3676)


No hard feeling if you dont :wink:

## Instructions
To upload the firmware to the pi pico
1. Unplug the usb cable
2. Press the "BOOTSEL" button and hold it
3. Plug the usb cable
4. Copy the "Open_Source_chronograph.ino.uf2" to the drive that showed up

## Components used
* TP4056 module with BMS
* 18650 battery
* 1x small power switch  (I used a generic 10x15mm switch)
* SSD1306 128x64 I2C OLED display
* Pi pico with headers soldered
* 4pin right angle male header
* 2x 20pin female headers
* 5v buzzer
* 3 tactile switches
* 2 IR LEDs (and appropriete resistors)
* 1A fuse



## PCB
Should be easy to do on a protoboard
Created proper PCB that you can send to your prefered PCB manufacturer