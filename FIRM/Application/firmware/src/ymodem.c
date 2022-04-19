
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
#include "ymodem.h"

#define ymodemUartIndex             SYS_CONSOLE_INDEX_0
#define YMODEM_MAX_ERRS             10

#define YMODEM_TIMEOUT_SEC          30
#define YMODEM_CHECK_INTERVAL_SEC   3
#define POLY                        0x1021
#define NowTick()                   (uint64_t)CORETIMER_CounterGet()

#define isCapLetter(c)              (((c) >= 'A') && ((c) <= 'F'))
#define isLcLetter(c)               (((c) >= 'a') && ((c) <= 'f'))
#define is0_9(c)                    (((c) >= '0') && ((c) <= '9'))
#define isValidHexX(c)              (isCapLetter(c) || isLcLetter(c) || is0_9(c))
#define isValidDec(c)               is0_9(c)
#define convertDec(c)               (c - '0')

#define convertHexAlpha(c)          (isCapLetter(c) ? ((c) - 'A'+10) : ((c) - 'a'+10))
#define convertHex(c)               (is0_9(c) ? ((c) - '0') : convertHexAlpha(c))

enum
{
    YMODEM_NULL=0x00,
    YMODEM_SOH=0x01,
    YMODEM_STX=0x02,    
    YMODEM_EOT=0x04,
    YMODEM_ACK=0x06,    
    YMODEM_NAK=0x15,
    YMODEM_CA =0x18,
    YMODEM_EOF=0x1A,
    YMODEM_CRD=0x24,
    YMODEM_ABRT1=0x41,
    YMODEM_CRC16=0x43,
    YMODEM_ABRT2=0x61,
    YMODEM_NEG  =0xFF
};


SYS_CONSOLE_HANDLE cHandle;

uint8_t     ymodemState=YMODEM_STATE_INIT;
uint8_t     ymodemStateOld=YMODEM_STATE_INIT;
ssize_t     sendFileSize;
ssize_t     sendFileMaxSize;
ssize_t     sendFilePos;
ssize_t     sendBlockNo;
uint32_t    *recieveFileSize;
ssize_t     packetPos;
uint8_t     *sendFileBuff;
uint8_t     *sendFileName;
uint32_t    timeoutSec;
uint32_t    checkIntervalSec=0;
uint64_t    timeoutIntervalSec=0;

uint8_t     packetData[YMODEM_PACKET_1K_SIZE + YMODEM_PACKET_DATA_INDEX + YMODEM_PACKET_TRAILER_SIZE];
uint8_t     fileNameStr[YMODEM_FILE_NAME_LENGTH];
uint8_t     fileSizeStr[YMODEM_FILE_SIZE_LENGTH];

static void ymodemWrite(ssize_t size, uint8_t *str)
{
    Console_UART_Write(ymodemUartIndex, str, size);
}

static void ymodemPut(uint8_t str)
{
    uint8_t strArry[1];
    strArry[0]=str;
    ymodemWrite(1, strArry);
}

static void clearBuffer()
{
    char buffer[1];
    while(SYS_CONSOLE_Read( cHandle, buffer, sizeof(buffer) )!=0);
}

static void valueToString(uint32_t val, uint8_t *str )
{
    uint32_t i=0;
    uint32_t divider = 1000000000;
    uint32_t pos = 0;
    uint32_t status = 0;

    for (i = 0; i < 10; i++)
    {
        str[pos++] = (val / divider) + 48;

        val = val % divider;
        divider /= 10;
        if ((str[pos-1] == '0') & (status == 0))
        {
          pos = 0;
        }
        else
        {
          status++;
        }
    }
}

