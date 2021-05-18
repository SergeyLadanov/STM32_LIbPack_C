/*
 * sim800_main.h
 *
 *  Created on: 11 дек. 2020 г.
 *      Author: serge
 */

#ifndef SIM800_INC_SIM800_MAIN_H_
#define SIM800_INC_SIM800_MAIN_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "tsm.h"
#include <stdio.h>
//---------------------------------------------
#include "serial.h"
#include <stdbool.h>

#define SIM800_MAX_DELAY 0xFFFFFFFFU

#define SOCKET_BUF_LENGTH 1024
//Периоды опросов устройства
#define NET_CHECK_PERIOD 10000
#define SMS_SEND_TIMEOUT 15000
#define SMS_CHECK_PERIOD  30000
//---------------------------------
#define SMS_SEND_INDICATE_DELAY 3000
#define SOCK_SEND_INDICATE_DELAY 1000
#define SIM800_NOANSWER_LIMIT 50
#define SIM800_ERR_LIMIT 30


#define SIM800_SERVER_MAX_TRIES 35
#define SIM800_SERVER_RESTART_DELAY 960 // Период перезапуска сервера
#define SIM800_SERVER_RECEVIE_DELAY 5 // Задержка отложенного перезапуска сервера

//#define MONOSOCKET_MODE //Режим одного подключения
#define USE_GPRS //Использование пакетной передачи данных
#define USE_NTP //Использование NTP синхронизации времени
#define USE_KEEPALIVE //Использование таймаута на бездействие подключения

#ifdef USE_KEEPALIVE
#define KEEPALIVE_VALUE 8 //Значение представлено в количествах вызова функции проверки соедиения (по умолчанию 15 секунд)
#endif


#ifndef MONOSOCKET_MODE
#define SIM800_NUM_CONNECTIONS 6
#else
#define SIM800_NUM_CONNECTIONS 1
#endif



typedef enum{
	SIM800_OK = 0,
	SIM800_ERR = 1,
	SIM800_BUSY = 2,
	SIM800_EXCEPT = 3,
	SIM800_NORESULT = 4
}SIM800_StatusTypeDef;
//--------------------------
typedef enum{
	SOCKET_INITIAL = 0,
	SOCKET_READY = 1,
	SOCKET_IN_USE = 2
}SIM800_SoccketStatusTypeDef;
//---------------------------
typedef enum{
	NO_DEFINITION = 0,
	TCP_ACCEPTED_CLIENT_TYPE = 1,
	TCP_CLIENT_TYPE = 2,
	UDP_SERVER_TYPE = 3,
	UDP_CLIENT_TYPE = 4
}SIM800_SocketType;
//---------------------------
typedef enum{
	CLOSED = 0,
	CONNECTED = 1
}SIM800_socketStatusTypeDef;
//----------------------------

typedef void (*restart_fn_typedef)(void);

//---------------------------------
typedef struct {
	uint8_t netPresent;
	uint16_t noAnswerCnt;
	uint8_t tcpConnect;
	uint16_t busyTimeout;
	uint8_t gprsActive;
	uint8_t gprsEnable;
	uint32_t sysTick;
}sim800_prop_typedef;
//-------------------------------------
typedef struct{
	uint8_t bearer;
	char apn[40];
	char user[20];
	char pswd[20];
}sim800_gprsProfileTypeDef;
//--------------------------
typedef struct __sim800_SocketTypeDef{
	void *Arg;
	SIM800_SocketType type;
	uint8_t *socketBuf;
	char ip[16];
	char port[10];
	uint8_t efforts;
	uint8_t status;
	uint8_t id;
	uint8_t keepAliveCnt;
	uint16_t LastLength;
}sim800_socketTypeDef;
//-----------------------------------
typedef struct {
	sim800_socketTypeDef *socket;
	SIM800_SoccketStatusTypeDef sockStatus;
}sim800_socketManagerTypeDef;
//--------------------------------------
typedef struct
{
	uint8_t status;
	uint32_t tickstart;
}taskHandlerPropTypeDef;
//--------------------------------------------


