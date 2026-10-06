# E155 Lab 5 Interrupts (MCU)

This is the code base for Jessica's E155 Lab 5, which configures interrupts for the STM32L432KC.
It configures the EXTI controller to trigger an interrupt on the falling edge of GPIO pins.
The two input pins are connected to the encoder of a DC motor. The interrupts are used to determine both the direction and speed of the motors.

## Configuration Steps

0.   **RCC**: turn on the clock to the `SYSCFG` peripheral.
1.   **SYSCFG**: set the `EXTI` mux in `SYSCFG_EXTICR` so the button's port drives its EXTI line.
2.   **EXTI**: unmask the line in the interrupt mask register (`EXTI_IMR1`) and select the falling edge
     (`EXTI_RTSR1` and `EXTI_FTSR1`).
3.   **NVIC**: turn on the interrupt in `NVIC_ISER`. The bits in the NVIC registers correspond to the interrupt
     position in the vector table, not the EXTI line.
4.   **CPU**: enable interrupts globally with `__enable_irq()`.
5.   **Vector table**: name the handler exactly as it appears in the vector table. Inside the handler, check and
     clear the pending bit in `EXTI_PR1`.
