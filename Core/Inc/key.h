/*
 * key.h
 *
 *  Created on: Jan 26, 2024
 *      Author: Ventu
 */

#ifndef INC_KEY_H_
#define INC_KEY_H_

#include "main.h"
#include "WS2812B.h"

typedef enum{
	KEY_IDLE = 0,
	KEY_PRESSED = 1,
	KEY_REPEAT = 2,
	KEY_LONGPRESS = 3
}Key_Status_t;

typedef struct{
	GPIO_TypeDef*	Port;
	uint32_t		Pin;
	Key_Status_t 	Status;

	uint32_t		LastTick;
	uint32_t		TimerSinglePress;
	uint32_t		TimerLongPress;
	uint32_t		TimerRepeat;

	void(*SinglePress)(SegmentHSV_t *sHSV);
	void(*LongPress)(SegmentHSV_t *sHSV);
	void(*Repeat)(SegmentHSV_t *sHSV);
}Key_t;


void Key_Init(Key_t *sKey, GPIO_TypeDef* GpioPort, uint32_t GpioPin, uint32_t TimerSinglePress, uint32_t TimerHold, uint32_t TimerRepeat);
void Key_SetSinglePressTimer(Key_t *sKey, uint32_t ms);
void Key_SetLongPressTimer(Key_t *sKey, uint32_t ms);
void Key_SetRepeatTimer(Key_t *sKey, uint32_t ms);
void Key_RegisterSinglePressCallback(Key_t *sKey, void (*Callback)(SegmentHSV_t *sHSV));
void Key_RegisterLongPressCallback(Key_t *sKey, void (*Callback)(SegmentHSV_t *sHSV));
void Key_RegisterRepeatCallback(Key_t *sKey, void (*Callback)(SegmentHSV_t *sHSV));
void Key_Routine(Key_t *sKey, SegmentHSV_t *sHSV);
#endif /* INC_KEY_H_ */
