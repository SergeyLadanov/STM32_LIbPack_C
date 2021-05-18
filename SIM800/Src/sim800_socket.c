/*
 * sim800_socket.c
 *
 *  Created on: 11 дек. 2020 г.
 *      Author: serge
 */
#include "sim800_socket.h"

static uint8_t socketBuf[SOCKET_BUF_LENGTH];




static void SIM800_Server_GetStatus(SIM800_Obj *hsim800, char *repply)
{
	char *cut;
	char *server_info = repply;
	uint8_t start_server = 0;


	char *atrepply = NULL;

	if (repply != NULL)
	{
		repply = strstr(repply, "\r\n\r\n");
		repply[0] = '\0';
		server_info = strtok((char *)server_info, ",");
		server_info = strtok(NULL, ",");
		server_info = strtok(NULL, ",");
		server_info = strtok(NULL, ",");

		server_info++; //Статус соединения
		cut = strstr(server_info,"\"");
		*cut = '\0'; // Удаление кавычки

#ifdef DEBUG
		printf("Server status is %s\r\n", server_info);
#endif
		if (strcmp(server_info,"LISTENING"))
		{
				hsim800->ServerRestartDelay = 0;
				start_server = 1;
		}
		else
		{
			hsim800->ServerTries = 0;
			// Счетчик основной задержки до перезапуска сервера
			if (hsim800->ServerRestartDelay < SIM800_SERVER_RESTART_DELAY)
			{
				hsim800->ServerRestartDelay++;
			}
#ifdef DEBUG
			printf("Server restart delay is %lu\r\n", hsim800->ServerRestartDelay);
#endif
		}
	}
	else
	{
#ifdef DEBUG
		printf("Server status repply is NULL\r\n");
#endif
		if (hsim800->ServerPort > 0)
		{
			start_server = 1;
		}
	}


	if (hsim800->ServerRestartDelay >= SIM800_SERVER_RESTART_DELAY)
	{
		printf("Closing server...\r\n");
		atrepply = SIM800_AT_Send(hsim800, "AT+CIPSERVER=0\r\n", 3000);
		hsim800->ServerRestartDelay = 0;
	}


	if (start_server)
	{
		char tmp[64];
#ifdef DEBUG
		printf("Opening server...\r\n");
#endif

		sprintf(tmp, "AT+CIPSERVER=1,%d\r", hsim800->ServerPort);
		atrepply = SIM800_AT_Send(hsim800, tmp, 3000);


		if (strstr(atrepply, "SERVER ERROR"))
		{
#ifdef DEBUG
			printf("Server starting error!\r\n");
#endif
			hsim800->ServerTries++;

#ifdef DEBUG
			printf("Tries number is %d!\r\n", hsim800->ServerTries);
#endif

			if (hsim800->ServerTries > SIM800_SERVER_MAX_TRIES)
			{
				SIM800_RestartSystem(hsim800);
			}
		}

#ifdef DEBUG
		printf("Repply for server creation is %s\r\n", atrepply);
#endif
	}
}
//--------------------------------------------------------
static char* SIM800_Socket_GetStatus(SIM800_Obj *hsim800, char *sock_info, uint8_t sock_num)
{
	char *next_sock_info = NULL;
	char *cut;
	next_sock_info = strstr((char *)sock_info, "\r\n");
	*next_sock_info = '\0';
	next_sock_info+=2; //Указатель на информауию о следующем сокете
	sock_info = strtok((char *)sock_info, ",");
	sock_info = strtok(NULL, ",");
	sock_info = strtok(NULL, ",");
	sock_info = strtok(NULL, ",");
	sock_info = strtok(NULL, ",");
	sock_info = strtok(NULL, ",");
	sock_info++; //Статус соединения
	cut = strstr(sock_info,"\"");
	*cut = '\0'; // Удаление кавычки
	//cut += 2;
#ifdef DEBUG
	printf("Socket %d status is %s\r\n", sock_num, sock_info);
#endif
	//Анализ статуса
	if (!strcmp(sock_info,"CLOSED"))
	{
	  if (hsim800->SocketTable[sock_num].socket->status == CONNECTED)
	  {
		  hsim800->SocketTable[sock_num].socket->status = CLOSED;

		  SIM800_Socket_ClosedCallBack(hsim800->SocketTable[sock_num].socket);
	  }
	}

	//-------------------------------------------------------------------------------
	else if (!strcmp(sock_info,"CONNECTED"))
	{
	  if (hsim800->SocketTable[sock_num].socket->status == CLOSED)
	  {
		  hsim800->SocketTable[sock_num].socket->status = CONNECTED;

		  SIM800_Socket_ConnectedCallBack(hsim800->SocketTable[sock_num].socket);
	  }
	}
	//-----------------------------------
	else
	{

	}

	return next_sock_info;

}


