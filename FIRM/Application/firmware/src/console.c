


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "sys/kmem.h"
#include "system/debug/sys_debug.h"
#include "peripheral/coretimer/plib_coretimer.h"
#include "console.h"
#include "rtc_rv8803.h"
#include "system/fs/sys_fs.h"
#include "bitmap.h"
#include "app.h"
#include "xmodem.h"
#include "ymodem.h"
#include "serialcam.h"
#include "peripheral/gpio/plib_gpio.h"
#include "peripheral/rcon/plib_rcon.h"

#define BTL_TRIGGER_RAM_START   KVA0_TO_KVA1(0x80000000)
#define BTL_TRIGGER_PATTERN (0x5048434DUL)
#define DCACHE_CLEAN_BY_ADDR(start, sz)

static uint32_t *ramStart = (uint32_t *)BTL_TRIGGER_RAM_START;

/* DateTime */
#define STR_SET_TIME            "tim="
#define STR_GET_TIME            "tim?"
#define STR_SET_DATE            "day="
#define STR_GET_DATE            "day?"
#define STR_SET_WEEK            "wek="
#define STR_GET_WEEK            "wek?"
/* Camera */
#define STR_SET_INDEX           "idx="
#define STR_GET_INDEX           "idx?"
#define STR_GET_SETTING         "dsps"
#define STR_SET_INTERVAL        "int="
#define STR_GET_INTERVAL        "int?"
#define STR_SET_ALTIF           "alt="
#define STR_GET_ALTIF           "alt?"
#define STR_SET_DFINT           "dfi="
#define STR_GET_DFINT           "dfi?"
#define STR_SET_TRG             "trg!"
#define STR_GET_MONO            "mon?" //BMP Mono 
#define STR_SET_MONO            "mon=" //BMP Mono 
/* System */
#define STR_SET_BAUDRATE        "brt="
#define STR_GET_BAUDRATE        "brt?"
#define STR_SET_RESET           "rst!"
#define STR_SET_PAUSE           "pus!"
#define STR_SET_RESUME          "rsm!"
#define STR_FIRM_UPDATE         "fup!"
#define STR_HELP                "help"

/* File */
#define STR_XMODEM_START        "xmd!"
#define STR_YMODEM_START        "ymd!"


SYS_CONSOLE_HANDLE      consoleHandle;
ssize_t                 consoleReadSize;     //indicates the actual number of bytes read
char                    consoleBuffer[100];
char                    consoleStr[100];
ssize_t                 consoleStrSize;

extern bool             IsSwitchPushed;
extern bool             IsXmodemMode;
extern bool             IsYmodemMode;
extern APP_DATA         appData;
volatile uint32_t       startCount=0;
volatile uint32_t       endCount=0;

extern void SYS_Tasks_without_AppTasks(void);
static void APP_SystemReset( void );

