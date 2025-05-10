#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "i2cspm.h"
#include "si114x_algorithm.h"

#define DISPLAY_WIDTH  21
#define DISPLAY_HEIGHT 14

// The point's position
int pointX = DISPLAY_WIDTH / 2;
int pointY = DISPLAY_HEIGHT / 2;

volatile uint32_t msTicks = 0;

void Delay(uint32_t ms) {
  uint32_t start = msTicks;
  while ((msTicks - start) < ms);
}

void drawPoint() {

  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      if (x == pointX && y == pointY)
        printf("0");
      else
        printf(".");
    }
    printf("\n");
  }
  printf("\n");
}


void initSystem() {
  CHIP_Init();

  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
  I2CSPM_Init(&i2cInit);


  Si1147_ConfigureDetection(I2C0, SI1147_ADDR, 0);

  SysTick_Config(SystemCoreClock / 1000);
}

void hand_position(void) {
  initSystem();
  while (1) {
    gesture_t gesture = Si1147_NewSample(I2C0, SI1147_ADDR, msTicks);
    switch (gesture) {
      case UP:
        if (pointY > 0) pointY--;
        break;
      case DOWN:
        if (pointY < DISPLAY_HEIGHT - 1) pointY++;
        break;
      case LEFT:
        if (pointX > 0) pointX--;
        break;
      case RIGHT:
        if (pointX < DISPLAY_WIDTH - 1) pointX++;
        break;
      default:
        break;
    }

    drawPoint();
    Delay(200);
  }
}