// Функция проверки статуса сокета
static void SIM800_HandleSocketInfo(SIM800_Obj *hsim800, uint8_t *buf)
{
	char *ptr;
	ptr = strstr((char *)buf," ");
	ptr++;
#ifndef MONOSOCKET_MODE
	char *sock_info = strstr(ptr, "\r\n\r\n");
	char *server_info = NULL;
	sock_info[2] = '\0'; // Выделение глобального статуса подключений
	sock_info += 4; // Переход к информации о подключениях
	server_info = strstr(sock_info, "S:");
	sock_info = strstr(sock_info, "C:");

#ifdef DEBUG
	printf("Socket info is: %s\n", sock_info);
	printf("Server info is: %s\n", server_info);
#endif


// Выделение статуса сервера
	SIM800_Server_GetStatus(hsim800, server_info);

#endif

#ifdef DEBUG
	printf("TCP status is %s \r\n", ptr);
#endif
//-------------------------------------------------------------------------------
	  if (strstr(ptr,"TCP CLOSED"))
	  {
#ifdef MONOSOCKET_MODE
		  if (sim800_socketManager[0].socket->status == CONNECTED)
		  {
			  sim800_socketManager[0].socket->status = CLOSED;

			  if (sim800_socketManager[0].socket->closed_fn != NULL)
				  sim800_socketManager[0].socket->closed_fn();
		  }
#endif
	  }

//-------------------------------------------------------------------------------
	  else if (strstr(ptr,"CONNECT OK"))
	  {
#ifdef MONOSOCKET_MODE
		  if (sim800_socketManager[0].socket->status == CLOSED)
		  {
			  sim800_socketManager[0].socket->status = CONNECTED;

			  if (sim800_socketManager[0].socket->connected_fn != NULL)
				  sim800_socketManager[0].socket->connected_fn();
		  }
#endif
	  }
	  //-----------------------------------
	  else if(strstr(ptr,"PDP DEACT"))
	  {
#ifdef DEBUG
		printf("PDP DEACT detectdet, trying to retart SIM800\r\n");
#endif
		  SIM800_AT_Send(hsim800, "AT+CIPSHUT\r\n",2000);
		  SIM800_RestartSystem(hsim800);
	  }
	  else if (strstr(ptr,"TCP CONNECTING"))
	  {
#ifdef MONOSOCKET_MODE
		  if (sim800_socketManager[0].socket->status == CONNECTED)
		  {
			  sim800_socketManager[0].socket->status = CLOSED;

			  if (sim800_socketManager[0].socket->closed_fn != NULL)
				  sim800_socketManager[0].socket->closed_fn();
		  }
#endif
	  }
	  else if (strstr(ptr,"IP STATUS"))
	  {
#ifdef MONOSOCKET_MODE
		  sim800_socketManager[0].socket->status = CLOSED;

		  if (sim800_socketManager[0].socket->closed_fn != NULL)
			  sim800_socketManager[0].socket->closed_fn();
#endif
	  }
	  else if (strstr(ptr,"IP INITIAL"))
	  {
		  hsim800->GPRS_Status = false;
	  }
#ifndef MONOSOCKET_MODE
	  else if (strstr(ptr, "IP PROCESSING"))
	  {
		  //sim800_socketManager[0].socket->status = CLOSED;
			for (uint8_t sock_num = 0; sock_num < SIM800_NUM_CONNECTIONS; sock_num++)
			{
				sock_info = SIM800_Socket_GetStatus(hsim800, sock_info, sock_num);
			}

	  }
	  else if (strstr(ptr, "SERVER OK"))
	  {

	  }
	  else if (strstr(ptr, "SERVER LISTENING"))
	  {

	  }


#endif

}


