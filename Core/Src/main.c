/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "WS2812B.h"
#include "stdio.h"
#include "string.h"
#include "ring_buffer.h"
#include "parser.h"
#include "bme280.h"
#include <stdlib.h>
#include "key.h"
#include <stdlib.h>
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

/* USER CODE BEGIN PV */

/*------ STRUCTURES ------*/
// RTC Struct
RTC_DateTypeDef RtcDate;
RTC_TimeTypeDef RtcTime;

// WS2812B Struct
SegmentHSV_t HSV_Array = {.menu_state=STATE_NORMAL, .select_field=MINUTE_FIELD};

// BME280 Struct
BME280_HandleTypedef hBME280;

// Buttons Struct
Key_t Button1;
Key_t Button2;

/*------ UART ------*/
Ring_buffer_t UART_ReceiveBuffer;
uint8_t UART_Buffer[32];
uint8_t UART_ReceiveTmp;
uint8_t UART_ReceivedLine;
uint8_t UART_ReceiveDestination[32];

/*------ BME280 ------*/
int32_t BME280_Temperature;
uint16_t BME280_Size;
uint8_t BME280_Data[256];
float BME280_Temp, BME280_Pressure, BME280_Humidity;

/*------ ADC ------*/
volatile uint16_t ADCvalue;

/*------ WS2812B ------*/
uint8_t Clear_Flag;
uint8_t registered;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

// Buttons functions
void Menu(void);
void Menu_BrightnessUp(SegmentHSV_t *sHSV);
void Menu_BrightnessDown(SegmentHSV_t *sHSV);
void Menu_EnterField(SegmentHSV_t *sHSV);
void Menu_SelectField(SegmentHSV_t *sHSV);
void Menu_Enter(SegmentHSV_t *sHSV);
void Menu_Exit(SegmentHSV_t *sHSV);
void Menu_ChangeHour(SegmentHSV_t *sHSV);
void Menu_ChangeMinute(SegmentHSV_t *sHSV);

// WS2812B functions
void MATRIX_LED_EVENT(void);
void MATRIX_LED_BRIGHTNESS(void);
void MATRIX_LED_SET_TIME(void);
void MATRIX_LED_SET_TEMP(void);
void MATRIX_LED_PARSE_UART(void);
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
//  __HAL_RCC_BACKUPRESET_FORCE();
//  __HAL_RCC_BACKUPRESET_RELEASE();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

  // BME280 Initialization
  BME280_Init(&hBME280, &hi2c1, BME280_I2C_ADDRESS_0);
  HAL_Delay(10);

  // Buttons Initialization
