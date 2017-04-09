/**
 * Copyright ? BeiJing SOJO Electric Co.,Ltd. 2001-2016. All rights reserved.
 * File name: DriverIO.c
 * Author: 
 * Description: 
 * Others: 
 * History: 
 * 
 * @author  ZhangYufei
 * @version 0.01
 */
#include <LPC17xx.H> 
#include <string.h>

#include "DeviceNet.h"
#include "can.h"
#include "timer.h"
#include "RTuframe.h"
#include "Header.h"
#include "NetApp.h"

#define STEP_START        0xA1 //启动状态,上电初始状态
#define STEP_LINKING      0xA2 //正在建立连接中
#define STEP_STATUS_CHANGE        0xA4 //STATUS CHANGE模式
#define STEP_CYCLE        0xA8 //循环模式





BOOL IsTimeRemain(void);    //需要根据具体平台改写
void StartOverTimer(void);//需要根据具体平台改写

void ResponseMACID(struct DefFrameData* pSendFrame, BYTE config);
void InitYongciAData(void);
static BOOL MakeUnconnectVisibleRequestMessageOnlyGroup2(struct DefFrameData* pFrame, 
    BYTE destMAC,BYTE serverCode, BYTE config);
static void EstablishConnection(struct DefStationElement* pStation, USINT connectType);
static void SlaveStationStatusCycleService(WORD* pID, BYTE* pbuff, BYTE len);
static void SlaveStationStatusChangeService(WORD* pID, BYTE* pbuff, BYTE len);

////////////////////////////////////////////////////////////可考虑存入EEPROM
UINT  providerID = 0X1234;               // 供应商ID 
UINT  device_type = 0;                   // 通用设备
UINT  product_code = 0X00d2;             // 产品代码
USINT  major_ver = 0X01;
USINT  minor_ver = 0X01;                 // 版本
UDINT  serialID = 0x001169BC;            // 序列号
SHORT_STRING  product_name = {8, (unsigned char *)"Master"};// 产品名称
//////////////////////////////////////////////////////

////////////////////连接对象变量////////////////////////////////
struct DefConnectionObj  CycleInquireConnedctionObj;//循环IO响应
struct DefConnectionObj  VisibleConnectionObj;   //显示连接
//////////////////DeviceNet对象变量////////////////////////////
struct DefDeviceNetClass  DeviceNet_class = {2}; //
struct DefDeviceNetObj  DeviceNetObj;
struct DefIdentifierObject  IdentifierObj; 
/////////////////////////////////////////////////////////////////

BYTE  SendBufferData[10];//接收缓冲数据
BYTE  ReciveBufferData[10];//接收缓冲数据

struct DefStationElement MasterStation;  //主站




static BYTE  ReceiveDataBuffer0[10] = {0}; //接收缓冲数据
static BYTE  SendDataBuffer0[10] = {0};    //发送缓冲数据
static BYTE  ReceiveDataBuffer1[10] = {0}; //接收缓冲数据
static BYTE  SendDataBuffer1[10] = {0};    //发送缓冲数据
static BYTE  ReceiveDataBuffer2[10] = {0}; //接收缓冲数据
static BYTE  SendDataBuffer2[10] = {0};    //发送缓冲数据
static BYTE  ReceiveDataBuffer3[10] = {0}; //接收缓冲数据
static BYTE  SendDataBuffer3[10] = {0};    //发送缓冲数据
static BYTE  ReceiveDataBuffer4[10] = {0}; //接收缓冲数据
static BYTE  SendDataBuffer4[10] = {0};    //发送缓冲数据
static BYTE  ReceiveDataBuffer5[10] = {0}; //接收缓冲数据
static BYTE  SendDataBuffer5[10] = {0};    //发送缓冲数据


/**
 *  列表中对应的子站列表
 */
 struct DefStationElement StationList[STATION_COUNT];

/**
 * MAC地址合集,小于STATION_COUNT 为子站，等于STATION_COUNT为本机
 */
uint8_t MacList[STATION_COUNT + 1];

/**
 * 当前处理所指向的子站指针
 */
