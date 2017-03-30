/* 
 * File:   RtuFrame.h
 * Author: LiDehai
 *
 * Created on 2014年12月4日, 上午7:27
 */

#ifndef __RTUFRAME_H
#define	__RTUFRAME_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define LOCAL_ADDRESS 0xA1
#define  BOOL uint8_t
    
#ifndef TRUE 
#define TRUE 0xff
#endif

#ifndef FALSE 
#define FALSE 0
#endif   
    
#define SEND_FRAME_LEN 64UL

typedef struct
{
    uint8_t address; //地址
    uint8_t funcode; //功能代码
    uint8_t datalen; //数据长度
    uint8_t* pData; //指向发送数据指针
    BOOL completeFlag;
} frameRtu;

BOOL FrameQueneIn(uint8_t recivData);
BOOL FrameQueneOut(uint8_t* pData);
void  ReciveFrameDataInit(void);
uint8_t ReciveBufferDataDealing(frameRtu* pJudgeFrame, frameRtu* pReciveFrame);

void  GenRTUFrame(uint8_t addr, uint8_t funcode,
                        uint8_t sendData[], uint8_t datalen, uint8_t* pRtuFrame, uint8_t *plen);

void SendFrame(uint8_t* pFrame, uint8_t len);


#ifdef	__cplusplus
}
#endif

#endif	/* RTUFRAME_H */

