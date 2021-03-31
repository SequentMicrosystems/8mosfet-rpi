
[![8mosfet-rpi](res/sequent.jpg)](https://sequentmicrosystems.com)

# lib8mosfet

This is the python library to control the [8-MOSFETS Solid State Stackable Card for Raspberry Pi](https://sequentmicrosystems.com/collections/all-io-cards/products/raspberry-pi-mosfets-stackable-hat).

## Install

```bash
~$ sudo apt-get update
~$ sudo apt-get install build-essential python-pip python-dev python-smbus git
~$ git clone https://github.com/SequentMicrosystems/8mosfet-rpi.git
~$ cd 8mosfet-rpi/python/8mosfet/
~/8mosfet-rpi/python/8mosfet$ sudo python setup.py install
```
## Update

```bash
~$ cd 8mosfet-rpi/
~/8mosfet-rpi$ git pull
~$ cd 8mosfet-rpi/python/8mosfet/
~/8mosfet-rpi/python/8mosfet$ sudo python setup.py install
```

## Usage 

Now you can import the megaio library and use its functions. To test, read mosfets status from the board with stack level 0:

```bash
~$ python
Python 2.7.9 (default, Sep 17 2016, 20:26:04)
[GCC 4.9.2] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> import lib8mosfet
>>> lib8mosfet.get_all(0)
0
>>>
```

## Functions

### set(stack, mosfet, value)
Set one mosfet state.

stack - stack level of the 8-Relay card (selectable from address jumpers [0..7])

mosfet - mosfet number (id) [1..8]

value - mosfet state 1: turn ON, 0: turn OFF[0..1]


### set_all(stack, value)
Set all mosfets state.

stack - stack level of the 8-Relay card (selectable from address jumpers [0..7])

value - 4 bit value of all mosfets (ex: 15: turn on all mosfets, 0: turn off all mosfets, 1:turn on mosfet #1 and off the rest)

### get(stack, mosfet)
Get one mosfet state.

stack - stack level of the 8-Relay card (selectable from address jumpers [0..7])

mosfet - mosfet number (id) [1..8]

return 0 == mosfet off; 1 - mosfet on

### get_all(stack)
Return the state of all mosfets.

stack - stack level of the 8-Relay card (selectable from address jumpers [0..7])

return - [0..255]