// Получение ID сокета
uint8_t SIM800_Socket_GetId(sim800_socketTypeDef *socket)
{
	return socket->id;
}

// Получение сокета по ID
sim800_socketTypeDef* SIM800_Socket_GetById(SIM800_Obj *hsim800, uint8_t id)
{
	for (uint8_t i=0; i < SIM800_NUM_CONNECTIONS; i++)
	{
		if (hsim800->SocketTable[i].socket != 0)
		{
			if (hsim800->SocketTable[i].socket->id == id)
			{
				return hsim800->SocketTable[i].socket;
			}
		}
	}

	return NULL;
}
//--------------------------------------------------------

uint8_t SIM800_Socket_Add(SIM800_Obj *hsim800, sim800_socketTypeDef *socket)
{
	socket->Arg = hsim800;

	if (hsim800->SocketCnt > (SIM800_NUM_CONNECTIONS - 1))
		return 1;
	if (hsim800->SocketTable[hsim800->SocketCnt].socket == 0)
	{
		socket->id = hsim800->SocketCnt;
		hsim800->SocketTable[hsim800->SocketCnt].socket = socket;
		hsim800->SocketTable[hsim800->SocketCnt].sockStatus = SOCKET_INITIAL;
		hsim800->SocketTable[hsim800->SocketCnt].socket->socketBuf = socketBuf;
	}
	hsim800->SocketCnt++;

	return 0;
}
//-------------------------------------------------------------------

