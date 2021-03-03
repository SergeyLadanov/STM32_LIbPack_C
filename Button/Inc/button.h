/*
 * button.h
 *
 *  Created on: Jan 7, 2021
 *      Author: Acer
 */

#ifndef BUTTON_INC_BUTTON_H_
#define BUTTON_INC_BUTTON_H_
#include "main.h"

#define BUTTON_HOLD_TIME 1000
#define BUTTON_INVERSE 0


#define BUTTON_MULTICLICK_CLICK_NUM 3
#define BUTTON_MULTICLICK_CLICK_DELAY 350
#define BUTTON_MULTICLICK_CLICK_HOLD_DIVIDER 15


#ifdef __cplusplus
 extern "C" {
#endif


// Работа с кнопкой в обычном режиме
typedef struct __ButtonDescriptor{
	GPIO_TypeDef *GPIO_Port;
	uint16_t Pin;
	void (*PressedCallBack)(void);
	void (*HoldCallBack)(void);
} ButtonDescriptor;


typedef struct __ButtonProperties{
	uint16_t HoldDelay;
} ButtonProperties;

typedef struct __ButtonObj
{
	const ButtonDescriptor *Desc;
	ButtonProperties *Prop;
	uint8_t Count;
}ButtonObj;

// Работа с кнопкой в режиме мультикликов
typedef struct __ButtonMultiClickDescriptor{
	GPIO_TypeDef *GPIO_Port;
	uint16_t Pin;
	void (*X1ClickCallBack)(void);
	void (*X2ClickCallBack)(void);
	void (*X3ClickCallBack)(void);
	void (*HoldEventCallBack)(void);
	void (*HoldCallBack)(void);
} ButtonMultiClickDescriptor;


typedef struct __ButMultiClickProperties{
	uint16_t HoldDelay;
	uint8_t ClickNum;
	uint16_t ClickDelay;
	uint16_t HoldDivider;
} ButMultiClickProperties;

typedef struct __ButtonMultiClickObj
{
	const ButtonMultiClickDescriptor *Desc;
	ButMultiClickProperties *Prop;
	uint8_t Count;
}ButtonMultiClickObj;


void Button_Handle(const ButtonObj *hbut);
void ButtonMultiClick_Handle(const ButtonMultiClickObj *hbut);
const ButtonObj* Button_GetInstance(void);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_INC_BUTTON_H_ */
