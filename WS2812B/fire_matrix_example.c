/*
 * fire_mode.cpp
 *
 *  Created on: 17 февр. 2021 г.
 *      Author: serge
 */

#include <fire_mode.h>
#include <string.h>
#include <stdlib.h>
#include "random.h"
// настройки ленты
#define M_WIDTH 16      // ширина матрицы
#define M_HEIGHT 16      // высота матрицы
#define LED_PIN 4       // пин ленты
#define LED_BRIGHT 200  // яркость

// настройки пламени
#define FIRE_SCALE 35   // масштаб огня
#define HUE_GAP 60      // заброс по hue
#define FIRE_STEP 15    // шаг изменения "языков" пламени
#define HUE_START 5     // начальный цвет огня (0 красный, 80 зелёный, 140 молния, 190 розовый)
#define HUE_COEF 0.7    // коэффициент цвета огня (чем больше - тем дальше заброс по цвету)
#define SMOOTH_K 0.15   // коэффициент плавности огня
#define MIN_BRIGHT 50   // мин. яркость огня
#define MAX_BRIGHT 255  // макс. яркость огня
#define MIN_SAT 180     // мин. насыщенность
#define MAX_SAT 255     // макс. насыщенность

#define ORDER_GRB       // порядок цветов ORDER_GRB / ORDER_RGB / ORDER_BRG
#define COLOR_DEBTH 3   // цветовая глубина: 1, 2, 3 (в байтах)


#define NUM_LEDS M_WIDTH * M_HEIGHT


static unsigned char matrixValue[8][16];
static unsigned char line[M_WIDTH];
static int pcnt = 0;

// ленивая жопа
#define FOR_i(from, to) for(int i = (from); i < (to); i++)
#define FOR_j(from, to) for(int j = (from); j < (to); j++)



static inline int max(int a, int b)
{
	return (a > b) ? a : b;
}


// Прототипы функций
static void generateLine();
static void fireRoutine(WS2812MTRX_Obj *hmtrx, uint8_t brightness);
static void shiftUp();
static void drawFrame(int pcnt, WS2812MTRX_Obj *hmtrx, uint8_t brightness);


// Инициализация эффетка огня
void Fire_Init() {
  //matrix.setBrightness(LED_BRIGHT);  // яркость (0-255)
  generateLine();
  memset(matrixValue, 0, sizeof(matrixValue));
}


// Отрисовка эффекта огня
void Fire_Handle(WS2812MTRX_Obj *hmtrx, uint8_t brightness) {
  fireRoutine(hmtrx, brightness);
}

// ********************** огонь **********************
//these values are substracetd from the generated values to give a shape to the animation
static const unsigned char valueMask[8][16] = {
  {32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 , 32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 },
  {64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 , 64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 },
  {96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 , 96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 },
  {128, 64 , 32 , 0  , 0  , 32 , 64 , 128, 128, 64 , 32 , 0  , 0  , 32 , 64 , 128},
  {160, 96 , 64 , 32 , 32 , 64 , 96 , 160, 160, 96 , 64 , 32 , 32 , 64 , 96 , 160},
  {192, 128, 96 , 64 , 64 , 96 , 128, 192, 192, 128, 96 , 64 , 64 , 96 , 128, 192},
  {255, 160, 128, 96 , 96 , 128, 160, 255, 255, 160, 128, 96 , 96 , 128, 160, 255},
  {255, 192, 160, 128, 128, 160, 192, 255, 255, 192, 160, 128, 128, 160, 192, 255}
};

//these are the hues for the fire,
//should be between 0 (red) to about 25 (yellow)
static const unsigned char hueMask[8][16] = {
  {1 , 11, 19, 25, 25, 22, 11, 1 , 1 , 11, 19, 25, 25, 22, 11, 1 },
  {1 , 8 , 13, 19, 25, 19, 8 , 1 , 1 , 8 , 13, 19, 25, 19, 8 , 1 },
  {1 , 8 , 13, 16, 19, 16, 8 , 1 , 1 , 8 , 13, 16, 19, 16, 8 , 1 },
  {1 , 5 , 11, 13, 13, 13, 5 , 1 , 1 , 5 , 11, 13, 13, 13, 5 , 1 },
  {1 , 5 , 11, 11, 11, 11, 5 , 1 , 1 , 5 , 11, 11, 11, 11, 5 , 1 },
  {0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 , 0 , 1 , 5 , 8 , 8 , 5 , 1 , 0 },
  {0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 , 0 , 0 , 1 , 5 , 5 , 1 , 0 , 0 },
  {0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 1 , 1 , 0 , 0 , 0 }
};

