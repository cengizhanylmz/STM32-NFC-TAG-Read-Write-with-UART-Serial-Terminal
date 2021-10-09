/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rfal_nfc.h"
#include "utils.h"
#include "rfal_nfc.h"
#include "ndef_poller.h"
#include "ndef_t2t.h"
#include "ndef_t4t.h"
#include "ndef_t5t.h"
#include "ndef_message.h"
#include "ndef_types_rtd.h"
#include "ndef_dump.h"
#include "st25r95.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
static rfalNfcDiscoverParam params;
static ndefContext    ndefCtx;
static uint8_t st;
static ndefInfo info;
static uint8_t rawMessageBuff[8192];
char Rx_NfcMessage[9];
int count;
char Rx_Data[1];
static rfalNfcDevice *device;
static uint8_t *ndefStates[] =
{
    (uint8_t *)"INVALID",
    (uint8_t *)"INITIALIZED",
    (uint8_t *)"READ/WRITE",
    (uint8_t *)"READ-ONLY"
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
bool rfalInit();
void rfalStart(void);
static void rfalStates(rfalNfcState st);
void ndefText(rfalNfcDevice *NfcDevice);
void ReadTag(rfalNfcDevice *NfcDevice);
void ndefUri(rfalNfcDevice *NfcDevice);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  __HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);

  RetargetInit(&huart2);
   if(!rfalInit())
   	  {
   		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, SET);
   	  }
   	  else
   	  {

   		 int i;
   		 for (i=0;i<6;i++)
   		 {
   			 HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
   			 HAL_Delay(150);
   		 }
   	  }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  	 rfalStart();

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED2_NFC_Pin|LED3_NFC_Pin|LED4_NFC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED1_NFC_Pin|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LED2_NFC_Pin LED3_NFC_Pin LED4_NFC_Pin PB6 */
  GPIO_InitStruct.Pin = LED2_NFC_Pin|LED3_NFC_Pin|LED4_NFC_Pin|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_NFC_Pin PA9 */
  GPIO_InitStruct.Pin = LED1_NFC_Pin|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
