# Bluetooth Low Energy Enabled Remote Control System for LED Lights
The system is a Bluetooth-enabled remote-control system for LED lights using Silicon Labs’ Blue Gecko boards. It comprises two main components: a Blue Gecko board connected to a keypad (Board 1) and another Blue Gecko board connected to an LED strip (Board 2). The objective is to establish a wireless communication link between the two boards through Bluetooth, allowing users to remotely control the LED strip based on the keys pressed on the keypad.

## Keywords for Technologies/hardware used:
Bluetooth Low Energy, Server, Client, LED, Keypad, GATT, State Machine, Power Management, GPIO, Energy profiling, Keypad sensor, Bluetooth module, LED strip driver, Blue Gecko Board, State Machine

##Subsystem Summary
A.Keypad Input Processing Subsystem: Responsible for processing user input from the keypad on Board: Detects key presses and releases, translates them into corresponding data, and triggers Bluetooth
notifications to inform Board 2 of the user's input.
B.Bluetooth Communication Subsystem (Board 1): Manages the Bluetooth communication on Board 1. Utilizes the Bluetooth module to send notifications containing keypad input data to Board 2. Handles communication protocols and ensures secure and reliable data transmission.
C. Bluetooth Communication Subsystem (Board 2): Manages the Bluetooth communication on Board 2. Utilizes the Bluetooth module to receive notifications from Board 1. Extracts keypad input data from notifications and forwards it to the LED Pattern Interpretation Subsystem.
D. LED Pattern Interpretation Subsystem: Interprets the keypad input data received from Board 1. Determines the corresponding LED pattern based on the input. Controls the LED Strip Driver Subsystem to display the desired lighting pattern on the LED strip connected to Board 2. (this would depend a lot on which type of LED strip I choose to use, still undecided).
E. LED Strip Driver Subsystem: Controls the LED strip connected to Board 2. Receives LED pattern data from the LED Pattern Interpretation Subsystem and adjusts the LED strip accordingly. Handles the low- level details of controlling individual LEDs or RGB segments. (this would depend a lot on which type of LED strip I choose to use, still undecided).
F. Keypad Sensor Hardware Subsystem (Board 1): The physical hardware component on Board 1 that captures user input from the keypad. Converts physical key presses and releases into electrical signals for processing by the Keypad Input Processing Subsystem.
G. Bluetooth Module Subsystem (Both Boards): Responsible for enabling Bluetooth communication between Board 1 and Board 2. Facilitates the wireless transfer of keypad input data and LED pattern notifications.
H. Power Management Subsystem: Manages the power states of both boards to optimize energy consumption. Implements low-power modes during periods of inactivity and ensures efficient power usage for extended battery life.
I. User Interface Subsystem (LCD Displays): Provides visual feedback to users through LCD displays on both Board 1 and Board 2. Displays relevant information such as keypad input on Board 1 and the current LED pattern on Board 2.
J. System Control and Coordination Subsystem: Coordinates the overall system functionality. Manages the sequence of operations, ensures proper timing, and oversees the interaction between different subsystems to maintain a cohesive user experience.

## Key Learnings:
1) Expertise gained in interfacing GPIOs, configuring/manipulating input/output of pins.
2) Proficiency acquired in working with the I2C protocol for component communication.
3) Ability developed to interface with OLED displays, configure PWM, and work with ADC.
4) Experience coding in Bare Metal and using HAL, boosting confidence.
5) Improved problem-solving abilities through critical analysis of problem statements.
6) Skill in debugging errors within time constraints tested critical thinking.
7) Consideration of usability and user experience enhanced product perspective.
8) Time management skills honed, beneficial for future projects and endeavors.

## Demo Video
The Demo videos are present in the ppt presentation along with project summary. 
