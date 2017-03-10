/***************************************************************
*Copyright(c) 2016, Sojo
*��������Ȩ��
*�ļ�����:DeviceIO.h
*�ļ���ʶ:
*�������ڣ� 2016��11��8�� 
*ժҪ:	 ���ļ������������ģ���ʼ����ͷ�ļ��������漰����LED����
*��ǰ�汾:1.0
*����: ZFREE
*ȡ���汾:
*����:
*���ʱ��:
************************************************************/
#ifndef _DEVICEIO_H
#define _DEVICEIO_H
#include <stdint.h>
#define SET_BIT(x, n) {(x) |= 1<<(n);}     //��x��n bitΪ 1
#define CLR_BIT(x, n) {(x) &= ~(1<<(n));}  //��x��n bitΪ 0


#define LED1_BIT 29  //P0_29
#define LED2_BIT 25  //P3_25


/**********DATAn--�����Ӧ��ϵ
595_SHCP    -- DATA1 -- O
595_DS      -- DATA2 -- O
595_MR      -- DATA3 -- O
595_STCP    -- DATA4 -- O


165_SL      -- DATA5 -- O	
165_CLKINH 	-- DATA6 -- O
165_CLK     -- DATA7 -- O
165_DATA    -- DATA8 -- I

*****************************/

//595��������
#define SHCP595_BIT   26//DATA1  P1[26]
#define DS595_BIT     25//DATA2  P1[25]
#define MR595_BIT     24//DATA3  P1[24]  
#define STCP595_BIT   23//DATA4  P1[23]

//165��������
#define SL165_BIT     22//DATA5  P1[22]
#define CLKINH165_BIT 21//DATA6  P1[21] 
#define CLK165_BIT    20//DATA7  P1[20] 
#define DATA165_BIT   19//DATA8  P1[19]


void LedInit(void);
void Led1Set(uint8_t state);
void Led2Set(uint8_t state);
void Led1On(void);
void Led2On(void);
void Led1Off(void);
void Led2Off(void);
#endif


