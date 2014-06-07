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

See it in action: [http://youtu.be/vFpYjpEDkyg](http://youtu.be/vFpYjpEDkyg)
##Technical Information

### Theory
  The PID controller contains three tunable factors: The proportional factor (Kp), the integral factor (Ki), and the derivative factor (Kd). By tuning these three parameters, you can modify the behavior of the controller. The Kp factor modifies the output in an exact proportion to the error (T_current - T_set). The integral factor allows the controller to know its "history" by keeping a running total of all the errors. This allows us to stabilize a temperature in the event that the thing we're controlling requires power to maintain our goal. It also ramps up production if it's taking too long to reach the setpoint due to a low output. Likewise, it ramps down production if the power is staying too high even after we pass the setpoint. Finally, the Kd factor prevents us from climbing the temperature ladder too fast by measuring the rate of change of the temperature and subracting a chunk of output based on the reading.
  
  
  The output power to the device itself is controlled by modifying the duty cycle of the PWM from the microcontroller to the solid state relay. If we have a 50% duty cycle, then if the power is only on for half the time and off for the other half, then we can count that as receiving 50% power. The duty cycle is changed in response to the reading from the PID controller. The closer we get to the set point, the less power we get. If we were able to actively cool our device as well, we could even output a negative power to compensate for overshoot.
  
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