bool rfalInit()
{
	ReturnCode	err;
	err=rfalNfcInitialize();

	if(err==ERR_NONE)
	{
		params.compMode=RFAL_COMPLIANCE_MODE_NFC;
		params.devLimit=1U;
		params.nfcfBR=RFAL_BR_212;
		params.ap2pBR=RFAL_BR_424;
		params.maxBR=RFAL_BR_106;
		params.notifyCb=rfalStates;
		params.wakeupConfigDefault=true;
		params.wakeupEnabled=true;
		params.totalDuration=1000U;
		params.techs2Find=(RFAL_NFC_POLL_TYPE_NFCA | RFAL_NFC_POLL_TYPE_NFCB | RFAL_NFC_POLL_TYPE_NFCF| RFAL_NFC_POLL_TYPE_AP2P |RFAL_NFC_POLL_TYPE_NFCV);

		return true;
	}
	return false;
}
static void rfalStates(rfalNfcState st)
{
	if(st==RFAL_NFC_STATE_WAKEUP_MODE)
	{
		printf("Wake Up Mode Enabled...\r\n");
		printf("Press 1 time to Button for WRITE TEXT Record\r\n");
		printf("Press 2 time to Button for WRITE URI Record\r\n");
		printf("Press 3 time to Button for READ the Tag\r\n");
	}
	else if (st==RFAL_NFC_STATE_POLL_TECHDETECT)
	{
		printf("State: Poll_techdetect\r\n");
	}
}
void rfalStart()
{
	rfalNfcWorker();
	HAL_Delay(75);
	rfalNfcDiscover( &params );
	st=rfalNfcGetState();
	if(rfalNfcIsDevActivated(st))
	{
		printf("Device Found!\r\n");
		uint8_t nfcatype;
		uint8_t type;
		uint8_t rfInter;
		rfalNfcGetActiveDevice(&device);
		type=device->type;
			if(type==RFAL_NFC_LISTEN_TYPE_NFCA)
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5,SET);
				nfcatype=device->dev.nfca.type;
				rfInter=device->rfInterface;
				if(nfcatype==RFAL_NFCA_T1T)
				{
					printf("ISO14443A (NFC-A T1T) device found!\r\n");
					printf("Device ID: %s\r\n",hex2Str( device->nfcid, device->nfcidLen ) );
				}
				else if(nfcatype==RFAL_NFCA_T4T)
				{
				printf("NFCA Passive ISO-DEP device found!\r\n");
				printf("Device ID: %s\r\n",hex2Str( device->nfcid, device->nfcidLen ) );
				}
				else if((nfcatype==RFAL_NFCA_T4T_NFCDEP)||(nfcatype==RFAL_NFCA_NFCDEP))
				{
					printf("NFCA Passive P2P device found.\r\n");
					printf("Device ID: %s\r\n",hex2Str( device->nfcid, device->nfcidLen ));
				}
				else
				{
					printf("ISO14443A NFCA device found!\r\n");
					printf("Device ID: %s\r\n",hex2Str( device->nfcid, device->nfcidLen ));
				}
			}
			else if (type==RFAL_NFC_LISTEN_TYPE_NFCV);
			{
				uint8_t devUID[RFAL_NFCV_UID_LEN];
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, SET);
				ST_MEMCPY( devUID, device->nfcid, device->nfcidLen );
			REVERSE_BYTES( devUID, RFAL_NFCV_UID_LEN );
			printf("ISO15693/NFC-V card found. UID: %s\r\n", hex2Str(devUID, RFAL_NFCV_UID_LEN));

			if(count==1)
			{
				ndefText(device);
				count=0;
				HAL_Delay(500);
				//ReadTag();
				rfalNfcvPollerInitialize();
				rfalNfcvInventoryRes  invRes;
				uint16_t              rcvdLen;
				while (rfalNfcvPollerInventory( RFAL_NFCV_NUM_SLOTS_1, RFAL_NFCV_UID_LEN * 8U, device->dev.nfcv.InvRes.UID, &invRes, &rcvdLen) == ERR_NONE)
				{
					HAL_Delay(200);
				}
			}
			else if(count==2)
			{

				ndefUri(device);
				count=0;
				HAL_Delay(500);
				//ReadTag();
				rfalNfcvPollerInitialize();
				rfalNfcvInventoryRes  invRes;
				uint16_t              rcvdLen;
				while (rfalNfcvPollerInventory( RFAL_NFCV_NUM_SLOTS_1, RFAL_NFCV_UID_LEN * 8U, device->dev.nfcv.InvRes.UID, &invRes, &rcvdLen) == ERR_NONE)
				{
					HAL_Delay(200);
				}


			}
			else if(count==3)
			{
				ReadTag(device);
				count=0;
				rfalNfcvPollerInitialize();
				rfalNfcvInventoryRes  invRes;
				uint16_t              rcvdLen;
				while (rfalNfcvPollerInventory( RFAL_NFCV_NUM_SLOTS_1, RFAL_NFCV_UID_LEN * 8U, device->dev.nfcv.InvRes.UID, &invRes, &rcvdLen) == ERR_NONE)
				{
					HAL_Delay(200);
				}
			}

		}

			rfalNfcDeactivate( false );

	}

	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, RESET);
	}

}
void ndefText(rfalNfcDevice *NfcDevice)
{
	ndefMessage message;
	ndefBuffer BufrawMessage;
	ReturnCode err;

	ndefConstBuffer bufText;

	 ndefConstBuffer8 bufTextLangCode;

	 ndefType text;

	 ndefRecord record;

	 ndefInfo info;

	 bufTextLangCode.buffer="tr";
	 bufTextLangCode.length=strlen(bufTextLangCode.buffer);

	 uint8_t ndefstate;

	err = ndefPollerContextInitialization(&ndefCtx, NfcDevice);
	err= ndefPollerNdefDetect(&ndefCtx, &info);
	uint8_t a;
	a=info.state;
	if(err==ERR_NONE)
	{
		printf("NDEF Detected: %s\r\n",ndefStates[info.state]);
	}

	ndefstate=info.state;
	err=ndefMessageInit(&message);

	bufText.buffer=Rx_NfcMessage;
	bufText.length=strlen(Rx_NfcMessage);

	err=ndefRtdText(&text,TEXT_ENCODING_UTF8,&bufTextLangCode,&bufText);

	err=ndefRtdTextToRecord(&text, &record);

	err=ndefMessageAppend(&message, &record);

	if(err!=ERR_NONE)
	{
		printf("Message Creation Failed!\r\n");
	}
	else
	{
		printf("Message Created!\r\n");
	}
	 err = ndefPollerWriteMessage(&ndefCtx, &message);


	if(err==ERR_NONE)
	{
		printf("Message Writed Successfully\r\n");
	}
	else
	{
		printf("Message can not writed!\r\n");
	}
	memset(Rx_NfcMessage,NULL,sizeof(Rx_NfcMessage));

}
void ndefUri(rfalNfcDevice *NfcDevice)
{
	 ndefMessage message;
	 ndefBuffer bufrawMessage;

	 ReturnCode err;

	 ndefConstBuffer bufUri;

	 ndefType uri;

	 ndefRecord record1;

//	 uint8_t ndefUri[]="sekiza.com";
	 uint8_t ndefstate;

	err = ndefPollerContextInitialization(&ndefCtx, NfcDevice);
	err= ndefPollerNdefDetect(&ndefCtx, &info);
	if(err==ERR_NONE)
	{
		printf("NDEF Detected: %s\r\n",ndefStates[info.state]);//state'i str olarak yazmıyor!!
	}

	ndefstate=info.state;

	 err=ndefMessageInit(&message);
	 if(err!=ERR_NONE)
	 {
		 printf("Message Initialaziton failed!\r\n");
		 return;
	 }
	 bufUri.buffer=Rx_NfcMessage;
	 bufUri.length=strlen(Rx_NfcMessage);

	 err =ndefRtdUri(&uri, NDEF_URI_PREFIX_HTTP_WWW, &bufUri);
	 err=ndefRtdUriToRecord(&uri, &record1);
	 err=ndefMessageAppend(&message, &record1);
	 if(err!=ERR_NONE)
	 {
		 printf("Message creation failed!!\r\n");
	 return;
	 }
	 else
	 {
		 printf("Message created successfully\r\n");
	 }
	 bufrawMessage.buffer = rawMessageBuff;
	 bufrawMessage.length = sizeof(rawMessageBuff);
	 err=ndefMessageEncode(&message, &bufrawMessage);
	 err=ndefPollerWriteMessage(&ndefCtx, &message);
	 if(err!=ERR_NONE)
	 {
		 printf("Message could not write!\r\n");

	 }
	 else
	 {
		 printf("Message Writed Successfully\r\n");
	 }

}

