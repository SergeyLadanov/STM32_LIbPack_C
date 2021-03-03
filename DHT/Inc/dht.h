#ifndef __DHT_H_
#define __DHT_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#define DHT_PIN_OUTPUT 0
#define DHT_PIN_INPUT 1
#define DHT_PIN_LOW 0
#define DHT_PIN_HIGH 1

// Структура результата измерения датчика
typedef struct __DHT_Result
{
    float Temperature;
    float Humidity;
}DHT_Result;

// Структура датчика
typedef struct __DHT_Obj
{
    DHT_Result Result;
    uint8_t Data[5];
}DHT_Obj;


void DHT_PinMode(DHT_Obj *hdht, uint8_t mode);
void DHT_PinWrite(DHT_Obj *hdht, uint8_t state);
uint8_t DHT_PinRead(DHT_Obj *hdht);
void DHT_Delay_us(DHT_Obj *hdht, uint32_t value);
void DHT_Delay_ms(DHT_Obj *hdht, uint32_t value);

DHT_Result* DHT_Read(DHT_Obj *hdht);
     
#ifdef __cplusplus
}
#endif



#endif