void CheckConsoleCmd()
{
    char *cmd=consoleStr;
    
    // Set Index
    if(strncmp(cmd,STR_SET_INDEX,4) == 0)
    {
        long index=0;
        char *p=strpbrk( cmd, "1234567890" );        
        if ( p != NULL ) index = atol( p ); 
        SetRtcRam(RV8803C7_REG_RAM_1, index);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Set Index:%d\r\n",index);
    }
    // Get Stream Setting 
    else if(strncmp(cmd, STR_GET_SETTING,4) == 0)
    {
        ConsoleDispStreamSetting();        
    }   
    // Get Index
    else if(strncmp(cmd,STR_GET_INDEX,4) == 0)
    {
        uint8_t index=0;
        GetRtcRam(RV8803C7_REG_RAM_1, &index);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Get Index:%d\r\n",index);    
    }
    // Set Interval
    else if(strncmp(cmd,STR_SET_INTERVAL,4) == 0)
    {
        long interval=-1;
        char *p=strpbrk( cmd, "-1234567890" );        
        if ( p != NULL ) interval = atol( p );
        if(interval==0)
            interval=-1;
        if(interval>0x7FFFFF)
        {
            interval=0x7FFFFF;
            SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "IntervalTimeMax = 8388607 sec\t");
        }
        SetIntervalTime(interval);        
        intervalTime=interval;        
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Set Interval:%d\r\n",interval);
    }
    // Get Interval
    else if(strncmp(cmd,STR_GET_INTERVAL,4) == 0)
    {
        int32_t interval=0;
        GetIntervalTime(&interval);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Get Interval:%d\r\n",interval);
    }
    // Set Alternate Interface
    else if(strncmp(cmd,STR_SET_ALTIF,4) == 0)
    {
        long num=0;
        char *p=strpbrk( cmd, "1234567890" );        
        if ( p != NULL ) num = atol( p ); 
        if(num>0x0F || num<0)
            num=0;
        SetAlternateInterface(num);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Set Alternate IF:%d\r\n",num);    
    }
    // Get Alternalte Interface    
    else if(strncmp(cmd,STR_GET_ALTIF,4) == 0)
    {
        uint8_t num=0;
        GetAlternateInterface(&num);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Get Alternate IF:%d\r\n",num);    
    }
    // Set Data Frame Interval
    else if(strncmp(cmd,STR_SET_DFINT,4) == 0)
    {
        long num=0;
        char *p=strpbrk( cmd, "1234567890" );        
        if ( p != NULL ) num = atol( p ); 
        if(num>0x0F || num<0)
            num=0;
        SetDataFrameInterval(num);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Set DataFrame INT:%d\r\n",num);     
    }
    // Get Data Frame Interval    
    else if(strncmp(cmd,STR_GET_DFINT,4) == 0)
    {
        uint8_t num=0;
        GetDataFrameInterval(&num);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Get DataFrame INT:%d\r\n",num);    
    }
    // Set Baudrate
    else if(strncmp(cmd,STR_SET_BAUDRATE,4) == 0)
    {
        long baud=-1;
        char *p=strpbrk( cmd, "1234567890" );        
        if ( p != NULL ) baud = atol( p );
        SetBaudRate(&baud);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Set Baudrate:%d\r\n",baud);
    }
    // Get Baudrate
    else if(strncmp(cmd,STR_GET_BAUDRATE,4) == 0)
    {
        long baud=0;
        GetBaudRate(&baud);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Get Baudrate:%d\r\n",baud);
    }   
    // Reset 
    else if(strncmp(cmd,STR_SET_RESET,4) == 0)  //Reset
    {
        //VUSB_Clear();
        CORETIMER_DelayMs(300);
        RCON_SoftwareReset();
    }
    // Firm Update
    else if(strncmp(cmd,STR_FIRM_UPDATE,4) == 0)  //Reset
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Firm Update Triggered\r\n");
        ramStart[0] = BTL_TRIGGER_PATTERN;
        ramStart[1] = BTL_TRIGGER_PATTERN;
        ramStart[2] = BTL_TRIGGER_PATTERN;
        ramStart[3] = BTL_TRIGGER_PATTERN;
        DCACHE_CLEAN_BY_ADDR(ramStart, 16);
        APP_SystemReset();
    }
    //Pause
    else if(strncmp(cmd,STR_SET_PAUSE,4) == 0)  
    {
        appData.state=APP_STATE_PAUSE_SET;
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Pause Stream\r\n");
    }
    //Resume
    else if(strncmp(cmd,STR_SET_RESUME,4) == 0)  
    {
        if(appData.state==APP_STATE_PAUSE_DONE)
        {
            
            SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Resume Stream\r\n");
            RCON_SoftwareReset();
        }
    }    
    // Trigger
    else if(strncmp(cmd,STR_SET_TRG,4) == 0)    //Triger
    {
        IsSwitchPushed=true;
    }
    //Set Mono 
    else if(strncmp(cmd,STR_SET_MONO,4) == 0)    //Triger
    {
        long num=0;
        char *p=strpbrk( cmd, "1234567890" );        
        if ( p != NULL ) num = atol( p ); 
        if(num==0)
            IsBitmapMonoMode=false;
        else
            IsBitmapMonoMode=true;        
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Set Bitmap Mono Mode:%d\r\n", IsBitmapMonoMode); 
    }
    // Get Mono
    else if(strncmp(cmd,STR_GET_MONO,4) == 0)
    {        
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Get Bitmap Mono Mode:%d\r\n", IsBitmapMonoMode);    
    }    
    // Set Time
    else if(strncmp(cmd,STR_SET_TIME,4) == 0)
    {
        long time=0;
        unsigned int  h,m,s;
        char *p=strpbrk( cmd, "1234567890" );        
        if ( p != NULL ) time = atol( p ); 
        h=(uint8_t)(time/10000);
        m=(uint8_t)((time/100)%100);
        s=(uint8_t)(time%100);
        SetRtcTime(h,m,s);       
    }
    // Get Time
    else if(strncmp(cmd,STR_GET_TIME,4) == 0)
    {
        uint8_t s,m,h;
        GetRtcTime(&h, &m, &s);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "NOW TIME:%02d:%02d:%02d\r\n",h, m, s);    
    }
    // Set Date
    else if(strncmp(cmd,STR_SET_DATE,4) == 0)
    {
        long date=0;
        unsigned int  y,m,d;
        char *p=strpbrk( cmd, "1234567890" );        
        if ( p != NULL ) date = atol( p ); 
        y=date/10000;
        m=(date/100)%100;
        d=date%100;
        SetRtcDate(y, m, d);
    }
    // Get Date
    else if(strncmp(cmd,STR_GET_DATE,4) == 0)
    {
        uint8_t d,m,y;
        GetRtcDate(&y, &m, &d);
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "NOW DATE:20%02d/%02d/%02d\r\n",y, m, d);  
    } 
    // Set Week
    else if(strncmp(cmd,STR_SET_WEEK,4) == 0)
    {
        long week=0;
        char *p=strpbrk( cmd, "1234567890" );        
        if ( p != NULL ) week = atol( p ); 
        
        if(0>week && week >6)
        {
            SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "SET DATE Format Err\r\n"); 
            return;
        }            
        SetRtcWeek(week);
    }   
    // Get Week
    else if(strncmp(cmd,STR_GET_WEEK,4) == 0)
    {
        uint8_t week;
        char weekStr[5]={'\0'};
        
        GetRtcWeek(&week);

        switch(week)
        {
            case 0:
                strcpy(weekStr,"sun");
                break;
            case 1:
                strcpy(weekStr,"mon");
                break;
            case 2:
                strcpy(weekStr,"tue");
                break;
            case 3:
                strcpy(weekStr,"wed");
                break; 
            case 4:
                strcpy(weekStr,"thu");
                break;
            case 5:
                strcpy(weekStr,"fri");
                break; 
            case 6:
                strcpy(weekStr,"sat");
                break; 
            default:
                break;
        }
        
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "NOW WEEK:(%s)\r\n",weekStr);   
    }
    // Xmodem
    else if(strncmp(cmd,STR_XMODEM_START,4) == 0)
    {
        IsXmodemMode=true;
        if(IsBitmapMode==false)
            SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Download Jpeg File using Xmodem/CheckSum\r\n");   
        else
            SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Download Bitmap File using Xmodem/CheckSum\r\n");   
    }
    // Ymodem
    else if(strncmp(cmd,STR_YMODEM_START,4) == 0)
    {
        IsYmodemMode=true;
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Download File using Ymodem\r\n");   
    }        
    // Help
    else if(strncmp(cmd,STR_HELP,4) == 0)
    {
        ConsoleDispHelp();
    }

}

