#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "capsense.h"
#include "../inc/bsp.h"

#define PWM_TOP 10 // The Resolution of PWM ( the bigger the value , the brighter the intensities of the LEDS )
#define LONG_PRESS_TIME 1000  // How long you have to press the buttons in order to change the intesity (in ms)

void setupGPIO(void) {
	CMU_ClockEnable(cmuClock_GPIO, true); //Turns on the clock for GPIO
	GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0); // The Led will be put in Pin PA0

	GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1); // The initialization of PB0
	GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1); // The initialization of PB1

}

void setupTIMER0_PWM(void) {

	CMU_ClockEnable(cmuClock_TIMER0, true);//Turns on the clock for TIMER0

	TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT; // to configure the TIMER (set him to DEFAULT first)

	timerCCInit.mode = timerCCModePWM; // Set the mode to PWM (Pulse Width Modulation - used for controlling the amount of power delivered)
	timerCCInit.cmoa = timerOutputActionSet; // For turning the signal on
	timerCCInit.cofoa = timerOutputActionClear; // For turning the signal off


	// The previous settings are applied to the channel 0 of TIMER0
	TIMER_InitCC(TIMER0, 0, &timerCCInit);

	// Send the output (CC0) to a pin and set the location (LOC0)
	TIMER0->ROUTE = TIMER_ROUTE_CC0PEN | TIMER_ROUTE_LOCATION_LOC0;

	TIMER_TopSet(TIMER0, PWM_TOP); // The Top Value of TIMER0
	TIMER_CompareBufSet(TIMER0, 0, 0); //The Duty Cycle is set to 0%

	TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT; // to configure the TIMER (set him to DEFAULT first)
	timerInit.prescale = timerPrescale64; //The prescaler will divide the clock by 64
	timerInit.enable = true; //Activate the timer

	TIMER_Init(TIMER0, &timerInit); //The settings are applied to TIMER0
}

void setPWMDutyCycle(uint8_t intensity) {
	uint32_t compareValue = (PWM_TOP * intensity) / 100; // Calculate the compare value based on the given intensity
	TIMER_CompareSet(TIMER0, 0, compareValue); // Set the new compare value for channel 0
}


void Intensitate_LED(void) {
	CHIP_Init(); // Initialize the microcontroller's chip and its peripherals

	setupGPIO();// Initialize the GPIO (General-Purpose Input/Output) pins for the LED and buttons
	setupTIMER0_PWM(); // Initialize the timer for PWM

	CAPSENSE_Init(); // Initialize the CAPSENSE buttons
	DISPLAY_Init(); // Initialize the Display
	RETARGET_TextDisplayInit(); // Initialize the 'text' which will be shown on the Display

	uint8_t brightnessLevel = 0; //The Led will not be lit at the beginning
	const uint8_t levels[] = { 0, 25, 50, 75, 100 }; // The intensity levels (0%,25% etc.)

	uint8_t pb0_last = 1, pb1_last = 1; //The last state of the PB buttons. At the beginning the value is 1 (not pressed)

	// There are 5 intensity levels . The first level is 0
	uint8_t MAX_Level = 4;
	uint8_t MIN_Level = 0;

	uint32_t BUTTON0_press_time = 0, BUTTON1_press_time = 0;  // Initialize the press time of the 2 Capsesnse buttons

	printf("\f %d%%", levels[brightnessLevel]);

	while (1) {
		CAPSENSE_Sense();

		if (CAPSENSE_getPressed(BUTTON1_CHANNEL) && !CAPSENSE_getPressed(BUTTON0_CHANNEL)) {
			BUTTON1_press_time++; //The press time is increased

			// If BUTTON1 is pressed long enough , the intensity level will be changed
			if (BUTTON1_press_time > LONG_PRESS_TIME) {

				//If the current intensity is 100 , it will stay 100
				if (brightnessLevel < MAX_Level)
					brightnessLevel++;

				setPWMDutyCycle(levels[brightnessLevel]); //The Led will bright with the current intensity
				printf("\f %d%%", levels[brightnessLevel]); // Show the current intensity on the display
				BUTTON1_press_time = 0; // The press time is reset
			}
		}
		else {
			BUTTON1_press_time = 0;  // The press time is reset if the button is no longer pressed
		}


		if (CAPSENSE_getPressed(BUTTON0_CHANNEL) && !CAPSENSE_getPressed(BUTTON1_CHANNEL)) {
			BUTTON0_press_time++; //The press time is increased
			// If BUTTON1 is pressed long enough , the intensity level will be changed

			if (BUTTON0_press_time > LONG_PRESS_TIME) {

				//If the current intensity is 100 , it will stay 100
				if (brightnessLevel > MIN_Level)
					brightnessLevel--;

				setPWMDutyCycle(levels[brightnessLevel]); //The Led will bright with the current intensity

				printf("\f %d%%", levels[brightnessLevel]);// Show the current intensity on the display
				BUTTON0_press_time = 0; // The press time is reset
			}
		}
		else {
			BUTTON0_press_time = 0;  // The press time is reset if the button is no longer pressed
		}

		uint8_t pb0 = GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN); //get the current state of PB0
		uint8_t pb1 = GPIO_PinInGet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN); //get the current state of PB1

		//Return to main menu
		// if the current state of the button is 0 and the last state is 1 , it means the button has been pressed
		if (pb0_last == 1 && pb0 == 0 && pb1_last == 1 && pb1 == 0) {
			main();
		}
	}
}
