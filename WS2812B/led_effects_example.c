/*
 * led_actions.c
 *
 *  Created on: 8 февр. 2021 г.
 *      Author: serge
 */
#include "main.h"
#include "led_actions.h"
#include "random.h"

//ДИНАМИЧЕСКИЕ
void WS2812_SlowRainBow(WS2812_Obj *hws, uint16_t resolution, uint8_t brightness)
{
	static uint16_t step = 0;
	WS2812_HSV hsvTmp;
	uint16_t i;
	for(i=0;i < hws->Size; i++)
	{
		hsvTmp.h = (((step + i) % resolution) * (360 /resolution));
		hsvTmp.s = 255;
		hsvTmp.v = brightness;
		WS2812_SetPixelHSV(hws, hsvTmp, i);
	}

	step = (step + 1) % resolution;

	WS2812_Show(hws);
	WS2812_Delay_ms(hws, 33);
}
//-------------------------------------------------------
void WS2812_RunWhite(WS2812_Obj *hws, uint8_t brightness)
{
	static uint8_t pos = 0, movDir = 0, timeDelay = 0;
	WS2812_HSV outBuf[hws->Size];

	timeDelay = (timeDelay + 1) % 10;

	if (timeDelay == 0)
	{
		memset(outBuf,0,sizeof(WS2812_HSV) * hws->Size);

		if (!movDir)
		{
			if (pos < (hws->Size - 3))
				pos++;
			else
				movDir = ~movDir;
		}
		else
		{
			if (pos > 0)
				pos--;
			else
				movDir = ~movDir;
		}

	}

	outBuf[pos].v = 0.3*brightness;
	outBuf[pos + 1].v = brightness;
	outBuf[pos + 2].v = 0.3*brightness;

	for(uint16_t i=0; i < hws->Size; i++)
	{
		WS2812_SetPixelHSV(hws, outBuf[i], i);
	}

	WS2812_Show(hws);
	WS2812_Delay_ms(hws, 7);
}
//-----------------------------------------------------
void WS2812_FireTorch(WS2812_Obj *hws, uint8_t brightness)
{
	static uint8_t delay = 30;
	static uint16_t delayAspire = 30;
	static uint8_t pos = 0, cnt = 3;
	WS2812_HSV outBuf[hws->Size];


	static uint8_t h;
	uint8_t baseCnt = 4; //Количество светодиодов в основании

	memset(outBuf,0,sizeof(WS2812_HSV) * hws->Size); //Сброс буфера

	//Включение основания
	for (uint16_t i = 0; i < baseCnt; i++)
	{
		outBuf[i].v = brightness;
		outBuf[i].s = (255 - (10/hws->Size) * i * i);
		outBuf[i].h = ((((pos + i) % 12) * 24 /12) % 16) + RND_Get(0x3);
	}
	//Формирование вспышек
	for (uint16_t i = 0; i < cnt; i++)
	{
		if ((pos + i) < hws->Size)
		{
			if ((pos + i) > (cnt + 1))
			{
				outBuf[pos + i].v = (brightness/(pos + i)) + brightness*0.3;

			}
			else
			{
				outBuf[pos + i].v = brightness;
			}

			outBuf[pos + i].s = 255;
			outBuf[i].h = h;

		}
	}

	//Копирование значений на светодиоды
	for(uint16_t i=0; i < hws->Size; i++)
	{
		WS2812_SetPixelHSV(hws, outBuf[i], i);
	}

	//инкремент позиации вспышки
	pos = (pos + 1) % (hws->Size);

	if (pos == 0)
	{
		delayAspire = RND_GetRange(21, 31);
		cnt = RND_GetRange(1, 7);
		h = RND_Get(0x7) + 5;
	}
	//------------------------------
	if (delay < delayAspire)
	{
		delay++;
	}
	//------------------------------------
	if ( delay > delayAspire)
	{
		delay--;
	}



	WS2812_Show(hws);
	WS2812_Delay_ms(hws, delay);
}

//------------------------------------------------------
void WS2812_FireCandle(WS2812_Obj *hws, uint16_t resolution, uint8_t brightness)
{
	static uint16_t step = 0;
	WS2812_HSV hsvTmp;
	uint16_t i;
	static uint16_t delay = 30;
	static uint16_t res_test = 0;
	static uint16_t addition = 0;
	//uint16_t test1 = resolution;

	resolution += addition;


	for(i=0;i < hws->Size;i++)
	{
		hsvTmp.h = ((((step + i) % resolution) * 24 /resolution) % 16) + RND_Get(0x3);
		hsvTmp.s = (255 - (10 / hws->Size)*i*i);
		hsvTmp.v = (brightness / hws->Size)*i + brightness*0.01;
		WS2812_SetPixelHSV(hws, hsvTmp, (hws->Size-1)-i);
	}

	if (step == 0)
	{
		res_test = 10 + RND_Get(0x1F);
	}

	if (addition > res_test)
	{
		addition--;
	}

	if (addition < res_test)
	{
		addition++;
	}



	step = (step + 1) % resolution;



	WS2812_Show(hws);
	WS2812_Delay_ms(hws, delay);

	if (delay > 5)
	{
		delay -= 1;
	}
	else
	{
		delay = RND_Get(0x1F);
	}

}
//-------------------------------------------------------
void WS2812_StaticRainBow(WS2812_Obj *hws, uint16_t resolution, uint8_t brightness)
{
	static uint16_t step = 0;
	uint16_t i;
	WS2812_HSV hsvTmp;
	for(i=0; i < hws->Size; i++)
	{
		hsvTmp.h = (step * 360 /resolution);
		hsvTmp.s = 255;
		hsvTmp.v = brightness;
		WS2812_SetPixelHSV(hws, hsvTmp, i);
	}
	step = (step + 1) % resolution;
	WS2812_Show(hws);

	WS2812_Delay_ms(hws, 30);
}
//---------------------------------------------
void WS2812_Confetti(WS2812_Obj *hws, uint8_t brightness)
{
	static uint8_t timeDelay = 0;
	WS2812_HSV outBuf[hws->Size];

	timeDelay = (timeDelay + 1) % 10;
//Формирование задержки 200 мс
	if (timeDelay == 0)
	{
		for (uint16_t i = 0; i < 6; i++)
		{
			uint16_t ind = RND_GetRange(0, 11);
			outBuf[ind].v = brightness;
			outBuf[ind].s = 255;
			outBuf[ind].h = RND_GetRange(0, 359);
		}
	//-------------------------------------------
		for (uint16_t i = 0; i < 4; i++)
		{
			uint16_t ind = RND_GetRange(0, 11);
			outBuf[ind].v = 0;
			outBuf[ind].s = 255;
			outBuf[ind].h = 0;
		}

	}

	//-----------------------------------------
	for(uint16_t i=0; i < hws->Size; i++)
	{
		//Устанавливается текущее значение яркости для активных пикселей (для плавного вкл./выкл.)
		if (outBuf[i].v != 0)
		{
			outBuf[i].v = brightness;
		}
		WS2812_SetPixelHSV(hws, outBuf[i], i);
	}

	WS2812_Show(hws);
	WS2812_Delay_ms(hws, 20);

}

