// main.c
// Jessica Li
// jesli@g.hmc.edu
// 10/5/2026

#include "main.h"
#include <stdio.h>
#include "stm32l432xx.h"

int main(void) {
    // Enable two input pins for motor encoders (PA8 and PA10)
    gpioEnable(GPIO_PORT_A);
    pinMode(PIN_8, GPIO_INPUT);
    pinMode(PIN_10, GPIO_INPUT);
    
    GPIOA->PUPDR |= (0b01 << 2*gpioPinOffset(PIN_8)); // Set PA8 as pull-up (PUPD8 = 01)
    GPIOA->PUPDR |= (0b01 << 2*gpioPinOffset(PIN_10)); // Set PA10 as pull-up (PUPD10 = 01)

    // Initialize timer
    RCC->APB1ENR1 |= (1 << 0); // TIM2EN
    initTIM(DELAY_TIM);

    // 1. Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= (1 << 0); // SYSCFGEN
    // 2. Configure EXTICR for the input button interrupt
    // EXTI7 is bits 14:12 of EXTICR2 (EXTICR[1] in C). Port A is 0b000, so clearing the field selects PA7.
    SYSCFG->EXTICR[1] &= ~(0b111 << 12);

    // Enable interrupts globally
    __enable_irq();

    // Configure interrupt for falling edge of GPIO pin for button
    EXTI->IMR1 |= (1 << gpioPinOffset(BUTTON_PIN));   // 1. Configure mask bit
    EXTI->RTSR1 &= ~(1 << gpioPinOffset(BUTTON_PIN)); // 2. Disable rising edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(BUTTON_PIN));  // 3. Enable falling edge trigger
    NVIC->ISER[0] |= (1 << 23);                       // 4. Turn on EXTI interrupt in NVIC_ISER (EXTI9_5 is IRQ 23)

    while(1){
        delay_millis(DELAY_TIM, 200);
    }

}

// EXTI lines 5-9 share this handler
void EXTI9_5_IRQHandler(void){
    // Check that the button was what triggered our interrupt
    if (EXTI->PR1 & (1 << gpioPinOffset(BUTTON_PIN))){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 = (1 << gpioPinOffset(BUTTON_PIN));

        // Then toggle the LED
        togglePin(LED_PIN);

    }
}
