/*
 * ring_buffer.h
 *
 *  Created on: Sep 28, 2022
 *      Author: Ventu
 */

#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#include "main.h"
#include "usart.h"
#include "stdlib.h"
#include "string.h"

#define BUF_SIZE 32

typedef enum{
	RB_OK = 0,
	RB_ERROR = 1
}RB_Status_t;

typedef struct{

	uint16_t Head;
	uint16_t Tail;
	uint8_t Buffer[BUF_SIZE];
}Ring_buffer_t;


void Usart_Log(char *Message);
RB_Status_t Ring_Buf_Write(Ring_buffer_t *Ring_buf, uint8_t Value);
RB_Status_t Ring_Buf_Read(Ring_buffer_t *Ring_buf, uint8_t *buffer);

void Ring_Buf_Clear(Ring_buffer_t *Ring_buf);

#endif /* INC_RING_BUFFER_H_ */
