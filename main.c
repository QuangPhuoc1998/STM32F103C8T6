#include "main.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "MY_ILI9341.h"
#include "TSC2046.h"

// global variable
uint32_t freq_value, duty_value;
uint8_t step_value;
char buffer[5];
char set_pre = 1;
char set_step = 0;
bool status = false;
// speed
uint64_t capture_value;
uint64_t sum = 0, speed = 0;
uint8_t m = 0;
// bluethooth
char Rx_Data[2], Rx_Buffer[50];
uint8_t Rx_Indx = 0;
bool bluetooth_status = false;
bool dir_status = true;
int16_t step = 0;
uint16_t t;
bool Run_status;
char step_buffer[5];
//
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
UART_HandleTypeDef huart1;
TS_TOUCH_DATA_Def myTS_Handle;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART1_UART_Init(void);
void mode_main();
void mode_control();
void mode_prescaler();
void mode_paint();
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_SPI1_Init();
	MX_SPI2_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
	MX_TIM4_Init();
	MX_USART1_UART_Init();
	//
	ILI9341_Init(&hspi1, LCD_CS_GPIO_Port, LCD_CS_Pin, LCD_DC_GPIO_Port, LCD_DC_Pin, LCD_RST_GPIO_Port, LCD_RST_Pin);
	ILI9341_setRotation(2);
	TSC2046_Begin(&hspi2, TS_CS_GPIO_Port, TS_CS_Pin);
	Calibrate();
	//
	// mode_paint();
	//
	ILI9341_Fill(COLOR_BLACK);
	ILI9341_fillCircle(160,120,50,COLOR_YELLOW);
	ILI9341_fillCircle(160,120,40,COLOR_BLACK);
	ILI9341_Fill_Rect(145,70,175,90,COLOR_BLACK);
	ILI9341_Fill_Rect(155,60,165,120,COLOR_YELLOW);
  while(1)
	{
		myTS_Handle = TSC2046_GetTouchData();
		if(myTS_Handle.isPressed)
		{
			if(myTS_Handle.X >= 110 && myTS_Handle.X < 210 && myTS_Handle.Y >= 70 && myTS_Handle.Y < 170) 
			{
				mode_main();
			}
		}
	}
}
/**------------------------------------------------*/
void mode_main()
{
	//
	HAL_TIM_Base_Stop_IT(&htim3);
	HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_1);
	capture_value = 0;
	speed = 0;
	status = false;
	//
	ILI9341_Fill(WHITE);
	ILI9341_printText("Menu",120,10,DARKBLUE,WHITE,4);
	ILI9341_printText("Control step",50,75,DARKBLUE,WHITE,3);
	ILI9341_printText("Prescaler",75,130,DARKBLUE,WHITE,3);
	ILI9341_printText("Paint",100,180,DARKBLUE,WHITE,3);
	//
	ILI9341_Fill_Rect(30,70,290,71,DARKBLUE);
	ILI9341_Fill_Rect(30,99,290,100,DARKBLUE);
	ILI9341_Fill_Rect(30,70,31,99,DARKBLUE);
	ILI9341_Fill_Rect(290,70,291,100,DARKBLUE);
	//
	ILI9341_Fill_Rect(30,125,290,126,DARKBLUE);
	ILI9341_Fill_Rect(30,154,290,155,DARKBLUE);
	ILI9341_Fill_Rect(30,125,31,154,DARKBLUE);
	ILI9341_Fill_Rect(290,125,291,155,DARKBLUE);
	//
	ILI9341_Fill_Rect(30,175,290,176,DARKBLUE);
	ILI9341_Fill_Rect(30,204,290,205,DARKBLUE);
	ILI9341_Fill_Rect(30,175,31,204,DARKBLUE);
	ILI9341_Fill_Rect(290,175,291,205,DARKBLUE);
	//
	while(1)
	{
		myTS_Handle = TSC2046_GetTouchData();
		if(myTS_Handle.isPressed)
		{
			if(myTS_Handle.X >= 30 && myTS_Handle.X < 290 && myTS_Handle.Y >= 70 && myTS_Handle.Y < 100) 
			{
				ILI9341_fillTriangle(5,80,5,90,20,85,COLOR_GREEN);
				mode_control();
			}
			if(myTS_Handle.X >= 30 && myTS_Handle.X < 290 && myTS_Handle.Y >= 125 && myTS_Handle.Y < 155) 
			{
				ILI9341_fillTriangle(5,135,5,145,20,140,COLOR_GREEN);
				mode_prescaler();
			}
			if(myTS_Handle.X >= 30 && myTS_Handle.X < 290 && myTS_Handle.Y >= 175 && myTS_Handle.Y < 205) 
			{
				ILI9341_fillTriangle(5,185,5,195,20,190,COLOR_GREEN);
				mode_paint();
			}
		}
	}
}
void mode_control()
{
	//
	HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_1);
  HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_2);
	//
	ILI9341_Fill(WHITE);
	ILI9341_Fill_Rect(0,0,329,35,GRAYBLUE);
	ILI9341_printText("Control",90,5,COLOR_WHITE,GRAYBLUE,3);
	ILI9341_Fill_Rect(20,10,40,20,COLOR_WHITE);
	ILI9341_fillTriangle(20,5,20,25,0,15,COLOR_WHITE);
	//
	ILI9341_printText("Duty:        us",10,50,DARKBLUE,WHITE,2);
	ILI9341_printText("Freq:        Hz",10,75,DARKBLUE,WHITE,2);
	//
	ILI9341_printText("Speed:",10,110,DARKBLUE,WHITE,3);
	ILI9341_printText("rpm",150,160,DARKBLUE,WHITE,3);
	//
	ILI9341_drawFastVLine(230,0,239,GRAYBLUE);
	ILI9341_drawFastVLine(229,0,239,GRAYBLUE);
	ILI9341_Fill_Rect(235,90,315,130,GRAYBLUE);
	ILI9341_printText("Reset",240,100,WHITE,GRAYBLUE,2);
	ILI9341_Fill_Rect(235,140,315,180,GRAYBLUE);
	ILI9341_printText("Right",240,150,RED,GRAYBLUE,2);
	ILI9341_Fill_Rect(235,190,315,230,GRAYBLUE);
	ILI9341_printText("Left",240,200,WHITE,GRAYBLUE,2);
	//
	ILI9341_drawFastHLine(0,99,230,GRAYBLUE);
	ILI9341_drawFastHLine(0,100,230,GRAYBLUE);
	//
	ILI9341_drawFastHLine(0,200,230,GRAYBLUE);
	ILI9341_drawFastHLine(0,199,230,GRAYBLUE);
	switch(set_pre)
	{
		case 1:
			ILI9341_printText("Pre: 1/1",10,215,GRAYBLUE,WHITE,2);
		  break;
		case 2:
			ILI9341_printText("Pre: 1/2",10,215,GRAYBLUE,WHITE,2);
		  break;
		case 3:
			ILI9341_printText("Pre: 1/4",10,215,GRAYBLUE,WHITE,2);
		  break;
		case 4:
			ILI9341_printText("Pre: 1/8",10,215,GRAYBLUE,WHITE,2);
		  break;
		case 5:
			ILI9341_printText("Pre: 1/16",10,215,GRAYBLUE,WHITE,2);
		  break;
		default: break;
	}
	//
	ILI9341_drawFastHLine(235,80,80,RED);
	ILI9341_drawFastHLine(235,80,80,RED);
	ILI9341_printText("OFF",250,50,RED,WHITE,3);
	//
	htim2.Instance -> CNT = 2000;
	while(1)
	{
		myTS_Handle = TSC2046_GetTouchData();
		if(myTS_Handle.isPressed)
		{
			if(myTS_Handle.X > 0 && myTS_Handle.X < 40 && myTS_Handle.Y > 0 && myTS_Handle.Y < 35) 
			{
				ILI9341_Fill_Rect(20,8,40,22,COLOR_WHITE);
	      ILI9341_fillTriangle(20,3,20,27,0,15,COLOR_WHITE);
				mode_main();
			}
			if(myTS_Handle.X > 235 && myTS_Handle.X < 315 && myTS_Handle.Y > 90 && myTS_Handle.Y < 130) 
			{
				htim2.Instance -> CNT = 500;
				HAL_TIM_Base_Stop_IT(&htim3);
				HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_1);
				capture_value = 0;
				speed = 0;
				ILI9341_printText("OFF",250,50,RED,WHITE,3);
				status = false;
			}
			if(myTS_Handle.X > 235 && myTS_Handle.X < 315 && myTS_Handle.Y > 140 && myTS_Handle.Y < 180) 
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
				ILI9341_printText("Right",240,150,RED,GRAYBLUE,2);
				ILI9341_printText("Left",240,200,WHITE,GRAYBLUE,2);
				
			}
			if(myTS_Handle.X > 235 && myTS_Handle.X < 315 && myTS_Handle.Y > 190 && myTS_Handle.Y < 230) 
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
				ILI9341_printText("Right",240,150,WHITE,GRAYBLUE,2);
				ILI9341_printText("Left",240,200,RED,GRAYBLUE,2);
			}
		}
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)==0)
	  {
			if(status == false)
			{
				while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)==0);
				HAL_TIM_Base_Start_IT(&htim3);
				HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_1);
				ILI9341_printText("ON ",250,50,RED,WHITE,3);
				status = true;
			}
			else
			if(status == true)
			{
				while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)==0);
				HAL_TIM_Base_Stop_IT(&htim3);
				HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_1);
				ILI9341_printText("OFF",250,50,RED,WHITE,3);
				status = false;
			}
		}
		duty_value = TIM2 -> CNT;
		if(duty_value < 25 )
		{
			htim2.Instance -> CNT = 10000;
			duty_value = 10000;
		}
		else if( duty_value > 10000)
		{
			htim2.Instance -> CNT = 25;
			duty_value = 25;
		}
		//
		htim3.Instance -> ARR = duty_value;
		// duty_value
		ILI9341_drawChar(70, 50,duty_value/10000+48,DARKBLUE,WHITE,2);
		ILI9341_drawChar(85, 50,duty_value%10000/1000+48,DARKBLUE,WHITE,2);
		ILI9341_drawChar(100, 50,duty_value%1000/100+48,DARKBLUE,WHITE,2);
		ILI9341_drawChar(115, 50,duty_value%100/10+48,DARKBLUE,WHITE,2);
		ILI9341_drawChar(130, 50,duty_value%10+48,DARKBLUE,WHITE,2);
		// freq_value
		freq_value = 1000000/(duty_value*2);
		ILI9341_drawChar(70, 75,freq_value/10000+48,DARKBLUE,WHITE,2);
		ILI9341_drawChar(85, 75,freq_value%10000/1000+48,DARKBLUE,WHITE,2);
		ILI9341_drawChar(100, 75,freq_value%1000/100+48,DARKBLUE,WHITE,2);
		ILI9341_drawChar(115, 75,freq_value%100/10+48,DARKBLUE,WHITE,2);
		ILI9341_drawChar(130, 75,freq_value%10+48,DARKBLUE,WHITE,2);
		// speed
		ILI9341_drawChar(40, 155,speed/1000+48,DARKBLUE,WHITE,4);
		ILI9341_drawChar(65, 155,speed%1000/100+48,DARKBLUE,WHITE,4);
		ILI9341_drawChar(90, 155,speed%100/10+48,DARKBLUE,WHITE,4);
		ILI9341_drawChar(115, 155,speed%10+48,DARKBLUE,WHITE,4);
	}
}
void mode_prescaler()
{
	ILI9341_Fill(WHITE);
	ILI9341_Fill_Rect(0,0,329,35,GRAYBLUE);
	ILI9341_printText("Prescaler",75,5,COLOR_WHITE,GRAYBLUE,3);
	ILI9341_Fill_Rect(20,10,40,20,COLOR_WHITE);
	ILI9341_fillTriangle(20,5,20,25,0,15,COLOR_WHITE);
	ILI9341_printText("Full step",30,50,BLACK,COLOR_WHITE,3);
	ILI9341_printText("Half step",30,80,BLACK,COLOR_WHITE,3);
	ILI9341_printText("Quarter step",30,110,BLACK,COLOR_WHITE,3);
	ILI9341_printText("Eighth step",30,140,BLACK,COLOR_WHITE,3);
	ILI9341_printText("Sixteenth step",30,170,BLACK,COLOR_WHITE,3);
	//
	ILI9341_drawFastHLine(0, 210, 329,GRAYBLUE);
	ILI9341_printText("Desigh by Quang Phuoc",10,215,GRAYBLUE,WHITE,2);
	while(1)
	{
		myTS_Handle = TSC2046_GetTouchData();
		if(myTS_Handle.isPressed)
		{
			if(myTS_Handle.X > 0 && myTS_Handle.X < 40 && myTS_Handle.Y > 0 && myTS_Handle.Y < 35) 
			{
				ILI9341_Fill_Rect(20,8,40,22,COLOR_WHITE);
	      ILI9341_fillTriangle(20,3,20,27,0,15,COLOR_WHITE);
				mode_main();
			}
			// full
			if(myTS_Handle.X > 30 && myTS_Handle.X < 300 && myTS_Handle.Y >= 50 && myTS_Handle.Y < 80) 
			{
				set_pre = 1;
			}
			// half
      if(myTS_Handle.X > 30 && myTS_Handle.X < 300 && myTS_Handle.Y >= 80 && myTS_Handle.Y < 110)  
			{
				set_pre = 2;
			}
			// quarter
			if(myTS_Handle.X > 30 && myTS_Handle.X < 300 && myTS_Handle.Y >= 110 && myTS_Handle.Y < 140)  
			{
				set_pre = 3;
			}
			// eighth
      if(myTS_Handle.X > 30 && myTS_Handle.X < 300 && myTS_Handle.Y >= 140 && myTS_Handle.Y < 170)  
			{
				set_pre =4;
			}
			// sixteeth
			if(myTS_Handle.X > 30 && myTS_Handle.X < 300 && myTS_Handle.Y >= 170 && myTS_Handle.Y < 200)   
			{
				set_pre = 5;
			}
		}
		switch (set_pre)
			{
				case 1:
					ILI9341_fillTriangle(5,50,5,70,15,60,COLOR_RED);
					ILI9341_fillTriangle(5,80,5,100,15,90,WHITE);
					ILI9341_fillTriangle(5,110,5,130,15,120,WHITE);
					ILI9341_fillTriangle(5,140,5,160,15,150,WHITE);
					ILI9341_fillTriangle(5,170,5,190,15,180,WHITE);
				  //
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
				  // 
				  break;
				case 2:
					ILI9341_fillTriangle(5,50,5,70,15,60,WHITE);
					ILI9341_fillTriangle(5,80,5,100,15,90,COLOR_RED);
					ILI9341_fillTriangle(5,110,5,130,15,120,WHITE);
					ILI9341_fillTriangle(5,140,5,160,15,150,WHITE);
					ILI9341_fillTriangle(5,170,5,190,15,180,WHITE);
				  //
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
				  // 
				  break;
				case 3:
					ILI9341_fillTriangle(5,50,5,70,15,60,WHITE);
					ILI9341_fillTriangle(5,80,5,100,15,90,WHITE);
					ILI9341_fillTriangle(5,110,5,130,15,120,COLOR_RED);
					ILI9341_fillTriangle(5,140,5,160,15,150,WHITE);
					ILI9341_fillTriangle(5,170,5,190,15,180,WHITE);
				  //
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
				  // 
				  break;
				case 4:
					ILI9341_fillTriangle(5,50,5,70,15,60,WHITE);
					ILI9341_fillTriangle(5,80,5,100,15,90,WHITE);
					ILI9341_fillTriangle(5,110,5,130,15,120,WHITE);
					ILI9341_fillTriangle(5,140,5,160,15,150,COLOR_RED);
					ILI9341_fillTriangle(5,170,5,190,15,180,WHITE);
				  //
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
				  // 
				  break;
				case 5:
					ILI9341_fillTriangle(5,50,5,70,15,60,WHITE);
					ILI9341_fillTriangle(5,80,5,100,15,90,WHITE);
					ILI9341_fillTriangle(5,110,5,130,15,120,WHITE);
					ILI9341_fillTriangle(5,140,5,160,15,150,WHITE);
					ILI9341_fillTriangle(5,170,5,190,15,180,COLOR_RED);
				  //
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
				  // 
				  break;
				default: break;
			}
	}
}
void mode_paint()
{
	htim3.Instance -> ARR = 2000;
	bluetooth_status = false;
	step = 0;
	//
	ILI9341_Fill(WHITE);
	ILI9341_Fill_Rect(0,0,329,35,GRAYBLUE);
	ILI9341_printText("Bluetooth",75,5,COLOR_WHITE,GRAYBLUE,3);
	ILI9341_Fill_Rect(20,10,40,20,COLOR_WHITE);
	ILI9341_fillTriangle(20,5,20,25,0,15,COLOR_WHITE);
	//
	ILI9341_Fill_Rect(0,125,230,126,GRAYBLUE);
	ILI9341_drawFastHLine(0, 210, 230,GRAYBLUE);
	ILI9341_drawFastVLine(230,0,239,GRAYBLUE);
	ILI9341_drawFastVLine(229,0,239,GRAYBLUE);
	ILI9341_printText("Desigh by Phuoc",10,215,GRAYBLUE,WHITE,2);
	// duong thang duoi motor status
	ILI9341_drawFastHLine(235,90,80,RED);
	ILI9341_drawFastHLine(235,89,80,RED);
	//
	HAL_UART_Receive_IT(&huart1, (uint8_t*) Rx_Data, 1);
	while(1)
	{
		myTS_Handle = TSC2046_GetTouchData();
		if(myTS_Handle.isPressed)
		{
			if(myTS_Handle.X > 0 && myTS_Handle.X < 40 && myTS_Handle.Y > 0 && myTS_Handle.Y < 35) 
			{
				ILI9341_Fill_Rect(20,8,40,22,COLOR_WHITE);
	      ILI9341_fillTriangle(20,3,20,27,0,15,COLOR_WHITE);
				mode_main();
			}
		}
		if(bluetooth_status == true)
		{
			switch (set_pre)
			{
				case 1:
					ILI9341_printText("Prescaler: 1/1 ",10,50,DARKBLUE,WHITE,2);
				  break;
				case 2:
					ILI9341_printText("Prescaler: 1/2 ",10,50,DARKBLUE,WHITE,2);
				  break;
				case 3:
					ILI9341_printText("Prescaler: 1/4 ",10,50,DARKBLUE,WHITE,2);
				  break;
				case 4:
					ILI9341_printText("Prescaler: 1/8 ",10,50,DARKBLUE,WHITE,2);
				  break;
				case 5:
					ILI9341_printText("Prescaler: 1/16 ",10,50,DARKBLUE,WHITE,2);
				  break;
				default: break;
			}
			switch ((uint8_t)dir_status)
			{
				case true:
					ILI9341_printText("Dir: Right",10,75,DARKBLUE,WHITE,2);
				  break;
				case false:
					ILI9341_printText("Dir: Left ",10,75,DARKBLUE,WHITE,2);
				  break;
			}
			ILI9341_printText("Step: ",10,100,DARKBLUE,WHITE,2);
			ILI9341_drawChar(70, 100,step/1000+48,DARKBLUE,WHITE,2);
			ILI9341_drawChar(85, 100,step%1000/100+48,DARKBLUE,WHITE,2);
			ILI9341_drawChar(100, 100,step%100/10+48,DARKBLUE,WHITE,2);
			ILI9341_drawChar(115, 100,step%10+48,DARKBLUE,WHITE,2);
			//
			ILI9341_printText("RUN",250,50,RED,WHITE,4);
			//
			HAL_TIM_Base_Start_IT(&htim3);
			//
			while(step!=0)
			{
				myTS_Handle = TSC2046_GetTouchData();
				if(myTS_Handle.isPressed)
				{
					if(myTS_Handle.X > 0 && myTS_Handle.X < 40 && myTS_Handle.Y > 0 && myTS_Handle.Y < 35) 
					{
						ILI9341_Fill_Rect(20,8,40,22,COLOR_WHITE);
						ILI9341_fillTriangle(20,3,20,27,0,15,COLOR_WHITE);
						mode_main();
					}
				}
				//
				if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)==0)
				{
					HAL_Delay(10);
					if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)==0)
	        {
						while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)==0);
						ILI9341_printText("STO",250,50,RED,WHITE,4);
						HAL_TIM_Base_Stop_IT(&htim3);
						while(1)
						{
							myTS_Handle = TSC2046_GetTouchData();
							if(myTS_Handle.isPressed)
							{
								if(myTS_Handle.X > 0 && myTS_Handle.X < 40 && myTS_Handle.Y > 0 && myTS_Handle.Y < 35) 
								{
									ILI9341_Fill_Rect(20,8,40,22,COLOR_WHITE);
									ILI9341_fillTriangle(20,3,20,27,0,15,COLOR_WHITE);
									mode_main();
								}
							}
							if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)==0)
							{
								HAL_Delay(10);
								if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)==0)
								{
									while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)==0);
									break;
								}
							}
						}
						HAL_TIM_Base_Start_IT(&htim3);
					}
				}
				//
				ILI9341_drawChar(50, 155,(step/2)/1000+48,DARKBLUE,WHITE,4);
				ILI9341_drawChar(75, 155,(step/2)%1000/100+48,DARKBLUE,WHITE,4);
				ILI9341_drawChar(100, 155,(step/2)%100/10+48,DARKBLUE,WHITE,4);
				ILI9341_drawChar(125, 155,(step/2)%10+48,DARKBLUE,WHITE,4);
			}			
			ILI9341_printText("0000 ",50,155,DARKBLUE,WHITE,4);
				/*
				
				ILI9341_drawChar(50, 155,step/1000+48,DARKBLUE,WHITE,4);
				ILI9341_drawChar(75, 155,step%1000/100+48,DARKBLUE,WHITE,4);
				ILI9341_drawChar(100, 155,step%100/10+48,DARKBLUE,WHITE,4);
				ILI9341_drawChar(125, 155,step%10+48,DARKBLUE,WHITE,4);
				
				if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15)==0)
	      { 
					
				}
				*/
			ILI9341_printText("OK  ",250,50,RED,WHITE,4);
			bluetooth_status = false;
		}
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim -> Instance == htim3.Instance)
	{ 
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
		if(bluetooth_status ==  true)
		{
			step--;
			if(step == 0) HAL_TIM_Base_Stop_IT(&htim3);
		}
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t i;
	if(huart -> Instance == huart1.Instance)
	{
		if(Rx_Indx == 0) for(i=0;i<50;i++) Rx_Buffer[i]=0;
		if(Rx_Data[0] != 13) 
		{
			Rx_Buffer[Rx_Indx++] = Rx_Data[0];
		}
    else
		{ 
			Rx_Indx = 0;
			if(Rx_Buffer[0] == 'R') 
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
				dir_status = true;
			}
      else if(Rx_Buffer[0] == 'L')	
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);	
				dir_status = false;
			}
			else if(Rx_Buffer[0] == 'F') set_pre = 1;
			else if(Rx_Buffer[0] == 'H') set_pre = 2;
			else if(Rx_Buffer[0] == 'Q') set_pre = 3;
			else if(Rx_Buffer[0] == 'E') set_pre = 4;
			else if(Rx_Buffer[0] == 'S') set_pre = 5;
			else 
				{
					switch (set_pre)
					{
						case 1:						
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);							
							break;
						case 2:
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
							break;
						case 3:							
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
							break;
						case 4:							
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
							break;
						case 5:					
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
							break;
						default: break;
					}
					step = atoi(Rx_Buffer)*2;
			    bluetooth_status = true;
				}
		}
    HAL_UART_Receive_IT(&huart1, (uint8_t*) Rx_Data, 1);		
	}
}
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
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 71;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 50000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 71;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RS_Pin|D_DC_Pin|D_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DIR_Pin|STEP_Pin|MS3_Pin|MS2_Pin 
                          |MS1_Pin|EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : SW_Pin */
  GPIO_InitStruct.Pin = SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RS_Pin D_DC_Pin D_CS_Pin */
  GPIO_InitStruct.Pin = RS_Pin|D_DC_Pin|D_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DIR_Pin STEP_Pin MS3_Pin MS2_Pin 
                           MS1_Pin EN_Pin */
  GPIO_InitStruct.Pin = DIR_Pin|STEP_Pin|MS3_Pin|MS2_Pin 
                          |MS1_Pin|EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
