#include "hal.h"
#include <cmath>

int t = 0;
int base_time = 500;
int delay = 100;
int i = 0;
int leds_num[] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5,
                  GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9,
                  GPIO_PIN_11, GPIO_PIN_12};
unsigned int sw_num[] = {GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};

void TIM6_IRQ_Handler() {
    if (i < 4) {
        HAL_GPIO_WritePin(GPIOD, leds_num[i], GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, leds_num[7-i], GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(GPIOD, leds_num[i], GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOD, leds_num[7-i], GPIO_PIN_RESET);
    }
    i++;
    if (i == 8) {
        i = 0;
    }

    int number = 0;
    for (int j = 0; j < 4; j++) {
        GPIO_PinState state = HAL_GPIO_ReadPin(GPIOE, sw_num[j]);
        if (state == GPIO_PIN_SET) number += (int) pow(2,(3-j));
    }
    if (t != number) {
        t = number;
        WRITE_REG(TIM6_ARR, base_time + t * delay);
    }
}

int umain() {
    registerTIM6_IRQHandler(TIM6_IRQ_Handler);
    __enable_irq();

    WRITE_REG(TIM6_ARR, base_time + t * delay);
    WRITE_REG(TIM6_DIER, TIM_DIER_UIE);
    WRITE_REG(TIM6_PSC, 0);

    WRITE_REG(TIM6_CR1, TIM_CR1_CEN);

    return 0;
}
