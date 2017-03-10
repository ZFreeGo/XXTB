/***************************************************************
*Copyright(c) 2016, Sojo
*保留所有权利
*文件名称:UART.h
*文件标识:
*创建日期： 2016年11月8日 
*摘要:	 此文件用来初始化UART模块
*当前版本:1.0
*作者: ZFREE
*取代版本:
*作者:
*完成时间:
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
