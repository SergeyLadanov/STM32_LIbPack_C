/*
 * serial.h
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: Acer
 */

#ifndef SERIAL_INC_SERIAL_H_
#define SERIAL_INC_SERIAL_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "string.h"
#include <stdlib.h>

//Настройки библиотеки
//#define SERIAL_RING_BUF_LEN 1536
#define SERIAL_READ_BUF_LEN 1024
//--------------------------------------
typedef struct{
	void *Arg;
	uint8_t *RingBuffer; //Приемный кольцевой буфер
	uint32_t Size; // Размер приемного кольцевого буфера
	uint16_t WriteIndex; //Индекс записи в кольцевой буфер
	uint16_t UnReadCnt; //Количество непрочитанных байт
	uint16_t RecTimeOutCnt; //Переменная счета времени таймаута приема
	uint16_t TimeOutValue; //Значение таймаута
} SerialTypeDef;
//-----------------------------------------------
//Прототипы функций
void Serial_SysTick(SerialTypeDef *hs);
void Serial_ByteHandle(SerialTypeDef *hs, uint8_t data);
uint8_t Serial_SendData(SerialTypeDef *hs, uint8_t *data, uint16_t len);
uint8_t Serial_GetReceivedByte(SerialTypeDef *hs);
uint16_t Serial_Available(SerialTypeDef *hs);
char* Serial_GetBytes(SerialTypeDef *hs);
void Serial_SetTimeOutVal(SerialTypeDef *hs, volatile uint16_t val);
char* Serial_GetBytesUntil(SerialTypeDef *hs, char terminator);
uint16_t Serial_GetBytesUntilToBuf(SerialTypeDef *hs, uint8_t *rxBuf, char terminator, uint16_t maxLength);
uint16_t Serial_GetBytesToBuf(SerialTypeDef *hs, uint8_t *rxBuf, uint16_t maxLength);
char Serial_GetLastChar(SerialTypeDef *hs);
uint8_t Serial_LastSymbolIs(SerialTypeDef *hs, char symbol);

// Обработчик таймаута
void Serial_TimeOutCallBack(SerialTypeDef *hs);
//Функция отправки данных
uint8_t Serial_SendData(SerialTypeDef *hs, uint8_t *data, uint16_t len);
void Serial_SetArg(SerialTypeDef *hs, void *arg);
void Serial_Init(SerialTypeDef *hs, uint8_t *ringBuf,  uint32_t ringbufSize);
void Serial_DeInit(SerialTypeDef *hs);

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_INC_SERIAL_H_ */
