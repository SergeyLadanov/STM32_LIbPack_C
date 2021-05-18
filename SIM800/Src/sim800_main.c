/*
 * sum800_main.c
 *
 *  Created on: 11 дек. 2020 г.
 *      Author: serge
 */
#include "sim800_main.h"


uint32_t SIM800_GetSysTick(SIM800_Obj *hsim800)
{
	return hsim800->Tick;
}
//--------------------------------------------
SIM800_StatusTypeDef SIM800_MessageEnable(SIM800_Obj *hsim800, char *rep)
{
	for (uint8_t i = 0; i < 3; i++)
	{
		if (strstr(rep, "\r\n> "))
		{
			return SIM800_OK;
		}
		SIM800_Delay(hsim800, 80);
		rep = Serial_GetBytes(hsim800->Serial);
	}

	return SIM800_ERR;

}
//---------------------------------------------
void SIM800_RestartSystem(SIM800_Obj *hsim800)
{
#ifdef DEBUG
	printf("Restarting SIM800...\r\n");
#endif

	SIM800_Restart(hsim800);
	SIM800_Start(hsim800);
}
//-------------------------------------------------
uint8_t SIM800_SendData(SIM800_Obj *hsim800, uint8_t *pData, uint16_t length)//, volatile uint16_t timeoutDelay)
{
	//Если модуль занят то команда не отправляется
	if (hsim800->BusyTimeOut > 0)
	{
		return SIM800_BUSY;
	}

	Serial_SendData(hsim800->Serial, pData, length);

	return 0;
}
//-----------------------------------------------------
static char* SIM800_WaitRepply(SIM800_Obj *hsim800, volatile uint16_t timeoutDelay)
{
	char *repply = NULL;


	if (timeoutDelay > 0)
	{
		uint32_t wait = timeoutDelay;
		uint32_t tickStart = SIM800_GetSysTick(hsim800);

		hsim800->NoAnswCnt++;

		if (wait < SIM800_MAX_DELAY)
		{
		  wait++;
		}

		while((!Serial_Available(hsim800->Serial)) && ((SIM800_GetSysTick(hsim800) - tickStart) < wait));

		repply = Serial_GetBytes(hsim800->Serial);
	}

	return repply;
}
//----------------------------------------------------
char* SIM800_AT_Send(SIM800_Obj *hsim800, char *str, volatile uint16_t timeoutDelay)
{

	//Если модуль занят то команда не отправляется
	if (hsim800->BusyTimeOut > 0)
	{
		return NULL;
	}

	Serial_SendData(hsim800->Serial, (uint8_t *) str, strlen(str));

	return SIM800_WaitRepply(hsim800, timeoutDelay);
}
//-----------------------------------------------------------
void SIM800_IMEI_Get(SIM800_Obj *hsim800, char *outBuf)
{
	char *tmp;
	char *repply = NULL;

	repply = SIM800_AT_Send(hsim800, "AT+GSN\r", 1000);

	if (repply)
	{
		tmp = strtok(repply,"\r\n");
		sprintf(outBuf,"%s", tmp);
	}

}
//--------------------------------------------------------------
void SIM800_ResetNoAnswer(SIM800_Obj *hsim800)
{
	hsim800->NoAnswCnt = 0;
}
//---------------------------------------------------------------
void SIM800_Start(SIM800_Obj *hsim800)
{
	hsim800->InitDelay = 7000;
}
//------------------------------------------
void SIM800_IncTick(SIM800_Obj *hsim800)
{

	//Системное время
	hsim800->Tick++;

	if (hsim800->Serial != NULL)
		Serial_SysTick(hsim800->Serial);
	//Таймаут отправки СМС
	if (hsim800->BusyTimeOut > 0)
	{
		hsim800->BusyTimeOut--;
	}
	//---------------------------
	if ((hsim800->InitDelay > 1) && (hsim800->InitDelay < 10000))
	{
		hsim800->InitDelay--;
	}
}


