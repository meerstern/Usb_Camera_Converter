
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "app.h"
#include "device.h"
#include "system/debug/sys_debug.h"
#include "system/console/src/sys_console_uart.h"
#include "peripheral/coretimer/plib_coretimer.h"
#include "xmodem.h"

#define xmodemUartIndex SYS_CONSOLE_INDEX_0

#define XMODEM_NACK     0x15
#define XMODEM_SOH      0x01
#define XMODEM_EOT      0x04
#define XMODEM_ACK      0x06

#define XMODEM_TIMEOUT_SEC  30

#define NowTick()      (uint64_t)CORETIMER_CounterGet()

uint8_t     xmodemState=XMODEM_STATE_INIT;
ssize_t     sendFileSize;
ssize_t     sendFilePos;
uint8_t     *sendFileBuff;
uint64_t    timeoutSec;

static void xmodemWrite(ssize_t size, uint8_t *str)
{
    Console_UART_Write(xmodemUartIndex, str, size);
}

static void xmodemPut(uint8_t str)
{
    uint8_t strArry[1];
    strArry[0]=str;
    xmodemWrite(1, strArry);
}


void XmodemInit(ssize_t sendSize, uint8_t *sendBuff)
{
    xmodemState = XMODEM_STATE_INIT;
    sendFileSize = sendSize;
    sendFileBuff = sendBuff;
    sendFilePos = 0;
}

uint8_t XmodemGetState()
{
    return xmodemState;
}

void XmodemSendProcess(ssize_t consoleBuffSize, uint8_t *consoleBuff)
{

    ssize_t i=0;
    ssize_t j=0;
    uint8_t block=0;
    uint8_t checkSum=0;
    
    for(i=0; i<consoleBuffSize; i++)
    {
        char recieve=*(consoleBuff + i);
        
        switch(xmodemState)
        {
            case XMODEM_STATE_INIT:
                xmodemState=XMODEM_STATE_NACK;
                timeoutSec=CORETIMER_FrequencyGet()*XMODEM_TIMEOUT_SEC +NowTick();
                break;

            case XMODEM_STATE_NACK:
                if(recieve==XMODEM_NACK)
                {
                    xmodemState=XMODEM_STATE_SOH;
                    timeoutSec=(uint64_t)CORETIMER_FrequencyGet()*XMODEM_TIMEOUT_SEC +NowTick();
                }
                break;

            case XMODEM_STATE_SOH:
                if(sendFilePos < sendFileSize )
                {
                    xmodemPut(XMODEM_SOH);
                    block = sendFilePos/128 + 1;
                    xmodemPut(block);
                    xmodemPut(block^0xFF);
                    checkSum=0;
                    for( j=0; j<128; j++ )
                    {
                        uint8_t s = *(sendFileBuff +sendFilePos +j);
                        checkSum = s+checkSum;
                        xmodemPut(s);
                    }
                    xmodemPut(checkSum);
                }
                else
                {
                    xmodemPut(XMODEM_EOT);
                }
                xmodemState=XMODEM_STATE_ACK;
                timeoutSec=(uint64_t)CORETIMER_FrequencyGet()*XMODEM_TIMEOUT_SEC +NowTick();
                break;

            case XMODEM_STATE_ACK:
                if(recieve==XMODEM_ACK)
                {
                    if(sendFilePos < sendFileSize )
                    {
                        sendFilePos=sendFilePos+128;
                         xmodemState=XMODEM_STATE_SOH;
                    }
                    else
                    {
                        xmodemState=XMODEM_STATE_END;
                    }
                    
                }
                break;
            default:
                break;

        }
        
        if( NowTick() > timeoutSec )
        {
            xmodemState=XMODEM_STATE_TIMEOUT;
        }
    
    }
    
    
  
}