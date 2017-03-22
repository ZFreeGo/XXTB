/***************************************************************
*Copyright(c) 2016, Sojo
*保留所有权利
*文件名称:DeviceIO.h
*文件标识:
*创建日期： 2016年11月8日 
*摘要:	 此文件用来定义基本模块初始化的头文件，现在涉及的有LED控制
*当前版本:1.0
*作者: ZFREE
*取代版本:
*作者:
*完成时间:
************************************************************/
#ifndef _DEVICEIO_H
#define _DEVICEIO_H
#include <stdint.h>
#define SET_BIT(x, n) {(x) |= 1<<(n);}     //置x的n bit为 1
#define CLR_BIT(x, n) {(x) &= ~(1<<(n));}  //置x的n bit为 0


#define LED1_BIT 29  //P0_29
#define LED2_BIT 25  //P3_25


#define ON    0xFF
#define OFF   0x00
#define OUT   1
#define IN    0



/**********DATAn--外设对应关系
595_SHCP    -- DATA1 -- O
595_DS      -- DATA2 -- O
595_MR      -- DATA3 -- O
595_STCP    -- DATA4 -- O


165_SL      -- DATA5 -- O	
165_CLKINH 	-- DATA6 -- O
165_CLK     -- DATA7 -- O
165_DATA    -- DATA8 -- I

*****************************/

//595驱动引脚
#define SHCP595_BIT   26//DATA1  P1[26]
#define DS595_BIT     25//DATA2  P1[25]
#define MR595_BIT     24//DATA3  P1[24]  
#define STCP595_BIT   23//DATA4  P1[23]

//165驱动引脚
#define SL165_BIT     22//DATA5  P1[22]
#define CLKINH165_BIT 21//DATA6  P1[21] 
#define CLK165_BIT    20//DATA7  P1[20] 
#define DATA165_BIT   19//DATA8  P1[19]

/////////////////////////////////////////////////////////////////////////////////////////////
/**************************************
 * 以下程序段为实现LPC1768的IO口的位带操作
 * 以下宏定义问题在于不能选择IO口的功能
 * 现只实现了其作为普通IO口功能的控制
 **************************************/
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2)) 

#define MEM_ADDR(addr)  *((volatile unsigned long *) (addr))

#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

//IO口地址映射
//IO口功能选择寄存器
#define GPIO0_SEL0_ADDR    (LPC_PINCON_BASE + 0x00) //0x4002C00
#define GPIO0_SEL1_ADDR    (LPC_PINCON_BASE + 0x04) //0x4002C04

#define GPIO1_SEL2_ADDR    (LPC_PINCON_BASE + 0x08) //0x4002C08
#define GPIO1_SEL3_ADDR    (LPC_PINCON_BASE + 0x0C) //0x4002C0C

//输出寄存器
#define GPIO0_ODR_ADDR    (LPC_GPIO0_BASE + 0x18)   //0x2009C018
#define GPIO1_ODR_ADDR    (LPC_GPIO1_BASE + 0x18)   //0x2009C038
#define GPIO2_ODR_ADDR    (LPC_GPIO2_BASE + 0x18)   //0x2009C058
#define GPIO3_ODR_ADDR    (LPC_GPIO3_BASE + 0x18)   //0x2009C078
#define GPIO4_ODR_ADDR    (LPC_GPIO4_BASE + 0x18)   //0x2009C098

//输入寄存器
#define GPIO0_IDR_ADDR    (LPC_GPIO0_BASE + 0x14)    //0x2009C014 
#define GPIO1_IDR_ADDR    (LPC_GPIO1_BASE + 0x14)    //0x2009C034 
#define GPIO2_IDR_ADDR    (LPC_GPIO2_BASE + 0x14)    //0x2009C054 
#define GPIO3_IDR_ADDR    (LPC_GPIO3_BASE + 0x14)    //0x2009C074 
#define GPIO4_IDR_ADDR    (LPC_GPIO4_BASE + 0x14)    //0x2009C094 

//方向寄存器
#define GPIO0_DIR_ADDR    (LPC_GPIO0_BASE + 0x00) //0x2009C000
#define GPIO1_DIR_ADDR    (LPC_GPIO1_BASE + 0x00) //0x2009C020
#define GPIO2_DIR_ADDR    (LPC_GPIO2_BASE + 0x00) //0x2009C040
#define GPIO3_DIR_ADDR    (LPC_GPIO3_BASE + 0x00) //0x2009C060
#define GPIO4_DIR_ADDR    (LPC_GPIO4_BASE + 0x00) //0x2009C080 