struct DefStationElement*    g_pCurrrentStation; 



   //回传到上位机
    USINT SendData[16] = {0};
    USINT TempData[16] = {0};


/**
 * 初始化所涉及的基本数据
 */
static void InitSlaveStationData(void)
{
                       
    
    StationList[0].ReciveFrame.len = 8;
    StationList[0].ReciveFrame.pBuffer = ReceiveDataBuffer0;
    StationList[0].SendFrame.len = 8;
    StationList[0].SendFrame.pBuffer = SendDataBuffer0;
    
    StationList[1].ReciveFrame.len = 8;
    StationList[1].ReciveFrame.pBuffer = ReceiveDataBuffer1;
    StationList[1].SendFrame.len = 8;
    StationList[1].SendFrame.pBuffer = SendDataBuffer1;
    
    StationList[2].ReciveFrame.len = 8;
    StationList[2].ReciveFrame.pBuffer = ReceiveDataBuffer2;
    StationList[2].SendFrame.len = 8;
    StationList[2].SendFrame.pBuffer = SendDataBuffer2;
    
    StationList[3].ReciveFrame.len = 8;
    StationList[3].ReciveFrame.pBuffer = ReceiveDataBuffer3;
    StationList[3].SendFrame.len = 8;
    StationList[3].SendFrame.pBuffer = SendDataBuffer3;
    
    StationList[4].ReciveFrame.len = 8;
    StationList[4].ReciveFrame.pBuffer = ReceiveDataBuffer4;
    StationList[4].SendFrame.len = 8;
    StationList[4].SendFrame.pBuffer = SendDataBuffer4;
    
    StationList[5].ReciveFrame.len = 8;
    StationList[5].ReciveFrame.pBuffer = ReceiveDataBuffer5;
    StationList[5].SendFrame.len = 8;
    StationList[5].SendFrame.pBuffer = SendDataBuffer5;
   
    
    for(USINT i =0; i < STATION_COUNT; i++)
    {
        StationList[i].StationInformation.step = STEP_START;
        StationList[i].StationInformation.complete =TRUE;
        StationList[i].StationInformation.startTime = g_MsTicks +  1000*i;
        StationList[i].StationInformation.delayTime = 1000;
        StationList[i].StationInformation.endTime = 0;
        StationList[i].StationInformation.OverTimeCount = 0;
        
        StationList[i].StationInformation.online = 0;
        StationList[i].StationInformation.state = 0;  
        StationList[i].StationInformation.oldState = 0;          
    }
    StationList[0].StationInformation.macID =  MacList[0]; //永磁控制器A
    StationList[1].StationInformation.macID =  MacList[1]; //永磁控制器B
    StationList[2].StationInformation.macID =  MacList[2]; //永磁控制器C
    StationList[3].StationInformation.macID =  MacList[3]; //DSP控制器
    StationList[4].StationInformation.macID =  MacList[4]; //监控B
    StationList[5].StationInformation.macID =  MacList[5]; //监控C

    StationList[0].StationInformation.enable = TRUE;
    StationList[1].StationInformation.enable = FALSE;
    StationList[2].StationInformation.enable = FALSE;
    StationList[3].StationInformation.enable = TRUE;
    StationList[4].StationInformation.enable = FALSE;
    StationList[5].StationInformation.enable = FALSE;     
}
/**
 * 初始化主站所涉及的基本数据
 */
