/*
 * File:   Action.h
 * Author: ZFreeGo
 *
 * Created on 2014年9月22日, 下午3:35
 */

#ifndef ACTION_H
#define	ACTION_H

#include "RtuFrame.h"
#include <stdint.h>



#define  RESET_MCU   0x01

#define      LED1_TOGLE  0x11
#define      LED2_TOGLE  0x12
#define      LED3_TOGLE  0x13
#define      LED4_TOGLE  0x14
#define      LED5_TOGLE  0x15
#define      LED6_TOGLE  0x16
#define      LED7_TOGLE  0x17
#define      LED8_TOGLE  0x18



#define      ACK  0xFA


#ifdef	__cplusplus
extern "C" {
#endif

void ExecuteFunctioncode(frameRtu* pRtu);

//void SendSampleData(void);


#ifdef	__cplusplus
}
#endif

#endif	/* ACTION_H */

