#include "em_device.h"
#include <stdint.h>
#include <stdbool.h>
#include "em_chip.h"
#include "em_cmu.h"
#include "../inc/bsp.h"
#include "../inc/bspconfig.h"
#include "em_gpio.h"

static volatile uint32_t msTicks; // Used to store the millisecond tick count

void SysTick_Handler(void) {
	// Increment the millisecond tick counter
	msTicks++;
}

static void Delay(uint32_t dlyTicks) {
	// Delay function using SysTick
	uint32_t curTicks = msTicks;
	while ((msTicks - curTicks) < dlyTicks); // Wait until the specified delay time has passed
}

static void gpioSetup(void) {
	CMU_ClockEnable(cmuClock_GPIO, true); //Turns on the clock for GPIO

	GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1); // The initialization of PB0
	GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1); // The initialization of PB1
}

void leduri(void) {
	CHIP_Init(); // Initialize the microcontroller's chip and its peripherals

	// To configure SysTick in order to generate an interrupt every millisecond
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
		while (1); // If the configuration fails, it loops forever
	}

	BSP_LedsInit(); // Initialize LED0 and LED1
	gpioSetup(); // Initialize the GPIO (General-Purpose Input/Output) pins for the buttons

	uint8_t pb0_last = 1, pb1_last = 1; //The last state of the PB buttons. At the beginning the value is 1 (not pressed)
	uint32_t pb0_press_time = 0, pb1_press_time = 0; // Initialize the press time of the 2 PB buttons
	uint32_t pb1_last_click_time = 0; // Initialize the last click time of PB1 (The last time PB1 was pressed)

	uint32_t count = 0; //count every Delay(10)
	uint32_t DEBOUNCE = 30; // 30 * Delay(10) = 300ms Debounce
	uint32_t Delay_time=10;

	bool led0_blink = false, led1_blink = false; //Check if the LEDs are blinking (They are turned off in the beginning)

	bool led0_state = false, led1_state = false; //Check if the LEDs are tuned on/off

	uint32_t led0_blink_time = 0, led1_blink_time = 0; // For how long the LEDs have been blinking

	const uint32_t LONG_PRESS = 2000; // It takes to press PB0 for at least 2000ms or 2s in order for LED0 to start blinking
	const uint32_t DOUBLE_PRESS = 600; // It takes to press PB1 for at most 600ms or 0.6s in order for LED1 to start blinking

	uint32_t blink0_time = 1000; //1 second blink
	uint32_t blink1_time = 2000; //2 second blink

	while (1) {
		printf("\f %d", count); //to the count value in real time

		// PB0
		uint8_t pb0 = GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN); //get the current state of PB0

		// if the current state of the button is 0 and the last state is 1 , it means the button has been pressed

		if (pb0_last == 1 && pb0 == 0) {
			pb0_press_time = msTicks; // msTicks has the press time of PB0
		}

		// if pb0 is long-pressed and LED0 isn't already blinking
		if (pb0 == 0 && (msTicks - pb0_press_time) > LONG_PRESS && !led0_blink) {

			//if 300ms haven't passed , nothing will happen
			if (count >= DEBOUNCE) {

				led0_blink = true; //It means the led0 must enter blinking mode

				led0_blink_time = msTicks; //store the blink time of LED0

				count = 0; //The count is reset
			}
		}

		//If pb0 was pressed last time
		if (pb0_last == 0 && pb0 == 1) {

			if (count >= DEBOUNCE) {
				if ((msTicks - pb0_press_time) < LONG_PRESS) {
				led0_state = !led0_state; //the state has changed
				led0_blink = false; //it's not blinking

				if (led0_state)
					BSP_LedSet(0);//turns on LED0
				else
					BSP_LedClear(0);//turns off LED0
				count = 0;
				}
			}
		}

		//if LED0 has been in 'blink mode' for more than 1s it needs to change its state
		if (led0_blink && (msTicks - led0_blink_time) >= blink0_time) {
			led0_state = !led0_state; //the state is changing
			if (led0_state)
				BSP_LedSet(0); //turns on LED0
			else
				BSP_LedClear(0); //turns off LED1
			led0_blink_time = msTicks; //resets the blink timer
		}



		// PB1
		uint8_t pb1 = GPIO_PinInGet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN); //get the current state of PB1

		if (pb1_last == 1 && pb1 == 0) {
			uint32_t current_time = msTicks;

			//if pb1 is double-pressed
			if ((current_time - pb1_last_click_time) < DOUBLE_PRESS) {

				led1_blink = !led1_blink; //if its not blinking already it does now and vice-versa
				pb1_last_click_time = 0;  // reset
			}
			else {
				pb1_last_click_time = current_time; //record the time when pb1 was pressed
			}
			pb1_press_time = current_time;//record how long pb1 was pressed
		}

		//If pb1 was pressed last cycle
		if (pb1_last == 0 && pb1 == 1) {
			if (count >= DEBOUNCE) {
				if ((msTicks - pb1_press_time) < LONG_PRESS) {
				led1_blink = false; //its not in blinking mode
				led1_state = !led1_state; //the state is changing
				if (led1_state)
					BSP_LedSet(1);
				else
					BSP_LedClear(1);
				count = 0;
				}
			}
		}
		//if LED1 has been in 'blink mode' for more than 2s it needs to change its state
		if (led1_blink && (msTicks - led1_blink_time) >= blink1_time) {

			led1_state = !led1_state;
			if (led1_state)
				BSP_LedSet(1);
			else
				BSP_LedClear(1);
			led1_blink_time = msTicks; //resets the blink timer


		}

		//Return to main menu
		if (pb0_last == 1 && pb0 == 0 && pb1_last == 1 && pb1 == 0) {
			if (count >= DEBOUNCE) {
				count = 0;
				main();
			}
		}

		pb1_last = pb1; //Pb1's current state becomes the previous state
		pb0_last = pb0; //Pb0's current state becomes the previous state
		Delay(Delay_time);
		count++;

	}
}