static void InitMasterStationData(void)
{
    
    //////////初始化DeviceNetObj对象////////////////////////////////
	DeviceNetObj.MACID =   MacList[STATION_COUNT];      //            
	DeviceNetObj.baudrate = 2;                   //500Kbit/s
	DeviceNetObj.assign_info.select = 0;         //初始的配置选择字节清零
	DeviceNetObj.assign_info.master_MACID =0x0A; //默认主站地址，在预定义主从连接建立过程中，主站还会告诉从站：主站的地址
//////////////连接对象为不存在状态//////////////////////////
	VisibleConnectionObj.state = 0;
	CycleInquireConnedctionObj.state = 0;//状态：没和主站连接，主站还没有配置从站
///////////////初始化标识符对象///////////////
	IdentifierObj.providerID = providerID;        //providerID = 0X2620; 供应商ID 
	IdentifierObj.device_type = device_type;      //device_type = 0;通用设备
	IdentifierObj.product_code = product_code;    //product_code =0X00d2;产品代码
	IdentifierObj.version.major_ver = major_ver;  //major_ver = 1;
	IdentifierObj.version.minor_ver = minor_ver;  //minor_ver = 1;版本
	IdentifierObj.serialID = serialID;            //serialID = 0x001169BC;;序列号
	IdentifierObj.product_name = product_name;    //product_name = {8, "ADC4"};产品名称
    
    
    MasterStation.ReciveFrame.complteFlag = 0xff;
    MasterStation.ReciveFrame.waitFlag = 0;
    MasterStation.ReciveFrame.pBuffer = ReciveBufferData;
    MasterStation.SendFrame.complteFlag = 0xff;
    MasterStation.SendFrame.pBuffer = SendBufferData;
    MasterStation.SendFrame.waitFlag = 0;
    
    MasterStation.StationInformation.step = STEP_START;
    MasterStation.StationInformation.complete =TRUE;
    MasterStation.StationInformation.startTime = 0;
    MasterStation.StationInformation.delayTime = 0;
    MasterStation.StationInformation.endTime = 0;
    MasterStation.StationInformation.OverTimeCount = 0;
    
    MasterStation.StationInformation.online = 0;
    MasterStation.StationInformation.state = 0; 
    
   
}

/**
 * 初始化DeviceNet所涉及的基本数据
 */
void InitDeviceNet(void)
{    
    //地址赋值--可根据情况进行更新
    MacList[0]= 0x10;//永磁控制器A
    MacList[1]= 0x12;//永磁控制器B
    MacList[2]= 0x14;//永磁控制器C
    MacList[3]= 0x0D;//DSP控制器
    
    MacList[4]= 0x1A;//监控A
    MacList[5]= 0x1C;//监控B
    
    MacList[6]= 0x02;//本机控制
    
    InitMasterStationData();
  
    
    BOOL result =  CheckMACID(&MasterStation.ReciveFrame, &MasterStation.SendFrame);
    
    while(result);
    
    InitSlaveStationData();
     g_pCurrrentStation = StationList; //当前指针变量指向从站第一站点
}
/**
 * DeviceNet 打包数据发送
 *
 * @param   pStation 指向站点信息
 * 
 * @param   pData    指向数据包指针
 *
 * @param   type     生成报文类型
 *
 * @bref    DeviceNet 发送IO报文数据
 */
void DeviceNetSendIOData( struct DefStationElement* pStation, USINT* pData, USINT datalen)
{
    pStation->SendFrame.len = 8;   
    BYTE result = MakeIOMessage( &pStation->SendFrame, pStation->StationInformation.macID, pData,  datalen);
    if (result)
    {
        DeviceNetSendData( &pStation->SendFrame);
        pStation->SendFrame.waitFlag = TRUE;
        
    }       
}


/**
 * 通过仅限组2显示信息服务建立连接
 *
 * @param   null
 *
 * @bref   建立连接
 */
static void EstablishConnection(struct DefStationElement* pStation, USINT connectType)
{
    //建立显示连接
    pStation->SendFrame.len = 8;  
    BOOL result =  MakeUnconnectVisibleRequestMessageOnlyGroup2( &pStation->SendFrame,  pStation->StationInformation.macID,
    SVC_AllOCATE_MASTER_SlAVE_CONNECTION_SET, connectType);
    if (result)
    {
        DeviceNetSendData( &pStation->SendFrame);
        pStation->SendFrame.waitFlag = TRUE;
    }
    else
    {
        while(TRUE);
    }  
}




/**
 * 单个支线任务
 *
 * @param   pStation 指向站点信息
 *
 * @bref   建立连接
 */
