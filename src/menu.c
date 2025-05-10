#include "em_chip.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include "bsp.h"
#include <stdio.h>

static uint8_t selected_program = 0; // Used for pointing at the selected problem


// The menu
const char *menu_items[] = {
    "P1: Aprindere/Stingere LED-uri\n",
    "P2: Schimbare Intensitate LED\n",
    "P3: Senzorii de Temperatura-Umiditate\n",
    "P4: Senzorul de proximitate\n"
};

void delay_ms(uint32_t ms) {
    volatile uint32_t count;
    while (ms--) {
        count = 7000;
        while (count--) __NOP(); //create delay
    }
}

void gpio_init(void) {

    CMU_ClockEnable(cmuClock_GPIO, true);
    GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
}

void menu_display(void) {
    printf("\f");
    for (int i = 0; i < 4; i++) {
        if (i == selected_program) {
            printf("> %s\n", menu_items[i]);
        } else {
            printf("  %s\n", menu_items[i]);
        }
    }
}

int main(void) {
    CHIP_Init(); // Initialize the microcontroller's chip and its peripherals
    gpio_init(); // Initialize the GPIO (General-Purpose Input/Output) pins for the buttons
    DISPLAY_Init();// Initialize the Display
    RETARGET_TextDisplayInit();// Initialize the 'text' which will be shown on the Display

    menu_display(); //show the menu

    uint8_t pb1_last = 1, pb0_last = 1;//The last state of the PB buttons. At the beginning the value is 1 (not pressed)
    uint8_t menu_delay=150;//ms

    while (1) {
        uint8_t pb1_state = GPIO_PinInGet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN); //get the current state of PB1
        uint8_t pb0_state = GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN); //get the current state of PB0

        //if pb1 button is pressed
        if (pb1_last == 1 && pb1_state == 0) {
            selected_program = (selected_program + 1) % 4; //change the selected program
            menu_display();
            delay_ms(150);
        }
        //if pb0 is pressed
        else if (pb0_last == 1 && pb0_state == 0) {
            switch(selected_program)
            {
            case 0:
            	printf("\f");
            	leduri();
            	break;
            case 1:
            	printf("\f");
            	Intensitate_LED();
            	break;
            case 2:
            	printf("\f");
            	weatherstation();
            	break;
            case 3:
                printf("\f");
                hand_position();
                break;
            }
            menu_display();
            delay_ms(menu_delay);
         }

        pb1_last = pb1_state; //Pb1's current state becomes the previous state
        pb0_last = pb0_state; //Pb0's current state becomes the previous state
    }
}
