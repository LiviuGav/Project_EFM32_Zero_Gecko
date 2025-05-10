#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "i2cspm.h"
#include "si7013.h"
#include "i2cspm.h"
#include "si114x_algorithm.h"
#include "sl_sleeptimer.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include <stdio.h>
#include "bspconfig.h"
#include "bsp.h"

#define PERIODIC_UPDATE_MS 1000 //1000ms (1s) timer
#define PAS_HISTEREZIS 500  // step value for hysteresis (0.5 degrees Celsius)
#define PAS_AMBIANT 500  // step value for the ambient temperature (0.5 degrees Celsius)

static volatile bool updateMeasurement = true; //it is time for the sensor to read
static sl_sleeptimer_timer_handle_t periodicUpdateTimerId;

static int32_t temperatura_ambiant = 24000; // Initial values for ambient temperature in millidegrees Celsius (24.0°C)
static int32_t max_ambiant = 27000;//Max value for ambient temperature
static int32_t min_ambiant = 20000;//Min value for ambient temperature


static int32_t hysteresis = 500; // Initial values for hysteresis in millidegrees Celsius (0.5°C)
static int32_t max_hysteresis = 2500; // Hysteresis Max Value = 2.5°C
static int32_t min_hysteresis = 0; // Histerezis Min Value = 0.0°C

static uint32_t rhData = 0; //Initialize the umidity value which will be given by the sensor
static int32_t tempData = 0; //Initialize the temperature value which will be given by the sensor
static uint16_t uvData = 0;//Initialize the UV Index which will be given by the sensor

/* Prototipuri funcții */
static void gpioSetup(void);
static int performMeasurements(uint32_t *rhData, int32_t *tData, uint16_t *uvData);
static void periodicUpdateCallback(sl_sleeptimer_timer_handle_t *handle, void *data);
static bool checkSensors(void);
static void sensorErrorHandler(void);
static void controlLeds(int32_t temperature);

void weatherstation(void)
{
	I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;//Initialize I2C
    CHIP_Init(); // Initialize the microcontroller's chip and its peripherals
    gpioSetup(); // Initialize the GPIO (General-Purpose Input/Output) pins for the buttons


    DISPLAY_Init(); // Initialize the Display
    RETARGET_TextDisplayInit();// Initialize the 'text' which will be shown on the Display
    printf("\fInitializare...\n");


    CMU_ClockEnable(cmuClock_HFLE, true);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    sl_sleeptimer_init();

    I2CSPM_Init(&i2cInit); //Starts I2C
    printf("I2C initializat\n");

    if(!checkSensors()) {
        sensorErrorHandler();
    }
    else {
        // Inițializează senzorul UV
        if (Si1147_ConfigureDetection(I2C0, SI1147_ADDR,1) != 0) {
            printf("Configurare Si1147 eșuată!");
        } else {
            printf("Si1147 configurat cu succes.");
        }
    }

    sl_sleeptimer_start_periodic_timer_ms(&periodicUpdateTimerId,
                                        PERIODIC_UPDATE_MS,
                                        periodicUpdateCallback,
                                        NULL, 0, 0);
    uint8_t pb0_last = 1, pb1_last = 1; //The last state of the PB buttons. At the beginning the value is 1 (not pressed)
    printf("\f\nT = Temperatura (C)\nRH = Umiditatea (%%)\nUV = UV Index \nH = Histerezis (C)\nTa=Temperatura ambianta (C)\n");
    printf("PB0 LONG PRESS - Schimbare Ta\n");
    printf("PB1 LONG PRESS - Schimbare H\n");
    printf("\n  T   RH  UV  H   Ta\n");

    while (1) {

    	uint8_t pb0 = GPIO_PinInGet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN);
    	uint8_t pb1 = GPIO_PinInGet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN);

	//Return to main menu
    	if (pb0_last == 1 && pb0 == 0 && pb1_last == 1 && pb1 == 0) {
    	            main();
    	 }
	//Change the value of hysteresis by pressing pb0
    	if (pb1_last == 1 && pb1 == 0  && pb0 != 0) {
    	     if(hysteresis < max_hysteresis)
    	    	 hysteresis += PAS_HISTEREZIS;
    	     else
    	    	 hysteresis = min_hysteresis;
    	 }
	//Change the value of the ambient temperature by pressing pb1
    	 if (pb0_last == 1 && pb0 == 0  && pb1 != 0) {
    		 if(temperatura_ambiant < max_ambiant)
    	    	  temperatura_ambiant += PAS_AMBIANT;
    	     else
    	    	  temperatura_ambiant = min_ambiant;
    	 }


        if(performMeasurements(&rhData, &tempData, &uvData) == 0) {
	//The readings of the sensors , alongside the hysteresis and the ambient temperature are printed
           printf("\r%2ld.%01ld\t%2ld.%01ld%2d\t%2ld.%01ld\t%2ld.%01ld",
            	        tempData / 1000, abs((tempData % 1000) / 100),
            	        rhData / 1000, abs((rhData % 1000) / 100),
            	        uvData,
            	        hysteresis / 1000, abs((hysteresis % 1000) / 100),
            	        temperatura_ambiant / 1000,abs((temperatura_ambiant % 1000) / 100));

          }


        controlLeds(tempData);//set the leds according to the given conditions
        EMU_EnterEM2(false);
    }
}

