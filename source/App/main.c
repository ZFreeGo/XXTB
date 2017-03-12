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
//#define PLL0CFG_Val           0x00050063  MSEL0    M= 99  N= 5  Fcco = 400M
//#define CCLKCFG_Val           0x00000003   4       CPU时钟 100M = 400/4

volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
//__INLINE static void Delay (uint32_t dlyTicks) {
//  uint32_t curTicks;

//  curTicks = msTicks;
//  while ((msTicks - curTicks) < dlyTicks);
//}

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


/**
 * main主函数 
 */
int main (void) 
{
  uint16_t led1 = 0;
  uint16_t led2 = 1; 
  uint16_t i = 0;
  SystemInit();
  if (SysTick_Config(SystemCoreClock / 1000)) { /* Setup SysTick Timer for 100 msec interrupts  */
    while (1);                                  /* Capture error */
  }
  //init_timer( 0, TIME_INTERVAL ); // 10ms	
  //enable_timer( 0 );

  LedInit();                         
  CanInit();
 
  	//Led1On();
	//Led2Off();
	Led1Set(led1);
	Led2Set(led2);
	led1 = 1 - led1;
	
	//Led2On();
	//Led1Off();
//	Led1Set(0);
//	Led2Set(1);
	
   // delayMs(1, 500);

	CAN_TxMsg[1].data[0] = i++;
    CAN_TxMsg[1].data[0] = i++;
    CAN_TxMsg[1].id = 0x023;    
	CAN_TxMsg[1].len = 2;
	CAN_TxMsg[1].format = STANDARD_FORMAT;
	CAN_TxMsg[1].type = DATA_FRAME;
	CAN_waitReady (CAN2);   
    
    
    InitDeviceNet();//初始化DeviceNet
    
    
    
    while(1);
//	if (CAN_TxRdy[1])
//	{
//        led2 = 1 - led2;
//	    CAN_TxRdy[1] = 0;
//		CAN_wrMsg (CAN2, &CAN_TxMsg[1]);               /* transmit message */
//	}
    
  
  
}

