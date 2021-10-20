# Lumos
Low-cost, modular luminometer 

**How it works** \
Lumos is a low cost, modular luminometer which can measure the light intensity of luminescent enzymes. The device works by 
measuring the light intensity of the analyte through a TSL237 Light-to-Frequency photodiode. The sensor then converts the
incident light into a 50% duty cycle square wave voltage, which is then picked up by the ESP32 microcontroller's interrupt pins. 
At every rising edge of the square wave input, the microcontroller will call the interrupt service routine (ISR), which increments
the counter variable by 1. The value held by the counter variable is then measured after one second in order to determine the 
frequency in Hz. The frequency detected is directly proportional to the light intensity received from the light source. The output
is then displayed through either the LCD screen built in with the sensor, or on a mobile phone application (currently only available
on Android). The mobile application was made on Thunkable to facilitate BLE connection between the client (phone) and the server (ESP32).

**Materials** 
* ESP32 microcontroller
* TSL237 light-to-frequency photodiode
* LCD1602 screen (with I2C module built-in)
* 0.1uF capacitor
* PLA (for 3D printing)
* Wires

**How to Build**
1. Follow the schematic diagram for the correct wiring on the ESP32 microcontroller. 
2. Upload the .ino sketch to the microcontroller. 
3. On the Thunkable app, search up 'Lumos' by *johncedricaciertot*.
4. Print the CAD models.
