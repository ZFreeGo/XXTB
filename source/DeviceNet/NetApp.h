#ifndef __NETAPP_H
#define __NETAPP_H

#include <stdint.h> 
#include "DeviceNet.h"


extern void CheckDeviceNetWorkMode(void);
extern void InitNetApp(void);
extern void StationStatusCycleService(struct DefStationElement* pStation,  BYTE* pbuff, BYTE len);

#endif
