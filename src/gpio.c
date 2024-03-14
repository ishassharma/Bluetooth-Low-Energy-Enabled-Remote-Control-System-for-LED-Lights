/*
  gpio.c

   Created on: Dec 12, 2018
       Author: Dan Walkes
   Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

   March 17
   Dave Sluiter: Use this file to define functions that set up or control GPIOs.

   Jan 24, 2023
   Dave Sluiter: Cleaned up gpioInit() to make it less confusing for students regarding
                 drive strength setting.

 *
 * @student    Isha Sharma, Isha.Sharma@colorado.edu
 *

 */


// *****************************************************************************
// Students:
// We will be creating additional functions that configure and manipulate GPIOs.
// For any new GPIO function you create, place that function in this file.
// *****************************************************************************

#include "gpio.h"


// Student Edit: Define these, 0's are placeholder values.
//
// See the radio board user guide at https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf
// and GPIO documentation at https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__GPIO.html
// to determine the correct values for these.
// If these links have gone bad, consult the reference manual and/or the datasheet for the MCU.
// Change to correct port and pins:


// Set GPIO drive strengths and modes of operation
void gpioInit()
{
    // Set the port's drive strength. In this MCU implementation, all GPIO cells
    // in a "Port" share the same drive strength setting.
  //GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthStrongAlternateStrong); // Strong, 10mA
  GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthWeakAlternateWeak); // Weak, 1mA

  // Set the 2 GPIOs mode of operation
  GPIO_PinModeSet(LED_port, LED0_pin, gpioModePushPull, false); //push-pull output
  GPIO_PinModeSet(LED_port, LED1_pin, gpioModePushPull, false);

  GPIO_PinModeSet(LCD_PORT, EXTCOMIN_PIN, gpioModePushPull, false);

  //for Keypad button
  GPIO_PinModeSet(GPIO_PORT2, KEYPAD_GPIO_PIN3, gpioModeInput, true);
  GPIO_ExtIntConfig (GPIO_PORT2, KEYPAD_GPIO_PIN3, KEYPAD_GPIO_PIN3, true, true, true);

  GPIO_PinModeSet(GPIO_PORT2, KEYPAD_GPIO_PIN4, gpioModeInput, true);
  GPIO_ExtIntConfig (GPIO_PORT2, KEYPAD_GPIO_PIN4, KEYPAD_GPIO_PIN4, true, true, true);

  GPIO_PinModeSet   (PB0_BUTTON_PORT, PB0_BUTTON_PIN, gpioModeInput, true); // out values determines glitch filter on/off, turn on filtering
  GPIO_ExtIntConfig (PB0_BUTTON_PORT, PB0_BUTTON_PIN, PB0_BUTTON_PIN, true, true, true);

} // gpioInit()


void gpioLed0SetOn()
{
  GPIO_PinOutSet(LED_port, LED0_pin);
}


void gpioLed0SetOff()
{
  GPIO_PinOutClear(LED_port, LED0_pin);
}


void gpioLed1SetOn()
{
  GPIO_PinOutSet(LED_port, LED1_pin);
}


void gpioLed1SetOff()
{
  GPIO_PinOutClear(LED_port, LED1_pin);
}

//needed for lcd
void TempSensorOn()
{
  GPIO_PinOutSet(TEMP_SENSOR_PORT, TEMP_SENSOR_PIN);
}

void TempSensorOff()
{
  GPIO_PinOutSet(TEMP_SENSOR_PORT, TEMP_SENSOR_PIN);
}

void gpioSetDisplayExtcomin(bool value)
{
  if(value==0)
    {
      GPIO_PinOutClear(LCD_PORT, EXTCOMIN_PIN);
    }
  else
    {
      GPIO_PinOutSet(LCD_PORT, EXTCOMIN_PIN);
    }
}
