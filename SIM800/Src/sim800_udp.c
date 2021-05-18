/*
 * sim800_udp.c
 *
 *  Created on: 11 дек. 2020 г.
 *      Author: serge
 */
#include "sim800_udp.h"

void sim800_udpConnect(sim800_socketTypeDef *udp, char *ip, char *port)
{
	SIM800_Obj *hsim800 = (SIM800_Obj *) udp->Arg;
	sprintf(udp->ip,"%s", ip);
	sprintf(udp->port,"%s", port);
	udp->type = UDP_CLIENT_TYPE;
	hsim800->SocketTable[SIM800_Socket_GetId(udp)].sockStatus = SOCKET_IN_USE;
	TSM_Execute(&hsim800->SocketTask);
}

