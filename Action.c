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
        else if (pData[0] == MacList[LOCAL_INDEX]) //ARM控制
            
        {
             PointUint8 Point;
             Point.pData = pData + 2; //去除2字节地址，移到有效数据位置
             Point.len = len - 2; // 修正长度             
             MasterStationServer(&Point);
   
        }
       
    }
}

/**
*发送命令数组,临时存储区
 */
static uint8_t CommandArray[8] = {0};
static PointUint8  SendCommand;

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
    //至少大于等于3，才能包含必要的数据
    if (pData->len < 3) 
    {
        return;
    }
   
    uint8_t function = pData->pData[0];
    
    switch(function)
    {
        case 1: //复位ARM
        {
            NVIC_SystemReset();
            break;
        }
        case 2://重新所有连接建立
        {
            if (pData->len == 4)
            {
                RestartEstablishLink( pData->pData[3]);
            }
            break;
        }
        case 0x20://执行同步命令
       
        {
            pData->pData = pData->pData + 3;//移除功能码,2字节地址，余下部分为完整命令
            pData->len = pData->len - 3; //修正长度
            if (pData->len >= 3) //剩余命令至少大于等于3
            {
                uint8_t i =0;
                CommandArray[i++] = 0x05;
                CommandArray[i++] = 0x00;
                //CommandArray[i++] = 0x00;
                //CommandArray[i++] = 0x00;
                SendCommand.pData = CommandArray;
                SendCommand.len = i;
                SynchronousOperationReady(pData, &SendCommand);
            }            
            break;
        }
        case 0x21: //执行同步执行命令
        {
            pData->pData = pData->pData + 3;//移除功能码,2字节地址，余下部分为完整命令
            pData->len = pData->len - 3; //修正长度
            if (pData->len >= 3) //剩余命令至少大于等于3
            {
                SynchronousOperationAction(pData, 0);
            }     
            break;            
        }
        default:
        {
            break;
        }
            
    }
    
}


