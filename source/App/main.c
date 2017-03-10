/***************************************************************
*Copyright(c) 2016, Sojo
*保留所有权利
*文件名称:Main.c
*文件标识:
*创建日期： 2016年11月8日 
*摘要:
*当前版本:1.0
*作者: ZFREE
*取代版本:
*作者:
*完成时间:
************************************************************/

#include "LPC17xx.h"
#include<math.h>

#include "DeviceIO.h"
#include "CAN.h"
volatile uint32_t msTicks;                            /* counts 1ms timeTicks */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*------------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *------------------------------------------------------------------------------*/
__INLINE static void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

 /*----------------------------------------------------------------------------
  initialize CAN interface
 *----------------------------------------------------------------------------*/
void CanInit (void) 
{

 // CAN_setup (1);                                /* setup CAN Controller #1 */

  CAN_setup (1);                                  /* setup CAN Controller #2 */
  //LPC_CANAF->AFMR = 0x40000000;  
  CAN_wrFilter(1,  33, STANDARD_FORMAT);          /* Enable recep                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          tion of messages */

 // CAN_start (1);                                  /* start CAN Controller #2 */
  CAN_start (1);                                  /* start CAN Controller #2 */

 // CAN_waitReady (1);                              /* wait til tx mbx is empty */
  CAN_waitReady (1);                              /* wait til tx mbx is empty */


}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) 
{
  uint16_t led1 = 0;
  uint16_t led2 = 1; 
  uint16_t i = 0;
  SystemInit();
  if (SysTick_Config(SystemCoreClock / 1000)) { /* Setup SysTick Timer for 1 msec interrupts  */
    while (1);                                  /* Capture error */
  }
  
  LedInit();                         
  CanInit();
  while(1) 
  {
  	//Led1On();
	//Led2Off();
	Led1Set(led1);
	Led2Set(led2);
	led1 = 1 - led1;
	led2 = 1 - led2;
	//Led2On();
	//Led1Off();
//	Led1Set(0);
//	Led2Set(1);
	Delay (100);



                       /* initialise message to send */
//	for (i = 0; i < 8; i++) 
	CAN_TxMsg[0].data[0] = i++;

    CAN_TxMsg[0].id = 0x021;    
	CAN_TxMsg[0].len = 1;
	CAN_TxMsg[0].format = STANDARD_FORMAT;
	CAN_TxMsg[0].type = DATA_FRAME;
	 CAN_waitReady (1);   

	if (CAN_TxRdy[0])
	{
	    CAN_TxRdy[0] = 0;
		CAN_wrMsg (1, &CAN_TxMsg[0]);               /* transmit message */
	}
   
  }

       
  
}

