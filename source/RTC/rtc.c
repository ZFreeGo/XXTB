/***************************************************************
*Copyright(c) 2016, Sojo
*��������Ȩ��
*�ļ�����:rtc.c
*�ļ���ʶ:
*�������ڣ� 2016��11��8�� 
*ժҪ:	 ������ʼ��RTCģ��
*��ǰ�汾:1.0
*����: ZFREE
*ȡ���汾:
*����:
*���ʱ��:
************************************************************/
#include "lpc17xx.h"
#include "rtc.h"

volatile uint32_t alarm_on = 0;

/******************************************************************************
 *�������� RTC_IRQHandler()
 *�βΣ� void
 *����ֵ��void
 *���ܣ�  RTC�ж�
 *���ú����� null
 *�����ⲿ������ null
 *****************************************************************************/
void RTC_IRQHandler (void) 
{  
  LPC_RTC->ILR |= ILR_RTCCIF;		/* clear interrupt flag */
  alarm_on = 1;
  return;
}

/******************************************************************************
 *�������� RTCInit()
 *�βΣ� void
 *����ֵ��void
 *���ܣ�  RTC�ж�
 *���ú����� null
 *�����ⲿ������ null
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
 *�������� RTCStart()
 *�βΣ� void
 *����ֵ��void
 *���ܣ�  ����RTC
 *���ú����� null
 *�����ⲿ������ null
 *****************************************************************************/
void RTCStart( void ) 
{
  /*--- Start RTC counters ---*/
  LPC_RTC->CCR |= CCR_CLKEN;
  LPC_RTC->ILR = ILR_RTCCIF;
  return;
}

/******************************************************************************
 *�������� RTCStop()
 *�βΣ� void
 *����ֵ��void
 *���ܣ�  ֹͣRTC
 *���ú����� null
 *�����ⲿ������ null
 *****************************************************************************/
void RTCStop( void )
{   
  /*--- Stop RTC counters ---*/
  LPC_RTC->CCR &= ~CCR_CLKEN;
  return;
} 

/******************************************************************************
 *�������� RTC_CTCReset()
 *�βΣ� void
 *����ֵ��void
 *���ܣ�  ��λRTC
 *���ú����� null
 *�����ⲿ������ null
 *****************************************************************************/
void RTC_CTCReset( void )
{   
  /*--- Reset CTC ---*/
  LPC_RTC->CCR |= CCR_CTCRST;
  return;
}

/******************************************************************************
 *�������� RTCSetTime()
 *�βΣ� RTCTime Time -- ���õ�ʱ��
 *����ֵ��void
 *���ܣ�  ���õ�ʱ��
 *���ú����� null
 *�����ⲿ������ null
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
 *�������� RTCSetAlarm()
 *�βΣ� RTCTime Alarm -- ���õı���ʱ��
 *����ֵ��void
 *���ܣ�  ���õı���ʱ��
 *���ú����� null
 *�����ⲿ������ null
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
 *�������� RTCGetTime()
 *�βΣ� void
 *����ֵ��RTCTime -- ��ȡ��ʱ��
 *���ܣ�  ��ȡRTCʱ��
 *���ú����� null
 *�����ⲿ������ null
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
 *�������� RTCSetAlarmMask()
 *�βΣ� uint32_t AlarmMask -- RTC������
 *����ֵ��void
 *���ܣ�  
 *���ú����� null
 *�����ⲿ������ null
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

