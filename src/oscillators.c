/**
 * @file oscillators.c
 * @author Isha Sharma
 * @date 18-Sep-2023
 * @brief  ALL EMLIB CMU_***() FUNCTIONS ARE DEFINED IN THIS FILE.
**/

#include "oscillators.h"
#include "app.h"

//function to init oscialltors based on the energy mode
void oscilator_init(void)
{
    //enable lfxo or ulfrco
   //high power modes, em0,em1,em2 use LFXO
  if(LOWEST_ENERGY_MODE < 3)
    {
      CMU_OscillatorEnable(cmuOsc_LFXO, true, true); // Low frequency crystal oscillator enabled
      CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO); //Low-frequency crystal oscillator selected with Low-frequency A clock
    }
  //low power mode, em3, ULFRCO
  else
    {
      CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true); // Ultra low-frequency RC oscillator enabled
      CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO); //Ultra low-frequency RC oscillator selected with Low-frequency A clock
    }

  //enable letimer0
  CMU_ClockEnable(cmuClock_LFA, true);     //enabling lfa
  CMU_ClockEnable(cmuClock_LETIMER0, true);       //enabling LETIMER0

  //set prescalar
  if(LOWEST_ENERGY_MODE < 3)
    {
      CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_4); // dividing clock by 4 for em0,1,2
    }
  else
    {
      CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_1); // dividing clock by 1 for em3
    }
}
