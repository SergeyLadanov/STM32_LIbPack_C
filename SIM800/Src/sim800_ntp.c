/*
 * sim800_ntp.c
 *
 *  Created on: 11 дек. 2020 г.
 *      Author: serge
 */

#include "sim800_ntp.h"


// Включение NTP
void SIM800_NTP_Enable(SIM800_Obj *hsim800)
{
	hsim800->NTP_Enable = true;
}
// Выключение NTP
void SIM800_NTP_Disable(SIM800_Obj *hsim800)
{
	hsim800->NTP_Enable = false;
}

// Обработка ответа NTP
void SIM800_NTP_RepplyHandler(SIM800_Obj *hsim800)
{
	//char *tmp;
	char* repply = NULL;
	repply = SIM800_AT_Send(hsim800, "AT+CCLK?\r",1000); //Запрос текущего времени
	SIM800_NTP_RepplyCallBack(hsim800, repply);
}
//----------------------------------------------------------------------
SIM800_StatusTypeDef SIM800_NTP_Handle(void *arg)
{
	SIM800_Obj *hsim800 = (SIM800_Obj *) arg;

	if (hsim800->GPRS_Status)
	{
		SIM800_NTP_BeforeRequestCallBack(hsim800);
		SIM800_AT_Send(hsim800, "AT+CNTP\r",1000);
	}

	return SIM800_OK;
}

//----------------------------------------------------------------------
void SIM800_NTP_Init(SIM800_Obj *hsim800, char *server)
{
	  char tmp[64];
	  sprintf(tmp, "AT+CNTPCID=%d\r", hsim800->GPRS_Profile.bearer);
	  SIM800_AT_Send(hsim800, tmp,1000); //Привязка настроек интернета к NTP
	  sprintf(tmp, "AT+CNTP=\"%s\",12\r", server);
	  SIM800_AT_Send(hsim800, tmp,1000); //Привязка настроек интернета к NTP
}
//--------------------------------------------------------------------

