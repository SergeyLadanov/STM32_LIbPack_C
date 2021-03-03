#include <ads8665.h>
#include "stdio.h"


static uint32_t voidBuf[MAX_DEVICES_NUMBER];// = {0xAAAA5555, 0xAAAA5555, 0xAAAA5555, 0xAAAA5555, 0xAAAA5555, 0xAAAA5555};
static uint32_t cmdBuf[MAX_DEVICES_NUMBER] = {0};


/************************ШАБЛОНЫ БАЗЫОВЫХ ФУНКЦИЙ****************************/
// Функции передачи данных данными
__weak void ADS8665_WriteRawData(ADS8665_HandleObj *hadc, uint8_t *txData, uint16_t len)
{
	UNUSED(hadc);
	UNUSED(txData);
	UNUSED(len);
}

// Функция приема данных
__weak void ADS8665_ReadRawData(ADS8665_HandleObj *hadc, uint8_t *rxData, uint16_t len)
{
	UNUSED(hadc);
	UNUSED(rxData);
	UNUSED(len);
}

// Чтения RVS
__weak uint16_t ADS8665_ReadRVS(ADS8665_HandleObj *hadc)
{
	UNUSED(hadc);

	return 0;
}
// Обработчик окончания преобразования
__weak void ADS8665_ConvComplete(ADS8665_HandleObj *hadc, float *dataf)
{
	UNUSED(hadc);
	UNUSED(dataf);
}
// Функция запуска непрерывного преобразования
__weak void ADS8665_StartContinuousConv(ADS8665_HandleObj *hadc)
{
	UNUSED(hadc);
}
// Функция остановки непрерывного преобразования
__weak void ADS8665_StopContinuousConv(ADS8665_HandleObj *hadc)
{
	UNUSED(hadc);
}
// Функция управления пином CONV
__weak void ADS8665_WriteConv(ADS8665_HandleObj *hadc, uint8_t state)
{
	UNUSED(hadc);
	UNUSED(state);
}
/**********************************************************************************/
// Передача аргумента
void ADS8665_SetArg(ADS8665_HandleObj *hadc, void *arg)
{
	hadc->Arg = arg;
}
// Получение указателя на пустой буфер
uint32_t *ADS8665_GetVoidBuf(void)
{
	return voidBuf;
}
//Функция передачи данных на АЦП
void ADS8665_Write(ADS8665_HandleObj *hadc, uint8_t *txData, uint16_t len)
{
	uint16_t timeOut = 3000;

	ADS8665_WriteConv(hadc, ADS8665_CONV_RESET);

	while (ADS8665_ReadRVS(hadc))
	{
		HAL_Delay(1);

		if (timeOut > 0)
			timeOut--;
		else
		{
#ifdef DEBUG
			printf("Error of init ADC!\r\n");
#endif
			break;
		}

	}

	ADS8665_WriteRawData(hadc, txData, len);
	ADS8665_WriteConv(hadc, ADS8665_CONV_SET);
}
//-------------------------------------------------
void ADS8665_Read(ADS8665_HandleObj *hadc, uint8_t *rxData, uint16_t len)
{
	ADS8665_WriteConv(hadc, ADS8665_CONV_RESET);
	while (ADS8665_ReadRVS(hadc));
	ADS8665_ReadRawData(hadc, rxData, len);
	ADS8665_WriteConv(hadc, ADS8665_CONV_SET);
}
//---------------------------------------------------
void ADS8665_SendCommands(ADS8665_HandleObj *hadc)
{
	ADS8665_Write(hadc, (uint8_t *)hadc->CmdBuf, hadc->DeviceNumber * 4);
}
// Функция подготовки команды
uint8_t ADS8665_CmdPrepare(ADS8665_HandleObj *hadc, uint8_t deviceNumver, uint32_t cmd, uint16_t adr, uint16_t data)
{
	uint32_t tmp = cmd | (adr << 16) | data;
	uint8_t *ptr = (uint8_t *) &hadc->CmdBuf[deviceNumver];

	ptr[0] = ((tmp & 0xFF000000) >> 24);
	ptr[1] = ((tmp & 0x00FF0000) >> 16);
	ptr[2] = ((tmp & 0x0000FF00) >> 8);
	ptr[3] = ((tmp & 0x000000FF));

	return sizeof(uint32_t);
}

//Функция обработки результата
void ADS8665_HandleResult(ADS8665_HandleObj *hadc, uint8_t *input, int32_t *output)
{
	int32_t result = 0;
	uint8_t pos = 0;
	for (uint8_t i = 0; (i < hadc->DeviceNumber * 4); i+=4)
	{
		//result = (((input[i] << 16) | (input[i+1] << 8) | (input[i+2])) >> 6) - 131072;

		result = ((input[i] << 4) | (input[i+1] >> 4)) - 2048;
		output[pos] = result;//*0.00009375;
		pos++;
	}
}

// Функция инициализации объекта
void ADS8665_Init(ADS8665_HandleObj *hadc, uint8_t deviceNumber)
{
	hadc->VoidBuf = voidBuf;
	hadc->CmdBuf = cmdBuf;
	hadc->DeviceNumber = deviceNumber;
}
