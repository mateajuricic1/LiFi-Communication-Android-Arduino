# LiFi-Communication-Android-Arduino
The goal of the project is the realization of the transmission of information via light
signals. The transfer of text in the form of numbers and letters coded according to Morse code
into a unique series of dots and dashes was achieved. The project uses Li-Fi communication
to send optical Morse code. The developed system includes the implementation of a Li-Fi
transmitter and receiver unit. The Android device, ie the created Android application,
represents the transmitter and encoder of the mentioned Morse code, and the encrypted
message is sent using the built-in LED flash on the phone. On the receiving side, the
photoresistor receives flashes of light from the phone and sends them to the Arduino
microcontroller for decoding. The decoded alphanumeric characters are then printed on the
LCD display to allow the user to compare the decoded data with the data sent from the
Android application. Printing on the LCD display allows the user to easily see the correctness
of the implemented system.

The developed system performs all the intended actions and the result is functional,
but there is a need for improvement. Namely, by using a photodiode instead of a
photoresistor, the system could be significantly faster than the one developed in this project.
