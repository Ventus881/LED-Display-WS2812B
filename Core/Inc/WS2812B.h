/*
 * WS2812B.h
 *
 *  Created on: 16 wrz 2022
 *      Author: Ventu
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include "main.h"
#include "tim.h"
#include "rtc.h"
#include "adc.h"
#include "parser.h"

#define MAX_LED 90

#define COLON_X_LOCATION 	7
#define COLON_Y1_LOCATION 	1
#define COLON_Y2_LOCATION 	3

#define LOW_BRIGHTNESS		15
#define MEDIUM_BRIGHTNESS	70
#define HIGH_BRIGHTNESS		220

extern uint8_t DataSentFlag;

extern const uint8_t gamma_lut[255];

typedef enum
{
	WS2812B_OK,		// 0
	WS2812B_ERROR	// 1

}WS2812B_status_t;

typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;

	uint8_t x;
	uint8_t y;

}WS2812B_t;

typedef enum
{
	MINUTE_FIELD,
	HOUR_FIELD
}WS2812B_field_t;

typedef enum
{
	STATE_NORMAL,
	STATE_SELECT,
	STATE_EDIT_HOUR,
	STATE_EDIT_MINUTE
}WS2812B_menu_t;

typedef struct
{
	uint16_t hour_hue;
	uint16_t minute_hue;
	uint16_t colon_hue;
	uint16_t strip_hue;

	uint16_t temp_hue;
	uint16_t degree_symbol_hue;
	uint16_t celcius_hue;

	uint8_t saturation;
	uint8_t brightness;

	WS2812B_menu_t menu_state;
	WS2812B_field_t select_field;
	uint32_t last_tick;
	uint8_t tmp_brightness;
	uint8_t FLAG;

}SegmentHSV_t;

void WS2812B_SetDiodeHSV(uint8_t x, uint8_t y, uint16_t Hue, uint8_t Saturation, uint8_t Brightness);
void WS2812B_DrawStrip(SegmentHSV_t * sHSV, RTC_TimeTypeDef * RtcTime);
void WS2812B_SetTemperature(SegmentHSV_t * sHSV, int32_t temperature);
void WS2812B_SetTime(SegmentHSV_t * sHSV, RTC_TimeTypeDef * RtcTime);
void WS2812B_SetHour(SegmentHSV_t * sHSV, uint8_t hour);
void WS2812B_SetMinute(SegmentHSV_t * sHSV, uint8_t minute);
void WS2812B_BlinkFieldHour(SegmentHSV_t *sHSV, RTC_TimeTypeDef * RtcTime);
void WS2812B_BlinkFieldMinute(SegmentHSV_t *sHSV, RTC_TimeTypeDef * RtcTime);

void WS2812B_SetDiodeColor(uint8_t x, uint8_t y, uint32_t color);
void WS2812B_ClearDisplay(void);
void WS2812B_ClearDiode(uint8_t x, uint8_t y);
void WS2812B_Send(void);

void WS2812B_ChangeHourHue(SegmentHSV_t * sHSV, uint16_t hue);
void WS2812B_ChangeMinuteHue(SegmentHSV_t * sHSV, uint16_t hue);
void WS2812B_ChangeColonHue(SegmentHSV_t * sHSV, uint16_t hue);
void WS2812B_ChangeStripHue(SegmentHSV_t * sHSV, uint16_t hue);
void WS2812B_ChangeTempHue(SegmentHSV_t * sHSV, uint16_t hue);
void WS2812B_ChangeDegreeHue(SegmentHSV_t * sHSV, uint16_t hue);
void WS2812B_ChangeCelciusHue(SegmentHSV_t * sHSV, uint16_t hue);
void WS2812B_ChangeBrightness(SegmentHSV_t * sHSV, uint8_t saturation, uint8_t brightness);

#endif /* INC_WS2812B_H_ */