static void NormalTask(struct DefStationElement* pStation)
{
    if ( pStation->StationInformation.complete)//若已经完成任务则致0
    {
        pStation->StationInformation.OverTimeCount = 0;
    }
    else
    {
         pStation->StationInformation.OverTimeCount++;
    }
    
    pStation->StationInformation.complete = FALSE;
    
    pStation->SendFrame.complteFlag = 0; //可以使用
    pStation->SendFrame.waitFlag = 0; //等在应答
    switch(pStation->StationInformation.step)
    {
        case STEP_START: //启动开始
        {
            pStation->StationInformation.startTime = g_MsTicks; 
            if( pStation->StationInformation.OverTimeCount > 3)
            {
                pStation->StationInformation.delayTime = 3000;//1000mS超时间
            }
            else
            {
                pStation->StationInformation.delayTime = 1000;//1000mS超时间
            }
            
            EstablishConnection(pStation, VISIBLE_MSG); //建立显示连接         
            break;
        }
        case STEP_LINKING: //正在建立连接
        {
            pStation->StationInformation.startTime = g_MsTicks; 
            if( pStation->StationInformation.OverTimeCount > 3)
            {
                pStation->StationInformation.delayTime = 3000;//1000mS超时间
            }
            else
            {
                pStation->StationInformation.delayTime = 1000;//1000mS超时间
            }
            EstablishConnection(pStation, STATUS_CHANGE); //建立状态改变连接
            break;
        }
        case STEP_STATUS_CHANGE://建立状态改变连接
        {
             pStation->StationInformation.startTime = g_MsTicks; 
            if( pStation->StationInformation.OverTimeCount > 3)
            {
                pStation->StationInformation.delayTime = 3000;//1000mS超时间
            }
            else
            {
                pStation->StationInformation.delayTime = 1000;//1000mS超时间
            }
            EstablishConnection(pStation, CYC_INQUIRE); //建立循环连接
        }
        
        case STEP_CYCLE: //循环建立连接
        {
            pStation->StationInformation.startTime = g_MsTicks; 
            pStation->StationInformation.delayTime = 2000;//1000mS超时间
            
                       
            
            break;
        }
       
    }   
}

/**
 * 通讯主任务
 *
 * @param   pStation 指向站点信息
 *
 * @bref   建立连接
 */
void MainDeviceNetTask(void)
{
    
        
        for(USINT i = 0; i < STATION_COUNT; i++)//STATION_COUNT
        {
            if (StationList[i].StationInformation.enable != TRUE)
            {
                continue;
            }
              //是否超时，时间是否到。
            if (IsOverTime(StationList[i].StationInformation.startTime, StationList[i].StationInformation.delayTime) )
            {
                NormalTask(StationList + i);
            } 
              //判读状态是否发生改变，若改变发送信息
            if (StationList[i].StationInformation.state != StationList[i].StationInformation.oldState)
            {
                uint8_t k = 0;
                uint8_t datalen = 0;
                StationList[i].StationInformation.oldState = StationList[i].StationInformation.state;
                
                TempData[k++] = DeviceNetObj.MACID;//添加上传信息
                TempData[k++] = 0xAA;
                TempData[k++] = 0x1A | 0x80;
                TempData[k++] = StationList[i].StationInformation.macID;
                TempData[k++] = 0;                                       
                TempData[k++] = StationList[i].StationInformation.oldState;
                GenRTUFrame(UP_ADDRESS, CAN_MESSAGE_TO_UP, TempData, k,SendData, &datalen);
                SendFrame(SendData, datalen); 
            }
        }
    
}


/**
 * 获取站点指针
 *
 * @param   macID 站点MAC地址
 *
 * @bref   通过站点MACID获取相应的站点信息指针，没有则返回<code>null</code>
 */
struct DefStationElement* GetStationPoint(USINT macID)
{
    for (USINT i =0 ; i < STATION_COUNT; i++)
    {
        if (StationList[i].StationInformation.macID == macID)
        {
            return &StationList[i];
        }
            
    }
    return 0;
}

/**
 * 从站显示信息应答报文处理函数
 *
 * @param   pReciveFrame   指向接收报文数据的指针,表示已经接收到的数据
 * @param   pHistoryFrame  指向历史发送报文,其和接收帧属于应答关系
 */
