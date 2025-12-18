/*
 * parser.c
 *
 *  Created on: Sep 28, 2022
 *      Author: Ventu
 */

#include "parser.h"

BrightnessFlag_t BrightnessFlag;
Color_t ColorFlag;
ColorT_t ColorTFlag;

/* ------------- Callbacks ------------- */
__weak void HC_TIME_Callback(char * time)
{

}

__weak void HC_COLOR_Callback(char * color)
{

}

__weak void HC_COLORT_Callback(char * color)
{

}

__weak void HC_COLORS_Callback(char * color)
{

}

__weak void HC_BRIGHT_Callback(char * brightness)
{

}

/* ------------- Functions ------------- */

void Parse(Ring_buffer_t *Ring_buf, uint8_t *Destination)
{
	uint8_t Tmp;
	uint8_t i = 0;

	do
	{
		Ring_Buf_Read(Ring_buf, &Tmp);

		if(Tmp == ENDLINE)
		{
			Destination[i] = 0;
		}
		else
		{
			Destination[i] = Tmp;
		}
		i++;

	}while(Tmp != ENDLINE);

}

// Change time
// time = hhmm i.e. 1230
// HC_TIME=time
void ParseTime(void)
{
	char *ParsePointer = strtok(NULL, "/n");
	HC_TIME_Callback(ParsePointer);
}

// Change time HSV
// 0-360
// HC_COLOR=hour, colon, minute
void ParseColor(void)
{
	char *ParsePointer = strtok(NULL, ",");;
	ColorFlag = COLOR_HOUR;
	while(ParsePointer)
	{
		HC_COLOR_Callback(ParsePointer);
		ParsePointer = strtok(NULL, ",");
		ColorFlag++;
	}
}

// Change temperature HSV
// 0-360
// HC_COLOR=temperature, degree_symbol, celcius
void ParseColorT(void)
{
	char *ParsePointer = strtok(NULL, ",");;
	ColorTFlag = COLOR_TEMPERATURE;
	while(ParsePointer)
	{
		HC_COLORT_Callback(ParsePointer);
		ParsePointer = strtok(NULL, ",");
		ColorTFlag++;
	}
}

// Change strip color
// color = 0-360
// HC_COLORS=color
void ParseColorS(void)
{
	char *ParsePointer = strtok(NULL, "\n");;
	HC_COLORS_Callback(ParsePointer);
}

// Change brightness
// AUTO - auto brightness from ADC
// HC_BRIGHT=AUTO
void ParseBrightness(void)
{
	char * ParsePointer = strtok(NULL, "/n");

	// Auto brightness from ADC
	if(strcmp("AUTO", ParsePointer) == 0)
	{
		Usart_Log("Brightness set to AUTO\n\r");
		BrightnessFlag = AUTO;
		return;
	}
	else if(strcmp("LOW", ParsePointer) == 0)
	{
		Usart_Log("Brightness set to LOW\n\r");
		BrightnessFlag = LOW;
	}
	else if(strcmp("MED", ParsePointer) == 0)
	{
		Usart_Log("Brightness set to MEDIUM\n\r");
		BrightnessFlag = MEDIUM;
	}
	else if(strcmp("HIGH", ParsePointer) == 0)
	{
		Usart_Log("Brightness set to HIGH\n\r");
		BrightnessFlag = HIGH;
	}

	HC_BRIGHT_Callback(ParsePointer);
}

void ParseEcho(void)
{
	Usart_Log("Change time/n/r");
	Usart_Log("HC_TIME=hhmm/n/r");
	Usart_Log("HC_TIME=1230/n/r/n");

	Usart_Log("Change color of the corresponding fields in HSV(0-360)/n/r");
	Usart_Log("HC_COLOR=hour,colon,minute/n/r");
	Usart_Log("HC_TIME=240,110,53/n/r/n");

	Usart_Log("Change color of the corresponding fields in HSV(0-360)/n/r");
	Usart_Log("HC_COLORT=temperature,degree,celcius/n/r");
	Usart_Log("HC_TIME=240,110,53/n/r/n");

	Usart_Log("Change brightness to auto or fixed levels/n/r");
	Usart_Log("HC_BRIGHT=AUTO; LOW; MID; HIGH/n/r");
	Usart_Log("HC_BRIGHT=MID/n/r/n");
}

void ParseData(uint8_t *Data)
{
	char *ParsePointer = strtok((char*)Data, "=");

	if(strcmp("TIME", ParsePointer) == 0)
	{
		ParseTime();
	}
	else if(strcmp("COLOR", ParsePointer) == 0)
	{
		ParseColor();
	}
	else if(strcmp("COLORT", ParsePointer) == 0)
	{
		ParseColorT();
	}
	else if(strcmp("BRIGHT", ParsePointer) == 0)
	{
		ParseBrightness();
	}
	else if(strcmp("?", ParsePointer) == 0)
	{
		ParseEcho();
	}
}
