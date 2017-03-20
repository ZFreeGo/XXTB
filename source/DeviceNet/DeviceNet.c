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
#include "DeviceNet.h"
#include "can.h"
#include "timer.h"

#define STEP_START        0xA1 //启动状态,上电初始状态
#define STEP_LINKING      0xA2 //正在建立连接中
#define STEP_CYCLE        0xA4 //循环模式




BOOL IsTimeRemain(void);    //需要根据具体平台改写
void StartOverTimer(void);//需要根据具体平台改写
void SendData(struct DefFrameData* pFrame);//需要根据具体平台改写
void ResponseMACID(struct DefFrameData* pSendFrame, BYTE config);
void InitYongciAData(void);
static BOOL MakeUnconnectVisibleRequestMessageOnlyGroup2(struct DefFrameData* pFrame, 
    BYTE destMAC,BYTE serverCode, BYTE config);
static void EstablishConnection(struct DefStationElement* pStation, USINT connectType);

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
static struct DefStationElement StationList[STATION_COUNT];



/**
 * 当前处理所指向的子站指针
 */
struct DefStationElement*    g_pCurrrentStation; 




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
        StationList[i].StationInformation.startTime = 0;
        StationList[i].StationInformation.delayTime = 0;
        StationList[i].StationInformation.endTime = 0;
        StationList[i].StationInformation.OverTimeCount = 0;
        
        StationList[i].StationInformation.online = 0;
        StationList[i].StationInformation.state = 0;       
    }
     StationList[0].StationInformation.macID = 0x10; //永磁控制器A
     StationList[1].StationInformation.macID = 0x12; //永磁控制器B
     StationList[2].StationInformation.macID = 0x14; //永磁控制器C
     StationList[3].StationInformation.macID = 0x18; //监控A
     StationList[4].StationInformation.macID = 0x1A; //监控B
     StationList[5].StationInformation.macID = 0x1C; //监控C

}


/**
 * 初始化DeviceNet所涉及的基本数据
 */
void InitDeviceNet(void)
{    

  
    //////////初始化DeviceNetObj对象////////////////////////////////
	DeviceNetObj.MACID =0x20 ;                   //如果跳键没有设置从站地址，默认主站地址0x02            
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
    StationList[0].StationInformation.macID = 0x02; //主站地址
    

    
    InitSlaveStationData();
    
    g_pCurrrentStation = StationList; //当前指针变量指向从站第一站点
    
    
    BOOL result =  CheckMACID(&MasterStation.ReciveFrame, &MasterStation.SendFrame);
    
    while(result);
    
    
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
    BOOL result =  MakeUnconnectVisibleRequestMessageOnlyGroup2( &pStation->SendFrame,  pStation->StationInformation.macID,
    SVC_AllOCATE_MASTER_SlAVE_CONNECTION_SET, connectType);
    if (result)
    {
        SendData( &pStation->SendFrame);
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
    pStation->StationInformation.startTime = g_MsTicks; 
    pStation->StationInformation.delayTime = 500;//500mS超时间
    pStation->SendFrame.complteFlag = 0; //可以使用
    pStation->SendFrame.waitFlag = 0; //等在应答
    switch(pStation->StationInformation.step)
    {
        case STEP_START: //启动开始
        {
            
            EstablishConnection(pStation, VISIBLE_MSG);
          
            break;
        }
        case STEP_LINKING: //正在建立连接
        {
            EstablishConnection(pStation, CYC_INQUIRE);
            break;
        }
        case STEP_CYCLE: //循环建立连接
        {
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
    for(USINT i = 0; i < STATION_COUNT; i++)
    {
          //是否超时，时间是否到。
          if (IsOverTime(StationList[i].StationInformation.startTime, StationList[i].StationInformation.delayTime) )
          {
              NormalTask(StationList + i);
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
static struct DefStationElement* GetStationPoint(USINT macID)
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
    if (GET_GROUP2_MAC(*pID) != DeviceNetObj.MACID) //判断是目的MAC与本机是否一致
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
            pStation->StationInformation.state |=  pStation->SendFrame.pBuffer[5];             
            
            if (pStation->StationInformation.state & CYC_INQUIRE) //若建立轮询连接则进入轮询连接建立步骤
            {
                pStation->StationInformation.step = STEP_CYCLE;
                pStation->SendFrame.waitFlag = FALSE;
                pStation->StationInformation.endTime = g_MsTicks; //设置结束时间
                pStation->StationInformation.complete = TRUE;
            }
            else if (pStation->StationInformation.state & VISIBLE_MSG) //若建立显示连接则进入轮询连接建立步骤
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
	SendData(pSendFrame);                      //发送报文
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
void SendData(struct DefFrameData* pFrame)
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
        return 0xFF;
    }
    return 0;
   
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
                     for(USINT i = 0; i < STATION_COUNT; i++)
                    {
                        if(StationList[i].StationInformation.macID== mac)
                        {
                            StationList[i].StationInformation.online = TRUE;
                            StationList[i].StationInformation.state = 0;
                        }
                    }
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


