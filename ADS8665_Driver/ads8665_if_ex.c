/*
 * ads8601_if.c
 *
 *  Created on: 24 нояб. 2020 г.
 *      Author: Acer
 */
#include "ads8665.h"
#include "app.h"

#define OPTMIZATION 1

extern TIM_HandleTypeDef htim3;
extern SPI_HandleTypeDef hspi1, hspi2, hspi4;
extern ADS8665_HandleObj adc_gr1, adc_gr2, adc_gr3;
extern SR_Obj hsr;




// Массивы для сбора raw данных
static uint8_t adc_mass1[ADC_DEVICE_NUMBER_GR1 * 4];
static uint8_t adc_mass2[ADC_DEVICE_NUMBER_GR2 * 4];
static uint8_t adc_mass3[ADC_DEVICE_NUMBER_GR3 * 4];

// Массивы с рассчитанными значениями
static int32_t adc_result1[ADC_DEVICE_NUMBER_GR1];
static int32_t adc_result2[ADC_DEVICE_NUMBER_GR2];
static int32_t adc_result3[ADC_DEVICE_NUMBER_GR3];



typedef struct
{
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
} DMA_Base_Registers;

typedef struct
{
  __IO uint32_t ISR;   /*!< BDMA interrupt status register */
  __IO uint32_t IFCR;  /*!< BDMA interrupt flag clear register */
} BDMA_Base_Registers;

static void SPI_CloseTransfer(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef ADS9665_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData,
                                              uint16_t Size);


// Функция записи сохранения данны
static void ADC_DataStore(SR_Obj *hsr, int32_t *data, uint32_t channelNum)
{
	static uint32_t Offset = 0;
	uint32_t tmp = Offset;
	for (uint32_t i = 0; i < channelNum; i++)
	{
		SR_WritetValue(hsr, data[i], i + tmp);
		tmp++;
	}

	SCB_CleanDCache_by_Addr(&Offset, sizeof(Offset));
	Offset = tmp;

	if (Offset >= (hsr->ChannelNumber))
	{
		SR_NextSample(hsr);
		Offset = 0;
	}
}

// Функция передачи данных АЦП
void ADS8665_WriteRawData(ADS8665_HandleObj *hadc, uint8_t *txData, uint16_t len)
{
	uint8_t rxData[len];

	if (hadc == &adc_gr1)
	{
		HAL_SPI_TransmitReceive(&hspi1, txData, rxData, len, 100);
	}
	else if (hadc == &adc_gr2)
	{
		HAL_SPI_TransmitReceive(&hspi2, txData, rxData, len, 100);
	}
	else if (hadc == &adc_gr3)
	{
		HAL_SPI_TransmitReceive(&hspi4, txData, rxData, len, 100);
	}
}

// Функция приема данных АЦП
void ADS8665_ReadRawData(ADS8665_HandleObj *hadc, uint8_t *rxData, uint16_t len)
{
	uint8_t *txData = (uint8_t *)ADS8665_GetVoidBuf();
	if (hadc == &adc_gr1)
	{
		HAL_SPI_TransmitReceive(&hspi1, txData, rxData, len, 100);
	}
	else if (hadc == &adc_gr2)
	{
		HAL_SPI_TransmitReceive(&hspi2, txData, rxData, len, 100);
	}
	else if (hadc == &adc_gr3)
	{
		HAL_SPI_TransmitReceive(&hspi4, txData, rxData, len, 100);
	}
}

// Функция чтения пина RVS
uint16_t ADS8665_ReadRVS(ADS8665_HandleObj *hadc)
{

	if (hadc == &adc_gr1)
	{
		return (RVS_V1_GPIO_Port->IDR & RVS_V1_Pin);
	}
	else if (hadc == &adc_gr2)
	{
		return (RVS_V2_GPIO_Port->IDR & RVS_V2_Pin);
	}
	else if (hadc == &adc_gr3)
	{
		return (RVS_I_GPIO_Port->IDR & RVS_I_Pin);
	}

	return 0;
}

