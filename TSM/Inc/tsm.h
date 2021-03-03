/*
 * tsm.h
 *
 *  Created on: Dec 8, 2020
 *      Author: serge
 *      TSM - Timer State Machine
 */

#ifndef TSM_INC_TSM_H_
#define TSM_INC_TSM_H_


#include "main.h"

// Статусы
typedef enum{
	TSM_OK = 0,
	TSM_ERR = 1,
	TSM_BUSY = 2,
	TSM_EXCEPT = 3,
	TSM_NORESULT = 4
}TSM_StatusTypeDef;

// Структура объекта менеджера задачи
typedef struct __TSM_Obj
{
	uint8_t Status;
	uint32_t TickStart;
}TSM_Obj;


uint32_t TSM_GetTick(void);
void TSM_IncTick(void);
void TSM_Handler(TSM_Obj *thp, uint32_t period, void *arg, uint8_t (*pf)(void *arg));
void TSM_Execute(TSM_Obj *thp);


#endif /* TSM_INC_TSM_H_ */