void ConsoleReadTask()
{
    consoleReadSize = SYS_CONSOLE_Read( consoleHandle, consoleBuffer, sizeof(consoleBuffer) );

    strncpy(&consoleStr[consoleStrSize],consoleBuffer,consoleReadSize);
    consoleStrSize=consoleStrSize+consoleReadSize;

    if (consoleReadSize == -1)
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "SYS_CONSOLE_Read Err\r\n");
    }
    else if(consoleReadSize!=0)
    {
        memset(consoleBuffer,'\0',sizeof(consoleBuffer));
        consoleReadSize=0;
    }

    if(consoleStr[consoleStrSize-1]=='\n' || consoleStr[consoleStrSize-1]=='\r' )
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, ">>%s",consoleStr);
        CheckConsoleCmd();
        memset(consoleStr,'\0',sizeof(consoleStr));
        consoleStrSize=0;
    }   


}

void ConsoleDispHelp()
{
    //SYS_DEBUG_PRINT(SYS_ERROR_WARNING,"->help\r\n");
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," \t\t*** Date Time *** \t \r\n");
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Time Get Cmd: tim?       \tex) tim?\r\n"); 
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Time Set Cmd: tim=HHMMSS \tex) tim=134502\r\n");
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Date Get Cmd: day?       \tex) day?\r\n");
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Date Set Cmd: day=YYMMDD \tex) day=210802\r\n");   
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING,"\r\n");
    
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," \t\t*** Camera *** \t \r\n");
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Stream Setting Get: dsps \tex) dsps\r\n");   
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Index Get Cmd: idx?      \tex) idx?\r\n");   
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Index Set Cmd: idx=*     \tex) idx=1\r\n");   
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Trigger Cmd: trg!        \tex) trg!\r\n"); 
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Alt IF Get Cmd: alt?     \tex) alt?\r\n");  
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Alt IF Set Cmd: alt=*    \tex) alt=1\r\n");
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," *If enable auto Alt IF, set 0 value\tex) alt=0\r\n\r\n"); 
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," DF Int Get Cmd: dfi?     \tex) dfi?\r\n");  
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," DF Int Set Cmd: dfi=*    \tex) dfi=1\r\n"); 
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," *If enable auto DF Int, set 0 value\tex) dfi=0\r\n\r\n"); 
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Interval Get Cmd: int?   \tex) int?\r\n");  
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Interval Set Cmd: int=*[sec]\tex) int=1\r\n"); 
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," *If disable interval mode, set negative value\tex) int=-1\r\n"); 
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Bitmap Monochrome Mode Get: mon?  \tex) mon?\r\n");  
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Bitmap Monochrome Mode Set: mon=* \tex) mon=1\r\n"); 
     
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING,"\r\n");
    
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," \t\t*** System *** \t \r\n");
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Baudrate Get Cmd: brt?       \tex) brt?\r\n");
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Baudrate Set Cmd: brt=*   \tex) brt=9600\r\n");    
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," System Restart Cmd: rst!     \tex) rst!\r\n");
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Stream Pause Cmd: pus!     \tex) pus!\r\n");
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," Stream Resume Cmd: rsm!    \tex) rsm!\r\n");
}

