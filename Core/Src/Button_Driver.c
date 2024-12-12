/*
 * Button_Driver.h
 *
 *  Created on: Dec 4, 2024
 *      Author: iifba
 */


#ifndef BUTTON_DRIVER_C_
#define BUTTON_DRIVER_C_

#include <Button_Driver.h>

void Button_Init() {





    GPIO_InitTypeDef ButtonConfig = {0};

    ButtonConfig.Pin = BUTTON_PIN_NUMBER;
    ButtonConfig.Speed = GPIO_SPEED_FREQ_MEDIUM;
    ButtonConfig.Pull = GPIO_NOPULL;
    ButtonConfig.Mode = GPIO_MODE_INPUT;

    Button_EnableClock();

    HAL_GPIO_Init(GPIOA,&ButtonConfig);
}

void Button_InterruptInit(){
	GPIO_InitTypeDef ButtonConfig = {0};

    ButtonConfig.Pin = BUTTON_PIN_NUMBER;
    ButtonConfig.Speed = GPIO_SPEED_FREQ_MEDIUM;
    ButtonConfig.Pull = GPIO_NOPULL;
    ButtonConfig.Mode = GPIO_MODE_IT_FALLING;
	//ButtonConfig.PinInterruptMode = FALLING_AND_RISING_EDGE_INTERRUPT;



	Button_EnableClock();
	HAL_GPIO_Init(GPIOA, &ButtonConfig);


	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}


void Button_EnableClock(){

	__HAL_RCC_GPIOA_CLK_ENABLE();
}

bool Button_isPressed() {

    uint8_t pinState = HAL_GPIO_ReadPin(BUTTON_PORT_NUMBER, BUTTON_PIN_NUMBER);


    return (pinState == BUTTON_PRESSED);

}



#endif /* INTTERUPT_CONTROL_C_ */