//-------------------------------------------------------------
static void SIM800_SMS_Read(SIM800_Obj *hsim800, uint16_t memInd)
{
	  char* text;
	  char* number;
	  char* repply = NULL;

	  char strTmp[24];

	  sprintf(strTmp,"AT+CMGR=%d\r", memInd); //Запись индекса памяти в команду
	  repply = SIM800_AT_Send(hsim800, strTmp,500);
#ifdef DEBUG
	  printf("Repply for reading text SMS is %s\r\n", repply);
#endif

	  if (repply) //Чтение сообщения
	  {
		  //Получение текста сообщения
		  text = strtok(repply,"\r\n");
		  text = strtok(NULL,"\r\n");
		  //----------------------------------
		  //Получение номера отправителя
		  number = strtok(repply,",");
		  number = strtok(NULL,",");
		  number = strtok(number,"\"");
#ifdef DEBUG
		  printf("Number SMS is %s\r\n", number);
		  printf("Text SMS is %s\r\n", text);
#endif


		  if (number != NULL)
		  {
			  SIM800_SMS_ReceivedCallBack(hsim800, number, text);
			 // SIM800_AT_Send("AT+CMGDA=\"DEL ALL\"\r",500); //очистка всех СМС
			  SIM800_AT_Send(hsim800, "AT+CMGDA=\"DEL READ\"\r",1000); //очистка прочитанных СМС
			  SIM800_AT_Send(hsim800, "AT+CMGDA=\"DEL SENT\"\r",1000); //очистка отправленных СМС
		  }
		  else
		  {

			  	SIM800_Reset_Enable();
				SIM800_Delay(hsim800, 1000);
				SIM800_Reset_Disable();
				SIM800_Delay(hsim800, 5000);
				SIM800_Init(hsim800, NULL);
				SIM800_AT_Send(hsim800, "AT+CMGDA=\"DEL ALL\"\r",500); //очистка всех СМС
				SIM800_RestartSystem(hsim800);
		  }
	  }

}
//--------------------------------------------------------
SIM800_StatusTypeDef SIM800_SMS_Handle(void *arg)
{
	SIM800_Obj *hsim800 = (SIM800_Obj *) arg;
	char* ptr = 0;
	uint16_t ind = 0;
	char *repply = NULL;
	char *pos;
	//if ((smsHandlerTimeout == 0) && (sim800_prop.netPresent != 0))
	if (hsim800->NetStatus)
	{

        if (hsim800->BusyTimeOut == 0)
        {
          repply = SIM800_AT_Send(hsim800, "AT+CMGL=\"REC UNREAD\",1\r",3000);
#ifdef DEBUG
          printf("Repply for SMS request is %s \r\n", repply);
#endif
		  if (repply)
		  {
			  pos = strstr(repply, "+CMGL: ");
			  if ((pos != 0) && ((pos-repply) < 10))
			  {
				  ptr = strtok(repply, ",");
				  ptr = strstr(repply, " ");
				  ptr++;
				  ind = atoi(ptr);
				  SIM800_SMS_Read(hsim800, ind);

				  return SIM800_BUSY;
			  }
			  else if (strcmp(repply, "0\r\n"))
			  {
				  SIM800_AT_Send(hsim800, "AT+CMGDA=\"DEL ALL\"\r",500);
			  }
			  else if ((pos-repply) >= 10)
			  {
				  SIM800_AT_Send(hsim800, "AT+CMGDA=\"DEL ALL\"\r",500);
			  }

		  }

        }
        else
        {
        	return SIM800_BUSY;
        }
	}

	return SIM800_OK;
}