static void controlLeds(int32_t temperature) {
    int32_t temperatureUpper = temperatura_ambiant + hysteresis;
    int32_t temperatureLower = temperatura_ambiant - hysteresis;
//if the temperature is smaller than the difference , LED0 will be turned on and LED1 off
    if (temperature < temperatureLower) {
    	BSP_LedSet(0);
        BSP_LedClear(1);
//if the temperature is bigger than the sum , LED1 will be turned on and LED0 off
    } else if (temperature > temperatureUpper) {
    	BSP_LedSet(1);
    	BSP_LedClear(0);
 //otherwise, both will be turned off
    } else {
    	BSP_LedClear(0);
    	BSP_LedClear(1);
    }
}

static bool checkSensors(void) {
    bool si7013_ok = Si7013_Detect(I2C0, SI7013_ADDR, NULL);//detect the Si7013 sensor (used for temperature and humidity reading)
    bool si1147_ok = Si1147_Detect_Device(I2C0, SI1147_ADDR);//detect the Si1147 sensor (used for UV Index reading)

    printf("Si7013: %s\n", si7013_ok ? "OK" : "EROARE");// does it detect the sensor or not?
    printf("Si1147: %s\n", si1147_ok ? "OK" : "EROARE");// does it detect the sensor or not?

    if(!si7013_ok || !si1147_ok) {
        printf("Senzori neconectati!\n");
        return false;
    }
    return true;
}


static void sensorErrorHandler(void) {
    while(1) {
        printf("Verifica conexiunile!\n");
        delay_ms(2000);
        if(checkSensors()) {
            break;
        }
    }
}
static int performMeasurements(uint32_t *rhData, int32_t *tData, uint16_t *uvData) {


    int objectDetect;
	//Si7013 measures the temperature and the umidity
    if (Si7013_MeasureRHAndTemp(I2C0, SI7013_ADDR, rhData, tData) != 0) {
        printf("Eroare citire Si7013!\n");
        return -1;
    }


    delay_ms(200);

	//Si1147 measures the UV
    if (Si1147_MeasureUVAndObjectPresent(I2C0, SI1147_ADDR, uvData, &objectDetect) != 0) {
         printf("Eroare citire Si1147!\n");
         return -4;
    }



    if (*tData < -40000 || *tData > 85000) return -2;  // Temperature between -40°C și +85°C
    if (*rhData > 100000) return -3;                      // Humidity between 0-100%
	 //-1 -2 -3 si -4 are error codes
    return 0;
}


static void gpioSetup(void) {
    CMU_ClockEnable(cmuClock_GPIO, true);//Turns on the clock for GPIO
    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);// The initialization of PB0
    GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);// The initialization of PB1
}

static void periodicUpdateCallback(sl_sleeptimer_timer_handle_t *handle, void *data) {
//for compatibility ( displaypalemlib.c needs it)
    (void)handle; (void)data;
    updateMeasurement = true;
}

int RtcIntCallbackRegister(void (*callback)(void*), void* arg, unsigned int freq) {
    (void)callback; (void)arg; (void)freq;
    return 0;
}
