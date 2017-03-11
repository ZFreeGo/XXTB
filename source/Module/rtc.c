/***************************************************************
*Copyright(c) 2016, Sojo
*保留所有权利
*文件名称:rtc.c
*文件标识:
*创建日期： 2016年11月8日 
*摘要:	 用来初始化RTC模块
*当前版本:1.0
*作者: ZFREE
*取代版本:
*作者:
*完成时间:
************************************************************/
#include "lpc17xx.h"
#include "rtc.h"

volatile uint32_t alarm_on = 0;

/******************************************************************************
 *函数名： RTC_IRQHandler()
 *形参： void
 *返回值：void
 *功能：  RTC中断
 *调用函数： null
 *引用外部变量： null
 *****************************************************************************/
void RTC_IRQHandler (void) 
{  
  LPC_RTC->ILR |= ILR_RTCCIF;		/* clear interrupt flag */
  alarm_on = 1;
  return;
}

/******************************************************************************
 *函数名： RTCInit()
 *形参： void
 *返回值：void
 *功能：  RTC中断
 *调用函数： null
 *引用外部变量： null
 *****************************************************************************/
void RTCInit( void )
{
  alarm_on = 0;

  /* Enable CLOCK into RTC */
  LPC_SC->PCONP |= (1 << 9);

  /* If RTC is stopped, clear STOP bit. */
  if ( LPC_RTC->RTC_AUX & (0x1<<4) )
  {
	LPC_RTC->RTC_AUX |= (0x1<<4);	
  }
  
  /*--- Initialize registers ---*/    
  LPC_RTC->AMR = 0;
  LPC_RTC->CIIR = 0;
  LPC_RTC->CCR = 0;
  return;
}

/******************************************************************************
 *函数名： RTCStart()
 *形参： void
 *返回值：void
 *功能：  启动RTC
 *调用函数： null
 *引用外部变量： null
 *****************************************************************************/
void RTCStart( void ) 
{
  /*--- Start RTC counters ---*/
  LPC_RTC->CCR |= CCR_CLKEN;
  LPC_RTC->ILR = ILR_RTCCIF;
  return;
}

/******************************************************************************
 *函数名： RTCStop()
 *形参： void
 *返回值：void
 *功能：  停止RTC
 *调用函数： null
 *引用外部变量： null
 *****************************************************************************/
void RTCStop( void )
{   
  /*--- Stop RTC counters ---*/
  LPC_RTC->CCR &= ~CCR_CLKEN;
  return;
} 

/******************************************************************************
 *函数名： RTC_CTCReset()
 *形参： void
 *返回值：void
 *功能：  复位RTC
 *调用函数： null
 *引用外部变量： null
 *****************************************************************************/
void RTC_CTCReset( void )
{   
  /*--- Reset CTC ---*/
  LPC_RTC->CCR |= CCR_CTCRST;
  return;
}

/******************************************************************************
 *函数名： RTCSetTime()
 *形参： RTCTime Time -- 设置的时间
 *返回值：void
 *功能：  设置的时间
 *调用函数： null
 *引用外部变量： null
 *****************************************************************************/
void RTCSetTime( RTCTime Time ) 
{
  LPC_RTC->SEC = Time.RTC_Sec;
  LPC_RTC->MIN = Time.RTC_Min;
  LPC_RTC->HOUR = Time.RTC_Hour;
  LPC_RTC->DOM = Time.RTC_Mday;
  LPC_RTC->DOW = Time.RTC_Wday;
  LPC_RTC->DOY = Time.RTC_Yday;
  LPC_RTC->MONTH = Time.RTC_Mon;
  LPC_RTC->YEAR = Time.RTC_Year;    
  return;
}


/******************************************************************************
 *函数名： RTCSetAlarm()
 *形参： RTCTime Alarm -- 设置的报警时间
 *返回值：void
 *功能：  设置的报警时间
 *调用函数： null
 *引用外部变量： null
 *****************************************************************************/
void RTCSetAlarm( RTCTime Alarm ) 
{   
  LPC_RTC->ALSEC = Alarm.RTC_Sec;
  LPC_RTC->ALMIN = Alarm.RTC_Min;
  LPC_RTC->ALHOUR = Alarm.RTC_Hour;
  LPC_RTC->ALDOM = Alarm.RTC_Mday;
  LPC_RTC->ALDOW = Alarm.RTC_Wday;
  LPC_RTC->ALDOY = Alarm.RTC_Yday;
  LPC_RTC->ALMON = Alarm.RTC_Mon;
  LPC_RTC->ALYEAR = Alarm.RTC_Year;    
  return;
}

/******************************************************************************
 *函数名： RTCGetTime()
 *形参： void
 *返回值：RTCTime -- 获取的时间
 *功能：  获取RTC时间
 *调用函数： null
 *引用外部变量： null
 *****************************************************************************/
RTCTime RTCGetTime( void ) 
{
  RTCTime LocalTime;
    
  LocalTime.RTC_Sec = LPC_RTC->SEC;
  LocalTime.RTC_Min = LPC_RTC->MIN;
  LocalTime.RTC_Hour = LPC_RTC->HOUR;
  LocalTime.RTC_Mday = LPC_RTC->DOM;
  LocalTime.RTC_Wday = LPC_RTC->DOW;
  LocalTime.RTC_Yday = LPC_RTC->DOY;
  LocalTime.RTC_Mon = LPC_RTC->MONTH;
  LocalTime.RTC_Year = LPC_RTC->YEAR;
  return ( LocalTime );    
}

/******************************************************************************
 *函数名： RTCSetAlarmMask()
 *形参： uint32_t AlarmMask -- RTC屏蔽码
 *返回值：void
 *功能：  
 *调用函数： null
 *引用外部变量： null
 *****************************************************************************/
void RTCSetAlarmMask( uint32_t AlarmMask ) 
{
  /*--- Set alarm mask ---*/    
  LPC_RTC->AMR = AlarmMask;
  return;
}

/*****************************************************************************
**                            End Of File
******************************************************************************/

