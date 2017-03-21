/***************************************************************
*Copyright(c) 2016, Sojo
*保留所有权利
*文件名称:UART.c
*文件标识:
*创建日期： 2016年11月8日 
*摘要:	 此文件用来初始化UART模块
*当前版本:1.0
*作者: ZFREE
*取代版本:
*作者:
*完成时间:
************************************************************/
#include "lpc17xx.h"
#include "UART.h"

#define UART0_BPS     115200                 /* 串口0通信波特率             */
#define UART1_BPS     115200                 /* 串口2通信波特率             */
#define UART2_BPS     115200                 /* 串口2通信波特率             */
#define UART3_BPS     115200                 /* 串口2通信波特率             */

 
#define FPCLK                      (SystemCoreClock / 4) //CPU主频/4
/******************************************************************************
 *函数名： UART0_Init()
 *形参： void
 *返回值：void
 *功能： 按默认值初始化串口0的引脚和通讯参数。设置为8位数据位，1位停止位，无奇偶校验
 *调用函数：null
 *引用外部变量：null
 *****************************************************************************/
void UART0_Init (void)
{
	uint16_t usFdiv;
    /* UART0 */
    LPC_PINCON->PINSEL0 |= (1 << 4);             /* Pin P0.2 used as TXD0 (Com0) */
    LPC_PINCON->PINSEL0 |= (1 << 6);             /* Pin P0.3 used as RXD0 (Com0) */
  
  	LPC_UART0->LCR  = 0x83;                      /* 允许设置波特率               */
    usFdiv = (FPCLK / 16) / UART0_BPS;           /* 设置波特率                   */
    LPC_UART0->DLM  = usFdiv / 256;
    LPC_UART0->DLL  = usFdiv % 256; 
    LPC_UART0->LCR  = 0x03;                      /* 锁定波特率                   */
    LPC_UART0->FCR  = 0x06; 				   
}

/******************************************************************************
 *函数名： UART0_SendByte()
 *形参： uint8_t ucData 待发送的字符
 *返回值：uint8_t 0--失败  非零成功
 *功能： 将字符放入发送寄存器
 *调用函数：null
 *引用外部变量：null
 *****************************************************************************/
uint8_t UART0_SendByte(uint8_t ucData)
{
	while (!(LPC_UART0->LSR & 0x20));
    return (LPC_UART0->THR = ucData);
}

/******************************************************************************
 *函数名： UART0_GetChar()
 *形参： void
 *返回值：uint8_t  获取的字符
 *功能： 判断并读取字符
 *调用函数：null
 *引用外部变量：null
 *****************************************************************************/
uint8_t UART0_GetChar(void) 
{
  	while (!(LPC_UART0->LSR & 0x01));
  	return (LPC_UART0->RBR);
}

/******************************************************************************
 *函数名：UART0_SendString()
 *形参： uint8 *s --待发送的字符串，需要以\0结尾
 *返回值：void
 *功能： 发送字符串
 *调用函数：UART0_SendByte()
 *引用外部变量：null
 *****************************************************************************/
void UART0_SendString (uint8_t *s) 
{
  	while (*s != 0) 
	{
   		UART0_SendByte(*s++);
	}
}

/******************************************************************************
 *函数名： UART2_Init()
 *形参： void
 *返回值：void
 *功能： 按默认值初始化串口2的引脚和通讯参数。设置为8位数据位，1位停止位，无奇偶校验
 *调用函数：null
 *引用外部变量：null
 *****************************************************************************/
void UART2_Init (void)
{
	uint16_t usFdiv;
    /* UART2 */
    LPC_PINCON->PINSEL0 |= (1 << 20);             /* Pin P0.10 used as TXD2 (Com2) */
    LPC_PINCON->PINSEL0 |= (1 << 22);             /* Pin P0.11 used as RXD2 (Com2) */

   	LPC_SC->PCONP = LPC_SC->PCONP|(1<<24);	      /*打开UART2电源控制位	           */

    LPC_UART2->LCR  = 0x83;                       /* 允许设置波特率                */
    usFdiv = (FPCLK / 16) / UART2_BPS;            /* 设置波特率                    */
    LPC_UART2->DLM  = usFdiv / 256;
    LPC_UART2->DLL  = usFdiv % 256; 
    LPC_UART2->LCR  = 0x03;                       /* 锁定波特率                    */
    LPC_UART2->FCR  = 0x06;

    LPC_UART2->IER |= 0x0001;                  //使能 Receive Data Available interrupt 


}

