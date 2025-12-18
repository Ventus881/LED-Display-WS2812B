/*
 * parser.h
 *
 *  Created on: Sep 28, 2022
 *      Author: Ventu
 */

#ifndef INC_PARSER_H_
#define INC_PARSER_H_

#include "main.h"
#include "tim.h"
#include "ring_buffer.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "WS2812B.h"

#define ENDLINE '\n'

typedef enum{
	AUTO = 0,
	LOW = 1,
	MEDIUM = 2,
	HIGH = 3,
	MANUAL = 4
}BrightnessFlag_t;

typedef enum{
	COLOR_HOUR = 0,
	COLOR_COLON = 1,
	COLOR_MINUTE = 2
}Color_t;

typedef enum{
	COLOR_TEMPERATURE = 0,
	COLOR_DEGREE_SYMBOL = 1,
	COLOR_CELCIUS = 2
}ColorT_t;

extern Color_t ColorFlag;
extern ColorT_t ColorTFlag;

extern BrightnessFlag_t BrightnessFlag;

void HC_TIME_Callback(char * time);
void HC_COLOR_Callback(char * color);
void HC_COLORT_Callback(char * color);
void HC_COLORS_Callback(char * color);
void HC_BRIGHT_Callback(char * brightness);

void Parse(Ring_buffer_t *Ring_buf, uint8_t *Destination);

void ParseData(uint8_t *Data);

#endif /* INC_PARSER_H_ */