//----------------------------------------------------------------------------------
uint8_t SIM800_SMS_Send(SIM800_Obj *hsim800, char *number, char *text, uint16_t tout)
{
	char strTmp[64];
	char* repply = NULL;
	sprintf(strTmp,"AT+CMGS=\"%s\"\r",number);

	if (hsim800->BusyTimeOut > 0)
	{
		return SIM800_BUSY;
	}
	//--------------------------------------------------------

	repply = SIM800_AT_Send(hsim800, strTmp, tout);

	if (repply)
	{
		if (SIM800_MessageEnable(hsim800, repply) == SIM800_OK)
		{
			SIM800_AT_Send(hsim800, text, 0);
			SIM800_ApplyMsg(hsim800);

			hsim800->BusyTimeOut = SMS_SEND_TIMEOUT;

			SIM800_Delay(hsim800, 50);

			return SIM800_OK;
		}
	}

	SIM800_Delay(hsim800, 1000);

	SIM800_CancelMsg(hsim800);

	return SIM800_ERR;
}
//--------------------------------------------------------------------------------
void SIM800_MessageHandler(SIM800_Obj *hsim800)
{
	if (Serial_Available(hsim800->Serial))
	{
		char *msg = Serial_GetBytes(hsim800->Serial);
		char *ptr = NULL;



#ifdef DEBUG
		printf("Sporadic msg is %s \r\n", msg);
#endif

#ifdef MONOSOCKET_MODE
		  if (strstr(msg, "\r\nCONNECT OK") != 0)
		  {
			  if (hsim800->SocketTable[0].socket->status == CLOSED)
			  {
				  hsim800->SocketTable[0].socket->status = CONNECTED;

				  if (hsim800->SocketTable[0].socket->connected_fn != NULL)
					  hsim800->SocketTable[0].socket->connected_fn();
			  }
		  }
		  //---------------------------------------------------------------------
		  else if (strstr(msg, "\r\nALREADY CONNECT") != 0)
		  {

			  if (hsim800->SocketTable[0].socket->status == CLOSED)
			  {
				  hsim800->SocketTable[0].socket->status = CONNECTED;

				  if (hsim800->SocketTable[0].socket->connected_fn != NULL)
					  hsim800->SocketTable[0].socket->connected_fn();
			  }
		  }
		  else if (strstr(msg, "\r\nSEND OK") != 0)
		  {
			  if (hsim800->SocketTable[0].socket->sent_fn != NULL)
				  hsim800->SocketTable[0].socket->sent_fn();
		  }
		  //---------------------------------------------------------
		  else if (strstr(msg, "\r\nCLOSED") != 0)
		  {
			  if (hsim800->SocketTable[0].socket->status == CONNECTED)
			  {
				  hsim800->SocketTable[0].socket->status = CLOSED;

				  if (hsim800->SocketTable[0].socket->closed_fn != NULL)
					  hsim800->SocketTable[0].socket->closed_fn();
			  }
		  }

#else
		  if ((ptr = strstr(msg, "CONNECT OK")) != 0)
		  {
			  ptr = ptr - 3;
			  uint8_t index = ptr[0] - '0';
			  if (hsim800->SocketTable[index].socket->status == CLOSED)
			  {
				  hsim800->SocketTable[index].socket->status = CONNECTED;

				  SIM800_Socket_ConnectedCallBack(hsim800->SocketTable[index].socket);
			  }
		  }
		  else if ((ptr=strstr(msg, ", REMOTE IP:")) != 0)
		  {
			  TSM_Execute(&hsim800->SocketTask);
		  }
		  else if ((ptr=strstr(msg, "CLOSED")) != 0)
		  {
			  ptr = ptr - 3;
			  uint8_t index = ptr[0] - '0';
			  if (hsim800->SocketTable[index].socket->status == CONNECTED)
			  {
				  hsim800->SocketTable[index].socket->status = CLOSED;

				  SIM800_Socket_ClosedCallBack(hsim800->SocketTable[index].socket);
			  }
		  }
		  else if ((ptr = strstr(msg, "SEND OK")) != 0)
		  {
			  ptr = ptr - 3;
			  uint8_t index = ptr[0] - '0';

			  SIM800_Socket_SentCallBack(hsim800->SocketTable[index].socket);
		  }
#endif
		  else if (strstr(msg, "\r\n+CIPRXGET: ") != 0)
		  {
			  TSM_Execute(&hsim800->SocketReceiveTask);
		  }
		  else if (strstr(msg, "\r\nSTATE: ") != 0)
		  {
			  //checkSocketsStatus((uint8_t *)msg);
		  }
		  //--------------------------------------------------------------

		  else if ((ptr = strstr(msg, "\r\n+CNTP: ")) != 0) //Уведомление о синхронизации NTP
		  {
			  if (ptr[9] == '1')
				  SIM800_NTP_RepplyHandler(hsim800);
		  }
		  //------------------------------------------------------------------
		  else if (strstr(msg, "+CMGS:") != 0) //Уведомление об отправке СМС
		  {
			  SIM800_AT_Send(hsim800, "AT+CMGDA=\"DEL SENT\"\r",500); //очистка отправленных СМС
			  hsim800->BusyTimeOut = 0;
		  }
		  //-------------------------------------------------------------------
		  else if (strstr(msg, "+CMTI:") != 0) //Уведомление о приеме СМС
		  {
			  SIM800_Delay(hsim800, 1000);
			  TSM_Execute(&hsim800->SMS_Task); //Установка статуса срочной обработки функции
			//-------------------------------------------------------------------------
		  }
		  else //Другое сообщение
		  {

		  }
		  //----------------------------------------------------------------
		 // usartprop.ready = 0;
	  //}
	}
}

