#include "Action.h"
#include "RS485.h"
#include "DeviceNet.h"
#include "Header.h"

uint8_t SendDataBuffer[64] = {0};


static void FrameServer(uint8_t* pData, uint8_t  len, uint8_t mac_id);
static void MasterStationServer(PointUint8* pData);
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
      
        uint8_t len = 0;
        GenRTUFrame(UP_ADDRESS, pRtu->funcode, pRtu->pData + 3, pRtu->datalen,SendDataBuffer, &len);
        SendFrame(SendDataBuffer, len);     
        switch(pRtu->funcode)
        {
            case CAN_MESSAGE_TO_DOWN://CAN转发包
            {
               FrameServer( pRtu->pData + 3, pRtu->datalen, 0);
               break;
            }
            case DOWN_CONCTROL://控制ARM部分
            {
                PointUint8 Point;
                Point.pData = pRtu->pData + 3;
                Point.len = pRtu->datalen;
                
                MasterStationServer(&Point);
                break;
            }            
            default :
             {
                            
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
 * @param  数据信息长度
 * @param  mac id
 *
 * @brief   对完整帧进行提取判断
 */
static void FrameServer(uint8_t* pData, uint8_t  len, uint8_t mac_id)
{
    if(len >= 3) //至少1+2个字节//为MAC地址
    {
        
        struct DefStationElement* pStation = GetStationPoint(pData[0]);
        if (pStation != 0)
        {        
            len = len - 2;//仅考虑有效数据
            pData = pData + 2;//2bytes 偏移
            
            switch(pData[0])//0 1 2
            {
                case 1://合闸预制
                case 2://合闸同步
                case 3://分闸预制
                case 4://分闸同步
                {
                    if(len >= 3) //至少3个字节
                    {
                        DeviceNetSendIOData(pStation, pData, len);//发送IO报文
                    }
                    break;
                }
                case 5://同步合闸预制
                {
                    if(len >= 2) //至少2个字节
                    {
                        DeviceNetSendIOData(pStation, pData, len);//发送IO报文
                    }
                    break;
                }
                default://直接转发
                {
                    if(len <= 8)//小于8直接转发
                    {
                        DeviceNetSendIOData(pStation, pData, len);//发送IO报文
                    }
                    break;
                }
               
            }
        }
    }
}

/**
 * 主站服务
 *
 * @param  指向数据包内容
 * @param  数据信息长度
 * @param  mac id
 *
 * @brief   对完整帧进行提取判断
 */
static void MasterStationServer(PointUint8* pData)
{
    if (pData->len == 0)
    {
        return;
    }
    uint8_t function = pData->pData[0];
    switch(function)
    {
        case 1://重新所有连接建立
        {
            if (pData->len == 2)
            {
                RestartEstablishLink( pData->pData[1]);
            }
            break;
        }
        case 0x40://复位ARM
        {
            NVIC_SystemReset();
            break;
        }
    }
    
}


