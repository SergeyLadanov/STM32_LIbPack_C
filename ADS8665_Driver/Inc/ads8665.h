/*
 * ads8691.h
 *
 *  Created on: 19 апр. 2018 г.
 *      Author: Acer
 */

#ifndef ADS8665_DRIVER_INC_ADS8665_H_
#define ADS8665_DRIVER_INC_ADS8665_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "main.h"

//Количество устройств в цепочке
#define MAX_DEVICES_NUMBER 6

// Номера АЦП
#define DEVICE_1  0
#define DEVICE_2  1
#define DEVICE_3  2
#define DEVICE_4  3
#define DEVICE_5  4
#define DEVICE_6  5
#define DEVICE_7  6
#define DEVICE_8  7
#define DEVICE_9  8
#define DEVICE_10 9
#define DEVICE_11 10
#define DEVICE_12 11
/****************КОМАНДЫ*************************/
#define CMD_NOP 		(uint32_t)(0b00000000000000000000000000000000)
#define CMD_CLEAR_HWORD (uint32_t)(0b11000000000000000000000000000000)
#define CMD_READ_HWORD 	(uint32_t)(0b11001000000000000000000000000000)
#define CMD_READ_BYTE 	(uint32_t)(0b01001000000000000000000000000000)
#define CMD_WRITE	 	(uint32_t)(0b11010000000000000000000000000000)
#define CMD_WRITE_MSB	(uint32_t)(0b11010010000000000000000000000000)
#define CMD_WRITE_LSB	(uint32_t)(0b11010100000000000000000000000000)
#define CMD_SET_HWORD	(uint32_t)(0b11011000000000000000000000000000)
/************************************************/
//Регистры
#define DEVICE_ID_REG  	0x00
#define RST_PWRCTL_REG 	0x04
#define SDI_CTL_REG 	0x08
#define SDO_CTL_REG 	0x0C
#define DATAOUT_CTL_REG 0x10
#define RANGE_SEL_REG 	0x14
#define ALARM_REG 		0x20
#define ALARM_H_TH_REG 	0x24
#define ALARM_L_TH_REG 	0x28
/************************************************/
//Настройки
#define INTREF_DISABLE 	 (uint16_t)(0b0000000001000000)
#define INTREF_ENABLE	 (uint16_t)(0b0000000000000000)
//Настройки биполярного диапазона
#define RANGE_PN_3_0xVREF 		0b0000
#define RANGE_PN_2_5xVREF 		0b0001
#define RANGE_PN_1_5xVREF 		0b0010
#define RANGE_PN_1_25xVREF 		0b0011
#define RANGE_PN_0_625xVREF 	0b0100
//Настройки однополярного диапазона
#define RANGE_P_3_0xVREF 		0b1000
#define RANGE_P_2_5xVREF 		0b1001
#define RANGE_P_1_5xVREF 		0b1010
#define RANGE_P_1_25xVREF 		0b1011

#define ADS8665_CONV_SET 1
#define ADS8665_CONV_RESET 0
/************************************************/
typedef struct __ADS8665_HandleObj{
	void *Arg;
	uint8_t DeviceNumber;
	uint32_t *CmdBuf;
	uint32_t *VoidBuf;
} ADS8665_HandleObj;

//-------------------------------------------------------------------------------
// Прототипы шаблонных функций
void ADS8665_WriteRawData(ADS8665_HandleObj *hadc, uint8_t *txData, uint16_t len);
void ADS8665_ReadRawData(ADS8665_HandleObj *hadc, uint8_t *rxData, uint16_t len);
uint16_t ADS8665_ReadRVS(ADS8665_HandleObj *hadc);
void ADS8665_ConvComplete(ADS8665_HandleObj *hadc, float *dataf);
void ADS8665_WriteConv(ADS8665_HandleObj *hadc, uint8_t state);
//--------------------------------------------------------------------------------
void ADS8665_SetArg(ADS8665_HandleObj *hadc, void *arg);
void ADS8665_SendCommands(ADS8665_HandleObj *hadc);
void ADS8665_Read(ADS8665_HandleObj *hadc, uint8_t *rxData, uint16_t len);
void ADS8665_Write(ADS8665_HandleObj *hadc, uint8_t *txData, uint16_t len);
uint8_t ADS8665_CmdPrepare(ADS8665_HandleObj *hadc, uint8_t deviceNumver, uint32_t cmd, uint16_t adr, uint16_t data);

void ADS8665_Init(ADS8665_HandleObj *hadc, uint8_t deviceNumber);
void ADS8665_HandleResult(ADS8665_HandleObj *hadc, uint8_t *input, int32_t *output);
uint32_t *ADS8665_GetVoidBuf(void);
void ADS8665_StartContinuousConv(ADS8665_HandleObj *hadc);
void ADS8665_StopContinuousConv(ADS8665_HandleObj *hadc);

#ifdef __cplusplus
}
#endif

#endif /* ADS8665_DRIVER_INC_ADS8665_H_ */
