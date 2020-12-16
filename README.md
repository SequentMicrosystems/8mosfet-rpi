[![8mosfet-rpi](readmeres/sequent.jpg)](https://www.sequentmicrosystems.com)

# 8mosfet-rpi

[![8mosfet-rpi](readmeres/8-MOSFET.jpg)](https://www.sequentmicrosystems.com)

This is the command line and python functions to control the [8-MOSFETS Solid State Stackable Card for Raspberry Pi](hhttps://sequentmicrosystems.com/product/raspberry-pi-mosfets-stackable-hat/).

First enable I2C communication:
```bash
~$ sudo raspi-config
```

## Usage

```bash
~$ git clone https://github.com/SequentMicrosystems/8mosfet-rpi.git
~$ cd 8mosfet-rpi/
~/8mosfet-rpi$ sudo make install
```

Now you can access all the functions of the mosfets board through the command "8mosfet". Use -h option for help:
```bash
~$ 8mosfet -h
```

If you clone the repository any update can be made with the following commands:

```bash
~$ cd 8mosfet-rpi/  
~/8mosfet-rpi$ git pull
~/8mosfet-rpi$ sudo make install
```  

### [Python library](https://github.com/SequentMicrosystems/8mosfet-rpi/tree/master/python)
### [Arduino library](https://github.com/SequentMicrosystems/8mosfet-arduino)
### [NodeRed](https://github.com/SequentMicrosystems/8mosfet-rpi/tree/master/node-red-contrib-sm-8mosfet)
