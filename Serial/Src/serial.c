/*
 * serial.c
 *
 *  Created on: 29 авг. 2019 г.
 *      Author: Acer
 */
#include "serial.h"

static uint8_t SerialReadBuf[SERIAL_READ_BUF_LEN];

// Обработчик таймаута
__weak void Serial_TimeOutCallBack(SerialTypeDef *hs)
{
	UNUSED(hs);
}


//Функция отправки данных
__weak uint8_t Serial_SendData(SerialTypeDef *hs, uint8_t *data, uint16_t len)
{
	UNUSED(hs);
	UNUSED(data);
	UNUSED(len);

	return 0;
}

// Функция статической деинициализации объекта последовательного порта
void Serial_DeInit(SerialTypeDef *hs)
{
	memset(hs, 0, sizeof(SerialTypeDef));
}

// Функция статической инициализации объекта последовательного порта
void Serial_Init(SerialTypeDef *hs, uint8_t *ringBuf,  uint32_t ringbufSize)
{
	Serial_DeInit(hs);
	hs->RingBuffer = ringBuf;
	hs->Size = ringbufSize;
}

// Передача аргумента объекту
void Serial_SetArg(SerialTypeDef *hs, void *arg)
{
	hs->Arg = arg;
}
//Функция системного таймера
void Serial_SysTick(SerialTypeDef *hs)
{
	if (hs->RecTimeOutCnt > 0)
	{
		hs->RecTimeOutCnt--;

		if (hs->RecTimeOutCnt == 0)
		{
			Serial_TimeOutCallBack(hs);
		}
	}
}
//-------------------------------------------------------------
void Serial_ByteHandle(SerialTypeDef *hs, uint8_t data)
{
	//Установка счетчика таймаута
	hs->RecTimeOutCnt = hs->TimeOutValue;
	//Запись данных в кольцевой буфер
	hs->RingBuffer[hs->WriteIndex] = data;
	//Увеличение количества непрочитанных байт
	if (hs->UnReadCnt < (hs->Size - 1))
	{
		hs->UnReadCnt++;
	}
	//Увеличение индекса записи
	hs->WriteIndex = (hs->WriteIndex + 1) % hs->Size;
}

//--------------------------------------------------------------
//Функция чтения необработанного байта из кольцевого буфера
uint8_t Serial_GetReceivedByte(SerialTypeDef *hs)
{
	uint8_t byteResult = 0;

	if (hs->UnReadCnt)
	{
		byteResult = hs->RingBuffer[(hs->WriteIndex + (hs->Size - hs->UnReadCnt)) % hs->Size];

		if (hs->UnReadCnt > 0)
		{
			hs->UnReadCnt--;
		}

		return byteResult;
	}

	return byteResult;
}
//------------------------------------
uint16_t Serial_Available(SerialTypeDef *hs)
{
	volatile SerialTypeDef *serial = hs;
	while(serial->RecTimeOutCnt);
	return serial->UnReadCnt;
}
//-----------------------------------------
char* Serial_GetBytesUntil(SerialTypeDef *hs, char terminator)
{
	char *ptr = (char *) SerialReadBuf;
	uint32_t limit = 0;
	uint8_t recByte = 0;

	if (Serial_Available(hs))
	{
		memset(SerialReadBuf, 0, SERIAL_READ_BUF_LEN);
		while (Serial_Available(hs))
		{
			recByte = Serial_GetReceivedByte(hs);

			limit++;

			if (recByte == terminator)
			{
				break;
			}

			if (limit >= SERIAL_READ_BUF_LEN)
			{
				return NULL;
			}

			*ptr++ = recByte;
		}

		return (char *) SerialReadBuf;
	}

	return NULL;
}
//--------------------------------------------------------------
uint16_t Serial_GetBytesUntilToBuf(SerialTypeDef *hs, uint8_t *rxBuf, char terminator, uint16_t maxLength)
{
	char *ptr = (char *)rxBuf;
	uint32_t limit = 0;
	uint8_t recByte = 0;

	if (Serial_Available(hs))
	{
		memset(rxBuf, 0, maxLength);
		while (Serial_Available(hs))
		{
			recByte = Serial_GetReceivedByte(hs);

			limit++;

			if (recByte == terminator)
			{
				break;
			}

			if (limit >= maxLength)
			{
				break;
			}

			*ptr++ = recByte;
		}
		return limit;
	}

	return 0;
}
//--------------------------------------------------------------
uint16_t Serial_GetBytesToBuf(SerialTypeDef *hs, uint8_t *rxBuf, uint16_t maxLength)
{
	char *ptr = (char *)rxBuf;
	uint32_t limit = 0;
	if (Serial_Available(hs))
	{
		memset(rxBuf, 0, maxLength);
		while (Serial_Available(hs))
		{
			*ptr++ = Serial_GetReceivedByte(hs);

			limit++;

			if (limit >= maxLength)
			{
				break;
			}
		}
		return limit;
	}

	return 0;
}
//---------------------------------------------------------------
char* Serial_GetBytes(SerialTypeDef *hs)
{
	char *ptr = (char *) SerialReadBuf;
	uint32_t limit = 0;
	if (Serial_Available(hs))
	{
		memset(SerialReadBuf, 0, SERIAL_READ_BUF_LEN);
		while (Serial_Available(hs))
		{
			*ptr++ = Serial_GetReceivedByte(hs);
			limit++;
			if (limit >= SERIAL_READ_BUF_LEN)
			{
				break;
			}
		}
		return (char *) SerialReadBuf;
	}

	return NULL;
}
//---------------------------------------
void Serial_SetTimeOutVal(SerialTypeDef *hs, volatile uint16_t val)
{
	hs->TimeOutValue = val;
}
//------------------------------------------------------
char Serial_GetLastChar(SerialTypeDef *hs)
{
	uint16_t lastCharIndex = 0;

	if (hs->UnReadCnt > 0)
	{
		lastCharIndex = ((hs->WriteIndex + (hs->Size - 1))) % hs->Size;
		return (char)hs->RingBuffer[lastCharIndex];
	}
	else
	{
		return 0;
	}
}
//-----------------------------------------
uint8_t Serial_LastSymbolIs(SerialTypeDef *hs, char symbol)
{
	if (Serial_GetLastChar(hs) == symbol)
	{
		return 1;
	}

	return 0;
}