static void SlaveStationVisibleMsgService(WORD* pID, BYTE * pbuff, BYTE len)
{  
    struct DefStationElement* pStation = GetStationPoint(GET_GROUP2_MAC(*pID));
    if (pStation == 0)
    {
        //报错
        return;
    }
    //g_pCurrrentStation = pStation;
    if (!pStation->SendFrame.waitFlag)//判断是否为等待
    {
        return;
    }
    if (GET_GROUP2_MAC(*pID) != pStation->StationInformation.macID) //判断是目的MAC与本机是否一致
    {
        return;
    }
    if ((pbuff[0] &  0x3F) != DeviceNetObj.MACID) //判断是否为同一个源MAC
    {
        return;
    }
    //判断服务代码是否是对应的应答代码,或者错误响应代码
    if (((pStation->SendFrame.pBuffer[1] |0x80) != pbuff[1]) || (pbuff[1] == (0x80 | SVC_ERROR_RESPONSE)))
	{
        return;
    }
    if (len  > 10) //长度过长
    {
            return;
    }
    for(USINT i = 0; i < len; i++)
    {
        pStation->ReciveFrame.pBuffer[i] = pbuff[i];
    }
    pStation->ReciveFrame.ID = *pID;
    pStation->ReciveFrame.len = len;
    
    switch (pStation->ReciveFrame.pBuffer[1] & 0x7F)
    {
        case SVC_AllOCATE_MASTER_SlAVE_CONNECTION_SET://建立主从连接          
        {
            //配置连接字
            pStation->StationInformation.state |=  pStation->SendFrame.pBuffer[4];             
            if (pStation->StationInformation.state & CYC_INQUIRE ) //建立轮询连接
            {
                pStation->StationInformation.step = STEP_CYCLE;
                pStation->SendFrame.waitFlag = FALSE;
                pStation->StationInformation.endTime = g_MsTicks; //设置结束时间
                pStation->StationInformation.complete = TRUE;
            }
            else if (pStation->StationInformation.state &  STATUS_CHANGE ) //建立状态改变连接
            {
                pStation->StationInformation.step = STEP_STATUS_CHANGE;
                pStation->SendFrame.waitFlag = FALSE;
                pStation->StationInformation.endTime = g_MsTicks; //设置结束时间
                pStation->StationInformation.complete = TRUE;
            }
            else if (pStation->StationInformation.state & VISIBLE_MSG) //建立显示连接
            {
                pStation->StationInformation.step = STEP_LINKING;
                pStation->SendFrame.waitFlag = FALSE;
                pStation->StationInformation.endTime = g_MsTicks;//设置结束时间
                pStation->StationInformation.complete = TRUE;
            }
            else
            {
                //报错
            }
            
            break;
        }
        case SVC_RELEASE_GROUP2_IDENTIFIER_SET://释放主从连接 
        {
            //配置连接字
            pStation->StationInformation.state &=  (pStation->SendFrame.pBuffer[5]^ 0xFF); 
            break;           
        }
        case SVC_GET_ATTRIBUTE_SINGLE: //获取单个属性
        {
            //发送获取消息
            break;
        }
        case SVC_SET_ATTRIBUTE_SINGLE: //设置单个属性
        {
            //发送设置消息
            break;
        }
       
    }
 
}


/**
 * 生成仅限组2非连接显示请求信息
 *
 * @param   pFrame     指向报文数据的指针,长度指针表示所指向缓冲区的最小长度
 * @param   destMAC    目的MAC地址
 * @param   serverCode 服务代码
 * @param   config     配置字节
 * @return  <code>TRUE</code>   生成成功
 *          <code>FASLE</code>  生成失败 
 */