//  Key_Init(&Button1, B1_GPIO_Port, B1_Pin, 20, 2000, 200);
//  Key_Init(&Button2, B2_GPIO_Port, B2_Pin, 20, 2000, 200);

  // WS2812B Initialization
  WS2812B_ChangeHourHue(&HSV_Array, 50);
  WS2812B_ChangeMinuteHue(&HSV_Array, 100);
  WS2812B_ChangeColonHue(&HSV_Array, 150);
  WS2812B_ChangeStripHue(&HSV_Array, 290);
  WS2812B_ChangeBrightness(&HSV_Array, 200, 30);
  WS2812B_ChangeTempHue(&HSV_Array, 200);
  HSV_Array.last_tick = HAL_GetTick();

  // UART Initialization
  HAL_UART_Receive_IT(&huart3, &UART_ReceiveTmp, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  MATRIX_LED_EVENT();
	  // Main function
//	  Menu();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_RTC
                              |RCC_PERIPHCLK_TIM1|RCC_PERIPHCLK_ADC1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Adc1ClockSelection = RCC_ADC1PLLCLK_DIV1;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* USART3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* ADC1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(ADC1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(ADC1_IRQn);
}

/* USER CODE BEGIN 4 */
/*--------------------- Menu Functions ---------------------*/
void Menu(void)
{
	// Buttons
	Key_Routine(&Button1, &HSV_Array);
	Key_Routine(&Button2, &HSV_Array);

	switch(HSV_Array.menu_state)
	{

	case STATE_NORMAL:
		MATRIX_LED_EVENT();
		if(!registered)
		{
			Key_RegisterLongPressCallback(&Button1, Menu_Enter);		// Enter STATE_SELECT
			Key_RegisterRepeatCallback(&Button1, Menu_BrightnessUp);	// Manual increase brightness
			Key_RegisterRepeatCallback(&Button2,Menu_BrightnessDown);	// Manual decrease brightness
			registered = 1;
		}
		if(Button1.Status == KEY_LONGPRESS && Button2.Status == KEY_LONGPRESS) BrightnessFlag = AUTO; // Set brightness to auto
		break;

	case STATE_SELECT:
		if(HSV_Array.select_field == MINUTE_FIELD)
		{
			WS2812B_BlinkFieldMinute(&HSV_Array, &RtcTime);		// Blink minute field every 500ms
		}
		else
		{
			WS2812B_BlinkFieldHour(&HSV_Array, &RtcTime);		// Blink hour field every 500ms
		}
		if(registered)
		{
			Key_RegisterSinglePressCallback(&Button1, Menu_EnterField);		// Enter blinking field
			Key_RegisterLongPressCallback(&Button1, Menu_Exit);				// Return to STATE_NORMAL
			Key_RegisterSinglePressCallback(&Button2, Menu_SelectField);	// Select field to edit
			registered = 0;
		}

		break;

	case STATE_EDIT_HOUR:
		if(!registered)
		{
			Key_RegisterSinglePressCallback(&Button1, Menu_Exit);		// Return to STATE_SELECT
			Key_RegisterSinglePressCallback(&Button2, Menu_ChangeHour);		// Change hour
			Key_RegisterRepeatCallback(&Button2, Menu_ChangeHour);			// Change hour
			registered = 1;
		}

		break;

	case STATE_EDIT_MINUTE:
		if(!registered)
		{
			Key_RegisterSinglePressCallback(&Button1, Menu_Exit);		// Return to STATE_SELECT
			Key_RegisterSinglePressCallback(&Button2, Menu_ChangeMinute);	// Change minute
			Key_RegisterRepeatCallback(&Button2, Menu_ChangeMinute);		// Change minute
			registered = 1;
		}

		break;

	default:
		break;
	}
}

void Menu_BrightnessUp(SegmentHSV_t *sHSV)
{
	BrightnessFlag = MANUAL;
	sHSV->brightness++;
}

void Menu_BrightnessDown(SegmentHSV_t *sHSV)
{
	BrightnessFlag = MANUAL;
	sHSV->brightness--;
}

void Menu_EnterField(SegmentHSV_t *sHSV)
{
	if(sHSV->select_field == MINUTE_FIELD) sHSV->menu_state = STATE_EDIT_MINUTE;
	else sHSV->menu_state = STATE_EDIT_HOUR;

}

void Menu_SelectField(SegmentHSV_t *sHSV)
{
	sHSV->select_field = ((sHSV->select_field + 1) % 2); // Change between HOUR_FIELD and MINUTE_FIELD
}

void Menu_Enter(SegmentHSV_t *sHSV)
{
	sHSV->menu_state++;
}

void Menu_Exit(SegmentHSV_t *sHSV)
{
	sHSV->menu_state--;
}

void Menu_ChangeHour(SegmentHSV_t *sHSV)
{
	RtcTime.Hours = ((RtcTime.Hours + 1) % 24);
	HAL_RTC_SetTime(&hrtc, &RtcTime, RTC_FORMAT_BIN);
	WS2812B_SetHour(&HSV_Array, RtcTime.Hours);
	WS2812B_Send();
}

void Menu_ChangeMinute(SegmentHSV_t *sHSV)
{
	RtcTime.Minutes = ((RtcTime.Minutes + 1) % 60);
	HAL_RTC_SetTime(&hrtc, &RtcTime, RTC_FORMAT_BIN);
	WS2812B_SetMinute(&HSV_Array, RtcTime.Minutes);
	WS2812B_Send();
}

/*--------------------- Matrix Events ---------------------*/
void MATRIX_LED_EVENT(void)
{
	//  pobieranie aktualnego czasu
	HAL_RTC_GetDate(&hrtc, &RtcDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, &RtcTime, RTC_FORMAT_BIN);

	// Auto brightness
//	if(HAL_GetTick() - ADC_Delay_ms > 500)
	MATRIX_LED_BRIGHTNESS();

	// Display time
	MATRIX_LED_SET_TIME();

	// Clear display
	if(Clear_Flag) WS2812B_ClearDisplay();

	// Display temperature
	MATRIX_LED_SET_TEMP();

	// Parse data on UART
	MATRIX_LED_PARSE_UART();
}

void MATRIX_LED_BRIGHTNESS(void)
{

	if(BrightnessFlag == AUTO)
	{
		HAL_ADC_Start_IT(&hadc1);
	}
	else
	{
		HAL_ADC_Stop_IT(&hadc1);
	}

	WS2812B_ChangeBrightness(&HSV_Array, HSV_Array.saturation, abs((int8_t)(gamma_lut[ADCvalue] - HSV_Array.brightness)) > 20 ? gamma_lut[ADCvalue] : HSV_Array.brightness);
	WS2812B_DrawStrip(&HSV_Array, &RtcTime);
}

void MATRIX_LED_SET_TIME(void)
{
	if(RtcTime.Seconds < 30 || RtcTime.Seconds > 40)
	{
		WS2812B_SetTime(&HSV_Array, &RtcTime);
		Clear_Flag = 1;
	}
}

void MATRIX_LED_SET_TEMP(void)
{
	if(RtcTime.Seconds >= 30 && RtcTime.Seconds <= 40)
	{
		BME280_Read_Temp(&hBME280, &BME280_Temperature);
		WS2812B_SetTemperature(&HSV_Array, BME280_Temperature);
		Clear_Flag = 0;
	}
}

void MATRIX_LED_PARSE_UART(void)
{
	if(UART_ReceivedLine > 0)
	{
		Parse(&UART_ReceiveBuffer, UART_ReceiveDestination);

		UART_ReceivedLine--;

		ParseData(UART_ReceiveDestination);
	}
}

/*--------------------- UART Callbacks ---------------------*/
// They are being used in parser.c as a __weak function replacement
void HC_TIME_Callback(char * time)
{
	int16_t tmp;
	tmp = atoi(time);
	RtcTime.Hours = (tmp/100);
	RtcTime.Minutes = (tmp%100);
	RtcTime.Seconds = 0;
	HAL_RTC_SetTime(&hrtc, &RtcTime, RTC_FORMAT_BIN);
}

void HC_COLOR_Callback(char * color)
{
	int16_t tmp;
	tmp = atoi(color);

	switch(ColorFlag)
	{
	case COLOR_HOUR:
		HSV_Array.hour_hue = tmp;
		break;

	case COLOR_COLON:
		HSV_Array.colon_hue = tmp;
		break;

	case COLOR_MINUTE:
		HSV_Array.minute_hue = tmp;
		break;

	default:
		break;
	}
}

void HC_COLORT_Callback(char * color)
{
	int16_t tmp;
	tmp = atoi(color);

	switch(ColorTFlag)
	{
	case COLOR_TEMPERATURE:
		HSV_Array.temp_hue = tmp;
		break;

	case COLOR_DEGREE_SYMBOL:
		HSV_Array.degree_symbol_hue = tmp;
		break;

	case COLOR_CELCIUS:
		HSV_Array.celcius_hue = tmp;
		break;

	default:
		break;
	}
}

void HC_COLORS_Callback(char * color)
{
	int16_t tmp;
	tmp = atoi(color);
	WS2812B_ChangeStripHue(&HSV_Array, tmp);
}

void HC_BRIGHT_Callback(char * brightness)
{
	int16_t tmp;
	tmp = atoi(brightness);

	switch(tmp)
	{
	case 1:
		HSV_Array.brightness = LOW_BRIGHTNESS; // Low brightness
		break;

	case 2:
		HSV_Array.brightness = MEDIUM_BRIGHTNESS; // Medium brightness
		break;

	case 3:
		HSV_Array.brightness = HIGH_BRIGHTNESS; // High brightness
		break;

	default:
		break;
	}

}

/*---------------------------------- Interrupts ----------------------------------*/

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
	{
		HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
		DataSentFlag = 1;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART3)
    {
    	if(RB_OK == Ring_Buf_Write(&UART_ReceiveBuffer, UART_ReceiveTmp))
    	{
    		if(UART_ReceiveTmp == ENDLINE)
    		{
    			UART_ReceivedLine++;
    		}
    	}
    	HAL_UART_Receive_IT(&huart3, &UART_ReceiveTmp, 1);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1)
	{

		ADCvalue = HAL_ADC_GetValue(hadc);
//		if(ADCvalue < 10) ADCvalue = 10;
//		else if(ADCvalue > 10 && ADCvalue < 110) ADCvalue = 60;
//		else if(ADCvalue > 110 && ADCvalue < 180) ADCvalue = 140;
//		else ADCvalue = 230;

		HAL_ADC_Start_IT(&hadc1);
	}
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