// Функция управления пином CONV
void ADS8665_WriteConv(ADS8665_HandleObj *hadc, uint8_t state)
{
	switch(state)
	{
	case ADS8665_CONV_SET:
		ADC_CONV_GPIO_Port->ODR |= ADC_CONV_Pin;
		break;

	case ADS8665_CONV_RESET:
		ADC_CONV_GPIO_Port->ODR &= ~ADC_CONV_Pin;
		break;
	}
}
// Запуск непрерывного преобразования
void ADS8665_StartContinuousConv(ADS8665_HandleObj *hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ADC_CONV_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(ADC_CONV_GPIO_Port, &GPIO_InitStruct);

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void ADS8665_StopContinuousConv(ADS8665_HandleObj *hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	GPIO_InitStruct.Pin = ADC_CONV_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(ADC_CONV_GPIO_Port, &GPIO_InitStruct);
}
//-------------------------------------------------------
// Функция обработки получения данных с АЦП
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	static uint32_t delay = 0;

	if (!(delay = (delay + 1) % (40000 * 3)))
	{
		LED3_GPIO_Port->ODR ^= LED3_Pin;
	}

	if (hspi->Instance == SPI1)
	{
		SCB_InvalidateDCache_by_Addr ((uint32_t *)adc_mass1, sizeof(adc_mass1));
		ADS8665_HandleResult(&adc_gr1, adc_mass1, adc_result1);
		ADC_DataStore(&hsr, adc_result1, ADC_DEVICE_NUMBER_GR1);
	}
	else if (hspi->Instance == SPI2)
	{
		SCB_InvalidateDCache_by_Addr ((uint32_t *)adc_mass2, sizeof(adc_mass2));
		ADS8665_HandleResult(&adc_gr2, adc_mass2, adc_result2);
		ADC_DataStore(&hsr, adc_result2, ADC_DEVICE_NUMBER_GR2);
	}
	else if (hspi->Instance == SPI4)
	{
		SCB_InvalidateDCache_by_Addr ((uint32_t *)adc_mass3, sizeof(adc_mass3));
		ADS8665_HandleResult(&adc_gr3, adc_mass3, adc_result3);
		ADC_DataStore(&hsr, adc_result2, ADC_DEVICE_NUMBER_GR3);
	}

}
//------------------------------------------------------------

// Функция обработки готовности преобразования
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint8_t *txBuf = (uint8_t *) ADS8665_GetVoidBuf();

	// Организация сбора данных
	if (GPIO_Pin == RVS_V1_Pin)
	{
#if OPTMIZATION != 0
		ADS9665_SPI_TransmitReceive_DMA(&hspi1, txBuf, adc_mass1, sizeof(adc_mass1));
#else
		HAL_SPI_TransmitReceive_DMA(&hspi1, txBuf, adc_mass1, sizeof(adc_mass1));
#endif
	}
	else if (GPIO_Pin == RVS_V2_Pin)
	{
#if OPTMIZATION != 0
		ADS9665_SPI_TransmitReceive_DMA(&hspi2, txBuf, adc_mass2, sizeof(adc_mass2));
#else
		HAL_SPI_TransmitReceive_DMA(&hspi2, txBuf, adc_mass2, sizeof(adc_mass2));
#endif
	}
	else if (GPIO_Pin == RVS_I_Pin)
	{
#if OPTMIZATION != 0
		ADS9665_SPI_TransmitReceive_DMA(&hspi4, txBuf, adc_mass3, sizeof(adc_mass3));
#else
		HAL_SPI_TransmitReceive_DMA(&hspi4, txBuf, adc_mass3, sizeof(adc_mass3));
#endif
	}
}



/***********Функции запуска сбора данных***********/


/**
  * @brief  DMA SPI transmit receive process complete callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMATransmitReceiveCplt(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  if (hspi->State != HAL_SPI_STATE_ABORT)
  {

	  /* Enable EOT interrupt */
	  __HAL_SPI_ENABLE_IT(hspi, SPI_IT_EOT);
  }
}

/**
  * @brief  DMA SPI communication error callback.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAError(DMA_HandleTypeDef *hdma)
{
  SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  /* if DMA error is FIFO error ignore it */
  if (HAL_DMA_GetError(hdma) != HAL_DMA_ERROR_FE)
  {
    /* Call SPI standard close procedure */
    SPI_CloseTransfer(hspi);

    SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
    hspi->State = HAL_SPI_STATE_READY;
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1UL)
    hspi->ErrorCallback(hspi);
#else
    HAL_SPI_ErrorCallback(hspi);
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */
  }
}

// Настройка DMA
static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
	/* calculate DMA base and stream number */
	DMA_Base_Registers  *regs_dma  = (DMA_Base_Registers *)hdma->StreamBaseAddress;

	/* Clear all interrupt flags at correct offset within the register */
	regs_dma->IFCR = 0x3FUL << (hdma->StreamIndex & 0x1FU);

	/* Clear DBM bit */
	((DMA_Stream_TypeDef *)hdma->Instance)->CR &= (uint32_t)(~DMA_SxCR_DBM);

	/* Configure DMA Stream data length */
	((DMA_Stream_TypeDef *)hdma->Instance)->NDTR = DataLength;

	/* Peripheral to Memory */
	if((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH)
	{
		/* Configure DMA Stream destination address */
		((DMA_Stream_TypeDef *)hdma->Instance)->PAR = DstAddress;

		/* Configure DMA Stream source address */
		((DMA_Stream_TypeDef *)hdma->Instance)->M0AR = SrcAddress;
	}
	/* Memory to Peripheral */
	else
	{
		/* Configure DMA Stream source address */
		((DMA_Stream_TypeDef *)hdma->Instance)->PAR = SrcAddress;

		/* Configure DMA Stream destination address */
		((DMA_Stream_TypeDef *)hdma->Instance)->M0AR = DstAddress;
	}
}