/***************************НОВЫЕ СТРУКТУРЫ****************/
// Структура объекта
typedef struct __SIM800_Obj{
	SerialTypeDef *Serial;
	sim800_socketManagerTypeDef SocketTable[SIM800_NUM_CONNECTIONS];
	sim800_socketTypeDef Default_Sockets[SIM800_NUM_CONNECTIONS];
	sim800_gprsProfileTypeDef GPRS_Profile;
	uint8_t SocketCnt;
	TSM_Obj NetStatusTask;
	TSM_Obj SMS_Task;
	TSM_Obj NTP_Task;
	TSM_Obj SocketTask;
	TSM_Obj IndicationTask;
	TSM_Obj SocketReceiveTask;
	uint16_t NoAnswCnt;
	uint16_t BusyTimeOut;
	bool GPRS_Enable;
	uint16_t ServerPort;
	uint32_t Tick;
	uint32_t InitDelay;
	restart_fn_typedef Restart;
	uint8_t CheckNetEfforts;
	uint8_t HardResetDelay;
	bool NetStatus;
	bool GPRS_Status;
	uint16_t ErrCnt;
	bool NTP_Enable;
	uint32_t ServerRestartDelay;
	uint8_t ServerTries;
}SIM800_Obj;
/*********************************************************/



// Функции общего назначения
char* SIM800_AT_Send(SIM800_Obj *hsim800, char *str, volatile uint16_t timeoutDelay);
void SIM800_Restart(SIM800_Obj *hsim800);
void SIM800_Delay(SIM800_Obj *hsim800, uint32_t Delay);
SIM800_StatusTypeDef SIM800_MessageEnable(SIM800_Obj *hsim800, char *rep);
uint32_t SIM800_GetSysTick(SIM800_Obj *hsim800);
void SIM800_RestartSystem(SIM800_Obj *hsim800);
void SIM800_HardRestart(SIM800_Obj *hsim800);
uint8_t SIM800_SendData(SIM800_Obj *hsim800, uint8_t *pData, uint16_t length);
uint8_t SIM800_SMS_Send(SIM800_Obj *hsim800, char *number, char *text, uint16_t tout);
void SIM800_NTP_RepplyHandler(SIM800_Obj *hsim800);
void SIM800_ResetNoAnswer(SIM800_Obj *hsim800);
SIM800_StatusTypeDef SIM800_Init(SIM800_Obj *hsim800,  SerialTypeDef *hs);
void SIM800_Setup(SIM800_Obj *hsim800);
void SIM800_Start(SIM800_Obj *hsim800);
void SIM800_IncTick(SIM800_Obj *hsim800);
void SIM800_NTP_Init(SIM800_Obj *hsim800, char *server);
void SIM800_IMEI_Get(SIM800_Obj *hsim800, char *outBuf);
void SIM800_MessageHandler(SIM800_Obj *hsim800);
void SIM800_GPRS_SetProfile(SIM800_Obj *hsim800, uint8_t bearer,char *apn, char *user, char *pswd);
void SIM800_GPRS_Enable(SIM800_Obj *hsim800);
void SIM800_GPRS_Disable(SIM800_Obj *hsim800);
void SIM800_ApplyMsg(SIM800_Obj *hsim800);
void SIM800_CancelMsg(SIM800_Obj *hsim800);



// Обработчики задач
SIM800_StatusTypeDef SIM800_SMS_Handle(void *arg);
SIM800_StatusTypeDef SIM800_Network_Handle(void *arg);

// Прототипы внешних модулей
void SIM800_TCP_Init(SIM800_Obj *hsim800);
uint8_t SIM800_Socket_GetId(sim800_socketTypeDef *socket);
sim800_socketTypeDef* SIM800_Socket_GetById(SIM800_Obj *hsim800, uint8_t id);
void SIM800_Reset_Enable(void);
void SIM800_Reset_Disable(void);

// Прототипы обработчиков событий
void SIM800_NTP_RepplyCallBack(SIM800_Obj *hsim800, char *repply);
void SIM800_OnNetFoundCallBack(SIM800_Obj *hsim800);
void SIM800_OnNetLostCallBack(SIM800_Obj *hsim800);
void SIM800_NTP_BeforeRequestCallBack(SIM800_Obj * hsim800);
void SIM800_SMS_ReceivedCallBack(SIM800_Obj *hsim800, char* number, char* text);

// Прототипы внешних модулей
void SIM800_Socket_PollCallBack(sim800_socketTypeDef *socket);
void SIM800_Socket_ConnectedCallBack(sim800_socketTypeDef *socket);
void SIM800_Socket_ClosedCallBack(sim800_socketTypeDef *socket);
void SIM800_Socket_SentCallBack(sim800_socketTypeDef *socket);
void SIM800_Socket_ReceivedCallBack(sim800_socketTypeDef *socket, uint8_t *buf, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* SIM800_INC_SIM800_MAIN_H_ */
