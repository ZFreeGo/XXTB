/*************************************************
* Copyright ? BeiJing SOJO Electric Co,Ltd 2001-2016 
* All rights reserved
* File name: InitRS485c
* Author: ZDom ID：2217 Version: V10 Date: 20170302
* Description: 完成UART2的初始化，中断接收等。
* Others: // 其它内容的说明
* History: // 修改历史记录列表，每条修改记录应包括修改日期、修改
* // 者及修改内容简述
* 1 Date:
* Author: ID:
* Modification:
* 2 
*************************************************/

#include "Rs485.h"
#include "DeviceIO.h"


#define  PCLK            (SystemCoreClock / 4)         /* Peripheral clock, depends on VPBDIV */

///////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
/**************************************k
 * 以下宏定义为使用FIFO，其中THRE为发送中断
 * RLS为接收线状态，RDA为接收数据可用
 **************************************/
#define UART1  0x01
#define UART2  0x02
////////////////////////////////////////////////////////////

uint16_t    TXlen;
uint16_t    TXcn;

uint8_t TXBuffer[BUFFER_SIZE] = {0};  //发送缓冲区

/***********************************************************
 * 函数名： Init_UART2()
 * 形参：   无
 * 返回值： 无
 * 功能：   UART2初始化,按默认值初始化串口0的引脚和通讯参数
 * 设置为8位数据位，1位停止位，无奇偶校验
 **********************************************************/
void Init_UART2(uint32_t bps)
{
    uint32_t usFdiv = 0;

    LPC_PINCON->PINSEL0 |= (1 << 20);   //PIN010作为TXD2
    LPC_PINCON->PINSEL0 |= (1 << 22);   //PIN011作为RXD2

    LPC_SC->PCONP = LPC_SC->PCONP|(1<<24);  //打开UART2电源控制位

    LPC_UART2->LCR  = 0x83; //允许设置波特率
    usFdiv = (PCLK / 16) / bps;  // 设置波特率
    LPC_UART2->DLM  = usFdiv / 256;
    LPC_UART2->DLL  = usFdiv % 256; 

    LPC_UART2->LCR  = 0x03; //锁定波特率
    LPC_UART2->FCR  |= FCR_VAL; //启用FIFO功能，且在接收到8字节后进入中断
    //LPC_UART2->FCR  |= 0x00; //启用FIFO功能，且在接收到8字节后进入中断

    NVIC_EnableIRQ(UART2_IRQn);   //使能UART2的中断
    NVIC_SetPriority(UART0_IRQn, 0x02); //中断优先级最高

    LPC_UART2->IER = 0x01 ; //使能RDA中断

}

/***********************************************************
 * 函数名： UART_IRQ_Send()
 * 形参：   com：串口号
 * 返回值： 无
 * 功能：   读取串口2的数据,推荐使用中断接收，该函数也可以使用
 * 中间加入的cn变量是为了测试使用。
 * Read character from Serial Port   (blocking read)
 **********************************************************/
void UART2_IRQ_Send(void)
{
    //uint8_t fifo_len = TX_FIFO_SIZE;
    uint8_t i = 0;

    while (!(LPC_UART2->LSR & 0x20));   //等待THER为空

    LPC_UART2->IER &= 0x01;  //关闭发送中断
   
    
    for(i = 0;i < TXlen;i++)
    {
        while(!(LPC_UART2->LSR & 0x20));
        LPC_UART2->THR = TXBuffer[i];
        while(!(LPC_UART2->LSR & 0x40));
    }

  

}
/***********************************************************
 * 函数名： UART2_SendMessage()
 * 形参：   *data：要发送的数据指针;len：发送数据长度
 * 返回值： 无
 * 功能：   读取串口2的数据,推荐使用中断接收，该函数也可以使用
 * 中间加入的cn变量是为了测试使用。
 * Read character from Serial Port   (blocking read)
 **********************************************************/
void UART2_SendData(uint8_t *data,uint8_t len)
{
    uint8_t i = 0;

    for(i = 0;i < len;i++)
    {
        TXBuffer[i] = data[i];
    }    

    TXlen = len;
    TXcn = 0;

    UART2_IRQ_Send();

}
/***********************************************************
 * 函数名： UART2_GetChar()
 * 形参：   无
 * 返回值： 返回收到的数据
 * 功能：   读取串口2的数据,推荐使用中断接收，该函数也可以使用
 * 中间加入的cn变量是为了测试使用。
 * Read character from Serial Port   (blocking read)
 **********************************************************/
uint8_t UART2_GetChar (void) 
{
    uint8_t cn = 0;
  	while (!(LPC_UART2->LSR & 0x01))
    {
        cn++;
        if(cn > 100)
        {
            break;
        }
    }
  	return (LPC_UART2->RBR);
}





