/*
 * RNG_Driver.h
 *
 *  Created on: Dec 4, 2024
 *      Author: iifba
 */

#ifndef INC_RNG_DRIVER_H_
#define INC_RNG_DRIVER_H_
#include <stm32f4xx_hal.h>
#include <stdbool.h>
#include <string.h>

void initRNG(void);
uint32_t generateNum(void);
#endif /* INC_RNG_DRIVER_H_ */
