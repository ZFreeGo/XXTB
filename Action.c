#include "Action.h"
#include "RS485.h"

void ExecuteFunctioncode(frameRtu* pRtu)
{
    if (pRtu->completeFlag == TRUE)
    {
      
        
        switch(pRtu->funcode)
        {
            case RESET_MCU:
            {
               
                break;
            }
             default :
             {
                 
                 UART2_SendData("TEST", 5);
                 break;
             }
        }

        pRtu->completeFlag = FALSE;
    }
}
