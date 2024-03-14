/**
 * @file irq.h
 * @author Isha Sharma
 * @date 17-Sep-2023
 * @brief  header file for irq.c
**/

#ifndef SRC_IRQ_H_
#define SRC_IRQ_H_

#include "em_letimer.h"
#include "efr32bg13p632f512gm48.h"

void GPIO_EVEN_IRQHandler(void);
void GPIO_ODD_IRQHandler(void);

#endif /* SRC_IRQ_H_ */