static bool stringToValue(uint8_t *str, uint32_t *num)
{
    uint32_t i = 0;
    uint32_t val = 0;
    bool res = false;
    
    if ((str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X')))
    {
        i = 2;
        while ( ( i < 11 ) && ( str[i] != '\0' ) )
        {
            if (isValidHexX(str[i]))
            {
                val = (val << 4) + convertHex(str[i]);
            }
            else
            {
                res = 0;
                break;
            }
            i++;
        }
        
        if (str[i] == '\0')
        {
            *num = val;
            res = 1;
        }
    }
    else
    {
        while ( ( i < 11 ) && ( res != 1 ) )
        {
            if (str[i] == '\0')
            {
                *num = val;
                res = 1;
            }
            else if (((str[i] == 'k') || (str[i] == 'K')) && (i > 0))
            {
                val = val << 10;
                *num = val;
                res = 1;
            }
            else if (((str[i] == 'm') || (str[i] == 'M')) && (i > 0))
            {
                val = val << 20;
                *num = val;
                res = 1;
            }
            else if (isValidDec(str[i]))
            {
                val = val * 10 + convertDec(str[i]);
            }
            else
            {
                res = 0;
                break;
            }
            i++;
        }
    }
    return res;
}

static uint16_t ymodemCRC16( uint8_t *buf, size_t len)
{
    uint16_t crc=0;
    size_t length=len;
    while (length--)
    {
        crc ^= *buf++ << 8;
        crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
        crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
        crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
        crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
        crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
        crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
        crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
        crc = crc & 0x8000 ? (crc << 1) ^ POLY : crc << 1;
    }
    return crc & 0xffff;
}

static void makeHeaderPacket()
{
    uint32_t i,j;
    uint8_t fileLenStr[10]={'\0'};
    
    packetData[YMODEM_PACKET_START_INDEX]=YMODEM_SOH;
    packetData[YMODEM_PACKET_NUMBER_INDEX]=YMODEM_NULL;
    packetData[YMODEM_PACKET_CNUMBER_INDEX]=YMODEM_NEG;
    
    for (i = 0; (sendFileName[i] != '\0') && (i < YMODEM_FILE_NAME_LENGTH); i++)
    {
        packetData[ i + YMODEM_PACKET_DATA_INDEX ] = sendFileName[i];
    }
    packetData[ i + YMODEM_PACKET_DATA_INDEX ] = 0x00;
    
    valueToString(sendFileSize,fileLenStr);
    
    i = i + YMODEM_PACKET_DATA_INDEX + 1;
    j=0;
    while (fileLenStr[j] != '\0')
    {
        packetData[i++] = fileLenStr[j++];
    }
     
    for (j = i; j < YMODEM_PACKET_SIZE + YMODEM_PACKET_DATA_INDEX; j++)
    {
        packetData[j] = 0;
    }
}

static void makeDataPacket()
{
    uint32_t i=0;
    uint32_t packetSize=0;
    uint32_t size=0;
    
    if(sendFileSize>YMODEM_PACKET_1K_SIZE)
    {
        packetSize=YMODEM_PACKET_1K_SIZE;
        packetData[YMODEM_PACKET_START_INDEX]=YMODEM_STX;        
    }
    else
    {
        packetSize=YMODEM_PACKET_SIZE;
        packetData[YMODEM_PACKET_START_INDEX]=YMODEM_SOH;       
    }
    
    if(sendFileSize<packetSize)
        size=sendFileSize;
    else
        size=packetSize;
    
    packetData[YMODEM_PACKET_NUMBER_INDEX]=sendBlockNo;
    packetData[YMODEM_PACKET_CNUMBER_INDEX]=~sendBlockNo;

    for(i=YMODEM_PACKET_DATA_INDEX; i< size+YMODEM_PACKET_DATA_INDEX; i++ )
        packetData[i]=*sendFileBuff++;
    
    if(size<=packetSize)
    {
        for(i= size+YMODEM_PACKET_DATA_INDEX; i<packetSize+YMODEM_PACKET_DATA_INDEX; i++ )
            packetData[i]=YMODEM_EOF;
    }
    
    
}


static void makeEmptyPacket()
{
    uint32_t i=0;
    
    packetData[YMODEM_PACKET_START_INDEX] = YMODEM_SOH;
    packetData[YMODEM_PACKET_NUMBER_INDEX] = YMODEM_NULL;
    packetData[YMODEM_PACKET_CNUMBER_INDEX] = YMODEM_NEG;
    
    for(i=YMODEM_PACKET_DATA_INDEX; i< YMODEM_PACKET_SIZE+YMODEM_PACKET_DATA_INDEX; i++ )
    {
        packetData[i]=YMODEM_NULL;
    } 
    
}

static void getFileInfo(uint8_t *buff)
{
    uint16_t i=0;
    uint8_t *buffStr;
    uint32_t size=0;
    
    //Get File Name
    buffStr=buff+YMODEM_PACKET_DATA_INDEX;
    while( (*buffStr!='\0') && (i<YMODEM_FILE_NAME_LENGTH) )
    {
        fileNameStr[i++]=*buffStr++;
    }
    fileNameStr[i++] = *buffStr++;
    
    strcpy((char*)sendFileName,(char*)fileNameStr);
    
    //Get File Length
    i=0;
    while( (*buffStr!=0x20) && (i<YMODEM_FILE_NAME_LENGTH) )
    {
        fileSizeStr[i++]=*buffStr++;
    }
    fileSizeStr[i++] = '\0';
    stringToValue(fileSizeStr,&size);
    *recieveFileSize=size;
    
}

static void getBodyData(uint8_t *buff, uint32_t packetSize )
{
    uint8_t *buffStr;
    
    //Get Body
    buffStr=buff+YMODEM_PACKET_DATA_INDEX;
    
    if(sendFilePos<sendFileMaxSize)
    {
        memcpy(&sendFileBuff[sendFilePos],buffStr,packetSize) ;       
        sendFilePos=sendFilePos+packetSize;
    }
}

static void clearPacketData(bool bufferClear)
{
    packetPos=0;
    memset(packetData,0,sizeof(packetData));
    if(bufferClear==true)
    {
        clearBuffer();
    }   
}

void YmodemSendInit(ssize_t sendSize, uint8_t *sendBuff, uint8_t *sendName)
{
    ymodemState = YMODEM_STATE_INIT;
    sendFileSize = sendSize;
    sendFileBuff = sendBuff;
    sendFileName = sendName;
    sendFilePos = 0;
    sendBlockNo = 1;
    timeoutIntervalSec=(uint64_t)CORETIMER_FrequencyGet()*YMODEM_TIMEOUT_SEC + NowTick();
} 


void YmodemRecieveInit(uint8_t *buff, ssize_t buffMaxSize, uint8_t *recieveName, uint32_t *recieveSize)
{
    ymodemState = YMODEM_STATE_INIT;
    sendFileMaxSize = buffMaxSize;
    sendFileBuff = buff;
    sendFileName = recieveName;
    recieveFileSize=recieveSize;
    sendFileSize = 0;
    sendFilePos = 0;
    sendBlockNo = 1;
} 

uint8_t YmodemGetState()
{
    return ymodemState;
}

void YmodemSendProcess(ssize_t consoleBuffSize, uint8_t *consoleBuff)
{

    ssize_t i=0;
    uint16_t crc; 
    uint16_t err=0;
    static uint32_t packetSize=0;
    
    for(i=0; i<consoleBuffSize; i++)
    {
        uint8_t recieve=*(consoleBuff + i);
        
        switch(ymodemState)
        {
            case YMODEM_STATE_INIT:
                ymodemState=YMODEM_STATE_HEADER;
                break;
                
            case YMODEM_STATE_HEADER:
                makeHeaderPacket();
                ymodemWrite( YMODEM_PACKET_SIZE + YMODEM_PACKET_HEADER_SIZE, packetData);
                crc=ymodemCRC16(&packetData[YMODEM_PACKET_DATA_INDEX], YMODEM_PACKET_SIZE);
                ymodemPut(crc>>8);
                ymodemPut(crc&0xFF);
                ymodemState=YMODEM_STATE_ACK1;                
                break;        
                
            case YMODEM_STATE_ACK1:
                if(recieve==YMODEM_ACK)
                {
                    ymodemState=YMODEM_STATE_BODY;
                    err=0;
                }
                else if(recieve==YMODEM_CA)
                {
                    ymodemState=YMODEM_STATE_ABRT;   
                }             
                else if(recieve==YMODEM_CRC16)
                {
                    ymodemState=YMODEM_STATE_HEADER;
                    err=0;
                }
                else if( recieve!='\0' )
                {
                    err++;
                    if(err>YMODEM_MAX_ERRS)
                        ymodemState=YMODEM_STATE_ERR;   
                }
                break;
                
            case YMODEM_STATE_BODY:
                if(sendFileSize>0)
                {
                    err=0;
                    makeDataPacket();
                    packetSize=sendFileSize>=YMODEM_PACKET_1K_SIZE? YMODEM_PACKET_1K_SIZE:YMODEM_PACKET_SIZE;
                    ymodemWrite( packetSize + YMODEM_PACKET_HEADER_SIZE, packetData);
                    crc=ymodemCRC16(&packetData[YMODEM_PACKET_DATA_INDEX], packetSize);
                    ymodemPut(crc>>8);
                    ymodemPut(crc&0xFF);
                    ymodemState=YMODEM_STATE_ACK2;                    
                }
                else
                {
                    ymodemState=YMODEM_STATE_EOT;   
                }
                break;                
                
            case YMODEM_STATE_ACK2:
                if(recieve==YMODEM_ACK)
                {
                    if(sendFileSize>packetSize)
                    {
                        //sendFileBuff+=packetSize;
                        sendFileSize-=packetSize;
                        sendBlockNo++;
                    }
                    else
                    {
                        //sendFileBuff+=packetSize;
                        sendFileSize = 0;
                    }                    
                    ymodemState=YMODEM_STATE_BODY;   
                }
                else if( recieve!='\0' )
                {
                    err++;
                    if(err>YMODEM_MAX_ERRS)
                        ymodemState=YMODEM_STATE_ERR;   
                }
                break;
                
            case YMODEM_STATE_EOT:
                ymodemPut(YMODEM_EOT);                
                if(recieve==YMODEM_ACK)
                {
                    ymodemState=YMODEM_STATE_SOH;
                    err=0;   
                }
                else if(recieve==YMODEM_CA)
                {
                    ymodemState=YMODEM_STATE_ABRT;
                    err=0; 
                }
                else if( recieve!='\0' )
                {
                    err++;
                    if(err>YMODEM_MAX_ERRS)
                        ymodemState=YMODEM_STATE_ERR;   
                }
                break;                
                
            case YMODEM_STATE_SOH:
                makeEmptyPacket();               
                ymodemWrite( YMODEM_PACKET_SIZE + YMODEM_PACKET_HEADER_SIZE, packetData);
                crc=ymodemCRC16(&packetData[YMODEM_PACKET_DATA_INDEX], YMODEM_PACKET_SIZE);
                ymodemPut(crc>>8);
                ymodemPut(crc&0xFF);
                ymodemState=YMODEM_STATE_ACK3;     
                break;
                
            case YMODEM_STATE_ACK3:
                if(recieve==YMODEM_CA)
                {
                    ymodemState=YMODEM_STATE_ABRT;   
                }
                else if( recieve!='\0' )
                {
                    ymodemState=YMODEM_STATE_END;
                }               
                break;
            
            case YMODEM_STATE_END:
                break;
            case YMODEM_STATE_ABRT:
                break;
            case YMODEM_STATE_ERR:
                break;
            default:
                break;
        }
    }
    
    if(ymodemState!=ymodemStateOld)
    {
        ymodemStateOld=ymodemState;
        timeoutIntervalSec=(uint64_t)CORETIMER_FrequencyGet()*YMODEM_TIMEOUT_SEC +(uint64_t)NowTick();
    }
    else if( NowTick() > timeoutIntervalSec)
    {
        ymodemState=YMODEM_STATE_TIMEOUT;
    }
    
}

void YmodemRecieceProcess(ssize_t consoleBuffSize, uint8_t *consoleBuff)
{
    static uint32_t packetSize=0;

    if(consoleBuffSize>0)
    {
        if((packetPos+consoleBuffSize)<=sizeof(packetData))
        {
            memcpy(&packetData[packetPos],consoleBuff,consoleBuffSize);
            packetPos=packetPos+consoleBuffSize;
        }
    }
    
    char recieve=packetData[0];
    
    switch(ymodemState)
    {
        case YMODEM_STATE_INIT:

            if( NowTick() > checkIntervalSec )
            {
                ymodemPut(YMODEM_CRC16);
                checkIntervalSec=CORETIMER_FrequencyGet()*YMODEM_CHECK_INTERVAL_SEC +NowTick();
                clearPacketData(false);
            }
            else
            {
                if(recieve==YMODEM_SOH)
                {
                    packetSize=YMODEM_PACKET_SIZE;
                    ymodemState=YMODEM_STATE_HEADER;
                }
                else if(recieve==YMODEM_STX)
                {
                    packetSize=YMODEM_PACKET_1K_SIZE;
                    ymodemState=YMODEM_STATE_HEADER;
                }
            }
            break;

        case YMODEM_STATE_HEADER:
            
            if( NowTick() > checkIntervalSec )
            {
                ymodemPut(YMODEM_CRC16);
                checkIntervalSec=CORETIMER_FrequencyGet()*YMODEM_CHECK_INTERVAL_SEC +NowTick();
            }            

            if(packetPos>=(packetSize+YMODEM_PACKET_OVERHEAD_SIZE))
            {
                
                if (packetData[YMODEM_PACKET_NUMBER_INDEX] != ((packetData[YMODEM_PACKET_CNUMBER_INDEX]) ^ 0xFF))
                {
                    ymodemState=YMODEM_STATE_INIT;
                    packetPos=0;
                    break;
                }
                else
                {
                
                    uint16_t crc = ymodemCRC16(&packetData[YMODEM_PACKET_HEADER_SIZE], packetSize+YMODEM_PACKET_TRAILER_SIZE );
                    if(crc==0)
                    {
                        getFileInfo(packetData);
                        ymodemState=YMODEM_STATE_ACK1;
                        clearPacketData(true);
                        ymodemPut(YMODEM_ACK);
                        ymodemPut(YMODEM_CRC16);
                    }
                    else
                    {
                        ymodemState=YMODEM_STATE_INIT;
                        packetPos=0;
                    }
                }
            }
                        
            break;
            
        case YMODEM_STATE_ACK1:            
           
            if( NowTick() > checkIntervalSec )
            {
                if(sendBlockNo==1)
                {
                    ymodemPut(YMODEM_ACK);
                    ymodemPut(YMODEM_CRC16);
                }
                else
                {
                    clearPacketData(true);
                    ymodemPut(YMODEM_NAK);
                }
                checkIntervalSec=CORETIMER_FrequencyGet()*YMODEM_CHECK_INTERVAL_SEC +NowTick();
            }
            else
            {
                if(recieve==YMODEM_SOH)
                {
                    packetSize=YMODEM_PACKET_SIZE;
                    ymodemState=YMODEM_STATE_BODY;
                }
                else if(recieve==YMODEM_STX)
                {
                    packetSize=YMODEM_PACKET_1K_SIZE;
                    ymodemState=YMODEM_STATE_BODY;
                }
            }
            
            break;
            
        case YMODEM_STATE_BODY: 

            if(packetPos==0)
                break;
            
            if(packetPos>=(packetSize+YMODEM_PACKET_OVERHEAD_SIZE))
            {
                
                if(packetData[YMODEM_PACKET_NUMBER_INDEX]!=sendBlockNo)
                {
                    ymodemState=YMODEM_STATE_ACK1;
                    clearPacketData(true);
                    ymodemPut(YMODEM_NAK);
                    checkIntervalSec=CORETIMER_FrequencyGet()*YMODEM_CHECK_INTERVAL_SEC +NowTick();
                    break;
                }
                
                if (packetData[YMODEM_PACKET_NUMBER_INDEX] != ((packetData[YMODEM_PACKET_CNUMBER_INDEX]) ^ 0xFF))
                {
                    ymodemState=YMODEM_STATE_ACK1;
                    clearPacketData(true);
                    ymodemPut(YMODEM_NAK);
                    break;
                }
                else
                {                                    
                    uint16_t crc = ymodemCRC16(&packetData[YMODEM_PACKET_HEADER_SIZE], packetSize+YMODEM_PACKET_TRAILER_SIZE);
                    if(crc==0)
                    {
                        getBodyData(packetData,packetSize);
                        ymodemState=YMODEM_STATE_BODY;
                        clearPacketData(true);
                        sendBlockNo++;
                        
                        if(sendFilePos>*recieveFileSize)
                        {
                            ymodemState=YMODEM_STATE_SOH;//YMODEM_STATE_EOT;
                            ymodemPut(YMODEM_ACK);
                            ymodemPut(YMODEM_NAK);
                            ymodemPut(YMODEM_ACK);
                            ymodemPut(YMODEM_CRC16);
                            checkIntervalSec=0;                            
                        }
                        else
                        {
                            ymodemPut(YMODEM_ACK);
                            ymodemPut(YMODEM_NAK);
                        }
                    }
                    else
                    {
                        ymodemState=YMODEM_STATE_ACK1;
                        packetPos=0;
                        ymodemPut(YMODEM_NAK);
                    }
                }
            }
            break;
            
        case YMODEM_STATE_EOT:
            
            if( NowTick() > checkIntervalSec )
            {                               
                clearPacketData(false);
                ymodemPut(YMODEM_ACK);
                ymodemPut(YMODEM_CRC16);
                checkIntervalSec=CORETIMER_FrequencyGet()*YMODEM_CHECK_INTERVAL_SEC +NowTick();
                
            }
            else
            {
                if(recieve==YMODEM_SOH)
                {
                    packetSize=YMODEM_PACKET_SIZE;
                    ymodemState=YMODEM_STATE_SOH;
                }
                else if(recieve==YMODEM_STX)
                {
                    packetSize=YMODEM_PACKET_1K_SIZE;
                    ymodemState=YMODEM_STATE_SOH;
                }             
            
            }            
            break;
            
            
        case YMODEM_STATE_SOH:
            
            if(packetPos>=(packetSize+YMODEM_PACKET_OVERHEAD_SIZE))
            {                
                if(packetData[YMODEM_PACKET_NUMBER_INDEX]!=0)
                {
                    ymodemState=YMODEM_STATE_SOH;
                    sendBlockNo=1;
                    clearPacketData(false);
                    ymodemPut(YMODEM_ACK);
                    ymodemPut(YMODEM_CRC16);
                    checkIntervalSec=CORETIMER_FrequencyGet()*YMODEM_CHECK_INTERVAL_SEC +NowTick();
                    break;
                }                
                
                if (packetData[YMODEM_PACKET_NUMBER_INDEX] != ((packetData[YMODEM_PACKET_CNUMBER_INDEX]) ^ 0xFF))
                {
                    ymodemState=YMODEM_STATE_ERR;
                    ymodemPut(YMODEM_ACK);
                    packetPos=0;
                }
                else
                {
                    uint16_t crc = ymodemCRC16(&packetData[YMODEM_PACKET_HEADER_SIZE], packetSize+YMODEM_PACKET_TRAILER_SIZE);
                    if(crc==0)
                    {                        
                        ymodemState=YMODEM_STATE_END;
                        ymodemPut(YMODEM_ACK);
                        packetPos=0;
                    }
                    else
                    {
                        ymodemState=YMODEM_STATE_ERR;
                        ymodemPut(YMODEM_ACK);
                        packetPos=0;
                    }   
                }
            }
            else
            {
                if(recieve==YMODEM_SOH)
                {
                    packetSize=YMODEM_PACKET_SIZE;
                    ymodemState=YMODEM_STATE_SOH;
                }
                else if(recieve==YMODEM_STX)
                {
                    packetSize=YMODEM_PACKET_1K_SIZE;
                    ymodemState=YMODEM_STATE_SOH;
                }
                else if(recieve==YMODEM_EOT)
                {
                    clearPacketData(false);
                    ymodemState=YMODEM_STATE_SOH;
                }
            }           
            break;
            
        case YMODEM_STATE_END:
            break;
        case YMODEM_STATE_ABRT:
            break;
        case YMODEM_STATE_ERR:
            break;
        default:
            break;
    }
    
    if(ymodemState!=ymodemStateOld)
    {
        ymodemStateOld=ymodemState;
        timeoutIntervalSec=(uint64_t)CORETIMER_FrequencyGet()*YMODEM_TIMEOUT_SEC +NowTick();
    }
    else if( NowTick() > timeoutIntervalSec)
    {
        ymodemState=YMODEM_STATE_TIMEOUT;
    }
    
}