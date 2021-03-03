/*
 * ws2812b.c
 *
 *  Created on: 7 февр. 2021 г.
 *      Author: Acer
 */

#include "ws2812b.h"



// Отправка данных на светодиоды
__weak void WS2812_Show(WS2812_Obj *hws)
{
	UNUSED(hws);
}

// Функция задержки
__weak void WS2812_Delay_ms(WS2812_Obj *hws, volatile uint32_t value)
{
	UNUSED(hws);
	UNUSED(value);
}

// Инициализация объекта
void WS2812_Init(WS2812_Obj *hws, WS2812_RGB *rgb_buf, uint32_t size)
{
	hws->RGB_Buf = rgb_buf;
	hws->Size = size;
}

// Добавление аргумента
void WS2812_SetArg(WS2812_Obj *hws, void *arg)
{
	hws->Arg = arg;
}

// Установка цвета пикселя по цветовому коду
void WS2812_SetPixelCode(WS2812_Obj *hws, uint32_t color, uint32_t pos)
{
	hws->RGB_Buf[pos].r = (uint8_t)(color >> 16);
	hws->RGB_Buf[pos].g = (uint8_t)(color >> 8);
	hws->RGB_Buf[pos].b = (uint8_t)(color);
}


// Установка всех пикселей по коду
void WS2812_SetAllCode(WS2812_Obj *hws, uint32_t color)
{
	for (uint16_t i = 0; i < hws->Size; i++)
	{
		WS2812_SetPixelCode(hws, color, i);
	}
}

// Установка цвета пикселя в формате RGB
void WS2812_SetPixelRGB(WS2812_Obj *hws, WS2812_RGB color, uint32_t pos)
{
	hws->RGB_Buf[pos] = color;
}

// Установка цвета всех пикселей в формате RGB
void WS2812_SetAllRGB(WS2812_Obj *hws, WS2812_RGB color)
{
	for (uint16_t i = 0; i < hws->Size; i++)
	{
		WS2812_SetPixelRGB(hws, color, i);
	}
}

// Установка цвета пикслея в формате HSV
void WS2812_SetPixelHSV(WS2812_Obj *hws, WS2812_HSV color, uint32_t pos)
{
	hws->RGB_Buf[pos] = WS2812_HsvToRgb(color);
}


// Установка цвета всех пикселей в формате HSV
void WS2812_SetAllHSV(WS2812_Obj *hws, WS2812_HSV color)
{
	for (uint16_t i = 0; i < hws->Size; i++)
	{
		WS2812_SetPixelHSV(hws, color, i);
	}
}

// Погасить все светодиоды
void WS2812_Clear(WS2812_Obj *hws)
{
	WS2812_SetAllCode(hws, 0x00000000);
}

// Выключение всех светодиодов
void WS2812_Off(WS2812_Obj *hws)
{
	WS2812_Clear(hws);
	WS2812_Show(hws);
	WS2812_Delay_ms(hws, 5);
}

// Преобразование формата HSV в RGB
WS2812_RGB WS2812_HsvToRgb(WS2812_HSV hsv)
{
	uint8_t color[3] = {0};
	uint32_t base_V;
	WS2812_RGB rgb;

	if (hsv.s == 0)
	{
		color[0] = hsv.v;
		color[1] = hsv.v;
		color[2] = hsv.v;
	}
	else
	{
		base_V = ((255 - hsv.s) * hsv.v) >> 8;
		switch (hsv.h / 60)
		{
			case 0:
				color[0] = hsv.v;
				color[1] = (((hsv.v - base_V) * hsv.h) / 60) + base_V;
				color[2] = base_V;
			break;
			case 1:
				color[0] = (((hsv.v - base_V) * (60 - (hsv.h % 60))) / 60) + base_V;
				color[1] = hsv.v;
				color[2] = base_V;
			break;
			case 2:
				color[0] = base_V;
				color[1] = hsv.v;
				color[2] = (((hsv.v - base_V) * (hsv.h % 60)) / 60) + base_V;
			break;
			case 3:
				color[0] = base_V;
				color[1] = (((hsv.v - base_V) * (60 - (hsv.h % 60))) / 60) + base_V;
				color[2] = hsv.v;
			break;
			case 4:
				color[0] = (((hsv.v - base_V) * (hsv.h % 60)) / 60) + base_V;
				color[1] = base_V;
				color[2] = hsv.v;
			break;
			case 5:
				color[0] = hsv.v;
				color[1] = base_V;
				color[2] = (((hsv.v - base_V) * (60 - (hsv.h % 60))) / 60) + base_V;
			break;
		}
	}
	rgb.r = color[0];
	rgb.g = color[1];
	rgb.b = color[2];

	return rgb;
}

// Преобразование RGB в код
uint32_t WS2812_RGB_To_Code(WS2812_RGB color)
{
	return (((uint32_t)color.raw[0] << 16) | ((uint32_t)color.raw[1] << 8 ) | (uint32_t)color.raw[2]);
}