static BOOL MakeUnconnectVisibleRequestMessageOnlyGroup2(struct DefFrameData* pFrame, BYTE destMAC,BYTE serverCode, BYTE config)
{
   if ((pFrame->len >= 6) && (pFrame->complteFlag == 0))
   {
       pFrame->ID = MAKE_GROUP2_ID(GROUP2_VSILBLE_ONLY2, destMAC);
       pFrame->pBuffer[0] = DeviceNetObj.MACID & 0x3F;  //本地MAC ID  
       pFrame->pBuffer[1] = serverCode; 
       pFrame->pBuffer[2] = 3;   //类ID  
       pFrame->pBuffer[3] = 1;   //实例ID
       pFrame->pBuffer[4] = config;
       pFrame->pBuffer[5] = DeviceNetObj.MACID & 0x3F;
       pFrame->len = 6;
       pFrame->complteFlag = TRUE;
       return TRUE;
   }
   return FALSE;
}
/**
 * 生成主站IO报文(GROUP2_POLL_STATUS_CYCLE)消息
 *
 * @param   pFrame     指向报文数据的指针,长度指针表示所指向缓冲区的最小长度
 * @param   destMAC    目的MAC地址
 * @param   pData      数据指针
 * @param   datalen    数据长度
 * @return  <code>TRUE</code>   生成成功
 *          <code>FASLE</code>  生成失败 
 */
 BOOL MakeIOMessage(struct DefFrameData* pFrame, BYTE destMAC, BYTE* pData, BYTE datalen)
{
   if ((pFrame->len >= 6) && (pFrame->complteFlag == 0))
   {
       if (datalen <= 8)
       {
           pFrame->ID = MAKE_GROUP2_ID(GROUP2_POLL_STATUS_CYCLE, destMAC);
           memcpy(pFrame->pBuffer, pData, datalen);//拷贝数据，要求源地址与目的地址范围没有冲突。           
           pFrame->len = datalen;
           pFrame->complteFlag = TRUE;
           return TRUE;
       }     
   }
   return FALSE;
}
/**
 * 生成主站显示请求信息
 *
 * @param   pFrame     指向报文数据的指针,长度指针表示所指向缓冲区的最小长度
 * @param   serverCode 服务代码
 * @param   destMAC    目的MAC地址
 * @param   class      选择类的ID
 * @param   obj        选择的具体实例ID
 * @param   attribute  选择的具体属性ID
 * @return  <code>TRUE</code>   生成成功
 *          <code>FASLE</code>  生成失败 
 */
BOOL MakeVisibleMessage(struct DefFrameData* pFrame,BYTE serverCode, BYTE destMAC, BYTE class, BYTE obj, BYTE attribute)
{
   if ((pFrame->len >= 5) && (pFrame->complteFlag == 0))
   {
       pFrame->ID = MAKE_GROUP2_ID(GROUP2_VSILBLE_ONLY2, destMAC);
       pFrame->pBuffer[0] = DeviceNetObj.MACID & 0x3F;  //本地MAC ID  
       pFrame->pBuffer[1] = serverCode; 
       pFrame->pBuffer[2] = class;   //类ID  
       pFrame->pBuffer[3] = obj;   //实例ID
       pFrame->pBuffer[4] = attribute;
       pFrame->len = 5;
       pFrame->complteFlag = TRUE;
   }
   return FALSE;
}


/**
 * 响应或者发出MACID重复检测报文
 *
 * @param   pSendFrame   指向发送报文数据的指针
 * @param   config       配置字节       
 */
void ResponseMACID(struct DefFrameData* pSendFrame, BYTE config)
{                        //重复MACID检查
    pSendFrame->ID =  MAKE_GROUP2_ID( GROUP2_REPEAT_MACID, DeviceNetObj.MACID); 
	pSendFrame->pBuffer[0] = config;	                        //请求/响应标志=1，表示响应，端口号0
	pSendFrame->pBuffer[1] = IdentifierObj.providerID;	//制造商ID低字节
	pSendFrame->pBuffer[2] = IdentifierObj.providerID >> 8;	//制造商ID高字节
	pSendFrame->pBuffer[3] = IdentifierObj.serialID;	                    //序列号低字节
	pSendFrame->pBuffer[4] = IdentifierObj.serialID >> 8;                //序列号中间字节1
	pSendFrame->pBuffer[5] = IdentifierObj.serialID >>16;          //序列号中间字节2
	pSendFrame->pBuffer[6] = IdentifierObj.serialID >>24;	//序列号高字节
    pSendFrame->len = 7;
	DeviceNetSendData(pSendFrame);                      //发送报文
}

