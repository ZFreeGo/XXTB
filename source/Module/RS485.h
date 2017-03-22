/*************************************************
* Copyright ? BeiJing SOJO Electric Co.,Ltd. 2001-2016. 
* All rights reserved.
* File name: InitRS485.h
* Author: ZDom ID：2217 Version: V1.0 Date: 2017.03.02
* Description: 提供初始485驱动函数的一些宏定义，以及波特率
* Others: // 其它内容的说明
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
* // 者及修改内容简述
* 1. Date:
* Author: ID:
* Modification:
* 2. ...
*************************************************/

#ifndef _INITRS485_H_
#define	_INITRS485_H_

#include "LPC17xx.h"

#define RX_FIFO_SIZE    8   //FIFO接收触发深度设置
#define TX_FIFO_SIZE    16  //发送深度
#define BUFFER_SIZE     128 //缓冲区大小

#define FCR_VAL ((RX_FIFO_SIZE == 4)?0x47 : ((RX_FIFO_SIZE == 8)?0x87:((RX_FIFO_SIZE == 14)?0xC7:0x87)))

/**************************************
 * 以下宏定义为使用FIFO，其中THRE为发送中断
 * RLS为接收线状态，RDA为接收数据可用
 **************************************/
#define UART_IRQ_THRE  0x02
#define UART_IRQ_RLS   0x0C
#define UART_IRQ_RDA   0x04

void Init_UART2(uint32_t bps);
uint8_t UART2_GetChar (void);
void UART2_SendData(uint8_t *data,uint8_t len);

#endif
