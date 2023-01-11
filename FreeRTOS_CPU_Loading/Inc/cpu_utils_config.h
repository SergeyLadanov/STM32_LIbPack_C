/*
 * cpu_utils_config.h
 *
 *  Created on: 11 янв. 2023 г.
 *      Author: serge
 */

#ifndef INC_CPU_UTILS_CONFIG_H_
#define INC_CPU_UTILS_CONFIG_H_

#include "main.h"

/* Exported defines ------------------------------------------------------------*/
#define OS_CPU_CALCULATION_PERIOD    	1000
#define OS_TIMER_VALUE_FOR_PERIOD    	50000
#define OS_TIMER_MAXVALUE    			60000
#define OS_GET_TIMERVALUE() 			TIM1->CNT


#endif /* INC_CPU_UTILS_CONFIG_H_ */
