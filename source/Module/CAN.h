/***************************************************************
*Copyright(c) 2016, Sojo
*保留所有权利
*文件名称:CAN.h
*文件标识:
*创建日期： 2016年11月8日 
*摘要:	 此文件用来初始化ARM自身 CAN模块
*当前版本:1.0
*作者: ZFREE
*取代版本:
*作者:
*完成时间:
************************************************************/
#ifndef __CAN_H
#define __CAN_H


#define STANDARD_FORMAT  0	 //标准帧
#define EXTENDED_FORMAT  1	 //扩展帧

#define DATA_FRAME       0	  //数据帧
#define REMOTE_FRAME     1	  //远程帧


#include "buffer.h"



/* Functions defined in module CAN.c */
void CAN_setup         (uint32_t ctrl);
void CAN_start         (uint32_t ctrl);
void CAN_waitReady     (uint32_t ctrl);
void CAN_wrMsg         (uint32_t ctrl, CAN_msg *msg);
void CAN_rdMsg         (uint32_t ctrl, CAN_msg *msg);
void CAN_wrFilter      (uint32_t ctrl, uint32_t id, uint8_t filter_type);

extern CAN_msg       CAN_TxMsg[2];      /* CAN messge for sending */
extern CAN_msg       CAN_RxMsg[2];      /* CAN message for receiving */                                
extern unsigned int  CAN_TxRdy[2];      /* CAN HW ready to transmit a message */
extern unsigned int  CAN_RxRdy[2];      /* CAN HW received a message */




#define CAN1 1
#define CAN2 2

#endif
