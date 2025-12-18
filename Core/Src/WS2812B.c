/*
 * WS2812B.c
 *
 *  Created on: 16 wrz 2022
 *      Author: Ventu
 */

#include "WS2812B.h"

/******************************| PRE-PROCESSOR SECTION |******************************/
#define TLow 27
#define THigh 44
#define TReset 280

/******************************| VARIABLE SECTION |******************************/
uint16_t pwmData[(24*MAX_LED)+TReset];
uint8_t DataSentFlag;
WS2812B_t WS2812B_Array[MAX_LED];

uint8_t digits[11][5] = {
		{0b00000111, 0b00000101, 0b00000101, 0b00000101, 0b00000111},		// 0
		{0b00000010, 0b00000110, 0b00000010, 0b00000010, 0b00000111},		// 1
		{0b00000111, 0b00000001, 0b00000111, 0b00000100, 0b00000111},		// 2
		{0b00000111, 0b00000001, 0b00000111, 0b00000001, 0b00000111},		// 3
		{0b00000101, 0b00000101, 0b00000111, 0b00000001, 0b00000001},		// 4
		{0b00000111, 0b00000100, 0b00000111, 0b00000001, 0b00000111},		// 5
		{0b00000111, 0b00000100, 0b00000111, 0b00000101, 0b00000111},		// 6
		{0b00000111, 0b00000001, 0b00000001, 0b00000001, 0b00000001},		// 7
		{0b00000111, 0b00000101, 0b00000111, 0b00000101, 0b00000111},		// 8
		{0b00000111, 0b00000101, 0b00000111, 0b00000001, 0b00000111},		// 9
		{0b00000111, 0b00000100, 0b00000100, 0b00000100, 0b00000111}		// C
};

/**************************************************************************************************************/
/***********************************************| HSV SECTION |************************************************/
/**************************************************************************************************************/
//
//	Set diode with HSV model
//
//	Hue 0-359
//	Saturation 0-255
//	Brightness(Value) 0-255
//
void WS2812B_SetDiodeHSV(uint8_t x, uint8_t y, uint16_t Hue, uint8_t Saturation, uint8_t Brightness)
{
	uint8_t diode_num = ((6*x)+y);
	if(diode_num >= MAX_LED || diode_num < 0) return;
	uint16_t Sector, Fracts, p, q, t;

	if(Saturation == 0)
	{
		WS2812B_Array[diode_num].r = Brightness;
		WS2812B_Array[diode_num].g = Brightness;
		WS2812B_Array[diode_num].b = Brightness;
	}
	else
	{
		if(Hue >= 360) Hue = 359;

		Sector = Hue / 60; // Sector 0 to 5
		Fracts = Hue % 60;
		p = (Brightness * (255 - Saturation)) / 256;
		q = (Brightness * (255 - (Saturation * Fracts)/60)) / 256;
		t = (Brightness * (255 - (Saturation * (59 - Fracts))/60)) / 256;


		switch(Sector)
		{
		case 0:
			WS2812B_Array[diode_num].r = Brightness;
			WS2812B_Array[diode_num].g = (uint8_t)t;
			WS2812B_Array[diode_num].b = (uint8_t)p;
			break;
		case 1:
			WS2812B_Array[diode_num].r = (uint8_t)q;
			WS2812B_Array[diode_num].g = Brightness;
			WS2812B_Array[diode_num].b = (uint8_t)p;
			break;
		case 2:
			WS2812B_Array[diode_num].r = (uint8_t)p;
			WS2812B_Array[diode_num].g = Brightness;
			WS2812B_Array[diode_num].b = (uint8_t)t;
			break;
		case 3:
			WS2812B_Array[diode_num].r = (uint8_t)p;
			WS2812B_Array[diode_num].g = (uint8_t)q;
			WS2812B_Array[diode_num].b = Brightness;
			break;
		case 4:
			WS2812B_Array[diode_num].r = (uint8_t)t;
			WS2812B_Array[diode_num].g = (uint8_t)p;
			WS2812B_Array[diode_num].b = Brightness;
			break;
		default:		// case 5:
			WS2812B_Array[diode_num].r = Brightness;
			WS2812B_Array[diode_num].g = (uint8_t)p;
			WS2812B_Array[diode_num].b = (uint8_t)q;
			break;
		}
	}
}

