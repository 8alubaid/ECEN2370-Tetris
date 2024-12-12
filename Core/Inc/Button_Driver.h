/*
 * Button_Driver.h
 *
 *  Created on: Dec 4, 2024
 *      Author: iifba
 */

#ifndef BUTTON_DRIVER_H_
#define BUTTON_DRIVER_H_


#include <stdbool.h>
#include <stm32f4xx_hal.h>

#define BUTTON_PORT_NUMBER			GPIOA
#define BUTTON_PIN_NUMBER			GPIO_PIN_0

#define BUTTON_PRESSED     			1
#define BUTTON_NOT_PRESSED  		0

void Button_Init();
void Button_EnableClock();
bool Button_isPressed();
void Button_InterruptInit();


#endif /* BUTTON_DRIVER_H_ */
