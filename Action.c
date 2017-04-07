#include "Action.h"
#include "RS485.h"
#include "DeviceNet.h"
#include "Header.h"
#include "string.h"

uint8_t SendDataBuffer[64] = {0};


static void FrameServer(uint8_t* pData, uint8_t  len, uint8_t mac_id);
static void MasterStationServer(PointUint8* pData);

static float testA;
static uint16_t testB;


typedef struct TagConfigData
{
    uint8_t ID; //ID号
    void* pData;//指向数据
    uint8_t type;//类型  
    void (*fSetValue)(PointUint8*, struct TagConfigData* )  ;
    void (*fGetValue)(PointUint8*, struct TagConfigData* )  ;
}ConfigData;

void SetValue(PointUint8* pData, ConfigData* pConfig)
{
    
    if ( pConfig->type == 0x45)
    {
        if (pData->len >=  4)
        {
            
            uint32_t data1 = pData->pData[1];
            uint32_t data2 = pData->pData[2];
            uint32_t data3 = pData->pData[3];
            uint32_t data = (data3 << 24) | (data2 << 16) | (data1 << 8)|pData->pData[0] ;
            float result =  (float)data * 0.00001;
            *(float*)pConfig->pData = result;
           
        }
    }
    if (pConfig->type == 0x20)
    {
        if (pData->len >=  2)
        {
            
            uint16_t data1 = pData->pData[1];
           
            uint16_t data =  (data1 << 8)|pData->pData[0] ;
          
            *(uint16_t*)pConfig->pData = data;
            
        }
    }
}

void GetValue(PointUint8* pData, ConfigData* pConfig)
{
    if ( pConfig->type == 0x45)
    {
        if (pData->len >=  4)
        {          
            uint32_t result = (uint32_t)(*(float*)pConfig->pData * 100000);
            pData->pData[0] = (uint8_t)(result & 0x00FF);
            pData->pData[1] = (uint8_t)(result >> 8) ;
            pData->pData[2] = (uint8_t)(result >> 16) ;
            pData->pData[3] = (uint8_t)(result >> 24) ;
             pData->len = 4;
            
        }
    }
    if (pConfig->type == 0x20)
    {
        if (pData->len >=  2)
        {          
            uint16_t data =   *(uint16_t*)pConfig->pData;
            pData->pData[0] = (uint8_t)(data & 0x00FF);
            pData->pData[1] = (uint8_t)(data >> 8) ;
           pData->len = 2;
        }
    }
}

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
                
                
                
                //MasterStationServer(&Point);
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
    //testA = 1.23456;
    //testB = 0xAA55;
    ConfigData configCollect[2];
    configCollect[0].ID = 1;
    configCollect[0].pData= &testA;
    configCollect[0].type = 0x45;
    configCollect[0].fGetValue = &GetValue;
    configCollect[0].fSetValue = &SetValue;
    
      
    configCollect[1].ID = 2;
    configCollect[1].pData= &testB;
    configCollect[1].type = 0x20;
    configCollect[1].fGetValue = &GetValue;
    configCollect[1].fSetValue = &SetValue;
    
    //至少大于等于3，才能包含必要的数据
    if (pData->len < 1) 
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
        case 0x11: //参数设置--非顺序
        {     
            if(pData->len > 2)
            {      
                                 
            for(uint8_t i = 0; i < 2; i++)
            {
                  
                if (pData->pData[1] == configCollect[i].ID)
                {
                   pData->pData += 2;
                   pData->len -= 2; 
                    configCollect[i].fSetValue(pData, configCollect + i);
                }
            }
        }
            break;
        }
        case 0x13: //参数读取--顺序
        {     
                
            if(pData->len >= 1)
            {                
                for(uint8_t i = 0; i < 2; i++)
                {
                    if (pData->pData[1] == configCollect[i].ID)
                    {                       
                        
                        uint8_t data[8] = {0};
                         PointUint8 Point;
                        Point.pData = data;
                         Point.len = 8;
                        configCollect[i].fGetValue(&Point, configCollect + i);
                        
                        USINT sendData[16] = {0};
                        USINT tempData[16] = {0};
                        USINT datalen = 0;
                        tempData[0] = MacList[LOCAL_INDEX];//添加上传信息
                        tempData[1] = 0xAA;
                        memcpy(tempData + 2, Point.pData , Point.len);//拷贝数据，要求源地址与目的地址范围没有冲突。
                        GenRTUFrame(UP_ADDRESS, CAN_MESSAGE_TO_UP, tempData, Point.len + 2,sendData, &datalen);
                        SendFrame(sendData, datalen);  
                        
                    }
                }
            }
            break;
        }
        default:
        {
            break;
        }
            
    }
    
}