void WS2812B_ChangeHourHue(SegmentHSV_t * sHSV, uint16_t hue)
{
	sHSV->hour_hue = hue;
}
void WS2812B_ChangeMinuteHue(SegmentHSV_t * sHSV, uint16_t hue)
{
	sHSV->minute_hue = hue;
}
void WS2812B_ChangeColonHue(SegmentHSV_t * sHSV, uint16_t hue)
{
	sHSV->colon_hue = hue;
}
void WS2812B_ChangeStripHue(SegmentHSV_t * sHSV, uint16_t hue)
{
	sHSV->strip_hue = hue;
}
void WS2812B_ChangeTempHue(SegmentHSV_t * sHSV, uint16_t hue)
{
	sHSV->temp_hue = hue;
}
void WS2812B_ChangeDegreeHue(SegmentHSV_t * sHSV, uint16_t hue)
{
	sHSV->degree_symbol_hue = hue;
}
void WS2812B_ChangeCelciusHue(SegmentHSV_t * sHSV, uint16_t hue)
{
	sHSV->celcius_hue = hue;
}
void WS2812B_ChangeBrightness(SegmentHSV_t * sHSV, uint8_t saturation, uint8_t brightness)
{
	sHSV->saturation = saturation;
	sHSV->brightness = brightness;

}

void WS2812B_DrawStrip(SegmentHSV_t * sHSV, RTC_TimeTypeDef * RtcTime)
{
	uint8_t i;
	//	Draw strip at the bottom of the matrix led
	for(i = (RtcTime->Seconds/4); i > 0; i--)
	{
		WS2812B_SetDiodeHSV((RtcTime->Seconds/4)-i, 5, sHSV->strip_hue, sHSV->saturation, sHSV->brightness);
	}

	if(RtcTime->Seconds >= 59)
	{
		for(i = 0; i < 15; i++)
		{
			WS2812B_ClearDiode(i, 5);
		}
	}
}

void WS2812B_SetTemperature(SegmentHSV_t * sHSV, int32_t temperature)
{
	uint8_t decimal = (temperature%10);
	uint8_t unit = (temperature/10);
	uint8_t arr, i;

	// set temperature
	// first digit
	for(arr = 0; arr < 5; arr++) // 5 rows for a number
	{
		for(i = 0; i < 3; i++) // 3 columns for a number
		{
			if(digits[unit][arr] & (1<<i))
			{
				WS2812B_SetDiodeHSV(12+i, arr, sHSV->temp_hue, sHSV->saturation, sHSV->brightness);
			}
			else WS2812B_ClearDiode(12+i, arr);
		}
	}
	// second digit
	for(arr = 0; arr < 5; arr++) // 5 rows for a number
	{
		for(i = 0; i < 3; i++) // 3 columns for a number
			{
				if(digits[decimal][arr] & (1<<i))
				{
					WS2812B_SetDiodeHSV(8+i, arr, sHSV->temp_hue, sHSV->saturation, sHSV->brightness);
				}
				else WS2812B_ClearDiode(8+i, arr);
			}
	}

	// set degree symbol
	for(i = 0; i < 2; i++)
	{
		WS2812B_SetDiodeHSV(6, i, sHSV->degree_symbol_hue, sHSV->saturation, sHSV->brightness);
		WS2812B_SetDiodeHSV(5, i, sHSV->degree_symbol_hue, sHSV->saturation, sHSV->brightness);
	}

	// set Celcius symbol
	for(arr = 0; arr < 5; arr++) // 5 rows for a number
	{
		for(i = 0; i < 3; i++) // 3 columns for a number
			{
				if(digits[10][arr] & (1<<i))
				{
					WS2812B_SetDiodeHSV(1+i, arr, sHSV->celcius_hue, sHSV->saturation, sHSV->brightness);
				}
				else WS2812B_ClearDiode(1+i, arr);
			}
	}

	WS2812B_Send();
}

void WS2812B_SetTime(SegmentHSV_t *sHSV, RTC_TimeTypeDef * RtcTime)
{
	WS2812B_SetHour(sHSV, RtcTime->Minutes);
	WS2812B_SetMinute(sHSV, RtcTime->Seconds);

	//draw colon in between
	WS2812B_SetDiodeHSV(COLON_X_LOCATION, COLON_Y1_LOCATION,  sHSV->colon_hue, sHSV->saturation, sHSV->brightness);
	WS2812B_SetDiodeHSV(COLON_X_LOCATION, COLON_Y2_LOCATION, sHSV->colon_hue, sHSV->saturation, sHSV->brightness);

	WS2812B_Send();
}

