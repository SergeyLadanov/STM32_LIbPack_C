/*
 * ws2812_if.c
 *
 *  Created on: 8 февр. 2021 г.
 *      Author: serge
 */

#include "ws2812_if.h"
#include "lamp.h"

// Получить количество обрабатываемых светодиодов исходя из размера буфера
#define GET_FRAME_LED_COUNT(X) (X / 24)
// Проверка установки бита
#define BitIsSet(reg, bit) ((reg & (1<<bit)) != 0)

typedef enum{
	HIGH = 65,
	LOW = 26
}LogicLevel;


extern TIM_HandleTypeDef htim2;
extern Lamp_Obj hlamp;
extern DMA_HandleTypeDef hdma_tim2_ch2_ch4;

static void WS2812_FrameHandler(WS2812_Obj *hws, uint32_t offset);
static void WS2812_RGB_To_DMA(WS2812_Obj *hws, uint32_t offset);

void TIM_DMAHalfPulseCplt(DMA_HandleTypeDef *hdma);

// Инициализация фрейма передачи данных
void WS2812_FrameInit(WS2812_FrameControl *hfc, uint8_t *buf, uint32_t size)
{
	hfc->Buffer = buf;
	hfc->Size = size;
	hfc->FrameOffset = 0;
	hfc->Busy = WS2812_FRAME_STOP;
}

// Передача данных на светодиоды
void WS2812_Show(WS2812_Obj *hws)
{
	WS2812_FrameControl *hframe = (WS2812_FrameControl *) hws->Arg;
	hframe->Busy = WS2812_FRAME_START;
	hframe->FrameOffset = 0;
	//Заполнение второй половины буфера данными
	WS2812_RGB_To_DMA(hws, hframe->Size/2);
	//Заполнение первой половины буфера нулями
	memset(hframe->Buffer, 0, hframe->Size/2);
	DMA1_Channel7->CCR |= DMA_CCR_HTIE;
	HAL_TIM_PWM_Start_DMA(&htim2,TIM_CHANNEL_2, (uint32_t*)hframe->Buffer, hframe->Size);
}

void WS2812_Delay_ms(WS2812_Obj *hws, volatile uint32_t value)
{
	HAL_Delay(value);
}

// Обработчик половины переданных данных на светодиоды
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim2)
	{
		WS2812_FrameHandler(&hlamp.Hmtrx.Leds, 0);
	}
}

//----------------------------------------------------------
// Обработчик завершения передачи данных на светодиоды
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	WS2812_FrameControl *hframe = (WS2812_FrameControl *) hlamp.Hmtrx.Leds.Arg;
	if (htim == &htim2)
	{
		WS2812_FrameHandler(&hlamp.Hmtrx.Leds, hframe->Size/2);
	}
}


// Функция подготовки буфера DMA
static void WS2812_RGB_To_DMA(WS2812_Obj *hws, uint32_t offset)
{
	WS2812_FrameControl *hframe = (WS2812_FrameControl *) hws->Arg;
	volatile uint16_t j;
	uint8_t i;

	for(j=0; j < GET_FRAME_LED_COUNT(hframe->Size)/2; j++)
	{
		for(i=0; i<8; i++)
		{
			if (BitIsSet(hws->RGB_Buf[j + hframe->FrameOffset].r,(7-i)) == 1)
			{
				hframe->Buffer[offset + j*24+i+8] = HIGH;
			}
			else
			{
				hframe->Buffer[offset + j*24+i+8] = LOW;
			}
			if (BitIsSet(hws->RGB_Buf[j + hframe->FrameOffset].g,(7-i)) == 1)
			{
				hframe->Buffer[offset + j*24+i+0] = HIGH;
			}
			else
			{
				hframe->Buffer[offset + j*24+i+0] = LOW;
			}
			if (BitIsSet(hws->RGB_Buf[j + hframe->FrameOffset].b,(7-i)) == 1)
			{
				hframe->Buffer[offset + j*24+i+16] = HIGH;
			}
			else
			{
				hframe->Buffer[offset + j*24+i+16] = LOW;
			}
		}
	}
}

// Обработчик пакетов передачи данных на светодиоды
static void WS2812_FrameHandler(WS2812_Obj *hws, uint32_t offset)
{
	WS2812_FrameControl *hframe = (WS2812_FrameControl *) hws->Arg;


	// Загрузка очередных данных
	if (hframe->Busy == WS2812_FRAME_START)
	{
		if (hframe->FrameOffset < (hws->Size - (GET_FRAME_LED_COUNT(hframe->Size)/2)))
		{
			hframe->FrameOffset += (GET_FRAME_LED_COUNT(hframe->Size)/2);
			WS2812_RGB_To_DMA(hws, offset);
		}
		else
		{
			if (!offset)
			{
				memset(hframe->Buffer, 0, hframe->Size/2);
				hframe->Busy = WS2812_FRAME_STOP_REQUEST;
			}
		}
	}
	// Остановка кадра
	if (offset)
	{
		if (hframe->Busy == WS2812_FRAME_STOP_REQUEST)
		{
			hframe->Busy = WS2812_FRAME_STOP;
			HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_2);
		}
	}
}




