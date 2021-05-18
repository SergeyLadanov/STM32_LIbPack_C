/*
 * sim800_if.c
 *
 *  Created on: 10 дек. 2020 г.
 *      Author: serge
 */
#include "sim800_if.h"
#include "control.h"
#include "datacontrol.h"

#include <iec104_client.h>
#include <iec104_model.h>
#include <dlms_client.h>

#define SERIAL_SIZE 1536



SIM800_Obj sim800;
SerialTypeDef sim800_serial;
static uint8_t recByte;


// Инициализация связи
static void SIM800_ConnectionInit(void)
{
	iec104_model_init();
	if (settingsFlash.dlmsClientEnable)
	{
		dlms_client_init();
	}

	if (settingsFlash.iec104ClientEnable)
	{
		iec104_client_init();
	}

	if (settingsFlash.internalPort > 0)
	{
		SIM800_TCP_Listen(&sim800, settingsFlash.internalPort);
	}
}


// Включение приема UART
void SIM800_Serial_StartReceive(void)
{
	HAL_UART_Receive_IT(&SIM800_UART_HANDLE,(uint8_t*) &recByte,1);
}

// Инициализация последовательного порта
static void SIM800_Serial_Init(void)
{
	static uint8_t serialBuf[SERIAL_SIZE];

	Serial_Init(&sim800_serial, serialBuf, sizeof(serialBuf));
	Serial_SetArg(&sim800_serial, &SIM800_UART_HANDLE);
	SIM800_Serial_StartReceive();
	Serial_SetTimeOutVal(&sim800_serial, 50);
}

// Инициализация GSM модуля
void SIM800_Setup(SIM800_Obj *hsim800)
{
	memset(hsim800, 0, sizeof(SIM800_Obj));
	memset(&sim800_serial, 0, sizeof(SerialTypeDef));
	SIM800_ConnectionInit();
	SIM800_Serial_Init();
	if (SIM800_Init(hsim800, &sim800_serial) != SIM800_OK)
	{
		SIM800_RestartSystem(hsim800);
		return;
	}

	HAL_Delay(200);
	SIM800_IMEI_Get(&sim800, imei);

	if (settingsFlash.gprsActive)
	{
		SIM800_GPRS_SetProfile(&sim800, 1, (char *)settingsFlash.gprsApn, (char *)settingsFlash.gprsUser, (char *)settingsFlash.gprsPswd);
		SIM800_GPRS_Enable(&sim800);
	}

	if (settingsFlash.ntpActive)
	{
		SIM800_NTP_Enable(hsim800);
		SIM800_NTP_Init(&sim800, (char *)settingsFlash.ntpServer);
	}
}

// Обработка принятого байта по UART
void sim800_RxCpltCallback(void)
{
	  SIM800_ResetNoAnswer(&sim800);
	  GPIOD->ODR |= (1<<1);
	  Serial_ByteHandle(&sim800_serial, recByte);
}

//-------------------------------------------------
void sim800_init_if()
{
	SIM800_Start(&sim800);
}

// Управление перезагрузкой модуля
void SIM800_Reset_Enable(void)
{
	SIM800_RES_GPIO_Port->ODR &= ~SIM800_RES_Pin; //Сброс модуля
}
//-------------------------------------------------------------
void SIM800_Reset_Disable(void)
{
	SIM800_RES_GPIO_Port->ODR |= SIM800_RES_Pin; //Запуск модуля;
}
//------------------------------------------------------------
void SIM800_Restart(SIM800_Obj *hsim800)
{
	SIM800_Reset_Enable();
	SIM_PWR_GPIO_Port->ODR &= ~SIM_PWR_Pin;
	HAL_Delay(1500);
	SIM_PWR_GPIO_Port->ODR |= SIM_PWR_Pin;
	HAL_Delay(500);
	SIM800_Reset_Disable();
	SIM800_Start(hsim800);
}

//-------------------------------------------------
void SIM800_HardRestart(SIM800_Obj *hsim800)
{
	NVIC_SystemReset();
}
//---------------------------------------
void SIM800_OnNetFoundCallBack(SIM800_Obj *hsim800)
{
	GPIOD->ODR |= (1<<0);
}
//-----------------------------------------------------------
void SIM800_OnNetLostCallBack(SIM800_Obj *hsim800)
{
	GPIOD->ODR &= ~(1<<0);
}
//-----------------------------------------------------------
void SIM800_NTP_BeforeRequestCallBack(SIM800_Obj * hsim800)
{
	if (meterings.TimeSyncedTries > 0)
	{
		meterings.TimeSyncedTries--;
	}
	else
	{
		meterings.TimeSynced = 0;
	}

}

// Извлечение даты и времени из строки
static void timeDate_From_String(char *str, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate)
{
	char *tmp;
	memset(sTime, 0, sizeof(RTC_TimeTypeDef));
	memset(sDate, 0, sizeof(RTC_DateTypeDef));

	tmp = strtok(str, "\"");
	tmp = strtok(NULL, "\"");
	tmp = strtok(tmp, "/");
	//
	sDate->Year = atoi(tmp);
	tmp = strtok(NULL, "/");
	//
	sDate->Month = atoi(tmp);
	tmp = strtok(NULL, ",");
	//
	sDate->Date = atoi(tmp);
	tmp = strtok(NULL, ":");
	//
	sTime->Hours = atoi(tmp);
	tmp = strtok(NULL, ":");
	//
	sTime->Minutes = atoi(tmp);
	tmp = strtok(NULL, "+");
	//
	sTime->Seconds = atoi(tmp);
}

// Обработчик синхронизации времени
void SIM800_NTP_RepplyCallBack(SIM800_Obj *hsim800, char *repply)
{


	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	RTC_DateTypeDef dateTmp;

	uint8_t seconds_backup = 0;



	meterings.TimeSynced = 1;
	meterings.TimeSyncedTries = 10;

	timeDate_From_String(repply, &time, &date);

	seconds_backup = time.Seconds;

	for (uint8_t i = 0; i < 8; i++)
	{
		repply = SIM800_AT_Send(hsim800, "AT+CCLK?\r",1000); //Запрос текущего времени

		if (repply != NULL)
		{
			timeDate_From_String(repply, &time, &date);

			if (seconds_backup != time.Seconds)
			{

				memcpy(&dateTmp, &date, sizeof(RTC_DateTypeDef));

				date.WeekDay = getWeekday(&dateTmp);

				HAL_RTC_SetTime(&hrtc, &time, FORMAT_BIN);
				HAL_RTC_SetDate(&hrtc,&date, RTC_FORMAT_BIN);

				seconds_backup = time.Seconds;
				break;
			}
		}

		HAL_Delay(125);
	}



}

