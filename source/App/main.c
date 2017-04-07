/**************************************************************************//**
 * @file     main.c
 * @brief    选相合闸主程序
 * @version  V0.01
 * @date     2017/3/11
 *
 * @note
 * Copyright (C) 2017 ARM Beijing Sojo. All rights reserved.
 *
 * @par
 *
 * @par
 *
 ******************************************************************************/

#include "LPC17xx.h"
#include<math.h>

#include "DeviceIO.h"
#include "CAN.h"
#include "timer.h"
#include "DeviceNet.h"
#include "RS485.h"
#include "RtuFrame.h"
#include "Action.h"
#include "NetApp.h"


//#define PLL0CFG_Val           0x00050063  MSEL0    M= 99  N= 5  Fcco = 400M
//#define CCLKCFG_Val           0x00000003   4       CPU时钟 100M = 400/4
#define UART2_BPS   57600    //UART2的波特率


 /*----------------------------------------------------------------------------
  initialize CAN interface
 *----------------------------------------------------------------------------*/
void CanInit (void) 
{

 // CAN_setup (1);                                /* setup CAN Controller #1 */

  CAN_setup (CAN2);                                  /* setup CAN Controller #2 */
 
  CAN_wrFilter(CAN2,  33, STANDARD_FORMAT);          /* Enable recep   */                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
  LPC_CANAF->AFMR |= 0x00000002;       //接收滤波器处于旁路模式             
  CAN_start (CAN2);                                  /* start CAN Controller #2 */
  CAN_waitReady (CAN2);                              /* wait til tx mbx is empty */
}
frameRtu sendFrame, recvFrame;

CAN_msg ReciveMsg;
/**
 * main主函数 
 */
int main (void) 
{

    uint16_t led1 = 0;
    uint16_t led2 = 1; 
    uint16_t result = 0;
    SystemInit();
    if (SysTick_Config(SystemCoreClock / 1000)) 
    {                                               /* Setup SysTick Timer for 100 msec interrupts  */
        while (1);                                  /* Capture error */
    }

    InitDeviceIO();
    Init_UART2(UART2_BPS);                          
    CanInit();
    ReciveFrameDataInit(); //接收帧初始化
    sendFrame.address =  LOCAL_ADDRESS; //本机接收地址处理
  
	Led1Set(led1);
	Led2Set(led2);
	led1 = 1 - led1;
	
    InitDeviceNet();//初始化DeviceNet
    InitNetApp();
  
    while(1)
    {
        MainDeviceNetTask();
        result = ReciveBufferDataDealing(&sendFrame, &recvFrame);//返回剩余长度
        if (recvFrame.completeFlag == TRUE)
        {
            ExecuteFunctioncode(&recvFrame);
        }
        result = BufferDequeue(&ReciveMsg);
        if (result)
        {
            DeviceNetReciveCenter(&(ReciveMsg.id), ReciveMsg.data, ReciveMsg.len);
        }
                
        CheckDeviceNetWorkMode();    
    }    
   // while(1);         
}