void ConsoleDispInit()
{
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING,"\r\n----------------------------------------------------------------\r\n");    
    //Disp Firm Info
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING, FIRMWARE_NAME FIRMWARE_VERSION " (Build: "__DATE__")\r\n");
    
    if(IsRtcBOR()==false)
    {
        GetDateTime();
    }
    else
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "RTC was cleared! Please set datetime & settings.\r\n");
    }

}

void ConsoleDispInfo()
{
       
  
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING," *If need more command info, put 'help' cmd. \tex) help(Enter) \r\n");

    GetRtcRam(RV8803C7_REG_RAM_1, &uvcSelectedSettingIndex);

    if(uvcSelectedSettingIndex>=uvcSettingsCount)
        uvcSelectedSettingIndex=0;
    
    if(uvcSettings[uvcSelectedSettingIndex].format==4)
        IsBitmapMode=true;
    else
        IsBitmapMode=false;
    
    if(uvcSettings[uvcSelectedSettingIndex].format==4)//BMP
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "->Selected Setting Index:%d (Format:%d [BMP], Frame:%d [%d x %d])\r\n",
                uvcSelectedSettingIndex, 
                uvcSettings[uvcSelectedSettingIndex].format, 
                uvcSettings[uvcSelectedSettingIndex].frame,
                uvcSettings[uvcSelectedSettingIndex].width,  uvcSettings[uvcSelectedSettingIndex].height);
    else
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "->Selected Setting Index:%d (Format:%d [JPEG], Frame:%d [%d x %d])\r\n",
                uvcSelectedSettingIndex, 
                uvcSettings[uvcSelectedSettingIndex].format, 
                uvcSettings[uvcSelectedSettingIndex].frame,
                uvcSettings[uvcSelectedSettingIndex].width,  uvcSettings[uvcSelectedSettingIndex].height);
    
    ConsoleCheckAlternateInterface();  
    ConsoleCheckDataFrameInterval();
    GetIntervalTime(&intervalTime);
    
    if(intervalTime<0)
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING,"->Interval Mode OFF\r\n");
    else
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING,"->Interval Mode ON (%d sec)\r\n",intervalTime);
    
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING,"----------------------------------------------------------------\r\n");
 
    
    if(IsBitmapMode==true)
    {
        Bitmap565InitBuf((uint8_t*)videoRam,sizeof(videoRam),uvcSettings[uvcSelectedSettingIndex].width,uvcSettings[uvcSelectedSettingIndex].height);
        BitmapMonoInitBuf((uint8_t*)videoMono,sizeof(videoMono),uvcSettings[uvcSelectedSettingIndex].width,uvcSettings[uvcSelectedSettingIndex].height);
        if(videoRam[0]==0)
        {
            SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "\r\nBMP RAM SIZE OVER ERR\r\n");
            while(1)
            {
                SYS_Tasks_without_AppTasks();
                ConsoleReadTask();
            }
        }
    }
    else
    {
        //JpegInit((uint8_t*)videoRam,sizeof(videoRam));
    }

}

