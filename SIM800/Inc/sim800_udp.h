/*
 * sim800_udp.h
 *
 *  Created on: 11 дек. 2020 г.
 *      Author: serge
 */

#ifndef SIM800_INC_SIM800_UDP_H_
#define SIM800_INC_SIM800_UDP_H_
#ifdef __cplusplus
 extern "C" {
#endif
#include "sim800_main.h"

void sim800_udpConnect(sim800_socketTypeDef *udp, char *ip, char *port);

#ifdef __cplusplus
}
#endif
#endif /* SIM800_INC_SIM800_UDP_H_ */
