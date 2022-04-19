#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "app.h"
#include "device.h"
#include "rtc_rv8803.h"
#include "system/debug/sys_debug.h"
#include "system/fs/sys_fs.h"
#include "peripheral/i2c/master/plib_i2c2_master.h"
#include "typedefs.h"
#include "peripheral/uart/plib_uart_common.h"
#include "peripheral/uart/plib_uart2.h"

#define HEX2DEC(X)  (X/16*10+X%16)
#define DEC2HEX(X)  (X/10*16+X%10)

#define UART_MODE_REG   U2MODE
#define UART_BAUD_REG   U2BRG

void GetDateTime()
{
    uint8_t rdat[7];
    uint8_t wdat[1]={0};
    uint8_t sec,min,hour,week,day,month,year;
    char weekStr[5]={'\0'};

    I2C2_WriteRead(RV8803C7_ADDR, wdat, 1, rdat, 7);
    while(I2C2_IsBusy()==true);
    
    sec = rdat[0];
    min = rdat[1];
    hour = rdat[2];
    week = rdat[3];
    day = rdat[4];
    month = rdat[5];
    year = rdat[6];
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "\t\tRTC Time:%02X/%02X/%02X %02X:%02X:%02X\r\n",
        year, month, day, hour, min, sec); 
    return;
    
    switch(week)
    {
        case 1:
            strcpy(weekStr,"sun");
            break;
        case 2:
            strcpy(weekStr,"mon");
            break;
        case 4:
            strcpy(weekStr,"tue");
            break;
        case 8:
            strcpy(weekStr,"wed");
            break; 
        case 16:
            strcpy(weekStr,"thu");
            break;
        case 32:
            strcpy(weekStr,"fri");
            break; 
        case 64:
            strcpy(weekStr,"sat");
            break; 
        default:
            break;
    }
    
    SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "20%02X/%02X/%02X (%s) %02X:%02X:%02X\r\n",
        year, month, day, weekStr, hour, min, sec); 

}


void SetRtcRam(uint8_t addr, uint8_t data)
{
    uint8_t wdat[2]={0};
    wdat[0]=addr;
    wdat[1]=data;
    I2C2_Write(RV8803C7_ADDR, wdat, 2);
    while(I2C2_IsBusy()==true);

}

void GetRtcRam(uint8_t addr, uint8_t *data)
{
    uint8_t rdat[1]={0};
    uint8_t wdat[1]={0};
    wdat[0]=addr;
    I2C2_WriteRead(RV8803C7_ADDR, wdat, 1, rdat, 1);
    while(I2C2_IsBusy()==true);
    *data=rdat[0];

}

void SetRtcTime(uint8_t h, uint8_t m, uint8_t s)
{

    h=DEC2HEX(h);
    m=DEC2HEX(m);
    s=DEC2HEX(s);

    if(h>(0x24-1) || m>(0x60-1) || s>(0x60-1))
    {
        //SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "SET TIME Format Err\r\n"); 
        return;
    }            

    SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "SET TIME:%02X:%02X:%02X\r\n",h, m, s); 

    uint8_t wdat[4]={0};
    wdat[0]=RV8803C7_REG_SEC;
    wdat[1]=s;
    wdat[2]=m;
    wdat[3]=h;
    I2C2_Write(RV8803C7_ADDR, wdat, 4);
    while(I2C2_IsBusy()==true);

}

void GetRtcTime(uint8_t *h, uint8_t *m, uint8_t *s)
{
    uint8_t rdat[3]={0};
    uint8_t wdat[1]={0};
    
    wdat[0]=RV8803C7_REG_SEC;
    I2C2_WriteRead(RV8803C7_ADDR, wdat, 1, rdat, 3);
    while(I2C2_IsBusy()==true);
    *s=HEX2DEC(rdat[0]);
    *m=HEX2DEC(rdat[1]);
    *h=HEX2DEC(rdat[2]);
    
    if(*s==59)
    {
        I2C2_WriteRead(RV8803C7_ADDR, wdat, 1, rdat, 3);
        while(I2C2_IsBusy()==true);
        *s=HEX2DEC(rdat[0]);
        if(*s!=59)
        {
            //*s=HEX2DEC(rdat[0]);
            *m=HEX2DEC(rdat[1]);
            *h=HEX2DEC(rdat[2]);
        }
    }
}

void SetRtcDate(uint8_t y, uint8_t m, uint8_t d)
{
        
    y=DEC2HEX(y);
    m=DEC2HEX(m);
    d=DEC2HEX(d);

    if(y>(0x99) || m>(0x12) || d>(0x31))
    {
        //SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "SET DATE Format Err\r\n"); 
        return;
    }            

    SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "SET DATE:20%02X/%02X/%02X\r\n",y, m, d);        

    uint8_t wdat[4]={0};
    wdat[0]=RV8803C7_REG_DATE;
    wdat[1]=d;
    wdat[2]=m;
    wdat[3]=y;
    I2C2_Write(RV8803C7_ADDR, wdat, 4);
    while(I2C2_IsBusy()==true);
}

