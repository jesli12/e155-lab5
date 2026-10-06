// main.c
// Jessica Li
// jesli@g.hmc.edu
// 10/5/2026

#include "main.h"
#include <stdio.h>
#include "stm32l432xx.h"

// START of given code for debug terminal print display
// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}
// END of given code for debug terminal print

int main(void) {
    // Enable two input pins for motor encoders (PA8 and PA10)
    gpioEnable(GPIO_PORT_A);
    pinMode(PIN_8, GPIO_INPUT);
    pinMode(PIN_10, GPIO_INPUT);

    GPIOA->PUPDR |= (0b01 << 2*gpioPinOffset(PIN_8)); // Set PA8 as pull-up (PUPD8 = 01)
    GPIOA->PUPDR |= (0b01 << 2*gpioPinOffset(PIN_10)); // Set PA10 as pull-up (PUPD10 = 01)

    // Initialize timer
    RCC->APB1ENR1 |= (1 << 0); // TIM2EN
    initTIM(TIMER);
    delay_millis(TIMER, 10000); // start timer, ARR of 10000
    // ************************************************ JESSSICA COME BAC ******************************

    // 1. Enable SYSCFG clock domain in RCC
    RCC->APB2ENR |= (1 << 0); // SYSCFGEN
    // 2. Configure EXTICR for the two input pins (PA8 and PA10)
    // EXTI8 is bits 2:0 of EXTICR3 (EXTICR[2] in C). Port A is 0b000, so clearing the field selects PA8.
    SYSCFG->EXTICR[2] &= ~(0b111 << 0);
    // EXTI10 is bits 10:8 of EXTICR3 (EXTICR[2] in C). Port A is 0b000, so clearing the field selects PA10.
    SYSCFG->EXTICR[2] &= ~(0b111 << 8);

    // Configure interrupt for falling edge of GPIO pin for PA8
    EXTI->IMR1 |= (1 << gpioPinOffset(PIN_8));   // 1. Configure mask bit
    EXTI->RTSR1 &= ~(1 << gpioPinOffset(PIN_8)); // 2. Enable rising edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(PIN_8));  // 3. Enable falling edge trigger
    NVIC->ISER[0] |= (1 << 23);                  // 4. Turn on EXTI interrupt in NVIC_ISER (EXTI9_5 is IRQ 23)

    // Configure interrupt for falling edge of GPIO pin for PA10
    EXTI->IMR1 |= (1 << gpioPinOffset(PIN_10));   // 1. Configure mask bit
    EXTI->RTSR1 &= ~(1 << gpioPinOffset(PIN_10)); // 2. Enable rising edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(PIN_10));  // 3. Enable falling edge trigger
    NVIC->ISER[1] |= (1 << 8);                  // 4. Turn on EXTI interrupt in NVIC_ISER (EXTI15_10 is IRQ 40, RM322)
        // note: ISER[1] cuz Programming Manual p210 says ISER1 for interrupt 32 to 63, and ISER1 for 0 to 31

    // Enable interrupts globally
    __enable_irq();
    // Set NVIC priority (PM p218), and p38, (this uses CMSIS <3)
    __NVIC_SetPriority(TIM2_IRQn, 1);  // give timer priority
    __NVIC_SetPriority(EXTI9_5_IRQn, 2);
    __NVIC_SetPriority(EXTI15_10_IRQn, 3);

    
    while(1){
        
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
// EXTI lines 15-10 share this handler
void EXTI15_10_IRQHandler(void){
    // Check that the button was what triggered our interrupt
    if (EXTI->PR1 & (1 << gpioPinOffset(BUTTON_PIN))){
        // If so, clear the interrupt (NB: Write 1 to reset.)
        EXTI->PR1 = (1 << gpioPinOffset(BUTTON_PIN));

        // Then toggle the LED
        togglePin(LED_PIN);

    }
}