//--------------------------------------------------------------------
void SIM800_GPRS_Enable(SIM800_Obj *hsim800)
{
	hsim800->GPRS_Enable = true;
}

//--------------------------------------------------------------------
void SIM800_GPRS_Disable(SIM800_Obj *hsim800)
{
	hsim800->GPRS_Enable = false;
}
//---------------------------------------------------------
static uint8_t SIM800_GPRS_Available(SIM800_Obj *hsim800)
{
	char* repply = NULL;
	repply = SIM800_AT_Send(hsim800, "AT+CGATT?\r",1000);
	  if (repply) // Проверка доступа к услугам пакетнойпередачи данных;
	  {
		    repply = Serial_GetBytes(hsim800->Serial);
			//Проверка формата ответа
			if (strstr(repply, "+CGATT: ") != 0)
			{
				//Выделение статуса
				char *ptr;
				ptr = strstr(repply," ");
				ptr++;

				if (*ptr == '1')
				{
					return SIM800_OK;
				}
				else if (*ptr == '0')
				{
					return SIM800_ERR;
				}
			}
	  }
	  return SIM800_NORESULT;
}

//-------------------------------------------------------------------------
static SIM800_StatusTypeDef SIM800_GPRS_GetStatus(SIM800_Obj *hsim800)
{
	char tmp[16];
	char *repply = NULL;
	sprintf(tmp,"AT+SAPBR=2,%d\r",hsim800->GPRS_Profile.bearer);
	repply = SIM800_AT_Send(hsim800, tmp, 5000);
	if (repply)
	{
		//Проверка формата ответа
		if (strstr(repply, "\r\n+SAPBR: ") != 0)
		{
			//Выделение статуса
			char *ptr;
			ptr = strstr(repply," ");
			ptr++; //На этом этапе получен номер конфигурации из ответа
			ptr = strtok(repply,",");
			ptr = strtok(NULL,","); //Получение указателя на статус соединения

			if (*ptr == '1')
			{
				return SIM800_OK;
			}
			else if (*ptr == '3')
			{
				return SIM800_ERR;
			}

		}
	}

	return SIM800_NORESULT;
}
//---------------------------------------------------------------------

void SIM800_IP_Init(SIM800_Obj *hsim800)
{
	  char tmp[80];
	  //Настройки IP
	  sprintf(tmp,"AT+SAPBR=3,%d,\"CONTYPE\",\"GPRS\"\r", hsim800->GPRS_Profile.bearer);
	  SIM800_AT_Send(hsim800, "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r",3000); //Тип соединения GPRS
	  sprintf(tmp,"AT+SAPBR=3,%d,\"APN\",\"%s\"\r", hsim800->GPRS_Profile.bearer, hsim800->GPRS_Profile.apn);
	  SIM800_AT_Send(hsim800, tmp,3000); //Точка доступа
	  sprintf(tmp,"AT+SAPBR=3,%d,\"USER\",\"%s\"\r", hsim800->GPRS_Profile.bearer, hsim800->GPRS_Profile.user);
	  SIM800_AT_Send(hsim800, tmp,3000); //Пользователь
	  sprintf(tmp,"AT+SAPBR=3,%d,\"PWD\",\"%s\"\r", hsim800->GPRS_Profile.bearer, hsim800->GPRS_Profile.pswd);
	  SIM800_AT_Send(hsim800, tmp,3000); //Пароль


	  sprintf(tmp,"AT+SAPBR=1,%d\r", hsim800->GPRS_Profile.bearer);
	  if (SIM800_AT_Send(hsim800, tmp,5000)) //Активация
	  {
		  sprintf(tmp, "AT+SAPBR=2,%d\r", hsim800->GPRS_Profile.bearer);
		  SIM800_AT_Send(hsim800, tmp,5000); //IP адрес
		  hsim800->GPRS_Status = true;
	  }
}
//------------------------------------------------------------------