//-----------------------------------------------
void WS2812_GlowWorms(WS2812_Obj *hws, uint8_t brightness)
{
	const uint8_t quantity = 3;
	uint16_t colorBuf[quantity];

	static uint8_t pos[3];
	static uint8_t new_pos[3] = {0, 5, 10};
	static uint8_t delay[3] = {3, 3, 3};
	WS2812_HSV outBuf[hws->Size];

	memset(outBuf,0,hws->Size); //Сброс буфера

	colorBuf[0] = 0;
	colorBuf[1] = 45;
	colorBuf[2] = 320;


//--------------------------------------------
	for (uint16_t i = 0; i < quantity; i++)
	{
		outBuf[pos[i]].h = colorBuf[i];
		//Если координаты пересеклись
		if (outBuf[pos[i]].s == 0)
			outBuf[pos[i]].s = 255;
		else
			outBuf[pos[i]].s = 40;

		outBuf[pos[i]].v = brightness;
	}

//-------------------------------------------
	for(uint16_t i=0; i < quantity; i++)
	{
		if (pos[i] < new_pos[i])
		{
			pos[i]++;
			delay[i] = 2;
		}
		else if (pos[i] > new_pos[i])
		{
			pos[i]--;
			delay[i] = 2;
		}
		else
		{
			if (delay[i] > 0)
			{
				delay[i]--;
			}
			else
			{
				new_pos[i] = RND_GetRange(0, 11);
			}
		}

	}

	//Копирование значений на светодиоды
	for(uint16_t i=0; i < hws->Size; i++)
	{
		WS2812_SetPixelHSV(hws, outBuf[i], i);
	}

	WS2812_Show(hws);
	WS2812_Delay_ms(hws, 50);
}
//----------------------------------------------

//СТАТИЧЕСКИЕ
void WS2812_StaticWhite(WS2812_Obj *hws, uint8_t brightness)
{
	WS2812_HSV hsvTmp;

	hsvTmp.h = 0;
	hsvTmp.s = 0;
	hsvTmp.v = brightness;

	WS2812_SetAllHSV(hws, hsvTmp);
	WS2812_Show(hws);
	WS2812_Delay_ms(hws, 50);
}
//-----------------------------------------------
void WS2812_StaticOrange(WS2812_Obj *hws, uint8_t brightness)
{
	WS2812_HSV hsvTmp;

	hsvTmp.h = 40;
	hsvTmp.s = 255;
	hsvTmp.v = brightness;

	WS2812_SetAllHSV(hws, hsvTmp);
	WS2812_Show(hws);
	WS2812_Delay_ms(hws, 50);

}
//---------------------------------------------
void WS2812_StaticGreen(WS2812_Obj *hws, uint8_t brightness)
{
	WS2812_HSV hsvTmp;

	hsvTmp.h = 100;
	hsvTmp.s = 255;
	hsvTmp.v = brightness;

	WS2812_SetAllHSV(hws, hsvTmp);
	WS2812_Show(hws);
	WS2812_Delay_ms(hws, 50);

}
//-----------------------------------------------
void WS2812_StaticBlueLight(WS2812_Obj *hws, uint8_t brightness)
{
	WS2812_HSV hsvTmp;

	hsvTmp.h = 180;
	hsvTmp.s = 255;
	hsvTmp.v = brightness;

	WS2812_SetAllHSV(hws, hsvTmp);
	WS2812_Show(hws);
	WS2812_Delay_ms(hws, 50);

}
//--------------------------------------
void WS2812_StaticBlue(WS2812_Obj *hws, uint8_t brightness)
{
	WS2812_HSV hsvTmp;

	hsvTmp.h = 240;
	hsvTmp.s = 255;
	hsvTmp.v = brightness;

	WS2812_SetAllHSV(hws, hsvTmp);
	WS2812_Show(hws);
	WS2812_Delay_ms(hws, 50);

}
//--------------------------------------
void WS2812_StaticViolet(WS2812_Obj *hws, uint8_t brightness)
{
	WS2812_HSV hsvTmp;

	hsvTmp.h = 44;
	hsvTmp.s = 255;
	hsvTmp.v = brightness;

	WS2812_SetAllHSV(hws, hsvTmp);
	WS2812_Show(hws);
	WS2812_Delay_ms(hws, 50);

}
