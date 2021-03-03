/*
 * ws2812b_matrix.h
 *
 *  Created on: 7 февр. 2021 г.
 *      Author: Acer
 */

#ifndef WS2812B_INC_WS2812B_MATRIX_H_
#define WS2812B_INC_WS2812B_MATRIX_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "ws2812b_types.h"
#include "ws2812b.h"

typedef enum {
	WS2812MTRX_PARALLEL,
	WS2812MTRX_ZIGZAG
}WS2812MTRX_Type;


typedef struct __WS2812_MatrixObj
{
	WS2812_Obj Leds;
	uint16_t Height;
	uint16_t Width;
	uint8_t Type;
}WS2812MTRX_Obj;

void WS2812MTRX_Init(WS2812MTRX_Obj *hmtrx, uint16_t width, uint16_t height, WS2812_RGB *rgb_buf);
void WS2812MTRX_SetArg(WS2812MTRX_Obj *hmtrx, void *arg);
void WS2812MTRX_SetType(WS2812MTRX_Obj *hmtrx, uint8_t type);
void WS2812MTRX_SetPixelCode(WS2812MTRX_Obj *hmtrx, uint16_t x, uint16_t y, uint32_t color);
void WS2812MTRX_SetPixelRGB(WS2812MTRX_Obj *hmtrx, uint16_t x, uint16_t y, WS2812_RGB color);
void WS2812MTRX_SetPixelHSV(WS2812MTRX_Obj *hmtrx, uint16_t x, uint16_t y, WS2812_HSV color);
WS2812_RGB WS2812MTRX_GetColorRGB(WS2812MTRX_Obj *hmtrx, uint16_t x, uint16_t y);

#ifdef __cplusplus
}
#endif

#endif /* WS2812B_INC_WS2812B_MATRIX_H_ */
