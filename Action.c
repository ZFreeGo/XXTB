#include "Action.h"
#include "RS485.h"
#include "DeviceNet.h"


uint8_t SendDataBuffer[64] = {0};


/**
 * 执行功能代码
 *
 * @param   pRtu 指向RTU帧信息的指针
 *
 * @bref   对完整帧进行提取判断
 */
void ExecuteFunctioncode(frameRtu* pRtu)
{
    if (pRtu->completeFlag == TRUE)
    {
      
        
        switch(pRtu->funcode)
        {
            case 0:
            {
                
                break;
            }
            
            default :
             {
                 uint8_t len = 0;
                 GenRTUFrame(0x1A, pRtu->funcode, pRtu->pData + 3, pRtu->datalen,SendDataBuffer, &len);
                 SendFrame(SendDataBuffer, len);                 
                 break;
             }
        }

        pRtu->completeFlag = FALSE;
    }
}


/**
 * 引用帧服务
 *
 * @param  指向处理帧信息内容的指针
 *
 * @param  数据信息长度
 *
 * @param  mac id
 *
 * @bref   对完整帧进行提取判断
 */
void FrameServer(uint8_t* pData, uint8_t  len, uint8_t mac_id)
{
    if(len >= 3) //至少3个字节
    {
    switch(pData[0])
    {

        case 1://合闸预制
        case 2://合闸同步
        case 3://分闸预制
        case 4://分闸同步
        {
            if(len >= 3) //至少3个字节
            {
                DeviceNetSendIOData(StationList, pData, len);//发送IO报文
            }
            break;
        }
        case 5://同步合闸预制
        {
            break;
        }
        case 6://同步分闸预制
        {
            break;
        }
    }
}
}


