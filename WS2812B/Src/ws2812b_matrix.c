/*
 * ws2812b_matrix.c
 *
 *  Created on: 7 февр. 2021 г.
 *      Author: Acer
 */
#include "ws2812b_matrix.h"


#define MAX_POSITION 0xFFFFFFFF

// Инициализация светодиодной матрицы
void WS2812MTRX_Init(WS2812MTRX_Obj *hmtrx, uint16_t width, uint16_t height, WS2812_RGB *rgb_buf)
{
	hmtrx->Height = height;
	hmtrx->Width = width;
	hmtrx->Leds.RGB_Buf = rgb_buf;
	hmtrx->Leds.Size = (uint32_t) (width * height);
}

// Передача аргумента
void WS2812MTRX_SetArg(WS2812MTRX_Obj *hmtrx, void *arg)
{
	hmtrx->Leds.Arg = arg;
}

// Установка типа
void WS2812MTRX_SetType(WS2812MTRX_Obj *hmtrx, uint8_t type)
{
	hmtrx->Type = type;
}

// Получение позиции пикселя исходя из заданных координат
static uint32_t WS2812MTRX_GetPosition(WS2812MTRX_Obj *hmtrx, uint16_t x, uint16_t y)
{
	uint32_t position = 0;

	if ((x >= hmtrx->Width) || (y >= hmtrx->Height))
	{
		return MAX_POSITION;
	}

	if (hmtrx->Type == WS2812MTRX_ZIGZAG)
	{
		// Проверка на четность
		if ((y % 2) != 0)
		{
			x = (hmtrx->Width - 1) - x;
		}
	}

	position = x + (y * hmtrx->Width);


	return position;
}


// Установка пикселя по коду цвета
void WS2812MTRX_SetPixelCode(WS2812MTRX_Obj *hmtrx, uint16_t x, uint16_t y, uint32_t color)
{
	uint32_t pos = WS2812MTRX_GetPosition(hmtrx, x, y);
	if (pos != MAX_POSITION)
	{
		WS2812_SetPixelCode(&hmtrx->Leds, color, pos);
	}
}

// Установка пикселя в формате RGB
void WS2812MTRX_SetPixelRGB(WS2812MTRX_Obj *hmtrx, uint16_t x, uint16_t y, WS2812_RGB color)
{
	uint32_t pos = WS2812MTRX_GetPosition(hmtrx, x, y);
	if (pos != MAX_POSITION)
	{
		WS2812_SetPixelRGB(&hmtrx->Leds, color, pos);
	}
}

// Установка пикселя в формате HSV
void WS2812MTRX_SetPixelHSV(WS2812MTRX_Obj *hmtrx, uint16_t x, uint16_t y, WS2812_HSV color)
{
	uint32_t pos = WS2812MTRX_GetPosition(hmtrx, x, y);
	if (pos != MAX_POSITION)
	{
		WS2812_SetPixelHSV(&hmtrx->Leds, color, pos);
	}
}


// Получение цвета пикселя по координатам в формате RGB
WS2812_RGB WS2812MTRX_GetColorRGB(WS2812MTRX_Obj *hmtrx, uint16_t x, uint16_t y)
{
	uint32_t pos = WS2812MTRX_GetPosition(hmtrx, x, y);
	WS2812_RGB result = {0};

	if (pos != MAX_POSITION)
	{
		result = hmtrx->Leds.RGB_Buf[pos];
	}

	return result;
}

