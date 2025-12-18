/*
 * ring_buffer.c
 *
 *  Created on: Sep 28, 2022
 *      Author: Ventu
 */

#include "ring_buffer.h"

void Usart_Log(char *Message)
{
	HAL_UART_Transmit(&huart3, (uint8_t*)Message, strlen(Message), 100);
}

RB_Status_t Ring_Buf_Write(Ring_buffer_t *Ring_buf, uint8_t Value)
{
	uint8_t HeadTmp;

	HeadTmp = (Ring_buf->Head + 1) % BUF_SIZE;

	if(HeadTmp == Ring_buf->Tail)
	{
		return RB_ERROR;
	}

	Ring_buf->Buffer[Ring_buf->Head] = Value;

	Ring_buf->Head = HeadTmp;

	return RB_OK;

}

RB_Status_t Ring_Buf_Read(Ring_buffer_t *Ring_buf, uint8_t *buffer)
{
	if(Ring_buf->Head == Ring_buf->Tail)
	{
		return RB_ERROR;
	}

	*buffer = Ring_buf->Buffer[Ring_buf->Tail];

	Ring_buf->Tail = (Ring_buf->Tail + 1) % BUF_SIZE;

	return RB_OK;
}

void Ring_Buf_Clear(Ring_buffer_t *Ring_buf)
{
	Ring_buf->Head = 0;
	Ring_buf->Tail = 0;
}