#ifdef MONOSOCKET_MODE
uint8_t SIM800_Socket_SendData(uint8_t *data, uint16_t length, uint16_t tout)
{
	char tmp[32];
	char *ret = NULL;


	if (sim800_prop.busyTimeout > 0)
	{
		printf("SIM800 is busy...\n");
		return SIM800_BUSY;
	}

	sprintf(tmp,"AT+CIPSEND=%d\r",length);

	ret = SIM800_AT_Send(tmp,tout);
	if(!ret){
		printf("Couldn't send AT+CIPSEND AT command\n");
	}

	if (ret)
	{
		if (sim800_messageEnable(ret) == SIM800_OK)
		{

			for (uint16_t i = 0; i < (length/32); i++)
			{
				sim800_sendData(data, 32);
				data += 32;
				SIM800_Delay(hsim800, 5);
			}

			if ((length % 32) > 0)
			{
				sim800_sendData(data, (length % 32));
			}



			indication_time = SOCK_SEND_INDICATE_DELAY;

			SIM800_Delay(hsim800, 50);

			return SIM800_OK;
		} else {
			printf("sim800_messageEnable() is not OK\n");
		}
	}

	SIM800_Delay(hsim800, 1000);

	sim800_cancelMsg();

	return SIM800_ERR;
}
//---------------------------------------
uint8_t SIM800_SocketSendStr(uint8_t *data, uint16_t tout)
{

	uint16_t length = strlen((char *)data);
	char *repply = NULL;

	if (sim800_prop.busyTimeout > 0)
	{
		return SIM800_BUSY;
	}

	repply = SIM800_AT_Send("AT+CIPSEND\r",tout);

	if (repply)
	{
		if (sim800_messageEnable(repply) == SIM800_OK)
		{

			for (uint16_t i = 0; i < (length/32); i++)
			{
				sim800_sendData(data, 32);
				data += 32;
				SIM800_Delay(hsim800, 5);
			}

			if ((length % 32) > 0)
			{
				sim800_sendData(data, (length % 32));
				SIM800_Delay(hsim800, 5);
			}

			sim800_applyMsg();

			indication_time = SOCK_SEND_INDICATE_DELAY;

			SIM800_Delay(hsim800, 50);

			return SIM800_OK;
		}
	}

	SIM800_Delay(hsim800, 1000);

	sim800_cancelMsg();

	return SIM800_ERR;
}
#else
uint8_t SIM800_Socket_SendStr(sim800_socketTypeDef *socket, uint8_t *data, uint16_t length, uint16_t tout)
{

	SIM800_Obj *hsim800 = (SIM800_Obj *) socket->Arg;
	char tmp[32];
	char *ret = NULL;

	uint8_t id = SIM800_Socket_GetId(socket);


	if (hsim800->BusyTimeOut > 0)
	{
#ifdef DEBUG
		printf("SIM800 is busy...\n");
#endif
		return SIM800_BUSY;
	}

	sprintf(tmp,"AT+CIPSEND=%d,%d\r", id, length);

	ret = SIM800_AT_Send(hsim800, tmp,tout);

#ifdef DEBUG
	if(!ret){
		printf("Couldn't send AT+CIPSEND AT command\n");
	}
#endif

	if (ret)
	{
		if (SIM800_MessageEnable(hsim800, ret) == SIM800_OK)
		{

			for (uint16_t i = 0; i < (length/32); i++)
			{
				SIM800_SendData(hsim800, data, 32);
				data += 32;
				SIM800_Delay(hsim800, 5);
			}

			if ((length % 32) > 0)
			{
				SIM800_SendData(hsim800, data, (length % 32));
			}



			//indication_time = SOCK_SEND_INDICATE_DELAY;

			SIM800_Delay(hsim800, 50);

			return SIM800_OK;
		}
#ifdef DEBUG
		else
		{
			printf("sim800_messageEnable() is not OK\n");
		}
#endif
	}

	SIM800_Delay(hsim800, 1000);

	SIM800_CancelMsg(hsim800);

	return SIM800_ERR;
}


//----------------------------------------------------------------------------
uint8_t SIM800_Socket_SendData(sim800_socketTypeDef *socket, uint8_t *data, uint16_t length, uint16_t tout)
{

	SIM800_Obj *hsim800 = (SIM800_Obj *) socket->Arg;
	char tmp[32];
	char *ret = NULL;

	uint8_t id = SIM800_Socket_GetId(socket);


	if (hsim800->BusyTimeOut > 0)
	{
#ifdef DEBUG
		printf("SIM800 is busy...\r\n");
#endif
		return SIM800_BUSY;
	}

	sprintf(tmp,"AT+CIPSEND=%d,%d\r", id, length);

	ret = SIM800_AT_Send(hsim800, tmp, tout);

#ifdef DEBUG
	if(!ret){
		printf("Couldn't send AT+CIPSEND AT command\r\n");
	}
#endif

	if (ret)
	{
		if (SIM800_MessageEnable(hsim800, ret) == SIM800_OK)
		{

			for (uint16_t i = 0; i < (length/32); i++)
			{
				SIM800_SendData(hsim800, data, 32);
				data += 32;
				SIM800_Delay(hsim800, 5);
			}

			if ((length % 32) > 0)
			{
				SIM800_SendData(hsim800, data, (length % 32));
			}

			SIM800_Delay(hsim800, 50);

			return SIM800_OK;
		}
#ifdef DEBUG
		else
		{
			printf("sim800_messageEnable() is not OK\r\n");
		}
#endif
	}

	SIM800_Delay(hsim800, 1000);

	SIM800_CancelMsg(hsim800);

	return SIM800_ERR;
}

