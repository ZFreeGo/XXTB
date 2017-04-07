#include "NetApp.h"
#include "Buffer.h"
#include "timer.h"


/**
 *工作模式  bit 1,0--正常模式，bit 3,2--同步预制模式， bit 5,4--同步执行模式
 */

#define WORK_READY_SYN    0x000C //准备同步
#define WORK_ACTION_SYN   0x0030 //执行同步
#define WORK_NORMOL_TASK    0x00C0 //执行普通任务

#define SET_WORK_MODE(mode)    {g_WorkMode |= (uint16_t)(mode);}
#define CLREA_WORK_MODE(mode)  {g_WorkMode &= ~((uint16_t)(mode));}
#define WORK_MODE_RUNING(mode) ((g_WorkMode & (uint16_t)(mode)) == (uint16_t)(mode))

/**
 *工作模式  bit 1,0--正常模式，bit 3,2--同步预制模式， bit 5,4--同步执行模式
 */
uint16_t g_WorkMode = 0;
/**
 * 同步状态控制
 */
static SynchronousControl SynchronousState;


uint8_t  SynchronousOperationReadyACK(struct DefStationElement* pStation, BYTE* pbuff, BYTE len);
uint8_t  SynchronousOperationActionACK(struct DefStationElement* pStation, BYTE* pbuff, BYTE len);
uint8_t  SynchronousOperationReady(PointUint8* pData, PointUint8* pCommand);
uint8_t  SynchronousOperationAction(PointUint8* pData, PointUint8* pCommand);


/**
 * 数据到达
 */


/**
 * 初始化DeviceNet应用层数据
 */
void InitNetApp(void)
{
     g_WorkMode = 0;
     BufferInit();
}

/**
 * 从站IO轮询或状态变化/循环应答消息
 *
 * @param   pID     ID号指针
 * @param   pbuff   指向缓冲区数据指针
 * @param   len     缓冲区数据长度
 */
void StationStatusCycleService(struct DefStationElement* pStation,  BYTE* pbuff, BYTE len)
{
     //回传到上位机
    //是否为同步预制模式           
    UartSendMessage(pStation->StationInformation.macID, pbuff, len);
    if (WORK_MODE_RUNING(WORK_READY_SYN))
    {
        SynchronousOperationReadyACK(pStation, pbuff, len);        
        return;
    }
    
    //是否为同步执行模式
    if (WORK_MODE_RUNING(WORK_ACTION_SYN))
    {     
        SynchronousOperationActionACK(pStation, pbuff, len);
        return;
    }
    //执行普通任--转移到普通模式下运行           
    SET_WORK_MODE(WORK_NORMOL_TASK);
}

/**
 * 使能一般循环
 */
inline static void EnableNormalCycle(void)
{
     StationList[0].StationInformation.enable = TRUE;
     StationList[1].StationInformation.enable = FALSE;
     StationList[2].StationInformation.enable = FALSE;
     StationList[SYNCHRONIZATION_INDEX].StationInformation.enable = TRUE;  
}

/**
 * 禁止一般循环
 */
inline static void DisableNormalCycle(void)
{
     StationList[0].StationInformation.enable = FALSE;
     StationList[1].StationInformation.enable = FALSE;
     StationList[2].StationInformation.enable = FALSE;
     StationList[SYNCHRONIZATION_INDEX].StationInformation.enable = FALSE;  
}



/**
 * 复位连接 ，此函数若中断中调用，需注意发送与接收冲突问题。
 *
 * @param   pData     DSP命令控制字
 * @param   pCommand  永磁控制器命令字
 * @return            非0-错误代码，0--正确
 */
