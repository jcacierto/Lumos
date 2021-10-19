# Lumos
Low-cost, modular luminometer 

Lumos is a low cost, modular luminometer which can measure the light intensity of luminescent enzymes. The device works by 
measuring the light intensity of the analyte through a TSL237 Light-to-Frequency photodiode. The sensor then converts the
incident light into a 50% duty cycle square wave voltage, which is then picked up by the ESP32 microcontroller's interrupt pins. 
At every rising edge of the square wave input, the microcontroller will call the interrupt service routine (ISR), which increments
the counter variable by 1. The value held by the counter variable is then measured after one second in order to determine the 
frequency in Hz. The frequency detected is directly proportional to the light intensity received from the light source. The output
is then displayed 
