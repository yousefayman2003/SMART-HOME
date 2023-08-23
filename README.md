# Arduino Smart Home Security System

The Arduino Smart Home Security System is a project that implements a comprehensive security system for a smart home using Arduino and various sensors. The system includes features such as keypad authentication, gas and fire detection, motion detection, and remote monitoring through a web application with Firebase . In case of an theif or emergency,such as a fire or gas leak, the system triggers an alarm by turning on the buzzer and turning off and on the lamps. This README provides an overview of the functionality of the  functionality.

## Hardware Requirements

Our Arduino Smart Home Security System, Has the following hardware components:

- DOIT Esp32 DevKit v1
- Servo motor
- Keypad
- LiquidCrystal I2C display
- Gas sensor
- PIR motion sensor
- LDR sensor
- Flame sensor
- Ultrasonic sensor
- LEDs
- Buzzer

## Software Requirements

The following software components are required to run the Arduino Smart Home Security System:

- Arduino IDE
- ESP32Servo library
- Keypad library
- LiquidCrystal_I2C library
- WiFi library
- FirebaseESP32 library


## Functionality

The Arduino Smart Home Security System provides the following functionality:

1. **Pin Mode Configuration:** The `setPinsMode()` function sets the pin modes for the various sensors, actuators, and components used in the system. It configures the input and output modes for pins such as the gas sensor pin, PIR motion sensor pin, ultrasonic sensor pins, LEDs pin, door LED pin, and buzzer pins. This function should be called during the system initialization to ensure proper pin configuration.

1. **Time Retrieval:** The `getTime()` function retrieves the current epoch time. It uses the ESP32's built-in function `getLocalTime()` to obtain the local time information and then converts it to epoch time format. If the time retrieval is successful, the function returns the current time; otherwise, it returns 0.

1. **Password Validation:** The system includes a keypad for user authentication. The `valid_password()` function checks the entered password against a predefined password. If the entered password matches the predefined password, the function returns `true`; otherwise, it returns `false`. The entered keys are displayed on the LCD screen, and once the "#" key is pressed or the maximum number of key inputs (16 in this case) is reached, the function compares the entered password with the predefined password.

1. **Presence Detection:** The system uses an ultrasonic sensor to detect if there is someone in front of the door. The `is_there_someone()` function triggers a distance measurement by sending a short LOW pulse followed by a HIGH pulse to the ultrasonic sensor. It then measures the duration of the echo pulse and calculates the distance in centimeters. If the measured distance is less than or equal to 50 cm, the function returns `true`, indicating the presence of someone in front of the door; otherwise, it returns `false`.

1. **Theif Detection:** When a theif is detected i.e (someone enters the house without entering a password). the system triggers an emergency by turning on the buzzer and turning off the lamps. A notification is shown on the web application that there is a theif detected. The LCD display shows an "THEIF" message the system continuously checks that the theif is still inside or not once the theif is outside, the system displays a "THE HOME IS SAFE" message on the LCD and stops the emergency alarm.


1. **Smart Door System:** When the `is_there_someone()` returns true a LED above the door turns ON and the LCD prompts for the user to Enter `#` so he can enter his password if the `valid_password()` returns true the doors open then closes after 1 second otherwise  the user is notified on the web application that someone has entered a wrong password and the door is not opened.

1. **Emergency Function:** When a fire or gas leak is detected, the system triggers an emergency by turning on the buzzer and turning off the lamps. A notification is shown on the web application that there is a gas leak or fire is detected. The LCD display shows an "EMERGENCY" message, and the system continuously checks the gas sensor for readings above a threshold (1200 in this case). The buzzer and LEDs alternate between on and off states every 100 milliseconds to create an alarm effect. Once the gas readings fall below the threshold, the system displays a "THE HOME IS SAFE" message on the LCD and stops the emergency alarm.


1. **Data Writing Functions:** The system includes two functions, `writeDataInt()` and `writeDataBool()`, to write data to the Firebase Realtime Database. These functions take akey and a value as parameters and write the data to the specified key in the database. The `writeDataInt()` function is used to write integer values, while the `writeDataBool()` function is used to write boolean values.

1. **Data Reading Function:** The Data is read by opening a stream using the Server Sent Events Method when an update happens on the server i.e (Firebase RTDB) its sends it instanly to the client
i.e (ESP32) 

1. **Remote Control**: The system connects to the Firebase Realtime Database to enable remote control and monitoring. Users can control various aspects of the system, such as turning on/off the LEDs or opening/closing the door, through a  web application.

