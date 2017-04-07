#include "buffer.h"

#define TRUE 0xFF
#define FALSE 0

/**
 * 缓冲数据入队
 * <p>
 * 
 * @param  pID      11bit ID号 
 * @param  pbuff    指向缓冲数据指针
 * @param  len      缓冲区数据长度
 * @return          <code>TRUE</code>   信息符合要求，进行处理
 *                  <code>FASLE</code>  信息不符合要求，未进行处理
 */
BOOL BufferEnqueue(FifoInformation* pInf, CAN_msg* )
{
    //队列未满
    if (ReciveBufferLen < FRAME_QUENE_LEN)
    {
        ReciveBufferLen++;
        ReciveBuffer[FifoEnd] = recivData; //入队
        FifoEnd =( FifoEnd + 1)% FRAME_QUENE_LEN;

        return TRUE;
    }
    return FALSE;
}
/**
 * 缓冲数据出队
 * <p>
 * 
 * @param  pID      11bit ID号 
 * @param  pbuff    指向缓冲数据指针
 * @param  len      缓冲区数据长度
 * @return          <code>TRUE</code>   信息符合要求，进行处理
 *                  <code>FASLE</code>  信息不符合要求，未进行处理
 */
BOOL BufferDequeue(uint8_t* pData)
{
    //队列是否为空
    //OFF_UART_INT();//防止接收读取冲突 应配对使用
    if (ReciveBufferLen > 0)
    {
        ReciveBufferLen--;
         *pData = ReciveBuffer[FifoHead]; //首先出队
        FifoHead =( FifoHead + 1)% FRAME_QUENE_LEN;

        //ON_UART_INT();;//防止接收读取冲突 应配对使用
        return TRUE;
    }
    //ON_UART_INT();;//防止接收读取冲突 应配对使用
    return FALSE;
}
