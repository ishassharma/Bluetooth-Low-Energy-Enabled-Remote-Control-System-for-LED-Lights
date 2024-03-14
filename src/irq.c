/**
 * @file irq.c
 * @author Isha Sharma
 * @date 17-Sep-2023
 * @brief  ALL INTERRUPT SERVICE AND RELATED ROUTINES DEFINED IN THIS FILE
**/

#include "irq.h"
#include "gpio.h"
#include "scheduler.h"
#include "em_i2c.h"
#include "lcd.h"
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

//for even numbered keypad pins
void GPIO_EVEN_IRQHandler(void)
{
  // First: determine source of IRQ
  uint32_t flags = GPIO_IntGetEnabled() & 0x55555555; // DOS only look at even numbered bits 0,2,4...

  // Second: clear source of IRQ set in step 3
  GPIO_IntClear(flags);

  // Third: perform whatever processing is required
  uint8_t Keypad_PD12_state = GPIO_PinInGet(GPIO_PORT2,KEYPAD_GPIO_PIN4);
  uint8_t PBO_state = GPIO_PinInGet(PB0_BUTTON_PORT,PB0_BUTTON_PIN);



    schedulerSetEvent_Keypad_EVEN();

}

//for odd number keypad pins
void GPIO_ODD_IRQHandler(void)
 {
  // First: determine source of IRQ
  uint32_t flags = GPIO_IntGetEnabled() & 0xAAAAAAAA; // DOS only look at odd numbered bits 1,3,5...
  // Second: clear source of IRQ set in step 3
  GPIO_IntClear(flags);
  // Third: perform whatever processing is required
  schedulerSetEvent_Keypad_ODD();

}

