/***************************************************************
*Copyright(c) 2016, Sojo
*��������Ȩ��
*�ļ�����:UART.h
*�ļ���ʶ:
*�������ڣ� 2016��11��8�� 
*ժҪ:	 ���ļ�������ʼ��UARTģ��
*��ǰ�汾:1.0
*����: ZFREE
*ȡ���汾:
*����:
*���ʱ��:
************************************************************/
#ifndef _UART_H
#define _UART_H

#include <stdint.h>

void UART0_Init (void);
uint8_t  UART0_SendByte (uint8_t  ucData);
uint8_t   UART0_GetChar (void);
void UART0_SendString (uint8_t  *s); 



void UART2_Init (void);
uint8_t   UART2_SendByte (uint8_t ucData);
uint8_t   UART2_GetChar (void);
void UART2_SendString (uint8_t  *s);

void UART3_Init (void);
uint8_t   UART3_SendByte (uint8_t ucData);
uint8_t   UART3_GetChar (void);
void UART3_SendString (uint8_t  *s);

void UART0_SendChar(uint16_t disp);  




#endif