uint8_t  SynchronousOperationReady(PointUint8* pData, PointUint8* pCommand)
{
    uint8_t result = 0;
    //Step 1： 检查当前状态是否适合同步————检查通讯是否正常,检查当前是否同步
    
    
    //Step 2:  生成针对同步控制器IO报文    
    if (pData->len < 4)//命令控制字至少4个字节
    {
        return 0xA1;
    }
    //成同步控制器预制命令(DSP)
    StationList[SYNCHRONIZATION_INDEX].SendFrame.len = 8;
    result = MakeIOMessage(&StationList[SYNCHRONIZATION_INDEX].SendFrame, MacList[SYNCHRONIZATION_INDEX], pData->pData, pData->len);        
    if (result == FALSE)
    {
        return 0xA2;            
    }
    StationList[SYNCHRONIZATION_INDEX].SendFrame.waitFlag = TRUE;
    StationList[SYNCHRONIZATION_INDEX].StationInformation.enable = FALSE; //轮询置位False，暂停一般轮询发送
    //Step 3：置为同步模式 并 发送同步IO报文 
    SET_WORK_MODE(WORK_READY_SYN); 
     
    DeviceNetSendData(&StationList[SYNCHRONIZATION_INDEX].SendFrame);
    SynchronousState.startTime = g_MsTicks;
    SynchronousState.delayTime = 2000;
    SynchronousState.readyBit = 0;
    SynchronousState.readyAction = FALSE;
    //TODO:添加适当延时    
    
    //Step4 :针对其它控制器(永磁控制器)，执行同步操作
    for(uint8_t i = 0; i < 3; i++)
    {        
        StationList[i].SendFrame.len = 8;
        MakeIOMessage(&StationList[i].SendFrame, MacList[i], pCommand->pData, pCommand->len);        
        DeviceNetSendData(&StationList[i].SendFrame);
        StationList[i].SendFrame.waitFlag = TRUE;     
       //TODO:添加适当延时        
    } 
    DisableNormalCycle();  //轮询置位False，暂停一般轮询发送
    return 0;    
    
}
/**
 * 同步操作应答 
 *
 * @param  pStation   对应站点 
 * @param  pbuff      接收缓冲数据
 * @param  len        接收
 * @return            非0-错误代码，0--正确
 */
uint8_t  SynchronousOperationReadyACK(struct DefStationElement* pStation, BYTE* pbuff, BYTE len)
{
    uint8_t i = 0;
    //2S，超时判断
   if(IsOverTime(SynchronousState.startTime, SynchronousState.delayTime))
   {
       SynchronousState.readyAction = TRUE; 
       CLREA_WORK_MODE(WORK_READY_SYN);//清除 同步预制工作模式
      
        //恢复正常工作模式
       EnableNormalCycle();
       return 0xA1;
   }
   
   //判断是否发送等待
   if(pStation->SendFrame.waitFlag == 0)
   {
       return 0xA2;
   }
   //检测是否错误
   if (pbuff[0] == 0x14)
   {
       //此处取消本次任务
       
       return 0xA3;
   }
   //正常应答数据仅仅是ID号与0x80相与，其它相同
   //比较长度
   if(pStation->SendFrame.len != len)
   {
       return 0xA4;
   }
   //比较是否为ID应答
   if(pStation->SendFrame.pBuffer[0] != (pbuff[0] | 0x80))
   {
       return 0xA5;
   }
   for(i = 1; i < len; i++)
   {
       if(pStation->SendFrame.pBuffer[0] != pbuff[0])
       {
           return 0xA6;
       }
   }
   //属于正常应答模式，置位就绪等待bit 
   if(pStation->StationInformation.macID == MacList[0])
   {
       SynchronousState.readyBit |= 0x0001;
   }
   else  if (pStation->StationInformation.macID == MacList[SYNCHRONIZATION_INDEX])
   {
       SynchronousState.readyBit |= 0x0008;
   }
   //DSP与永磁控制A准备就绪
   if (SynchronousState.readyBit == 0x0009) //此时说明可以接收合闸同步执行指令
   {
        SynchronousState.readyAction = TRUE; 
        CLREA_WORK_MODE(WORK_READY_SYN);//清除 同步预制工作模式 
        SET_WORK_MODE(WORK_ACTION_SYN);//  置位同步执行模式 
        SynchronousState.startTime = g_MsTicks; //重新设计超时时间
        SynchronousState.delayTime = 2000;
        SynchronousState.readyBit = 0;
     
       return 0;
   }
   return 0;
   
}


/**
 * 同步操作执行
 *
 * @param   pData     DSP命令控制字
 * @param   pCommand  永磁控制器命令字
 * @return            非0-错误代码，0--正确
 */
