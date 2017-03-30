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

#endif
