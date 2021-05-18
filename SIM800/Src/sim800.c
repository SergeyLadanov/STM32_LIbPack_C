#include <Logging.h>
#include "sim800.h"

//#include "ioparams.h"
SIM800_TickFreqTypeDef SIM800_TickFreq = SIM800_TICK_FREQ_DEFAULT;

//---------------------------------------
SIM800_StatusTypeDef SIM800_Init(SIM800_Obj *hsim800,  SerialTypeDef *hs)
{
	char *repply = NULL;

	if (hs == NULL)
	{
		hs = hsim800->Serial;
	}

	hsim800->Serial = hs;
	SIM800_Sockets_Init(hsim800);

	repply = SIM800_AT_Send(hsim800, "AT\r", 1000);
	SIM800_Delay(hsim800, 100);
	repply = Serial_GetBytes(hsim800->Serial);
#ifdef DEBUG
	printf("AT Synchronization repply: %s\r\n", repply);
#endif

	repply = SIM800_AT_Send(hsim800, "AT\r", 1000);

#ifdef DEBUG
	printf("AT Checking repply: %s\r\n", repply);
#endif

	if (!strstr(repply, "OK"))
	{
		return SIM800_ERR;
	}
	repply = SIM800_AT_Send(hsim800, "ATE0\r", 1000); //Эхо в ответе на команду
#ifdef DEBUG
	printf("Echo Repply: %s\r\n", repply);
#endif

	if (!strstr(repply, "OK"))
	{
		return SIM800_ERR;
	}
	repply = SIM800_AT_Send(hsim800, "ATV0\r", 1000); //Цифровой ответ на команды

#ifdef DEBUG
	printf("Digit repply mode: %s\r\n", repply);
#endif
	if (!strstr(repply, "0\r\n"))
	{
		return SIM800_ERR;
	}
	repply = SIM800_AT_Send(hsim800, "AT+CMGF=1\r", 1000); //Текстовый режим СМС

#ifdef DEBUG
	printf("SMS text mode: %s\r\n", repply);
#endif

	if (!strstr(repply, "0\r\n"))
	{
		return SIM800_ERR;
	}


	return SIM800_OK;

}
//-------------------------------------------------------------------
__weak void SIM800_Setup(SIM800_Obj *hsim800)
{
	UNUSED(hsim800);
}
//--------------------------------------------------------------------
__weak void SIM800_SMS_ReceivedCallBack(SIM800_Obj *hsim800, char* number, char* text)
{
  UNUSED(number);
  UNUSED(text);
}
//-------------------------------------------------------------------
// Стандартный обработчик ответа NTP
__weak void SIM800_NTP_RepplyCallBack(SIM800_Obj *hsim800, char *repply)
{
	UNUSED(hsim800);
	UNUSED(repply);
}

// Обработчик перед запросом NTP
__weak void SIM800_NTP_BeforeRequestCallBack(SIM800_Obj * hsim800)
{
	UNUSED(hsim800);
}
//-----------------------------------------------------------
__weak void SIM800_OnNetFoundCallBack(SIM800_Obj *hsim800)
{
	UNUSED(hsim800);
}
//-----------------------------------------------------------
__weak void SIM800_OnNetLostCallBack(SIM800_Obj *hsim800)
{
	UNUSED(hsim800);
}

//------------------------------------------------------------
__weak void SIM800_Restart(SIM800_Obj *hsim800)
{
	UNUSED(hsim800);
}

//------------------------------------------------------------
__weak void SIM800_Reset_Enable(void)
{
	UNUSED(NULL);
}
//-------------------------------------------------------------
__weak void SIM800_Reset_Disable(void)
{
	UNUSED(NULL);
}

//------------------------------------------------------------
__weak void SIM800_Delay(SIM800_Obj *hsim800, uint32_t Delay)
{
	  uint32_t tickstart = SIM800_GetSysTick(hsim800);
	  uint32_t wait = Delay;

	  if (wait < HAL_MAX_DELAY)
	  {
	    wait += (uint32_t)(SIM800_TickFreq);
	  }

	  while ((SIM800_GetSysTick(hsim800) - tickstart) < wait)
	  {
	  }
}

// Перезагрузка всего приложения
__weak void SIM800_HardRestart(SIM800_Obj *hsim800)
{
	UNUSED(hsim800);
}

/***************ОБРАБОТЧИКИ СОБЫТИЙ СОКЕТОВ***************/

__weak void SIM800_Socket_PollCallBack(sim800_socketTypeDef *socket)
{
	UNUSED(NULL);
}
//------------------------------------------------------------------------
__weak void SIM800_Socket_ConnectedCallBack(sim800_socketTypeDef *socket)
{
	UNUSED(NULL);
}
//-----------------------------------------------------------------------
__weak void SIM800_SocketClosedCallBack(sim800_socketTypeDef *socket)
{
	UNUSED(NULL);
}
//--------------------------------------------------
__weak void SIM800_Socket_SentCallBack(sim800_socketTypeDef *socket)
{
	UNUSED(NULL);
}
//-------------------------------------------------------------------
__weak void SIM800_Socket_ReceivedCallBack(sim800_socketTypeDef *socket, uint8_t *buf, uint16_t length)
{
	UNUSED(buf);
	UNUSED(length);
}
//---------------------------------------------------------------------

void SIM800_Handle(SIM800_Obj *hsim800)
{
	//Формирование задержки до инициализации (ожидание готовности GSM модуля)
	if (hsim800->InitDelay > 1)
		return;

	if (hsim800->InitDelay == 1)
	{
		SIM800_Setup(hsim800);
		hsim800->InitDelay = 0;
	}
	SIM800_MessageHandler(hsim800);
	TSM_Handler(&hsim800->NetStatusTask, NET_CHECK_PERIOD, hsim800, &SIM800_Network_Handle);
	TSM_Handler(&hsim800->SMS_Task, SMS_CHECK_PERIOD, hsim800, &SIM800_SMS_Handle);
#ifdef USE_NTP
	if (hsim800->NTP_Enable)
		TSM_Handler(&hsim800->NTP_Task, 20000, hsim800,  &SIM800_NTP_Handle);
#endif
	TSM_Handler(&hsim800->SocketTask,15000, hsim800, &SIM800_Sockets_Handle);
	TSM_Handler(&hsim800->SocketReceiveTask, 100, hsim800, &SIM800_Sockets_ReceiveHandle);
}