uint8_t  SynchronousOperationAction(PointUint8* pData, PointUint8* pCommand)
{
    //首先判断是否为准备状态，若不是则拒绝执行
    if (SynchronousState.readyAction != TRUE)
    {
        return 0xA1;
    }
    
    uint8_t result = 0;
     //Step 2:  生成针对同步控制器IO报文    
    if (pData->len < 4)//命令控制字至少4个字节
    {
        return  0xA2;
    }
    //同步控制器预制命令(DSP)
    StationList[SYNCHRONIZATION_INDEX].SendFrame.len = 8;
    result = MakeIOMessage(&StationList[SYNCHRONIZATION_INDEX].SendFrame, MacList[SYNCHRONIZATION_INDEX], pData->pData, pData->len);  
    if (result == FALSE)
    {
        return 0xA3;
    }
    StationList[SYNCHRONIZATION_INDEX].SendFrame.waitFlag = TRUE;
    StationList[SYNCHRONIZATION_INDEX].StationInformation.enable = FALSE; //轮询置位False，暂停一般轮询发送
    
    DeviceNetSendData(&StationList[SYNCHRONIZATION_INDEX].SendFrame);
   
    return 0;
    
}
/**
 * 同步操作执行应答
 *
 * @param   pStation   DSP命令控制字
 * @param   pbuff      指向缓冲数据的指针
 * @param   len        缓冲区长度
 * @return             非0-错误代码，0--正确
 */
uint8_t  SynchronousOperationActionACK(struct DefStationElement* pStation, BYTE* pbuff, BYTE len)
{
//    uint8_t result = 0;
     uint8_t i = 0;
    //2S，超时判断
   if(IsOverTime(SynchronousState.startTime, SynchronousState.delayTime))
   {
       SynchronousState.readyAction = FALSE;
       g_WorkMode &= 0xFFCF; //清除工作模式  
       //使能一般循环
       EnableNormalCycle();
       return 0xA1;
   }
   
   //判断是否发送等待
   if(pStation->SendFrame.waitFlag == 0)
   {
       return 0xA2;
   }
   //检测是否错误
   if (pbuff[0] == 0x14)
   {
       //此处取消本次任务
       
       return 0xA3;
   }
   //正常应答数据仅仅是ID号与0x80相与，其它相同
   //比较长度
   if(pStation->SendFrame.len != len)
   {
       return 0xA4;
   }
   //比较是否为ID应答
   if(pStation->SendFrame.pBuffer[0] != (pbuff[0] | 0x80))
   {
       return 0xA5;
   }
   for(i = 1; i < len; i++)
   {
       if(pStation->SendFrame.pBuffer[0] != pbuff[0])
       {
           return 0xA6;
       }
   }
    SynchronousState.readyAction = FALSE;    
    CLREA_WORK_MODE(WORK_ACTION_SYN);//清除 同步工作模式 
        
        //使能一般循环
    EnableNormalCycle();
   //TODO:正常执行
   
   return 0;
    
}
/**
 * 校验工作模式
 */
void CheckDeviceNetWorkMode(void)
{
    uint8_t result = 0;
    //同步--预制模式
    if(WORK_MODE_RUNING(WORK_READY_SYN))
    {
        result = IsOverTime(SynchronousState.startTime, SynchronousState.delayTime);
        if(result != 0)//超时
        {            
            CLREA_WORK_MODE(WORK_READY_SYN);//清除标志位
             //使能一般循环
            EnableNormalCycle(); 
            SynchronousState.readyAction = FALSE;            
            //TODO:发送超时信息
        }
    }
    //同步--执行模式
    if(WORK_MODE_RUNING(WORK_ACTION_SYN))
    {
        result = IsOverTime(SynchronousState.startTime, SynchronousState.delayTime);
        if(result != 0)//超时
        {
             CLREA_WORK_MODE(WORK_ACTION_SYN);//清除标志位
            SynchronousState.readyAction = FALSE;    
             //使能一般循环
            EnableNormalCycle();           
            //TODO:发送超时信息
        }
    }
    
}