#endif
//---------------------------------------

SIM800_StatusTypeDef SIM800_Sockets_Handle(void *arg)
{
	SIM800_Obj *hsim800 = (SIM800_Obj *) arg;

#ifdef MONOSOCKET_MODE
		if ((hsim800->SocketTable[0].sockStatus != SOCKET_IN_USE) || (!sim800_prop.gprsActive))
		{
			hsim800->SocketTable[0].socket->status = CLOSED;

			if (hsim800->SocketTable[0].socket->closed_fn != NULL)
				hsim800->SocketTable[0].socket->closed_fn();
			return SIM800_ERR;
		}
#else
		if (!hsim800->GPRS_Status)
		{
			for (uint8_t sock_num = 0; sock_num < SIM800_NUM_CONNECTIONS; sock_num++)
			{
				if (hsim800->SocketTable[sock_num].sockStatus == SOCKET_IN_USE)
				{
					hsim800->SocketTable[sock_num].socket->status = CLOSED;

					SIM800_Socket_ClosedCallBack(hsim800->SocketTable[sock_num].socket);
				}
			}
			return SIM800_ERR;
		}
#endif

		char tmp[64]={0};
		char *repply = NULL;
		//Проверка статуса соединения
		repply = SIM800_AT_Send(hsim800, "AT+CIPSTATUS\r",3000);
#ifdef DEBUG
		printf(repply);
#endif
		SIM800_HandleSocketInfo(hsim800, (uint8_t *)repply);


#ifndef MONOSOCKET_MODE
		for (uint8_t sock_num = 0; sock_num < SIM800_NUM_CONNECTIONS; sock_num++)
		{
			//Если сокет не инициализирован, то итерация пропускается
			if (hsim800->SocketTable[sock_num].sockStatus != SOCKET_IN_USE)
				continue;
#else
			uint8_t sock_num = 0;
#endif

			//Если соединение не установлено, то организуется подключение
			if (hsim800->SocketTable[sock_num].socket->status != CONNECTED)
			{
				switch (hsim800->SocketTable[sock_num].socket->type)
				{
				case TCP_CLIENT_TYPE:
#ifdef MONOSOCKET_MODE
					sprintf(tmp,"AT+CIPSTART=\"TCP\",\"%s\",%s\r", hsim800->SocketTable[sock_num].socket->ip, hsim800->SocketTable[sock_num].socket->port);
#else
					sprintf(tmp,"AT+CIPSTART=%d,\"TCP\",\"%s\",%s\r",sock_num, hsim800->SocketTable[sock_num].socket->ip, hsim800->SocketTable[sock_num].socket->port);
#endif
					break;
				case UDP_CLIENT_TYPE:
#ifdef MONOSOCKET_MODE
					sprintf(tmp,"AT+CIPSTART=\"UDP\",\"%s\",%s\r", hsim800->SocketTable[sock_num].socket->ip, hsim800->SocketTable[sock_num].socket->port);
#else
					sprintf(tmp,"AT+CIPSTART=%d,\"UDP\",\"%s\",%s\r",sock_num, hsim800->SocketTable[sock_num].socket->ip, hsim800->SocketTable[sock_num].socket->port);
#endif
					break;
				default:
					tmp[0] = '\0';
					break;
				}

#ifdef DEBUG
				printf("Connect request is %s\r\n", tmp);
#endif
#ifdef USE_KEEPALIVE
				hsim800->SocketTable[sock_num].socket->keepAliveCnt = 0;
#endif

				if (strlen(tmp) > 0)
				{
					//SIM800_AT_Send(tmp,1000);
					repply = SIM800_AT_Send(hsim800, tmp,3000);
#ifdef DEBUG
					//printf("Client socket\n");
					printf("Repply for client connection request is %s\r\n", repply);
#endif

					//sim800_socketManager[sock_num].socket->status = CONNECTED;
				}
#ifdef DEBUG
				else
				{
					printf("Accept client socket\r\n");
				}
#endif

			}
			else
			{
				if (hsim800->SocketTable[sock_num].socket->keepAliveCnt < KEEPALIVE_VALUE)
				{
					hsim800->SocketTable[sock_num].socket->keepAliveCnt++;
				}
				else
				{
#ifdef DEBUG
					printf("Closing connection %d\r\n", sock_num);
#endif
					SIM800_TCP_Close(hsim800->SocketTable[sock_num].socket);
				}

				SIM800_Socket_PollCallBack(hsim800->SocketTable[sock_num].socket);
			}

#ifndef MONOSOCKET_MODE
	}
#endif

	return SIM800_OK;
}
//----------------------------------------------------------------------
#ifdef MONOSOCKET_MODE
static void SIM800_ReadSocketPacket(uint16_t len, uint32_t tout)
{
	 uint16_t counter = 0;
	 uint8_t readLength = 0;
	 uint16_t difference = 0;
	 uint32_t wait = tout;
	 uint32_t tickStart = sim800_GetSysTick();
	 char *ptr = NULL;
	 char *repply = NULL;

	 uint8_t tries = 0;

	 uint8_t status = 0;

	if (wait < HAL_MAX_DELAY)
	{
		wait++;
	}



	 while ((counter < len) && ((sim800_GetSysTick() - tickStart) < wait))
	 {
		difference = (len - counter);
		tries = 2;


		 if (difference > 200)
			 readLength = 200;
		 else
			 readLength = difference;

		 char tmp[64];
		 sprintf(tmp,"AT+CIPRXGET=2,%d\r",readLength);
		 repply = SIM800_AT_Send(tmp,5000);

		 while(--tries)
		 {
			ptr = strstr(repply,"\r\n+CIPRXGET: 2");
			//printf("Request for reading is %s \r\n", ptr);

			if (ptr != 0)
			{
				status = 1;
				break;
			}
			SIM800_Delay(hsim800, 80);
			repply = serial_getBytes(&sim800_serial);
		 }

		 if (status)
		 {
			 ptr = strstr(ptr," ");
			 ptr++; //Переход к информации о прочитанных данных
			 ptr = strstr(ptr, "\r\n"); //Переход к концу информации о данных
			 ptr += 2; //Переход к данным

			 memcpy(&sim800_socketManager[0].socket->socketBuf[counter],(uint8_t *)ptr,readLength);
			 counter += readLength;
		 }
	 }

}
#else

