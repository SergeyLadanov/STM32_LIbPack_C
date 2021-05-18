/*
 * sim800_socket.h
 *
 *  Created on: 11 дек. 2020 г.
 *      Author: serge
 */

#ifndef SIM800_INC_SIM800_SOCKET_H_
#define SIM800_INC_SIM800_SOCKET_H_
#ifdef __cplusplus
 extern "C" {
#endif
#include "sim800_main.h"


SIM800_StatusTypeDef SIM800_Sockets_Handle(void *arg);
SIM800_StatusTypeDef SIM800_Sockets_ReceiveHandle(void *arg);
uint8_t SIM800_Socket_Add(SIM800_Obj *hsim800, sim800_socketTypeDef *socket);
uint8_t SIM800_Socket_SendData(sim800_socketTypeDef *socket, uint8_t *data, uint16_t length, uint16_t tout);
void SIM800_Sockets_Init(SIM800_Obj *hsim800);

void SIM800_TCP_Close(sim800_socketTypeDef *socket);
void SIM800_TCP_Listen(SIM800_Obj *hsim800, uint16_t port);
void SIM800_TCP_Connect(sim800_socketTypeDef *tcp, char *ip, char *port);
void SIM800_TCP_Accept(sim800_socketTypeDef *tcp);
void SIM800_TCP_Listen(SIM800_Obj *hsim800, uint16_t port);


#ifdef __cplusplus
}
#endif

#endif /* SIM800_INC_SIM800_SOCKET_H_ */