SIM800_StatusTypeDef SIM800_Network_Handle(void *arg)
{
	SIM800_Obj *hsim800 = (SIM800_Obj *) arg;
	char *repply = NULL;
#ifdef DEBUG
	printf("Checking net power...\r\n");
#endif
	repply = SIM800_AT_Send(hsim800, "AT+CSQ\r", 3000);
#ifdef DEBUG
	printf("Repply for net power request: %s\r\n", repply);
#endif

#ifdef DEBUG
	printf("Checking net status...\r\n");
#endif
	repply = SIM800_AT_Send(hsim800, "AT+CREG?\r", 3000);


	if (repply) //Проверка регистрации в сети
	{
#ifdef DEBUG
		printf("Net status repply is %s\r\n", repply);
#endif
	  if ((repply = strstr(repply, "+CREG:")) != 0)
	  {

		  if ((repply[9] == '1') && (strstr(repply, "0\r\n")))
		  {
#ifdef DEBUG
			  printf("Net was detected...\r\n");
#endif
			  if (!hsim800->NetStatus)
			  {
				  SIM800_OnNetFoundCallBack(hsim800);
			  }
			  hsim800->NetStatus = true;
			  hsim800->HardResetDelay = 0;
			  hsim800->CheckNetEfforts = 0;
	#ifdef USE_GPRS
			  if (hsim800->GPRS_Enable)
			  {
#ifdef DEBUG
				  printf("Checking GPRS...\r\n");
#endif
				  if (SIM800_GPRS_Available(hsim800) != SIM800_ERR) //Проверка регистрации в сети GPRS
				  {
#ifdef DEBUG
					  printf("GPRS is Active\r\n");
#endif
					  if (!hsim800->GPRS_Status)
					  {
#ifdef DEBUG
						  printf("Initializing IP\r\n");
#endif
						  SIM800_IP_Init(hsim800);
						  SIM800_TCP_Init(hsim800);
					  }
					  else
					  {
						  if (SIM800_GPRS_GetStatus(hsim800) == SIM800_ERR)
						  {
							  hsim800->GPRS_Status = false;
						  }
					  }
				  }
				  else
				  {
#ifdef DEBUG
					  printf("GPRS is not active\r\n");
#endif
					  hsim800->GPRS_Status = false;
				  }
			  }
	#endif
		  }
		  else
		  {
			  //GPIOD->ODR &= ~(1<<0);

			  if (hsim800->NetStatus)
			  {
				  SIM800_OnNetLostCallBack(hsim800);
			  }
			  hsim800->NetStatus = false;
			  hsim800->CheckNetEfforts++;
		  }
	  }
	  else
	  {
		  hsim800->CheckNetEfforts++;
	  }
	}
	else
	{
#ifdef DEBUG
		  printf("No repply for net status request!!!\r\n");
#endif
	}
	//------------------------------------------------------
	if ((hsim800->NoAnswCnt > SIM800_NOANSWER_LIMIT) || (hsim800->CheckNetEfforts > (180000/NET_CHECK_PERIOD))) //Если модуль не отвечал долгое время
	{
		hsim800->HardResetDelay++;
		if (hsim800->HardResetDelay > 80)
			SIM800_HardRestart(hsim800);
		SIM800_RestartSystem(hsim800); //Перезапуск системы
		hsim800->CheckNetEfforts = 0;
	}

	return SIM800_OK;

}
//--------------------------------------------------------------------
void SIM800_GPRS_SetProfile(SIM800_Obj *hsim800, uint8_t bearer,char *apn, char *user, char *pswd)
{
	hsim800->GPRS_Profile.bearer = bearer;
	sprintf(hsim800->GPRS_Profile.apn, "%s", apn);
	sprintf(hsim800->GPRS_Profile.user, "%s", user);
	sprintf(hsim800->GPRS_Profile.pswd, "%s", pswd);
}
//-------------------------------------------------------------------
void SIM800_ApplyMsg(SIM800_Obj *hsim800)
{
    static uint8_t r = 26; //Символ CTRL+Z
    Serial_SendData(hsim800->Serial, (uint8_t *) &r, 1);
}
//---------------------
void SIM800_CancelMsg(SIM800_Obj *hsim800)
{
    static uint8_t c = 27; //Символ ESC
    Serial_SendData(hsim800->Serial, (uint8_t *) &c, 1);
}