static void SIM800_Socket_ReadPacket(sim800_socketTypeDef *socket, uint16_t len, uint32_t tout)
{
	 SIM800_Obj *hsim800 = (SIM800_Obj *) socket->Arg;
	 uint16_t counter = 0;
	 uint8_t readLength = 0;
	 uint16_t difference = 0;
	 uint32_t wait = tout;
	 uint32_t tickStart = SIM800_GetSysTick(hsim800);
	 char *ptr = NULL;
	 char *repply = NULL;

	 uint8_t tries = 0;

	 uint8_t status = 0;

	if (wait < SIM800_MAX_DELAY)
	{
		wait++;
	}



	 while ((counter < len) && ((SIM800_GetSysTick(hsim800) - tickStart) < wait))
	 {
		difference = (len - counter);
		tries = 2;


		 if (difference > 200)
			 readLength = 200;
		 else
			 readLength = difference;

		 char tmp[64];
		 sprintf(tmp,"AT+CIPRXGET=2,%d,%d\r",socket->id,readLength);
		 repply = SIM800_AT_Send(hsim800, tmp,5000);

		 while(--tries)
		 {
			ptr = strstr(repply,"\r\n+CIPRXGET: 2");
			//printf("Request for reading is %s \r\n", ptr);

			if (ptr != 0)
			{
				status = 1;
				break;
			}
			SIM800_Delay(hsim800, 80);
			repply = Serial_GetBytes(hsim800->Serial);
		 }

		 if (status)
		 {
			 ptr = strstr(ptr," ");
			 ptr++; //Переход к информации о прочитанных данных
			 ptr = strstr(ptr, "\r\n"); //Переход к концу информации о данных
			 ptr += 2; //Переход к данным

			 memcpy(&socket->socketBuf[counter],(uint8_t *)ptr,readLength);
			 counter += readLength;
		 }
	 }

}
#endif