/**
 * 主动检查重复MACID,判断网络中是否有与自己相同MAC的设备
 *
 * @param  pReciveFrame  指向接收报文数据的指针
 * @param  pSendFrame    指向发送报文数据的指针
 * @return   <code>TRUE</code>   检测到重复的MAC地址
 *           <code>FASLE</code>  未检测到重复的设备MAC地址
 */
BOOL CheckMACID(struct DefFrameData* pReciveFrame, struct DefFrameData* pSendFrame)
{	
    int sendCount = 0; 
    do
    {
        pReciveFrame->complteFlag = 0;
        
           //发送请求
        ResponseMACID( pSendFrame, 0);
        StartOverTimer();//启动超时定时器
        while( IsTimeRemain())
        {
            if ( pReciveFrame->complteFlag)//判断是否有未发送的数据
            {
                BYTE mac = GET_GROUP2_MAC(pReciveFrame->ID);
                BYTE function = GET_GROUP2_FUNCTION(pReciveFrame->ID);
                if (function == GROUP2_REPEAT_MACID)
                {                  
                    if (mac == DeviceNetObj.MACID)
                    {
                          return TRUE; //只要有MACID一致，无论应答还是发出，均认为重复                  
                    }
                }                
                else
                {
                    continue;
                }
            }
        }       
    }
    while(++sendCount < 2);
    
	return FALSE;	//没有重复地址
}


/**
 * 发送DeviceNet协议数据
 *
 * @param  pFrame 指向帧数据的指针   
 */
void DeviceNetSendData(struct DefFrameData* pFrame)
{
    for(USINT i = 0; i < pFrame->len; i++)
    {
        CAN_TxMsg[1].data[i] = pFrame->pBuffer[i];
    }
    
    
    CAN_TxMsg[1].id = pFrame->ID;    
	CAN_TxMsg[1].len =  pFrame->len;
	CAN_TxMsg[1].format = STANDARD_FORMAT;
	CAN_TxMsg[1].type = DATA_FRAME;
	CAN_waitReady (CAN2);  
    CAN_wrMsg (CAN2, &CAN_TxMsg[1]);    
    pFrame->complteFlag = 0;
}

/**
 * 启动超时定时器
 */
void StartOverTimer(void)
{
   MasterStation.StationInformation.startTime = g_MsTicks;
   MasterStation.StationInformation.delayTime = 1000; //1000ms
    
}
/**
 * 检测时间是否剩余 
 *
 * @return   <code>TRUE</code>   还有剩余时间
 *           <code>FASLE</code>  时间用尽,没有剩余
 */
BYTE IsTimeRemain(void)
{
    if (IsOverTime( MasterStation.StationInformation.startTime, MasterStation.StationInformation.delayTime))
    {
        return FALSE;
    }
    return TRUE;
   
}

/**
 * 主动检查重复MACID,判断网络中是否有与自己相同MAC的设备
 * <p>
 * 
 *
 * @param  pID      11bit ID号 
 * @param  pbuff    指向缓冲数据指针
 * @param  len      缓冲区数据长度
 * @return          <code>TRUE</code>   信息符合要求，进行处理
 *                  <code>FASLE</code>  信息不符合要求，未进行处理
 */
