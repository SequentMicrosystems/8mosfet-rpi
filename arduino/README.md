[![8mosfet-rpi](../readmeres/sequent.jpg)](https://www.sequentmicrosystems.com)

# 8mosfet arduino library

In order to use the [8-Mosfet](https://sequentmicrosystems.com/product/raspberry-pi-mosfets-stackable-hat/) card with Arduino  UNO or NANO, TEENSY, FEATHER or ESP32 boards you need a [Raspberry Pi Replacement Cad](https://sequentmicrosystems.com/product/raspberry-pi-replacement-card/) and this library.

## Install 

Copy the "SM_8mosfet" directory inside of the libraries sub-directory of your sketchbook directory ("This PC > Documents > Arduino > libraries" for a Windows PC or "Home > arduino > libraries" on Linux PC). Now launch the Arduino environment. If you open the Sketch > Import Library menu, you should see SM_8mosfet inside. The library will be compiled with sketches that use it.

## Usage

Connect 8mosfet card with the Raspberry Pi Replacement Cad then connect your favorite Arduino processor. Open Arduino IDE go to File > Examples > SM_8mosfet > 8mosfets. The example show how to use multiple 8-Mosfet cards with the same Arduino processor.


## Notice

The library work only with TEENSY 3.2/3.5/3.6/4.0/4.1, Arduino UNO, Arduino NANO, Adafruit FEATHER, ESP32 DEVKIT V1