void GetRtcDate(uint8_t *y, uint8_t *m, uint8_t *d)
{
    uint8_t rdat[3]={0};
    uint8_t wdat[1]={0};
    wdat[0]=RV8803C7_REG_DATE;
    I2C2_WriteRead(RV8803C7_ADDR, wdat, 1, rdat, 3);
    while(I2C2_IsBusy()==true);
    *d=HEX2DEC(rdat[0]);
    *m=HEX2DEC(rdat[1]);
    *y=HEX2DEC(rdat[2]);
}

void GetRtcDateTime(uint8_t *year, uint8_t *mon, uint8_t *day, uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    uint8_t rdat[7]={0};
    uint8_t wdat[1]={0};
    wdat[0]=RV8803C7_REG_SEC;
    I2C2_WriteRead(RV8803C7_ADDR, wdat, 1, rdat, 7);
    while(I2C2_IsBusy()==true);
    *sec=HEX2DEC(rdat[0]);
    *min=HEX2DEC(rdat[1]);
    *hour=HEX2DEC(rdat[2]);
    //*wek=HEX2DEC(rdat[3]);
    *day=HEX2DEC(rdat[4]);
    *mon=HEX2DEC(rdat[5]);
    *year=HEX2DEC(rdat[6]);
    
    if(*sec==59)
    {
        I2C2_WriteRead(RV8803C7_ADDR, wdat, 1, rdat, 7);
        while(I2C2_IsBusy()==true);
        *sec=HEX2DEC(rdat[0]);
        if(*sec!=59)
        {
            //*sec=HEX2DEC(rdat[0]);
            *min=HEX2DEC(rdat[1]);
            *hour=HEX2DEC(rdat[2]);
            //*wek=HEX2DEC(rdat[3]);
            *day=HEX2DEC(rdat[4]);
            *mon=HEX2DEC(rdat[5]);
            *year=HEX2DEC(rdat[6]);  
        }
    }
}

void SetRtcWeek(uint8_t w)
{
    uint8_t wdat[2]={0};
    wdat[0]=RV8803C7_REG_WEEK;
    wdat[1]=pow(2,w);
    I2C2_Write(RV8803C7_ADDR, wdat, 2);
    while(I2C2_IsBusy()==true);
}

void GetRtcWeek(uint8_t *w)
{
    uint8_t rdat[1]={0};
    uint8_t wdat[1]={0};
    wdat[0]=RV8803C7_REG_WEEK;
    I2C2_WriteRead(RV8803C7_ADDR, wdat, 1, rdat, 1);
    while(I2C2_IsBusy()==true);
    
    switch(rdat[0])
    {
        case 1://sun
            *w=0;
            break;
        case 2://mon
            *w=1;
            break;
        case 4://tue
            *w=2;
            break;
        case 8://wed
            *w=3;
            break;
        case 16://thu
            *w=4;
            break;
        case 32://fri
            *w=5;
            break;
        case 64:
            *w=6;
            break;
        default:
            *w=0;
            break;
    }
}

void GetIntervalTime(int32_t *interval)
{
    uint8_t val[4];
    bool neg=false;

    GetRtcRam(RV8803C7_REG_RAM_3, &val[0]);
    GetRtcRam(RV8803C7_REG_RAM_4, &val[1]);
    GetRtcRam(RV8803C7_REG_RAM_5, &val[2]);
    //GetRtcRam(RV8803C7_REG_RAM_6, &val[3]);        

    
    if((val[2]&0x80)==0x80)
        neg=true;    
    
    if(neg==true)
        val[3]=0xFF;
    else
        val[3]=0x00;
    
    *interval=((int32_t)val[3]<<24)+((int32_t)val[2]<<16)+((int32_t)val[1]<<8)+((int32_t)val[0]<<0);

    if(*interval==0)
        *interval=-1;
}

void SetIntervalTime(int32_t interval)
{
    uint8_t val[4];
    
    if(interval==0)
        interval=-1;    
    
    if(interval>0x7FFFFF)
        interval=0x7FFFFF;    
    
    val[0]=(uint8_t)((interval>>0) &0xFF);
    val[1]=(uint8_t)((interval>>8) &0xFF);
    val[2]=(uint8_t)((interval>>16)&0x7F);
    
   
    if(interval<0)
        val[2]=val[2] + 0x80;    

    //val[3]=(uint8_t)((interval>>24)&0xFF);        

    SetRtcRam(RV8803C7_REG_RAM_3, val[0]);
    SetRtcRam(RV8803C7_REG_RAM_4, val[1]);
    SetRtcRam(RV8803C7_REG_RAM_5, val[2]);
    //SetRtcRam(RV8803C7_REG_RAM_6, val[3]);
}

