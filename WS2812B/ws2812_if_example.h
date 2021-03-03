/*
 * ws2812_if.h
 *
 *  Created on: 8 февр. 2021 г.
 *      Author: serge
 */

#ifndef INC_WS2812_IF_H_
#define INC_WS2812_IF_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include <ws2812b.h>
#include "string.h"

// Получить размер буфера в байтах на основе количество обрабатываемых светодиодов
#define WS2812_GET_FRAME_BUFFER_SIZE(X) (X * 24)


// Структура управления пакетом данных
typedef struct
{
	uint8_t FrameOffset;
	uint8_t Busy;
	uint8_t *Buffer;
	uint32_t Size;
}WS2812_FrameControl;

// Статусы передачи пакета
typedef enum
{
	WS2812_FRAME_STOP = 0,
	WS2812_FRAME_STOP_REQUEST,
	WS2812_FRAME_START
} WS2812_FrameStatus;


void WS2812_FrameInit(WS2812_FrameControl *hfc, uint8_t *buf, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* INC_WS2812_IF_H_ */
