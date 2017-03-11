/***************************************************************
*Copyright(c) 2016, Sojo
*��������Ȩ��
*�ļ�����:CAN.h
*�ļ���ʶ:
*�������ڣ� 2016��11��8�� 
*ժҪ:	 ���ļ�������ʼ��ARM���� CANģ��
*��ǰ�汾:1.0
*����: ZFREE
*ȡ���汾:
*����:
*���ʱ��:
************************************************************/
#ifndef __CAN_H
#define __CAN_H

#define STANDARD_FORMAT  0	 //��׼֡
#define EXTENDED_FORMAT  1	 //��չ֡

#define DATA_FRAME       0	  //����֡
#define REMOTE_FRAME     1	  //Զ��֡

typedef struct  {
  unsigned int   id;                    /* 29 bit identifier */
  unsigned char  data[8];               /* Data field */
  unsigned char  len;                   /* Length of data field in bytes */
  unsigned char  format;                /* 0 - STANDARD, 1- EXTENDED IDENTIFIER */
  unsigned char  type;                  /* 0 - DATA FRAME, 1 - REMOTE FRAME */
} CAN_msg;

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