//清零寄存器
#define GPIO0_CLS_ADDR    (LPC_GPIO0_BASE + 0x1C) //0x2009C01C
#define GPIO1_CLS_ADDR    (LPC_GPIO1_BASE + 0x1C) //0x2009C03C
#define GPIO2_CLS_ADDR    (LPC_GPIO2_BASE + 0x1C) //0x2009C05C
#define GPIO3_CLS_ADDR    (LPC_GPIO3_BASE + 0x1C) //0x2009C07C
#define GPIO4_CLS_ADDR    (LPC_GPIO4_BASE + 0x1C) //0x2009C09C


#define P0HIGH(n)  BIT_ADDR(GPIO0_ODR_ADDR,n)  //输出 
#define P0IN(n)    BIT_ADDR(GPIO0_IDR_ADDR,n)  //输入 
#define P0LOW(n)   BIT_ADDR(GPIO0_CLS_ADDR,n)  //清除  0:输出不变 1:输出0
#define P0DIR(n)   BIT_ADDR(GPIO0_DIR_ADDR,n)  //方向  0:输入1:输出

#define P1SEL1(n)  BIT_ADDR(GPIO1_SEL2_ADDR,n) 
#define P1SEL2(n)  BIT_ADDR(GPIO1_SEL3_ADDR,n) 

#define P1HIGH(n)  BIT_ADDR(GPIO1_ODR_ADDR,n)  //输出  0:输出不变 1:输出为1
#define P1IN(n)    BIT_ADDR(GPIO1_IDR_ADDR,n)  //输入 
#define P1LOW(n)   BIT_ADDR(GPIO1_CLS_ADDR,n)  //清除  0:输出不变 1:输出0
#define P1DIR(n)   BIT_ADDR(GPIO1_DIR_ADDR,n)  //方向  0:输入1:输出

#define P2HIGH(n)  BIT_ADDR(GPIO2_ODR_ADDR,n)  //输出  0:输出不变 1:输出为1
#define P2IN(n)    BIT_ADDR(GPIO2_IDR_ADDR,n)  //输入
#define P2LOW(n)   BIT_ADDR(GPIO2_CLS_ADDR,n)  //清除  0:输出不变 1:输出0
#define P2DIR(n)   BIT_ADDR(GPIO2_DIR_ADDR,n)  //方向  0:输入1:输出

#define P3HIGH(n)  BIT_ADDR(GPIO3_ODR_ADDR,n)  //输出 
#define P3IN(n)    BIT_ADDR(GPIO3_IDR_ADDR,n)  //输入 
#define P3LOW(n)   BIT_ADDR(GPIO3_CLS_ADDR,n)  //清除  0:输出不变 1:输出0
#define P3DIR(n)   BIT_ADDR(GPIO3_DIR_ADDR,n)  //方向  0:输入1:输出

#define P4HIGH(n)  BIT_ADDR(GPIO4_ODR_ADDR,n)  //输出 
#define P4IN(n)    BIT_ADDR(GPIO4_IDR_ADDR,n)  //输入
#define P4LOW(n)   BIT_ADDR(GPIO4_CLS_ADDR,n)  //清除  0:输出不变 1:输出0
#define P4DIR(n)   BIT_ADDR(GPIO4_DIR_ADDR,n)  //方向  0:输入1:输出


////////////////////////////////////////
/*****************************
 * 以下宏定义为485通讯定义
 *****************************/ 
#define RS485_RD_DIR    P1DIR(29)

#define TX_MODE()   {P1HIGH(29) = ON;}
#define RX_MODE()   {P1LOW(29) = ON;}

////////////////////////////////////////

void LedInit(void);
void Led1Set(uint8_t state);
void Led2Set(uint8_t state);
void Led1On(void);
void Led2On(void);
void Led1Off(void);
void Led2Off(void);

void InitDeviceIO(void);


#define RS485_RD_DIR    P1DIR(29)

#define TX_MODE()   {P1HIGH(29) = ON;}
#define RX_MODE()   {P1LOW(29) = ON;}



#endif


