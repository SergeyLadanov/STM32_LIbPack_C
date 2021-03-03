#ifndef __DSP_FILTERS_H_
#define __DSP_FILTERS_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// Обработчик медианного фильтра из N значений
typedef struct{
	float *Buf;
	uint8_t Cnt;
    uint32_t Size;
}DSP_MFN_Obj;

// Обработчик медианного фильтра из трех значений
typedef struct{
	float Buf[3];
	uint8_t Cnt;
}DSP_MF3_Obj;



// Обработчик фильтра первого порядка
typedef struct{
    float X1; // Предыдущее значение входного сигнала
    float Y1; // Предыдущее значение фильтра
    float KX0; // Коэффициент текущего значения входного сигнала
    float KX1; // Коэффициент предыдущего значения входного сигнала
    float KY1; // Коэффициент предыдущего значения выходного сигнала
}DSP_LPF1_Obj;


void DSP_LPF1_Init(DSP_LPF1_Obj *hFilter, float kx0, float kx1, float ky1);
float DSP_LPF1_Handle(DSP_LPF1_Obj *hFilter, float X0);
void DSP_MFN_Init(DSP_MFN_Obj *hFilter, float *buf, uint32_t size);
void DSP_MFN_DeInit(DSP_MFN_Obj *hFilter);
float DSP_MFN_Handle(DSP_MFN_Obj *hFilter, float newVal);
float DSP_MF3_Handle(DSP_MF3_Obj *hFilter, float newVal);
	 
#ifdef __cplusplus
}
#endif


#endif
