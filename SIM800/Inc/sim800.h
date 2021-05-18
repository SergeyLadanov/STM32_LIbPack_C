/*
 * sim800.h
 *
 *  Created on: 21 дек. 2017 г.
 *      Author: 1
 */

#ifndef SIM800_H_
#define SIM800_H_
#ifdef __cplusplus
 extern "C" {
#endif
#include "sim800_main.h"
#include "sim800_tcp.h"
#include "sim800_udp.h"
#include "sim800_ntp.h"
#include "sim800_socket.h"


#ifndef UNUSED
#define UNUSED(X) (void)(X)
#endif

#ifndef __weak
#define __weak __attribute__((weak))
#endif

typedef enum
{
	SIM800_TICK_FREQ_10HZ         = 100U,
	SIM800_TICK_FREQ_100HZ        = 10U,
	SIM800_TICK_FREQ_1KHZ         = 1U,
	SIM800_TICK_FREQ_DEFAULT      = HAL_TICK_FREQ_1KHZ
} SIM800_TickFreqTypeDef;

void SIM800_Handle(SIM800_Obj *hsim800);

#ifdef __cplusplus
}
#endif

#endif /* SIM800_H_ */