// Запуск прерывания DMA
HAL_StatusTypeDef ADS8665_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
	HAL_StatusTypeDef status = HAL_OK;

	/* Check the parameters */

	/* Change DMA peripheral state */
	hdma->State = HAL_DMA_STATE_BUSY;

	/* Initialize the error code */
	hdma->ErrorCode = HAL_DMA_ERROR_NONE;

	/* Disable the peripheral */
	__HAL_DMA_DISABLE(hdma);

	/* Configure the source, destination address and the data length */
	DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);


	/* Enable Common interrupts*/
	MODIFY_REG(((DMA_Stream_TypeDef   *)hdma->Instance)->CR, (DMA_IT_TC | DMA_IT_TE | DMA_IT_DME ), (DMA_IT_TC | DMA_IT_TE | DMA_IT_DME));


	/* Enable the Peripheral */
	__HAL_DMA_ENABLE(hdma);


	return status;
}

// Запуск сбора данных
HAL_StatusTypeDef ADS9665_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData,
                                              uint16_t Size)
{
  HAL_SPI_StateTypeDef tmp_state;
  HAL_StatusTypeDef errorcode = HAL_OK;

  uint32_t             tmp_mode;

  /* Process locked */
  __HAL_LOCK(hspi);

  /* Init temporary variables */
  tmp_state   = hspi->State;
  tmp_mode    = hspi->Init.Mode;

  if (!(((tmp_mode == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES) && (tmp_state == HAL_SPI_STATE_BUSY_RX)) || (tmp_state == HAL_SPI_STATE_READY)))
  {
    errorcode = HAL_BUSY;
    __HAL_UNLOCK(hspi);
    return errorcode;
  }


  /* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
  if (hspi->State != HAL_SPI_STATE_BUSY_RX)
  {
    hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
  }

  /* Set the transaction information */
  hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
  hspi->pTxBuffPtr  = (uint8_t *)pTxData;
  hspi->TxXferSize  = Size;
  hspi->TxXferCount = Size;
  hspi->pRxBuffPtr  = (uint8_t *)pRxData;
  hspi->RxXferSize  = Size;
  hspi->RxXferCount = Size;

  /* Init field not used in handle to zero */
  hspi->RxISR       = NULL;
  hspi->TxISR       = NULL;

  /* Reset the Tx/Rx DMA bits */
  CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);


  /* Set the SPI Tx/Rx DMA Half transfer complete callback */
  hspi->hdmarx->XferCpltCallback     = SPI_DMATransmitReceiveCplt;


  /* Set the DMA error callback */
  hspi->hdmarx->XferErrorCallback = SPI_DMAError;

  /* Set the DMA AbortCallback */
  hspi->hdmarx->XferAbortCallback = NULL;

  /* Enable the Rx DMA Stream/Channel  */
  ADS8665_DMA_Start_IT(hspi->hdmarx, (uint32_t)&hspi->Instance->RXDR, (uint32_t)hspi->pRxBuffPtr, hspi->RxXferCount);

  /* Enable Rx DMA Request */
  SET_BIT(hspi->Instance->CFG1, SPI_CFG1_RXDMAEN);

  /* Set the SPI Tx DMA transfer complete callback as NULL because the communication closing
  is performed in DMA reception complete callback  */
  hspi->hdmatx->XferHalfCpltCallback = NULL;
  hspi->hdmatx->XferCpltCallback     = NULL;
  hspi->hdmatx->XferErrorCallback    = NULL;
  hspi->hdmatx->XferAbortCallback    = NULL;

  /* Enable the Tx DMA Stream/Channel  */
  ADS8665_DMA_Start_IT(hspi->hdmatx, (uint32_t)hspi->pTxBuffPtr, (uint32_t)&hspi->Instance->TXDR, hspi->TxXferCount);


  MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, Size);
  /* Enable Tx DMA Request */
  SET_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN);

  /* Enable the SPI Error Interrupt Bit */
  __HAL_SPI_ENABLE_IT(hspi, (SPI_IT_OVR | SPI_IT_UDR | SPI_IT_FRE | SPI_IT_MODF));

  /* Enable SPI peripheral */
  __HAL_SPI_ENABLE(hspi);

  if (hspi->Init.Mode == SPI_MODE_MASTER)
  {
    /* Master transfer start */
    SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
  }

  /* Process Unlocked */
  __HAL_UNLOCK(hspi);
  return errorcode;
}

