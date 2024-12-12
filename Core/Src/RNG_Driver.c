/*
 * RNG_Driver.c
 *
 *  Created on: Dec 4, 2024
 *      Author: iifba
 */

#include <RNG_Driver.h>

RNG_HandleTypeDef rngperph;
static uint32_t num;

void initRNG() {
    __HAL_RCC_RNG_CLK_ENABLE();
    rngperph.Instance = RNG;
    HAL_RNG_Init(&rngperph);
}
uint32_t generateNum() {
    HAL_RNG_GenerateRandomNumber(&rngperph, &num);
    num = num%7;
    return num;
}
