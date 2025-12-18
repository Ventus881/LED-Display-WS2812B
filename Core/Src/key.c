/*
 * key.c
 *
 *  Created on: Jan 26, 2024
 *      Author: Ventu
 */

#include "key.h"

void Key_Init(Key_t *sKey, GPIO_TypeDef* GpioPort, uint32_t GpioPin, uint32_t TimerSinglePress, uint32_t TimerLongPress, uint32_t TimerRepeat)
{
	sKey->Status = KEY_IDLE;

	// set port and pin
	sKey->Port = GpioPort;
	sKey->Pin = GpioPin;

	//	enable clock
	if(sKey->Port == GPIOA)
	{
		RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	}
	else if(sKey->Port == GPIOB)
	{
		RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	}
	else if(sKey->Port == GPIOC)
	{
		RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	}
	else if(sKey->Port == GPIOD)
	{
		RCC->AHBENR |= RCC_AHBENR_GPIODEN;
	}

	//	set as input
	sKey->Port->MODER &= ~(MODE_INPUT<<((sKey->Pin)*2));

	// set timers
	sKey->TimerSinglePress = TimerSinglePress;
	sKey->TimerLongPress = TimerLongPress;
	sKey->TimerRepeat = TimerRepeat;

}

void Key_SetSinglePressTimer(Key_t *sKey, uint32_t ms)
{
	sKey->TimerSinglePress = ms;
}

void Key_SetLongPressTimer(Key_t *sKey, uint32_t ms)
{
	sKey->TimerLongPress = ms;
}

void Key_SetRepeatTimer(Key_t *sKey, uint32_t ms)
{
	sKey->TimerRepeat = ms;
}

void Key_RegisterSinglePressCallback(Key_t *sKey, void (*Callback)(SegmentHSV_t *sHSV))
{
	sKey->SinglePress = Callback;
}

void Key_RegisterLongPressCallback(Key_t *sKey, void (*Callback)(SegmentHSV_t *sHSV))
{
	sKey->LongPress = Callback;
}

void Key_RegisterRepeatCallback(Key_t *sKey, void (*Callback)(SegmentHSV_t *sHSV))
{
	sKey->Repeat = Callback;
}

static void Key_IdleRoutine(Key_t *sKey)
{
	// check if button is pressed
	if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(sKey->Port, sKey->Pin)) // 0 = pressed, 1 = released
	{
		sKey->LastTick = HAL_GetTick();
		sKey->Status = KEY_PRESSED;
	}
}

static void Key_SinglePressRoutine(Key_t *sKey, SegmentHSV_t *sHSV)
{
	// if SinglePress time has elapsed
	if((HAL_GetTick() - sKey->LastTick) > sKey->TimerSinglePress)
	{
			// is button still pressed
			if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(sKey->Port, sKey->Pin))
			{
				sKey->Status = KEY_LONGPRESS;
				sKey->LastTick = HAL_GetTick();

				// check if callback is not empty
				if(sKey->SinglePress != NULL)
				{
					sKey->SinglePress(sHSV);
				}
			}
			else
			{
				sKey->Status = KEY_IDLE;
			}

	}
}

static void Key_LongPressRoutine(Key_t *sKey, SegmentHSV_t *sHSV)
{
	// if LongPress time has elapsed
	if((HAL_GetTick() - sKey->LastTick) > sKey->TimerLongPress)
	{
		// is button still pressed
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(sKey->Port, sKey->Pin))
		{
			sKey->Status = KEY_REPEAT;
			sKey->LastTick = HAL_GetTick();

			// check if callback is not empty
			if(sKey->LongPress != NULL)
			{
				sKey->LongPress(sHSV);
			}
		}
		else
		{
			sKey->Status = KEY_IDLE;
		}
	}
}

static void Key_RepeatRoutine(Key_t *sKey, SegmentHSV_t *sHSV)
{
	// if Repeat time has elapsed
	if((HAL_GetTick() - sKey->LastTick) > sKey->TimerRepeat)
	{
		// is button still pressed
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(sKey->Port, sKey->Pin))
		{
			sKey->LastTick = HAL_GetTick();

			// check if callback is not empty
			if(sKey->Repeat != NULL)
			{
				sKey->Repeat(sHSV);
			}
		}
		else
		{
			sKey->Status = KEY_IDLE;
		}
	}
}

void Key_Routine(Key_t *sKey, SegmentHSV_t *sHSV)
{
	switch(sKey->Status)
	{
	case KEY_IDLE:
		Key_IdleRoutine(sKey);
		break;

	case KEY_PRESSED:
		Key_SinglePressRoutine(sKey, sHSV);
		break;

	case KEY_LONGPRESS:
		Key_LongPressRoutine(sKey, sHSV);
		break;

	case KEY_REPEAT:
		Key_RepeatRoutine(sKey, sHSV);
		break;

	default:
		break;
	}
}

