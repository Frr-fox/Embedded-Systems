#include "hal.h"

int delay = 100;
int leds_num[] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5,
                  GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9,
                  GPIO_PIN_11, GPIO_PIN_12};
unsigned int sw_num[] = {GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};
unsigned int sw_states[] = {0, 0, 0, 0};

int check_sw_condition() {
    return ((HAL_GPIO_ReadPin(GPIOE, sw_num[0]) == 0 && HAL_GPIO_ReadPin(GPIOE, sw_num[2]) == 0)
            && (HAL_GPIO_ReadPin(GPIOE, sw_num[1]) == 1 && HAL_GPIO_ReadPin(GPIOE, sw_num[3]) == 1));
}

void set_GREEN() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}

void set_YELLOW() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
}
void set_RED() {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
}

void check_btn() {
    GPIO_PinState state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);
    if (state == GPIO_PIN_RESET) {
        set_RED();
        HAL_Delay(delay);
        state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);
        while(state == GPIO_PIN_SET) {
            HAL_Delay(delay);
            state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);
        }
    }
    set_GREEN();
}

void run_animation() {
    while (1) {
        set_GREEN();
        HAL_Delay(delay);
        for (int i = 0; i < 8; i++) {
            HAL_GPIO_WritePin(GPIOD, leds_num[i], GPIO_PIN_RESET);
        }
        HAL_Delay(delay);
        for (int i = 0; i < 8; i++) {
            HAL_GPIO_WritePin(GPIOD, leds_num[i], GPIO_PIN_SET);
            for (int j = 0; j < 5; j++) {
                check_btn();
                HAL_Delay(delay);
                if (!check_sw_condition()) return;
            }
        }
        for (int i = 7; i > 1; i--) {
            HAL_GPIO_WritePin(GPIOD, leds_num[i], GPIO_PIN_RESET);
            for (int j = 0; j < 5; j++) {
                check_btn();
                HAL_Delay(delay);
                if (!check_sw_condition()) return;
            }
        }
    }
}

int umain() {
    while (1) {
        while(1) {
            set_YELLOW();
            for (int i = 0; i < 4; i++) {
                GPIO_PinState state = HAL_GPIO_ReadPin(GPIOE, sw_num[i]);
                sw_states[i] = state;
                HAL_GPIO_WritePin(GPIOD, leds_num[i], state);
            }
            if ((sw_states[0] == 0 && sw_states[2] == 0) && (sw_states[1] == 1 && sw_states[3] == 1)) {
                break;
            }
        }
        run_animation();
        for (int i = 0; i < 8; i++) {
            HAL_GPIO_WritePin(GPIOD, leds_num[i], GPIO_PIN_RESET);
        }
    }
    return 0;
}