void ReadTag(rfalNfcDevice *NfcDevice)
{
	ReturnCode err;
	ndefType type;
	ndefRecord* record;
	ndefMessage message;
	ndefConstBuffer bufConstRawMessage;
	uint32_t rawMessageLen;
	ndefInfo info;

	err = ndefPollerContextInitialization(&ndefCtx, NfcDevice);
	err= ndefPollerNdefDetect(&ndefCtx, &info);



	if(err==ERR_NONE)
	{

	printf("NDEF Detected: %s\r\n",ndefStates[info.state]);//state'i str olarak yazmıyor!!

	}

	if( info.state == NDEF_STATE_INITIALIZED )
	      {

	                return;
	       }

	 err = ndefPollerReadRawMessage(&ndefCtx, rawMessageBuff, sizeof(rawMessageBuff), &rawMessageLen);
	          if( err != ERR_NONE )
	            {
	                printf("NDEF message cannot be read (ndefPollerReadRawMessage returns %d)\r\n", err);
	                return;
	            }

	            bufConstRawMessage.buffer = rawMessageBuff;
	            bufConstRawMessage.length = rawMessageLen;
	            err = ndefMessageDecode(&bufConstRawMessage, &message);
	            if( err != ERR_NONE )
	            {
	                printf("NDEF message cannot be decoded (ndefMessageDecode  returns %d)\r\n", err);
	                return;
	            }
	            record=ndefMessageGetFirstRecord(&message);
	            while(record!=NULL)
	            {
	            	ndefRecordToType(record, &type);
	            	if(type.id==NDEF_TYPE_RTD_TEXT)
	            	{
	            		printf("TEXT: ");
	            		int i;
	            		for(i=0;i<type.data.text.bufSentence.length;i++)
	            		{
	            			printf("%c",type.data.text.bufSentence.buffer[i]);
	            		}
	            		printf("\n");
	            		printf("ASCII numbers of TEXT:  ");
	            		printf("%s\r\n",hex2Str(type.data.text.bufSentence.buffer,type.data.text.bufSentence.length));
	            	}

	            	else if(type.id==NDEF_TYPE_RTD_URI)
	            	{
	            		printf("URI:");
	            		int i;
	            		for(i=0;i<type.data.uri.bufUriString.length;i++)
	            		{
	            		printf("%c",type.data.uri.bufUriString.buffer[i]);
	            		}
	            		printf("\n");
	            		printf("ASCII numbers of URI:  ");
	            		printf("%s\r\n",hex2Str(type.data.uri.bufUriString.buffer,type.data.uri.bufUriString.length));
	            	}
	            	record=ndefMessageGetNextRecord(record);
	            }
	          HAL_Delay(1000);
	          printf("Device Memory: %d\r\n",ndefCtx.cc.t5t.memoryLen*8);
	          HAL_Delay(3000);

}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