/******************************************************************************
 *函数名： UART2_SendByte()
 *形参： uint8_t ucData 待发送的字符
 *返回值：uint8_t 0--失败  非零成功
 *功能： 将字符放入发送寄存器
 *调用函数：null
 *引用外部变量：null
 *****************************************************************************/
uint8_t UART2_SendByte (uint8_t ucData)
{
	while (!(LPC_UART2->LSR & 0x20));
    return (LPC_UART2->THR = ucData);
}

/******************************************************************************
 *函数名： UART2_GetChar()
 *形参： void
 *返回值：uint8_t  获取的字符
 *功能： 判断并读取字符
 *调用函数：null
 *引用外部变量：null
 *****************************************************************************/
uint8_t UART2_GetChar (void) 
{
  	while (!(LPC_UART2->LSR & 0x01));
  	return (LPC_UART2->RBR);
}

/******************************************************************************
 *函数名：UART0_SendString()
 *形参： uint8 *s --待发送的字符串，需要以\0结尾
 *返回值：void
 *功能： 发送字符串
 *调用函数：UART2_SendByte()
 *引用外部变量：null
 *****************************************************************************/
void UART2_SendString (uint8_t *s) 
{
  	while (*s != 0) 
	{
   		UART2_SendByte(*s++);
	}
}

/******************************************************************************
 *函数名： UART3_Init()
 *形参： void
 *返回值：void
 *功能： 按默认值初始化串口0的引脚和通讯参数。设置为8位数据位，1位停止位，无奇偶校验
 *调用函数：null
 *引用外部变量：null
 *****************************************************************************/
void UART3_Init (void)
{
	uint16_t usFdiv;
    /* UART2 */
    LPC_PINCON->PINSEL9 |= (3 << 24);             /* Pin P4.28 used as TXD3 (Com3) */
    LPC_PINCON->PINSEL9 |= (3 << 26);             /* Pin P4.29 used as RXD3 (Com3) */

	LPC_GPIO2->FIODIR = 0x00000100;               /* Pin P2.8  used as RS485 DIR   */ 
    LPC_GPIO2->FIOPIN &=  ~(0x00000100);		  /* 485_DIR = 0 */				  

   	LPC_SC->PCONP = LPC_SC->PCONP|(1<<25);	      /*打开UART3电源控制位	           */

    LPC_UART3->LCR  = 0x83;                       /* 允许设置波特率                */
    usFdiv = (FPCLK / 16) / UART3_BPS;            /* 设置波特率                    */
    LPC_UART3->DLM  = usFdiv / 256;
    LPC_UART3->DLL  = usFdiv % 256; 
    LPC_UART3->LCR  = 0x03;                       /* 锁定波特率                    */
    LPC_UART3->FCR  = 0x06;
}

/******************************************************************************
 *函数名： UART3_SendByte()
 *形参： uint8_t ucData 待发送的字符
 *返回值：uint8_t 0--失败  非零成功
 *功能： 将字符放入发送寄存器
 *调用函数：null
 *引用外部变量：null
 *****************************************************************************/
uint8_t UART3_SendByte (uint8_t ucData)
{
	
	while (!(LPC_UART3->LSR & 0x20));
	LPC_GPIO2->FIOPIN |=  0x00000100;		  /* 485_DIR = 1 打开发送*/	
    LPC_UART3->THR = ucData;
	while (!(LPC_UART3->LSR & 0x20));		  // 等待发送完成
	LPC_GPIO2->FIOPIN &=  ~(0x00000100);      /* 485_DIR = 0 打开接收*/	
	return (ucData);
}

/******************************************************************************
 *函数名： UART3_GetChar()
 *形参： void
 *返回值：uint8_t  获取的字符
 *功能： 判断并读取字符
 *调用函数：null
 *引用外部变量：null
 *****************************************************************************/
uint8_t UART3_GetChar (void) 
{
  	while (!(LPC_UART3->LSR & 0x01));
  	return (LPC_UART3->RBR);
}

/******************************************************************************
 *函数名：UART3_SendString()
 *形参： uint8 *s --待发送的字符串，需要以\0结尾
 *返回值：void
 *功能： 发送字符串
 *调用函数：UART3_SendByte()
 *引用外部变量：null
 *****************************************************************************/
void UART3_SendString (unsigned char *s) 
{
  	while (*s != 0) 
	{
   		UART3_SendByte(*s++);
	}
}
/******************************************************************************
 *函数名：UART2_IRQHandler()
 *形参： void
 *返回值：void
 *功能： 发UART2接收中断响应函数
 *调用函数：UART3_SendByte()
 *引用外部变量：null
 *****************************************************************************/
 uint8_t data = 0;
void  UART2_IRQHandler(void) 
{
   LPC_UART2->RBR;
                         
}

