/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fatfs_sd.h"
#include "string.h"
#include "MY_ILI9341.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
FATFS fs;  // file system
FIL fil;  // file
FRESULT fresult;  // to store the result
char buffer[2]; // to store data
unsigned char pix1,pix2,pix3,_pix1,_pix2,_pix3;
unsigned long tft_pix;
unsigned int pix[320];
//
UINT br, bw;   // file read/write count
uint16_t x,y,a,b,c;
uint16_t i;
/* capacity related variables */
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;


/* to send the data to the uart */
void send_uart (char *string)
{
	uint8_t len = strlen (string);
	HAL_UART_Transmit(&huart1, (uint8_t *) string, len, 2000);  // transmit in blocking mode
}

/* to find the size of data in the buffer */
int bufsize (char *buf)
{
	int i=0;
	while (*buf++ != '\0') i++;
	return i;
}

void bufclear (void)  // clear buffer
{
	for (int i=0; i<1024; i++)
	{
		buffer[i] = '\0';
	}
}
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
  MX_USART1_UART_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
	ILI9341_Init(&hspi1, LCD_CS_GPIO_Port, LCD_CS_Pin, LCD_DC_GPIO_Port, LCD_DC_Pin, LCD_RST_GPIO_Port, LCD_RST_Pin);
	ILI9341_setRotation(2);
  /* USER CODE END 2 */
	fresult = f_open(&fil,"phuoc.txt", FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
	/* Mount SD Card */
  fresult = f_mount(&fs, "", 0);
  if (fresult != FR_OK) send_uart ("error in mounting SD CARD...\n");
  else send_uart("SD CARD mounted successfully...\n");
	/*open file */
	/*
	fresult = f_open(&fil, "003.bmp",FA_READ);
	//f_seek(&fsrc, 0x3E, F_SEEK_SET); // Seek to the data
	//f_lseek(&fsrc, 0x0A);
	fresult = f_lseek(&fil, 52); // 480 52 532 1012
	
	for(y=0; y<320; y++)
	{
		for(x=0; x<240; x++)
		{
		
			f_read(&fil,&pix1,1, &br);
			f_read(&fil,&pix2,1, &br);
			f_read(&fil,&pix3,1, &br);
		  _pix1=pix1/8;
			_pix2=pix2/4;
			_pix3=pix3/8;
			tft_pix=(_pix1<<11)+(_pix2<<5)+_pix3;
			ILI9341_DrawPixel(y,x,tft_pix);
			f_read (&fil,buffer,2, &br);	// 0 6 11	
			//i = ((buffer[1]>>3)<<11)|(buffer[0]&0x1F);
			//i = ((buffer[1]>>3)<<11)+((buffer[1]<<4)|(buffer[0]>>7)<<6)+(buffer[0]&0x1F);
			//i = ((((buffer[1]&0x07)<<3)|((buffer[0]&0xC0)>>5))<<0)|((buffer[1]>>3)<<5)|((buffer[0]&0x3F)<<11);
			//i = (buffer[1]<<8)+buffer[0]&0x3F;
			// ****** i = (((buffer[1]<<3)|(buffer[0]>>5))<<6);
		  i = (((buffer[1]<<3)|(buffer[0]>>5))<<6)|(buffer[0]&0x3F); // sang hon
			//i = (((buffer[1]<<3)|(buffer[0]>>5))<<6)|(buffer[0]&0x3F)|((buffer[1]>>3)<<11);
		  // i = (((buffer[1]<<3)|(buffer[0]>>5))<<11)|((buffer[1]>>3)<<0)|((buffer[0]&0x3F)<<6);
			ILI9341_DrawPixel(y,x,i);
		}
	} 
	f_close(&fil);
	*/
  while (1)
  {
    fresult = f_open(&fil, "002.bmp",FA_READ);
		fresult = f_lseek(&fil, 52);
		for(y=0; y<320; y++)
	  {
		  for(x=0; x<240; x++)
		  {
		  	f_read (&fil,buffer,2, &br);
		  	i = (((buffer[1]<<3)|(buffer[0]>>5))<<6)|(buffer[0]&0x3F);
		  	ILI9341_DrawPixel(y,x,i);
			}
		}
		f_close(&fil);
		HAL_Delay(500);
		//
		fresult = f_open(&fil, "003.bmp",FA_READ);
		fresult = f_lseek(&fil, 52);
		for(y=0; y<320; y++)
	  {
		  for(x=0; x<240; x++)
		  {
		  	f_read (&fil,buffer,2, &br);
		  	i = (((buffer[1]<<3)|(buffer[0]>>5))<<6)|(buffer[0]&0x3F);
		  	ILI9341_DrawPixel(y,x,i);
			}
		}
		f_close(&fil);
		HAL_Delay(500);
		//
		fresult = f_open(&fil, "004.bmp",FA_READ);
		fresult = f_lseek(&fil, 52);
		for(y=0; y<320; y++)
	  {
		  for(x=0; x<240; x++)
		  {
		  	f_read (&fil,buffer,2, &br);
		  	i = (((buffer[1]<<3)|(buffer[0]>>5))<<6)|(buffer[0]&0x3F);
		  	ILI9341_DrawPixel(y,x,i);
			}
		}
		f_close(&fil);
		HAL_Delay(500);
		//
		fresult = f_open(&fil, "005.bmp",FA_READ);
		fresult = f_lseek(&fil, 52);
		for(y=0; y<320; y++)
	  {
		  for(x=0; x<240; x++)
		  {
		  	f_read (&fil,buffer,2, &br);
		  	i = (((buffer[1]<<3)|(buffer[0]>>5))<<6)|(buffer[0]&0x3F);
		  	ILI9341_DrawPixel(y,x,i);
			}
		}
		f_close(&fil);
		HAL_Delay(500);
		//
		fresult = f_open(&fil, "006.bmp",FA_READ);
		fresult = f_lseek(&fil, 52);
		for(y=0; y<320; y++)
	  {
		  for(x=0; x<240; x++)
		  {
		  	f_read (&fil,buffer,2, &br);
		  	i = (((buffer[1]<<3)|(buffer[0]>>5))<<6)|(buffer[0]&0x3F);
		  	ILI9341_DrawPixel(y,x,i);
			}
		}
		f_close(&fil);
		HAL_Delay(500);
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RS_Pin|D_DC_Pin|D_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : RS_Pin D_DC_Pin D_CS_Pin */
  GPIO_InitStruct.Pin = RS_Pin|D_DC_Pin|D_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SD_CS_Pin */
  GPIO_InitStruct.Pin = SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SD_CS_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