BOOL DeviceNetReciveCenter(WORD* pID, BYTE * pbuff, BYTE len)
{   
    //BYTE i= 0;
    //UINT id = (*pID);
   
    if (((*pID) & 0x07C0) <= 0x3C0) //Group1
    {
        
        BYTE  function = GET_GROUP1_FUNCTION(*pID);
        switch(function)
        {
            case GROUP1_POLL_STATUS_CYCLER_ACK:
            {
                SlaveStationStatusCycleService(pID, pbuff, len);
                break;
            }   
            case GROUP1_STATUS_CYCLE_ACK: //从站状态改变
            {
                SlaveStationStatusChangeService(pID, pbuff, len);
                break;
            }
    
            default:
            {
                break;
            }
        }
 
    }
    else if (((*pID) & 0x0600) == 0x0400) //Group2
    {
        BYTE  mac = GET_GROUP2_MAC(*pID);
        //从站显示应答服务
        switch(GET_GROUP2_FUNCTION(*pID))
        {
            case GROUP2_VISIBLE_UCN: //从站显示响应服务
            {
                if (len >= 2)
                {                   
                    SlaveStationVisibleMsgService(pID, pbuff, len);                                 
                }
                break;
            }
            case GROUP2_REPEAT_MACID: //从站响应MAC ID重复检测
            {
                if (mac == DeviceNetObj.MACID)
                {                     
                    if( MasterStation.SendFrame.complteFlag) //检测是否被占用
                    {
                        return FALSE;
                    }
                    ResponseMACID(&MasterStation.SendFrame, 0x80);       //重复MACID检查响应函数,应答，物理端口为0
                }
                else
                {                     
                    RestartEstablishLink(mac);
                }
               
                break;
            }
            
        }
    }
    else
    {
        //其他信息
    }


    
    
   
    return 0;
}
/**
 * 复位连接 
 *
 * @param  mac     指定mac指定重新建立连接的回路 
 * @return          null
 */
void RestartEstablishLink(uint8_t mac)
{
    struct DefStationElement* pStation = GetStationPoint(mac);
    if ( pStation != 0)
    {
        pStation->StationInformation.step = STEP_START;
        pStation->StationInformation.complete = TRUE;
        pStation->StationInformation.startTime = g_MsTicks;
        pStation->StationInformation.delayTime = 300;
        pStation->StationInformation.endTime = 0;
        pStation->StationInformation.OverTimeCount = 0;
        
        pStation->StationInformation.online = 0;
        pStation->StationInformation.state = 0;     
    }        
    
}
/**
 * 转发帧信息
 *
 * @param   mac     mac地址
 * @param   pbuff   指向缓冲区数据指针
 * @param   len     缓冲区数据长度
 */
void UartSendMessage(uint8_t mac, BYTE* pbuff, BYTE len)
{
 
    USINT datalen = 0;
    TempData[0] = mac;//添加上传信息
    TempData[1] = 0xAA;
    memcpy(TempData + 2, pbuff, len);//拷贝数据，要求源地址与目的地址范围没有冲突。
    GenRTUFrame(UP_ADDRESS, CAN_MESSAGE_TO_UP, TempData, len + 2,SendData, &datalen);
    SendFrame(SendData, datalen);  
}

/**
 * 从站从站IO轮询或状态变化/循环应答消息
 *
 * @param   pID     ID号指针
 * @param   pbuff   指向缓冲区数据指针
 * @param   len     缓冲区数据长度
 */
static void SlaveStationStatusCycleService(WORD* pID, BYTE* pbuff, BYTE len)
{
    BYTE  mac = GET_GROUP1_MAC(*pID);
    struct DefStationElement* pStation =  GetStationPoint(mac);
    if (pStation != 0)
    {
        //检测是否已经建立循环连接
        if((pStation->StationInformation.state & CYC_INQUIRE) ==CYC_INQUIRE )
        {
            StationStatusCycleService(pStation, pbuff, len);            
        }
    }
     
   
}
/**
 * 从站从站状态改变应答信息
 *
 * @param   pID     ID号指针
 * @param   pbuff   指向缓冲区数据指针
 * @param   len     缓冲区数据长度
 */
static void SlaveStationStatusChangeService(WORD* pID, BYTE* pbuff, BYTE len)
{
    BYTE  mac = GET_GROUP1_MAC(*pID);
    struct DefStationElement* pStation =  GetStationPoint(mac);
    if (pStation != 0)
    {
        //检测是否已经建立循环连接
        if((pStation->StationInformation.state & CYC_INQUIRE) ==CYC_INQUIRE )
        {
            StationStatusChangeService(pStation, pbuff, len);            
        }
    }
     
   
}