void WS2812B_SetHour(SegmentHSV_t * sHSV, uint8_t hour)
{
	uint8_t decimal = (hour%10);
	uint8_t unit = (hour/10);
	uint8_t arr, i;

	// first digit
	for(arr = 0; arr < 5; arr++) // 5 rows for a number
	{
		for(i = 0; i < 3; i++)	// 3 columns for a number
		{
			if(digits[unit][arr] & (1<<i))
			{
				WS2812B_SetDiodeHSV(12+i, arr, sHSV->hour_hue, sHSV->saturation, sHSV->brightness);
			}
			else WS2812B_ClearDiode(12+i, arr);
		}
	}
	// second digit
	for(arr = 0; arr < 5; arr++) // 5 rows for a number
	{
		for(i = 0; i < 3; i++) // 3 columns for a number
		{
			if(digits[decimal][arr] & (1<<i))
			{
				WS2812B_SetDiodeHSV(8+i, arr, sHSV->hour_hue, sHSV->saturation, sHSV->brightness);
			}
			else WS2812B_ClearDiode(8+i, arr);
		}
	}
}

void WS2812B_SetMinute(SegmentHSV_t * sHSV, uint8_t minute)
{
	uint8_t decimal = (minute%10);
	uint8_t unit = (minute/10);
	uint8_t arr, i;

	// first digit
	for(arr = 0; arr < 5; arr++) // 5 rows for a number
	{
		for(i = 0; i < 3; i++) // 3 columns for a number
		{
			if(digits[unit][arr] & (1<<i))
			{
				WS2812B_SetDiodeHSV(4+i, arr, sHSV->minute_hue, sHSV->saturation, sHSV->brightness);
			}
			else WS2812B_ClearDiode(4+i, arr);
		}
	}
	// second digit
	for(arr = 0; arr < 5; arr++) // 5 rows for a number
	{
		for(i = 0; i < 3; i++) // 3 columns for a number
		{
			if(digits[decimal][arr] & (1<<i))
			{
				WS2812B_SetDiodeHSV(i, arr, sHSV->minute_hue, sHSV->saturation, sHSV->brightness);
			}
			else WS2812B_ClearDiode(i, arr);
		}
	}
}

void WS2812B_BlinkFieldHour(SegmentHSV_t *sHSV, RTC_TimeTypeDef * RtcTime)
{
	if(HAL_GetTick() - sHSV->last_tick > 500 && sHSV->FLAG)
	{
		sHSV->brightness = 0;
		sHSV->last_tick = HAL_GetTick();
		WS2812B_SetHour(sHSV, RtcTime->Minutes);
		sHSV->FLAG = 0;
	}
	else if(HAL_GetTick() - sHSV->last_tick > 500 && !sHSV->FLAG)
	{
		sHSV->tmp_brightness = sHSV->brightness;
		sHSV->last_tick = HAL_GetTick();
		WS2812B_SetHour(sHSV, RtcTime->Minutes);
		sHSV->FLAG = 1;
	}
	WS2812B_Send();
	sHSV->brightness = sHSV->tmp_brightness;
}

void WS2812B_BlinkFieldMinute(SegmentHSV_t *sHSV, RTC_TimeTypeDef * RtcTime)
{
	if(HAL_GetTick() - sHSV->last_tick > 500 && sHSV->FLAG)
	{
		sHSV->brightness = 0;
		sHSV->last_tick = HAL_GetTick();
		WS2812B_SetMinute(sHSV, RtcTime->Seconds);
		sHSV->FLAG = 0;
	}
	else if(HAL_GetTick() - sHSV->last_tick > 500 && !sHSV->FLAG)
	{
		sHSV->tmp_brightness = sHSV->brightness;
		sHSV->last_tick = HAL_GetTick();
		WS2812B_SetMinute(sHSV, RtcTime->Seconds);
		sHSV->FLAG = 1;
	}
	WS2812B_Send();
	sHSV->brightness = sHSV->tmp_brightness;
}

