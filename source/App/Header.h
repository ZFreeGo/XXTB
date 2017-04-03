#ifndef __HEADER_H
#define __HEADER_H

/**
 *带数据长度的数据指针
 */

typedef struct TagPointUint8
{
    uint8_t* pData; //指向数据指针
    uint8_t len; //指向数据的长度
}PointUint8;


#define UP_ADDRESS 0x1A

//上位机到下位机定义
#define CAN_MESSAGE_TO_DOWN  1  //CAN转发数据包通讯 到下位机
#define CAN_MESSAGE_TO_UP    2  //CAN转发数据包通讯 到上位机

#define DOWN_CONCTROL        0x20  //下位机控制     

#endif
