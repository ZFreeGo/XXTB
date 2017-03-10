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

#include "DeviceNet.h"

BOOL IsTimeRemain(void);    //需要根据具体平台改写
void StartOverTimer(void);//需要根据具体平台改写
void SendData(struct DefFrameData* pFrame);//需要根据具体平台改写


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
struct DefFrameData  DeviceNetReciveFrame; //接收帧处理
struct DefFrameData  DeviceNetSendFrame; //接收帧处理

BYTE  out_Data[8];//从站输出数组

/**
 * 初始化DeviceNet所涉及的基本数据
 */
void InitDeviceNet()
{    
    DeviceNetReciveFrame.complteFlag = 0xff;
    DeviceNetReciveFrame.pBuffer = ReciveBufferData;
    DeviceNetSendFrame.complteFlag = 0xff;
    DeviceNetSendFrame.pBuffer = SendBufferData;
  
    //////////初始化DeviceNetObj对象////////////////////////////////
	DeviceNetObj.MACID =0x02 ;                   //如果跳键没有设置从站地址，默认从站地址0x02            
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
        sendCount ++;        
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
    // CANSendData(pFrame->ID, pFrame->pBuffer, pFrame->len);
}

/**
 * 启动超时定时器
 */
void StartOverTimer(void)
{

}
/**
 * 检测时间是否剩余 
 *
 * @return   <code>TRUE</code>   还有剩余时间
 *           <code>FASLE</code>  时间用尽,没有剩余
 */
BYTE IsTimeRemain(void)
{
    return FALSE;
}

/**
 * 主动检查重复MACID,判断网络中是否有与自己相同MAC的设备
 *
 * @param  pID      11bit ID号 
 * @param  pbuff    指向缓冲数据指针
 * @param  len      缓冲区数据长度
 * @return          <code>TRUE</code>   信息符合要求进行处理
 *                  <code>FASLE</code>  信息不符合要求未进行处理
 */
BOOL DeviceNetReciveCenter(WORD* pID, BYTE * pbuff, BYTE len)
{   
    BYTE i= 0;
    
    
    
    
    if( GROUP2_MSG != GET_GROUP2_MAC(*pID))  //不是仅限组2报文处理
	{       
        return FALSE;    
    }        
    
    
    
    
    
    if( DeviceNetReciveFrame.complteFlag) //
    {
        return FALSE;
    }
    
   
    if (len <= 8) //最大长度限制
    {
         DeviceNetReciveFrame.ID = *pID;   
         DeviceNetReciveFrame.len = len;
        for(i = 0; i< len; i++) //复制数据
        {
            DeviceNetReciveFrame.pBuffer[i] = pbuff[i];
        }
         DeviceNetReciveFrame.complteFlag = 0xff;
         return TRUE;
    }
    return 0;
}