/**************************************************************************************************************/
/***********************************************| RGB SECTION |************************************************/
/**************************************************************************************************************/

void WS2812B_SetDiodeColor(uint8_t x, uint8_t y, uint32_t color)
{
	uint8_t diode_num = ((6*x)+y);
	if(diode_num >= MAX_LED || diode_num < 0) return;
	WS2812B_Array[diode_num].r = ((color>>16) & 0xFF);
	WS2812B_Array[diode_num].g = ((color>>8) & 0xFF);
	WS2812B_Array[diode_num].b = (color & 0xFF);

}
void WS2812B_ClearDisplay(void)
{
	uint8_t i, j;
	for(i = 0; i < 15; i++)
	{
		for(j = 0; j < 5; j++)
		{
			WS2812B_ClearDiode(i,j);
		}
	}
}
void WS2812B_ClearDiode(uint8_t x, uint8_t y)
{
	uint8_t diode_num = ((6*x)+y);
	if(diode_num >= MAX_LED || diode_num < 0) return;
	WS2812B_Array[diode_num].r = 0;
	WS2812B_Array[diode_num].g = 0;
	WS2812B_Array[diode_num].b = 0;
}

void WS2812B_Send(void)
{
	uint16_t index = 0;

	for(uint8_t CurrentLed = 0; CurrentLed < MAX_LED; CurrentLed++)
	{
		// Green
		for(int8_t j = 7; j >= 0; j--)
		{
			if((WS2812B_Array[CurrentLed].g & (1<<j)) == 0)
			{
				pwmData[index] = TLow;	// 1/3 of 90 '0'
			}
			else
			{
				pwmData[index] = THigh;	// 2/3 of 90 '1'
			}
			index++;
		}

		// Red
		for(int8_t j = 7; j >= 0; j--)
		{
			if((WS2812B_Array[CurrentLed].r & (1<<j)) == 0)
			{
				pwmData[index] = TLow;	// 1/3 of 90 '0'
			}
			else
			{
				pwmData[index] = THigh;	// 2/3 of 90 '1'
			}
			index++;
		}

		// Blue
		for(int8_t j = 7; j >= 0; j--)
		{
			if((WS2812B_Array[CurrentLed].b & (1<<j)) == 0)
			{
				pwmData[index] = TLow;	// 1/3 of 90 '0'
			}
			else
			{
				pwmData[index] = THigh;	// 2/3 of 90 '1'
			}
			index++;
		}
	}

	// Reset code >280us
	for(uint16_t i = 0; i < TReset; i++)
	{
		pwmData[index] = 0;
		index++;
	}

	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwmData, index);
	while(!DataSentFlag){};
	DataSentFlag = 0;

}

const uint8_t gamma_lut[255] = {
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,   1,
     1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   4,   4,
     4,   4,   5,   5,   5,   5,   6,   6,   6,   7,   7,   7,   8,   8,   8,   9,
     9,   9,  10,  10,  11,  11,  12,  12,  12,  13,  13,  14,  14,  15,  15,  16,
    16,  17,  17,  18,  18,  19,  19,  20,  20,  21,  22,  22,  23,  23,  24,  25,
    25,  26,  27,  27,  28,  29,  29,  30,  31,  31,  32,  33,  33,  34,  35,  36,
    36,  37,  38,  39,  40,  40,  41,  42,  43,  44,  44,  45,  46,  47,  48,  49,
    50,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,
    65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  79,  80,  81,
    82,  83,  84,  85,  87,  88,  89,  90,  91,  93,  94,  95,  96,  97,  99, 100,
   101, 102, 104, 105, 106, 108, 109, 110, 112, 113, 114, 116, 117, 118, 120, 121,
   122, 124, 125, 127, 128, 129, 131, 132, 134, 135, 137, 138, 140, 141, 143, 144,
   146, 147, 149, 150, 152, 153, 155, 157, 158, 160, 161, 163, 164, 166, 168, 169,
   171, 173, 174, 176, 178, 179, 181, 183, 184, 186, 188, 190, 191, 193, 195, 197,
   198, 200, 202, 204, 205, 207, 209, 211, 213, 215, 216, 218, 220, 222, 224, 226,
   228, 230, 231, 233, 235, 237, 239, 241, 243, 245, 247, 249, 251, 253, 255,};
