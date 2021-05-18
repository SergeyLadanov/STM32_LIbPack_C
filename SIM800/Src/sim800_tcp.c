/*
 * sim800_tcp.c
 *
 *  Created on: 11 дек. 2020 г.
 *      Author: serge
 */
#include "sim800_tcp.h"


void SIM800_TCP_Close(sim800_socketTypeDef *socket)
{
	SIM800_Obj *hsim800 = (SIM800_Obj *) socket->Arg;
	char tmp[64];
	sprintf(tmp,"AT+CIPCLOSE=%d,1\r",socket->id); //1 - быстрое закрытие соединения
	SIM800_AT_Send(hsim800, tmp, 1000);
	socket->status = CLOSED;
#ifdef USE_KEEPALIVE
	socket->keepAliveCnt = 0;
#endif
}
//----------------------------------------------------------------------

void SIM800_TCP_Connect(sim800_socketTypeDef *tcp, char *ip, char *port)
{
	SIM800_Obj *hsim800 = (SIM800_Obj *) tcp->Arg;
	sprintf(tcp->ip,"%s", ip);
	sprintf(tcp->port,"%s", port);
	tcp->type = TCP_CLIENT_TYPE;
	hsim800->SocketTable[SIM800_Socket_GetId(tcp)].sockStatus = SOCKET_IN_USE;
	TSM_Execute(&hsim800->SocketTask);
}
//--------------------------------------------------------------------
void SIM800_TCP_Accept(sim800_socketTypeDef *tcp)
{
	SIM800_Obj *hsim800 = (SIM800_Obj *) tcp->Arg;
	tcp->type = TCP_ACCEPTED_CLIENT_TYPE;
	hsim800->SocketTable[SIM800_Socket_GetId(tcp)].sockStatus = SOCKET_IN_USE;
}
//-----------------------------------------------------------------------
void SIM800_TCP_Listen(SIM800_Obj *hsim800, uint16_t port)
{
	hsim800->ServerPort = port;
}
//---------------------------------------------------------------------


//-----------------------------------------------------------------------
void SIM800_TCP_Init(SIM800_Obj *hsim800)
{
	   char tmp[100];
#ifdef DEBUG
	   char *repply;
#endif
	  //Настройки TCP IP
	  SIM800_AT_Send(hsim800, "AT+CIPMODE=0\r",3000); // Командный режим передачи данных;
#ifdef MONOSOCKET_MODE
	  SIM800_AT_Send(hsim800, "AT+CIPMUX=0\r",3000); // Моносокет;
#else
#ifdef DEBUG
	  repply = SIM800_AT_Send(hsim800, "AT+CIPMUX=1\r",3000);
#else
	  SIM800_AT_Send(hsim800, "AT+CIPMUX=1\r",3000);
#endif
#endif

#ifdef DEBUG
	  printf("%s\r\n", repply);
	  repply = SIM800_AT_Send(hsim800, "AT+CIPRXGET=1\r", 1000);
	  printf("%s\r\n", repply);
	  repply = SIM800_AT_Send(hsim800, "AT+CIPSTATUS\r",3000); //Проверка статуса соединения
	  printf("%s\r\n", repply);

	  //sim800WaitRepply(3000, 0);
      sprintf(tmp, "AT+CSTT=\"%s\",\"%s\",\"%s\"\r",hsim800->GPRS_Profile.apn, hsim800->GPRS_Profile.user, hsim800->GPRS_Profile.pswd);
      repply = SIM800_AT_Send(hsim800, tmp, 3000); //
      printf("%s\r\n", repply);
      repply = SIM800_AT_Send(hsim800, "AT+CIPSTATUS\r",3000);
      printf("%s\r\n", repply);

#else
	  SIM800_AT_Send(hsim800, "AT+CIPRXGET=1\r", 1000);
	  SIM800_AT_Send(hsim800, "AT+CIPSTATUS\r",3000); //Проверка статуса соединения
	  sprintf(tmp, "AT+CSTT=\"%s\",\"%s\",\"%s\"\r",hsim800->GPRS_Profile.apn, hsim800->GPRS_Profile.user, hsim800->GPRS_Profile.pswd);
      SIM800_AT_Send(hsim800, tmp, 3000); //
      SIM800_AT_Send(hsim800, "AT+CIPSTATUS\r",3000);
#endif

	  SIM800_AT_Send(hsim800, "AT+CIPRXGET?\r", 1000);



	  if (SIM800_AT_Send(hsim800, "AT+CIICR\r",5000)) // Активация контекста;
	  {
#ifdef DEBUG
		  repply = SIM800_AT_Send(hsim800, "AT+CIPSTATUS\r",3000);
		  printf("%s\r\n", repply);
		  repply = SIM800_AT_Send(hsim800, "AT+CIFSR\r",3000); // IP
		  printf("%s\r\n", repply);
		  repply = SIM800_AT_Send(hsim800, "AT+CIPSTATUS\r",3000);
		  printf("%s\r\n", repply);
#else
		  SIM800_AT_Send(hsim800, "AT+CIPSTATUS\r",3000);
		  SIM800_AT_Send(hsim800, "AT+CIFSR\r",3000); // IP
		  SIM800_AT_Send(hsim800, "AT+CIPSTATUS\r",3000);
#endif
	  }
}
