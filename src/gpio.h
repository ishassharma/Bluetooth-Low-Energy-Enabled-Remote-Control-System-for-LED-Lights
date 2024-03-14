/*
   gpio.h
  
    Created on: Dec 12, 2018
        Author: Dan Walkes

    Updated by Dave Sluiter Sept 7, 2020. moved #defines from .c to .h file.
    Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

    Editor: Feb 26, 2022, Dave Sluiter
    Change: Added comment about use of .h files.

 *
 * @student    Isha Sharma, Isha.Sharma@colorado.edu
 *
 
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

#include <stdbool.h>
#include "em_gpio.h"
#include <string.h>

#define LED_port   (gpioPortF)
#define LED0_pin   (4) //pf4
#define LED1_pin   (5)  //pf5

#define LCD_PORT (gpioPortD)
#define EXTCOMIN_PIN (13)

#define GPIO_PORT2 (gpioPortD)

#define KEYPAD_GPIO_PIN3 (11) //pd11//odd
#define KEYPAD_GPIO_PIN4 (12)//pd12

#define TEMP_SENSOR_PIN (15) //pd15
#define TEMP_SENSOR_PORT (gpioPortD)

#define PB0_BUTTON_PORT (gpioPortF)
#define PB0_BUTTON_PIN (6)//pf6

// Function prototypes
void gpioInit();
void gpioLed0SetOn();
void gpioLed0SetOff();
void gpioLed1SetOn();
void gpioLed1SetOff();
void gpioSetDisplayExtcomin(bool value);

#endif /* SRC_GPIO_H_ */