static void fireRoutine(WS2812MTRX_Obj *hmtrx, uint8_t brightness) {
  //static uint32_t prevTime = 0;

    if (pcnt >= 100) {
      shiftUp();
      generateLine();
      pcnt = 0;
    }
    drawFrame(pcnt, hmtrx, brightness);
    pcnt += 30;

    WS2812_Show(&hmtrx->Leds);
    HAL_Delay(30);

}


// Randomly generate the next line (matrix row)

static void generateLine() {
  for (uint8_t x = 0; x < M_WIDTH; x++) {
    line[x] = RND_GetRange(64, 255);
  }
}

//shift all values in the matrix up one row

static void shiftUp() {
  for (uint8_t y = M_HEIGHT - 1; y > 0; y--) {
    for (uint8_t x = 0; x < M_WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15) newX = x % 16;
      if (y > 7) continue;
      matrixValue[y][newX] = matrixValue[y - 1][newX];
    }
  }

  for (uint8_t x = 0; x < M_WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x % 16;
    matrixValue[0][newX] = line[newX];
  }
}

// draw a frame, interpolating between 2 "key frames"
// @param pcnt percentage of interpolation

static void drawFrame(int pcnt, WS2812MTRX_Obj *hmtrx, uint8_t brightness) {
  int nextv;

  float V_Corr = (float) brightness/0xFF;
  //each row interpolates with the one before it
  for (unsigned char y = M_HEIGHT - 1; y > 0; y--) {
    for (unsigned char x = 0; x < M_WIDTH; x++) {
      uint8_t newX = x;
      if (x > 15)
    	  newX = x % 16;
      if (y < 8)
      {
        nextv =
          (((100.0 - pcnt) * matrixValue[y][newX]
            + pcnt * matrixValue[y - 1][newX]) / 100.0)
          - valueMask[y][newX];

        WS2812_HSV color;


        color.h = HUE_START + hueMask[y][newX];
        color.s = 255;
        color.v = (uint8_t)max(0, nextv) * V_Corr;

        WS2812MTRX_SetPixelHSV(hmtrx, x, y, color);





      }
      else if (y == 8)
      {
        if (RND_GetRange(0, 20) == 0 && WS2812_RGB_To_Code(WS2812MTRX_GetColorRGB(hmtrx, x, y-1)) != 0)
            WS2812MTRX_SetPixelRGB(hmtrx, x, y, WS2812MTRX_GetColorRGB(hmtrx, x, y-1));
        else
            WS2812MTRX_SetPixelCode(hmtrx, x, y, 0);
      }
      else if (true)
      {

        // старая версия для яркости
        if (WS2812_RGB_To_Code(WS2812MTRX_GetColorRGB(hmtrx, x, y-1)) > 0)
        	WS2812MTRX_SetPixelRGB(hmtrx, x, y, WS2812MTRX_GetColorRGB(hmtrx, x, y-1));
        else
        	WS2812MTRX_SetPixelCode(hmtrx, x, y, 0);
      }
    }
  }

  //first row interpolates with the "next" line
  for (unsigned char x = 0; x < M_WIDTH; x++) {
    uint8_t newX = x;
    if (x > 15) newX = x % 16;
    WS2812_HSV color;
    color.h = HUE_START + hueMask[0][newX]; // H
    color.s = 255;
    color.v = (uint8_t)(((100.0 - pcnt) * matrixValue[0][newX] + pcnt * line[newX]) / 100.0) * V_Corr;

    WS2812MTRX_SetPixelHSV(hmtrx, x, 0, color);
  }
}