void GetAlternateInterface(uint8_t *val)
{
    uint8_t rdata;
    GetRtcRam(RV8803C7_REG_RAM_6, &rdata);
    *val=(rdata)&0x0F;
}

void SetAlternateInterface(uint8_t val)
{
    uint8_t rdata;
    uint8_t wdata;
    GetRtcRam(RV8803C7_REG_RAM_6, &rdata);   
    wdata=(val&0x0F)+(rdata&0xF0);//Set Data Lower Bit
    SetRtcRam(RV8803C7_REG_RAM_6, wdata);
}

void GetDataFrameInterval(uint8_t *val)
{
    uint8_t rdata;
    GetRtcRam(RV8803C7_REG_RAM_6, &rdata);
    *val=(rdata>>4)&0x0F;
}

void SetDataFrameInterval(uint8_t val)
{
    uint8_t rdata;
    uint8_t wdata;
    GetRtcRam(RV8803C7_REG_RAM_6, &rdata);
    wdata=val<<4;    
    wdata=(wdata&0xF0)+(rdata&0x0F);//Set Data Upper Bit
    SetRtcRam(RV8803C7_REG_RAM_6, wdata);
}



void GetBaudRate(long *baud)
{

    uint8_t index=0;
    GetRtcRam(RV8803C7_REG_RAM_2, &index);

    switch(index)
    {
        case 0:
            *baud=9600;
            break;
        case 1:
            *baud=19200;
            break;
        case 2:
            *baud=38400;
            break;
        case 3:
            *baud=57600;
            break;
        case 4:
            *baud=115200;
            break;
        default:
            *baud=9600;    
            break;
    }

}

void SetBaudRate(long *baud)
{
    uint8_t index=0;

    if( *baud <= 9600 )
    {
        index=0;
        *baud=9600;
    }
    else if( 9600 < *baud && *baud <= 19200 )
    {
        index=1;
        *baud=19200;
    }
    else if( 19200 < *baud && *baud <= 38400 )
    {
        index=2;
        *baud=38400;
    }
    else if( 38400 < *baud && *baud <= 57600 )
    {
        index=3;
        *baud=57600;
    }
    else if( 57600 < *baud && *baud <= 115200)
    {
        index=4;
        *baud=115200;
    }
    else
    {
        index=0;
        *baud=9600;
    }   
    SetRtcRam(RV8803C7_REG_RAM_2,index);

}

void SetBaudRateReg()
{
    uint8_t index=0;
    UART_SERIAL_SETUP uartSetReg;    
    uartSetReg.dataWidth    = UART_DATA_8_BIT;
    uartSetReg.parity       = UART_PARITY_NONE;
    uartSetReg.stopBits     = UART_STOP_1_BIT;
    
    GetRtcRam(RV8803C7_REG_RAM_2, &index);   
    
    switch(index)
    {
        case 0://9600bps            
            uartSetReg.baudRate=9600;
            break;
        case 1://19200bps
            uartSetReg.baudRate=19200;
            break;
        case 2://38400bps
            uartSetReg.baudRate=38400;
            break;
        case 3://57600bps
            uartSetReg.baudRate=57600;
            break;
        case 4://115200bps
            uartSetReg.baudRate=115200;
            break;
        default:
            uartSetReg.baudRate=9600;
            break;
    }
    
    UART2_SerialSetup(&uartSetReg, UART2_FrequencyGet());

}


bool IsRtcBOR()
{
    uint8_t rdat[1]={0};
    uint8_t wdat[2]={0};
    wdat[0]=RV8803C7_REG_FLG;
    I2C2_WriteRead(RV8803C7_ADDR, wdat, 1, rdat, 1);
    while(I2C2_IsBusy()==true);
    
    if((rdat[0]&0x03)!=0x00)
    {
        I2C2_Write(RV8803C7_ADDR, wdat, 2);
        while(I2C2_IsBusy()==true);
        return true;
    }
    return false;
}




DWORD get_fattime(void)
{
    /* RTC should return time here */
    /* For now, just a value */
    SYS_FS_TIME time;
    time.packedTime = 0;

    uint8_t year, month, day, hour, min, sec;
    
    GetRtcDateTime( &year, &month, &day, &hour, &min, &sec);
    // All FAT FS times are calculated based on 0 = 1980
    time.discreteTime.year = (2000+ year - 1980); // year is 2013
    time.discreteTime.month = month;    // August
    time.discreteTime.day = day;     // 9th date
    time.discreteTime.hour = hour;    // 3pm afternoon
    time.discreteTime.minute = min;  // 06 min
    time.discreteTime.second = sec;  // 00 sec

    return (time.packedTime);
}