/****************************************Copyright (c)****************************************************
**                                 http://www.PowerAVR.com
**								   http://www.PowerMCU.com
**--------------File Info---------------------------------------------------------------------------------
** File name:           timer.h
** Last modified Date:  2010-05-12
** Last Version:        V1.00
** Descriptions:        The main() function example template
**
**--------------------------------------------------------------------------------------------------------
** Created by:          PowerAVR
** Created date:        2010-05-10
** Version:             V1.00
** Descriptions:       编写示例代码
**
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#ifndef __TIMER_H 
#define __TIMER_H

	
#define TIME_INTERVAL	(9000000/100 - 1)

extern void delayMs(uint8_t timer_num, uint32_t delayInMs);
extern uint32_t init_timer( uint8_t timer_num, uint32_t timerInterval );
extern void enable_timer( uint8_t timer_num );
extern void disable_timer( uint8_t timer_num );
extern void reset_timer( uint8_t timer_num );
extern void TIMER0_IRQHandler (void);
extern void TIMER1_IRQHandler (void);

extern void SysTick_Handler(void);
extern uint8_t IsOverTime(uint32_t startTime, uint32_t delayTime);
extern void DelayMs (uint32_t dlyTicks) ;

extern volatile uint32_t g_MsTicks;   
#endif /* end __TIMER_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
