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
#include "serialcam.h"

#define serialCamUartIndex SYS_CONSOLE_INDEX_0


const uint8_t reset_cmd[4]  =   {0x56, 0x00, 0x26, 0x00};
const uint8_t reset_ack[4]  =   {0x76, 0x00, 0x26, 0x00};
const uint8_t capture_cmd[5]=   {0x56, 0x00, 0x36, 0x01, 0x00};
const uint8_t capture_ack[5]=   {0x76, 0x00, 0x36, 0x00, 0x00};
const uint8_t read_len_cmd[5]=  {0x56, 0x00, 0x34, 0x01, 0x00};
const uint8_t read_len_ack[9]=  {0x76, 0x00, 0x34, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00};
const uint8_t read_data_cmd[16]={0x56, 0x00, 0x32, 0x0C, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A};//0x0A or0xFF
const uint8_t read_data_ack[11]={0x76, 0x00, 0x32, 0x00, 0x00, 0x76, 0x00, 0x32, 0x00, 0x00, 0x00};


static void serialCamWrite(ssize_t size, uint8_t *str)
{
    Console_UART_Write(serialCamUartIndex, str, size);
}

void SerialCamAckDataLength(uint32_t dataLength)
{
    uint8_t sData[9];
    
    memcpy(sData,read_len_ack,sizeof(read_len_ack));
    sData[7]=(uint8_t)((dataLength>>8)&0xFF);
    sData[8]=(uint8_t)((dataLength>>0)&0xFF);
    serialCamWrite(sizeof(sData),sData);
}

void SerialCamAckData(uint32_t *dataBuff, uint32_t startAddr, uint32_t dataSize)
{


}