//-----------------------------------------------------------------------
#ifdef MONOSOCKET_MODE
SIM800_StatusTypeDef SIM800_SocketReceiveHandle()
{
	static uint16_t errCount = 0;
	char *repply = NULL;
	uint8_t tries = 2;
	uint8_t status = 0;

	if (sim800_socketManager[0].socket->status == CONNECTED)
	{
		 uint16_t length = 0;
		 char *ptr;
		 repply = SIM800_AT_Send("AT+CIPRXGET=4\r",5000);
		 //printf("Before checking: Repply for TCP packets request is %s\r\n", repply);

		 while(--tries)
		 {
			 ptr = strstr(repply,"\r\n+CIPRXGET: ");
			 if (ptr != 0)
			 {
				 status = 1;
				 break;
			 }
			 SIM800_Delay(hsim800, 80);
			 repply = serial_getBytes(&sim800_serial);
		 }

		 //sim800WaitRepply(5000, 100);
		 if (status)
		 {
			 ptr = strtok(ptr, ",");
			 ptr = strtok(NULL, ",");
			 if (ptr != NULL)
			 {
				 ptr = strtok(ptr, "\r\n");
				 length = atoi(ptr);
				 //printf("Packet len is %d \r\n", length);
			 }

			 if (length > 0)
			 {
				 if (length > SOCKET_BUF_LENGTH)
				 {
					 length = SOCKET_BUF_LENGTH - 4;
				 }
				 readSocketPacket(length, 10000);

				 sim800_socketManager[0].socket->recv_fn(sim800_socketManager[0].socket->socketBuf, length);

				 memset(sim800_socketManager[0].socket->socketBuf,0, SOCKET_BUF_LENGTH);
#ifdef USE_KEEPALIVE
				 socket->keepAliveCnt = 0;
#endif

				 errCount = 0;

				 if (length == (SOCKET_BUF_LENGTH - 4))
				 {
					 return SIM800_EXCEPT;
				 }
			 }
		 }
		 else
		 {
			 printf("Error of reading tcp gprs packet!\r\n");
			 printf("Errors count: %d \r\n", errCount);
			 printf("REPPLY after error is %s\r\n", repply);


			 //printf("Repply for TCP packets request is %s\r\n", repply);
			 errCount++;
			 if (errCount > SIM800_ERR_LIMIT)
			 {
				 //Закрытие соединения с деактивацией PDP контекста
				 SIM800_AT_Send("AT+CIPSHUT\r\n",2000);
				 //Сброс флага активации GPRS для переинициализации PDP контекста
				 sim800_prop.gprsActive = 0;
				 errCount = 0;
			 }
			 return SIM800_ERR;
		 }
	}


	return SIM800_OK;
}
#else

