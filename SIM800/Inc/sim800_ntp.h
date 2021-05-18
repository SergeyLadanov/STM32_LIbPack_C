/*
 * sim800_ntp.h
 *
 *  Created on: 11 дек. 2020 г.
 *      Author: serge
 */

#ifndef SIM800_INC_SIM800_NTP_H_
#define SIM800_INC_SIM800_NTP_H_
#ifdef __cplusplus
 extern "C" {
#endif
#include "sim800_main.h"

SIM800_StatusTypeDef SIM800_NTP_Handle(void *arg);
void SIM800_NTP_Enable(SIM800_Obj *hsim800);
void SIM800_NTP_Disable(SIM800_Obj *hsim800);

#ifdef __cplusplus
}
#endif

#endif /* SIM800_INC_SIM800_NTP_H_ */
