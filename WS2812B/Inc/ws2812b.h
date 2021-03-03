/*
 * ws2812b.h
 *
 *  Created on: 7 февр. 2021 г.
 *      Author: Acer
 */

#ifndef WS2812B_INC_WS2812B_H_
#define WS2812B_INC_WS2812B_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "ws2812b_types.h"

#ifndef UNUSED
#define UNUSED(X) (void)X
#endif

#ifndef __weak
#define __weak __attribute__((weak))
#endif


typedef struct __WS2812_Obj
{
	void *Arg;
	WS2812_RGB *RGB_Buf;
	uint32_t Size;
}WS2812_Obj;


void WS2812_SetPixelCode(WS2812_Obj *hws, uint32_t color, uint32_t pos);
void WS2812_SetAllCode(WS2812_Obj *hws, uint32_t color);
void WS2812_Init(WS2812_Obj *hws, WS2812_RGB *rgb_buf, uint32_t size);
void WS2812_SetArg(WS2812_Obj *hws, void *arg);
void WS2812_SetPixelRGB(WS2812_Obj *hws, WS2812_RGB color, uint32_t pos);
void WS2812_SetPixelHSV(WS2812_Obj *hws, WS2812_HSV color, uint32_t pos);
void WS2812_SetAllHSV(WS2812_Obj *hws, WS2812_HSV color);
void WS2812_SetAllRGB(WS2812_Obj *hws, WS2812_RGB color);
void WS2812_Show(WS2812_Obj *hws);
void WS2812_Delay_ms(WS2812_Obj *hws, volatile uint32_t value);
void WS2812_Clear(WS2812_Obj *hws);

uint32_t WS2812_RGB_To_Code(WS2812_RGB color);
WS2812_RGB WS2812_HsvToRgb(WS2812_HSV hsv);



#ifdef __cplusplus
}
#endif

#endif /* WS2812B_INC_WS2812B_H_ */