static SIM800_StatusTypeDef SIM800_SocketReceive(sim800_socketTypeDef *socket)
{
	SIM800_Obj *hsim800 = (SIM800_Obj *) socket->Arg;
	//static uint16_t errCount = 0;
	char *repply = NULL;
	uint8_t tries = 2;
	uint8_t status = 0;
	char cmd[64];

	if (socket->status == CONNECTED)
	{
		 uint16_t length = 0;
		 char *ptr;
		 sprintf(cmd, "AT+CIPRXGET=4,%d\r", socket->id);
		 repply = SIM800_AT_Send(hsim800, cmd,5000);
		 //printf("Before checking: Repply for TCP packets request is %s\r\n", repply);

		 while(--tries)
		 {
			 ptr = strstr(repply,"\r\n+CIPRXGET: ");
			 if (ptr != 0)
			 {
				 status = 1;
				 break;
			 }
			 SIM800_Delay(hsim800, 80);
			 repply = Serial_GetBytes(hsim800->Serial);
		 }

		 //sim800WaitRepply(5000, 100);
		 if (status)
		 {
			 ptr = strtok(ptr, ",");
			 ptr = strtok(NULL, ",");
			 ptr = strtok(NULL, ",");
			 if (ptr != NULL)
			 {
				 ptr = strtok(ptr, "\r\n");
				 length = atoi(ptr);
//				 printf("Packet len is %d \r\n", length);
			 }

			 if (length > 0)
			 {
				 if (socket->LastLength != length)
				 {
					 socket->LastLength = length;
					 return SIM800_EXCEPT;
				 }
				 printf("Total packet len is %d\r\n", length);
				 if (length > SOCKET_BUF_LENGTH)
				 {
					 length = SOCKET_BUF_LENGTH - 4;
				 }
				 SIM800_Socket_ReadPacket(socket, length, 10000);

				 if (socket->type == TCP_ACCEPTED_CLIENT_TYPE)
				 {
#ifdef DEBUG
					 printf("Received from accepted client\r\n");
#endif
					 if (hsim800->ServerRestartDelay > (SIM800_SERVER_RESTART_DELAY - SIM800_SERVER_RECEVIE_DELAY))
					 {
						 hsim800->ServerRestartDelay = (SIM800_SERVER_RESTART_DELAY - SIM800_SERVER_RECEVIE_DELAY);
					 }
				 }

				 SIM800_Socket_ReceivedCallBack(socket, socket->socketBuf, length);

				 memset(socket->socketBuf,0, SOCKET_BUF_LENGTH);

#ifdef USE_KEEPALIVE
				 socket->keepAliveCnt = 0;
#endif

				 hsim800->ErrCnt = 0;

				 if (length == (SOCKET_BUF_LENGTH - 4))
				 {
					 return SIM800_EXCEPT;
				 }
			 }
			 socket->LastLength = length;
		 }
		 else
		 {
			 printf("Error of reading tcp gprs packet!\r\n");
			 printf("Errors count: %d \r\n", hsim800->ErrCnt);
			 printf("REPPLY after error is %s\r\n", repply);


			 //printf("Repply for TCP packets request is %s\r\n", repply);
			 hsim800->ErrCnt++;
			 if (hsim800->ErrCnt > SIM800_ERR_LIMIT)
			 {
				 //Закрытие соединения с деактивацией PDP контекста
				 SIM800_AT_Send(hsim800, "AT+CIPSHUT\r\n",2000);
				 //Сброс флага активации GPRS для переинициализации PDP контекста
				 hsim800->GPRS_Status = false;
				 hsim800->ErrCnt = 0;
			 }
			 return SIM800_ERR;
		 }
	}


	return SIM800_OK;
}

//---------------------------------------------------------------------
SIM800_StatusTypeDef SIM800_Sockets_ReceiveHandle(void *arg)
{
	SIM800_Obj *hsim800 = (SIM800_Obj *) arg;
	for (uint8_t sockNum = 0; sockNum < SIM800_NUM_CONNECTIONS; sockNum++)
	{
		SIM800_SocketReceive(hsim800->SocketTable[sockNum].socket);
		SIM800_Delay(hsim800, 10);
	}

	return SIM800_OK;
}
#endif


// Инициализация сокетов по умолчанию
void SIM800_Sockets_Init(SIM800_Obj *hsim800)
{
	for (uint8_t i = 0; i < SIM800_NUM_CONNECTIONS; i++)
	{
		if (SIM800_Socket_Add(hsim800, &hsim800->Default_Sockets[i]) == 0)
		{
			SIM800_TCP_Accept(&hsim800->Default_Sockets[i]);
		}
	}
}
