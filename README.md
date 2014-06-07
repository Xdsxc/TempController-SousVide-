#TempController
==============
AVR-based temperature control

##What is it?
This device allows you to control the output of a wide range of appliances. It’s as simple as plugging in a device of your choosing, entering the desired target and time, and watching it go after that.  The current implementation allows for modulating output based on the temperature reading, but it wouldn’t be hard at all to modify it to work with nearly any type of sensor and output device.

I originally created this device to mimic the $400.00 SousVide machine. SousVide is a method of cooking where food is sealed in a plastic bag and submerged in a temperature-regulated water bath for a period of time. Cooking times range from a few hours, to a few days, depending on the item and desired result.
##Requirements
An appliance that heats up fluid. I’ve had success using a tea kettle, a rice cooker, and a crock pot. I wouldn’t suggest using any “smart” device that controls its own output, the results may be unpredictable. 

##How to use it
Plug the device into the female socket, and the power cord into the male socket. Connect the power, and follow the 3 simple menus. Then sit back and wait! For safety, make sure the device is completely disconnected from the power socket before opening the enclosure.

##Technical Information

###Technology used:

* 8MHz ATMega1284p
* DS18B20 Temperature Sensor
* Nokia 5110 Graphic LCD
* 16-button keypad
* 74LVC245AN Logic Level Converter
* 24 - 380VAC, 3 - 25VDC triggered Solid State Relay
* Random 120VAC -> 5VDC PSU from eBay

### How to use the code:
No modification should be neccessary if the schematic is followed. If one wishes to use different pins for control, adjust the component initialization in main.c. The timers are intended for use with an 8MHz ATMega1284p. To use with any other device, some component registers may need to be modified (particularly the timer in timer.c, and the pwm setup in pwm.c.

###Device Setup:
![Imgur](http://i.imgur.com/MycyPM2.png)
![Pic1](http://i.imgur.com/zDEDG0H.jpg)
![Imgur](http://i.imgur.com/SbO3p64.jpg)
![Imgur](http://i.imgur.com/oEXwkrK.jpg)