void ConsoleDispStreamSetting()
{
    int i=0;
    //Disp Supported Fromat & Frame
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "\r\n\t\t*** Supported Stream Settings ***\r\n");
    
    for(i=0; i<uvcSettingsCount; i++)
    {
        if( uvcSettings[i].format==4 )//YUY2:BMP
        {
            if( (uvcSettings[i].width*uvcSettings[i].height*2)< sizeof(videoRam) )
            {
                SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Index:%d,\t Format: %d(BMP %d),\t Frame: %d (%d x %d)\r\n", 
                    i, uvcSettings[i].formatidx, uvcSettings[i].format, uvcSettings[i].frame, uvcSettings[i].width,  uvcSettings[i].height);
            }
        }
        else //MJPEG
        {
            SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Index:%d,\t Format: %d(JPEG %d),\t Frame: %d (%d x %d)\r\n", 
                i,  uvcSettings[i].formatidx, uvcSettings[i].format, uvcSettings[i].frame, uvcSettings[i].width,  uvcSettings[i].height);        
        }    
    }
    
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "\r\n\t\t*** Supported Alternate Interface ***\r\n");
    
    for(i=0; i<sizeof(uvcAlternalte)/sizeof(uvcAlternalte[0]); i++)
    {
        if(uvcAlternalte[i].size>=8 && uvcAlternalte[i].size<=4096)
            SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Index:%d,\t Size: 0x%X byte\r\n", 
                uvcAlternalte[i].index, uvcAlternalte[i].size);
    }    

    SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "\r\n\t\t*** Supported Dataframe Interval ***\r\n");
        
    for(i=0; i<uvcSettings[uvcSelectedSettingIndex].intervalCount; i++)
    {
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Index:%i,\t Interval: 0x%X msec\r\n", 
                i+1, uvcSettings[uvcSelectedSettingIndex].interval[i]);
    } 
    

}

void ConsoleCheckInterval(bool *trig)
{
    //intervalTime=1;
    
    if(intervalTime<0)
        return;
    
    
    if((_CP0_GET_COUNT()-startCount)>endCount)
    {
        endCount=(CORE_TIMER_FREQUENCY);
        startCount=_CP0_GET_COUNT();        
        intervalTimeNext=intervalTimeNext-1;
        //SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Tick:%d\r\n",intervalTimeNext);  
    }
    
    if(intervalTimeNext<=0)
    {
        *trig=true;
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "Interval Trig!!\r\n");  
        intervalTimeNext=intervalTime;          
    }

}

void ConsoleCheckAlternateInterface()
{
    uint8_t ifIndex=0;
    uint8_t i=0;
    
    GetAlternateInterface(&ifIndex);
    
    if(ifIndex==0)
        ifIndex=1;
    
    for(i=0;i<sizeof(uvcAlternalte)/sizeof(uvcAlternalte[0]);i++)
    {
        if(uvcAlternalte[ifIndex-1].size<0x00FF || uvcAlternalte[ifIndex-1].size>0x1000)
            ifIndex++;
    }
    
    uvcAlternateInterfaceIndex=ifIndex-1;   
    
    appData.packetSize=uvcAlternalte[uvcAlternateInterfaceIndex].size;

    SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "->Selected Alternate Interface:%d, Size:0x%X\r\n",
            uvcAlternalte[uvcAlternateInterfaceIndex].index,
            uvcAlternalte[uvcAlternateInterfaceIndex].size);  
}

void ConsoleCheckDataFrameInterval()
{
    uint8_t dfi=0;
    uint32_t val=0;
    uint8_t i=0;
    uint8_t i_buff=0;
    
    GetDataFrameInterval(&dfi);
    
    if(IsBitmapMode==true)
        val=0xFFFFFFFF;
    
    if(dfi==0)
    {
        for( i=0; i<uvcSettings[uvcSelectedSettingIndex].intervalCount; i++ )
        {
            if(IsBitmapMode==true)//In Bitmap Mode, Find Large Value
            {
                if(uvcSettings[uvcSelectedSettingIndex].interval[i]>val)
                {
                    val=uvcSettings[uvcSelectedSettingIndex].interval[i];
                    i_buff=i;
                }
            }
            else//In Jpeg Mode, Find Small Value
            {
                if(uvcSettings[uvcSelectedSettingIndex].interval[i]!=0 && uvcSettings[uvcSelectedSettingIndex].interval[i]<val)
                {
                    val=uvcSettings[uvcSelectedSettingIndex].interval[i];
                    i_buff=i;
                }                
            }
           
            
        }
        uvcDataFrameIntervalIndex=i_buff;
    }
    else
    {
        val=uvcSettings[uvcSelectedSettingIndex].interval[dfi-1];
        uvcDataFrameIntervalIndex=dfi-1;
    }
    
    SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "->Selected Dataframe Interval Index:%d (0x%X)\r\n",
            uvcDataFrameIntervalIndex+1,
            uvcSettings[uvcSelectedSettingIndex].interval[uvcDataFrameIntervalIndex]);

}

static void APP_SystemReset( void )
{
    /* Perform system unlock sequence */ 
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;

    RSWRSTSET = _RSWRST_SWRST_MASK;
    (void)RSWRST;
}