/***********ОБРАБОТЧИКИ ПРЕРЫВАНИЙ*****************/
// Обработчик прерывания DMA на передачу
void ADS8665_DMA_TX_IRQHandler(DMA_HandleTypeDef *hdma)
{

	/* calculate DMA base and stream number */
	DMA_Base_Registers  *regs_dma  = (DMA_Base_Registers *)hdma->StreamBaseAddress;


	/* Clear the transfer complete flag */
	regs_dma->IFCR = DMA_FLAG_TCIF0_4 << (hdma->StreamIndex & 0x1FU);


	/* Disable the transfer complete interrupt */
	((DMA_Stream_TypeDef   *)hdma->Instance)->CR  &= ~(DMA_IT_TC);

	/* Process Unlocked */
	__HAL_UNLOCK(hdma);

	/* Change the DMA state */
	hdma->State = HAL_DMA_STATE_READY;

}

// Обработчик прерывания DMA на прием
void ADS8665_DMA_RX_IRQHandler(DMA_HandleTypeDef *hdma)
{
	/* calculate DMA base and stream number */
	DMA_Base_Registers  *regs_dma  = (DMA_Base_Registers *)hdma->StreamBaseAddress;

	/* Transfer Complete Interrupt management ***********************************/

	/* Clear the transfer complete flag */
	regs_dma->IFCR = DMA_FLAG_TCIF0_4 << (hdma->StreamIndex & 0x1FU);

	/* Disable the transfer complete interrupt */
	((DMA_Stream_TypeDef   *)hdma->Instance)->CR  &= ~(DMA_IT_TC);

	/* Process Unlocked */
	__HAL_UNLOCK(hdma);

	/* Change the DMA state */
	hdma->State = HAL_DMA_STATE_READY;

	/* Transfer complete callback */
	hdma->XferCpltCallback(hdma);

}

// Закрытие передачи
static void SPI_CloseTransfer(SPI_HandleTypeDef *hspi)
{

  __HAL_SPI_CLEAR_EOTFLAG(hspi);
  __HAL_SPI_CLEAR_TXTFFLAG(hspi);

  /* Disable SPI peripheral */
  __HAL_SPI_DISABLE(hspi);

  /* Disable ITs */
  __HAL_SPI_DISABLE_IT(hspi, (SPI_IT_EOT | SPI_IT_TXP | SPI_IT_RXP | SPI_IT_DXP | SPI_IT_UDR | SPI_IT_OVR | SPI_IT_FRE | SPI_IT_MODF));

  /* Disable Tx DMA Request */
  CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);

  hspi->TxXferCount = (uint16_t)0UL;
  hspi->RxXferCount = (uint16_t)0UL;
}

// Обработчик прерывания SPI
void ADS8665_SPI_IRQHandler(SPI_HandleTypeDef *hspi)
{
  uint32_t itsource = hspi->Instance->IER;
  uint32_t itflag   = hspi->Instance->SR;
  uint32_t trigger  = itsource & itflag;
  uint32_t cfg1     = hspi->Instance->CFG1;

  HAL_SPI_StateTypeDef State = hspi->State;



  /* SPI End Of Transfer: DMA or IT based transfer */
  if (HAL_IS_BIT_SET(trigger, SPI_FLAG_EOT))
  {
    /* Clear EOT/TXTF/SUSP flag */
    __HAL_SPI_CLEAR_EOTFLAG(hspi);
    __HAL_SPI_CLEAR_TXTFFLAG(hspi);
    __HAL_SPI_CLEAR_SUSPFLAG(hspi);

    /* Disable EOT interrupt */
    __HAL_SPI_DISABLE_IT(hspi, SPI_IT_EOT);

    /* DMA Normal Mode */
    if (HAL_IS_BIT_CLR(cfg1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN) ||                    // IT based transfer is done
      ((State != HAL_SPI_STATE_BUSY_RX) && (hspi->hdmatx->Init.Mode == DMA_NORMAL)) ||  // DMA is used in normal mode
      ((State != HAL_SPI_STATE_BUSY_TX) && (hspi->hdmarx->Init.Mode == DMA_NORMAL)))    // DMA is used in normal mode
    {


      /* Call SPI Standard close procedure */
      SPI_CloseTransfer(hspi);

      hspi->State = HAL_SPI_STATE_READY;

    }

    HAL_SPI_TxRxCpltCallback(hspi);
  }

}


