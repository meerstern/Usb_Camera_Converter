/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>            
#include "definitions.h"
#include "app.h"
#include "app_user.h"
#include "bitmap.h"
#include "jpeg.h"
#include "file.h"
#include "xmodem.h"
#include "ymodem.h"
#include "serialcam.h"
#include "console.h"
#include "rtc_rv8803.h"

#ifdef ENABLE_LCD
    #include "lcd/st7735.h"
#endif 



// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA USB_ALIGN             appData;

bool                           IsBitmapMode    = false;//True:Bitmap, False:Jpeg
bool                           IsBitmapMonoMode= false;//True:Mono,   False:Color
bool                           IsSettingMode   = false;//True:SettingMode, False:Streaming Mode
bool                           IsSwitchPushed  = false;
bool                           IsXmodemMode    = false;
bool                           IsYmodemMode    = false;
bool                           hasSDcard       = false;

APP_USB_HOST_UVC_SETTINGS      uvcSettings[50];
APP_USB_HOST_UVC_ALTERNATE     uvcAlternalte[16];
uint8_t                        uvcSettingsCount=0;
uint8_t                        uvcAlternateCount=0;
uint8_t                        uvcSelectedSettingIndex=0;
uint8_t                        uvcAlternateInterfaceIndex=0;
uint8_t                        uvcDataFrameIntervalIndex=0;

uint8_t                        videoSamples[5150];
uint16_t                       videoRam[80000];//23380
uint8_t                        videoMono[10000];

volatile bool                  ramDataReady=false;
volatile uint32_t              ramDataCount=0;

int32_t                        intervalTime=-1;
int32_t                        intervalTimeNext=0;

/* LCD */
// start
uint8_t lcdStart;
// end
uint8_t lcdEnd;

#ifdef ENABLE_LCD
    // Chip Select
    struct signal cs = {  .port = &LATD, .pin = 15 };
    // Data / Command
    struct signal dc = {  .port = &LATB, .pin = 0 };
    // Reset
    struct signal rs = {  .port = &LATB, .pin = 1 };

    struct st7735 lcd1 = { .cs = &cs, .bl = NULL, .dc = &dc, .rs = &rs };
#endif

extern void SYS_Tasks_without_AppTasks(void);

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    //appData.state = APP_STATE_INIT;
#ifdef ENABLE_LCD
    lcdStart = 30;
    lcdEnd = MAX_X - lcdStart;
    // init lcd 1
    ST7735_Init (&lcd1);
    // clear screen
    ST7735_ClearScreen (&lcd1, BLACK);

    // set position X, Y
    ST7735_SetPosition (lcdStart + 24, 10);  
    // draw string
    ST7735_DrawString (&lcd1, "Crescent", WHITE, X2);

    // set position X, Y
    ST7735_SetPosition (lcdStart - 5, 30);  
    // draw string
    ST7735_DrawString (&lcd1, "Web Cam Host Start", WHITE, X2);
#endif
    
    appData.state =  APP_STATE_BUS_ENABLE;
    appData.deviceIsAttached = false;
    APP_VideoDataSetDefault();
    
    VUSB_Set();
    
    WDT_Enable();
    
    if(PUSH_SW_Get()==0)
    {
        IsSettingMode=true;
    }
    else
    {
        IsSettingMode=false;
    }

   
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

static uint8_t  camState=CAMERA_READ_IDLE;

void APP_Tasks ( void )
{
    bool status = false;
    USB_HOST_UVC_RESULT videoResult;
    uint32_t interruptStatus;
    
    CheckTrigger(&IsSwitchPushed);    
    ConsoleReadTask();
    ConsoleCheckInterval(&IsSwitchPushed);
    
    if(appData.state == APP_STATE_WAIT_FOR_READ_COMPLETE)
        WDT_Clear();

    /* Check the application's current state. */
    switch ( appData.state )
    {
        case APP_STATE_BUS_ENABLE:
        {            
            ConsoleDispInit();            
            videoResult = USB_HOST_UVC_AttachEventHandlerSet(&App_USBHostUVCAttachHandler, (uintptr_t) 0);
            
            if(videoResult == USB_HOST_UVC_RESULT_SUCCESS)
            {
                USB_HOST_EventHandlerSet(APP_USBHostEventHandler, (uintptr_t)0);
                USB_HOST_BusEnable(USB_HOST_BUS_ALL);
                appData.state = APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE;
            }
            break;
        }
        case APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE:
            
            if(USB_HOST_BusIsEnabled(USB_HOST_BUS_ALL))
            {
                appData.state = APP_STATE_WAIT_FOR_DEVICE_ATTACH;
            }
            break;
            
        case APP_STATE_WAIT_FOR_DEVICE_ATTACH:
            
           
            if(appData.deviceIsAttached != true)
            {
                break; 
            }

            USB_HOST_UVC_EntityRequestCallbackSet( appData.uvcObj, App_USBUvcControlRequestCallback, 0); 
            
            appData.isVideoInStreamFound=true;            
            
            status = APP_UvcFeatureUnitObjectGet(appData.videoTerminalLink, &appData.videoFeatureUnitObj); 
            if(status == false)
              appData.state = APP_STATE_ERROR; 
            
            appData.inStreamHandle = USB_HOST_UVC_StreamOpen(appData.inStreamObj);
            if(appData.inStreamHandle == USB_HOST_UVC_STREAM_HANDLE_INVALID)
            {
                appData.state = APP_STATE_ERROR; 
                break;
            }

            videoResult = USB_HOST_UVC_StreamEventHandlerSet(appData.inStreamHandle, 
                APP_USBHostUVCStreamEventHandler, (uintptr_t)appData.inStreamObj);
            if(videoResult != USB_HOST_UVC_RESULT_SUCCESS)
            {
                appData.state = APP_STATE_ERROR; 
                break;
            }

            //Disp info
            SetBaudRateReg();
            ConsoleDispInfo();
            if( IsSettingMode==false )
                appData.state = APP_STATE_ZERO_BANDWIDTH_INTERFACE_SET;
            else 
                appData.state = APP_STATE_PAUSE_SET;
            break; 
            
        case APP_STATE_ZERO_BANDWIDTH_INTERFACE_SET:

            appData.isStreamInterfaceSetComplete = false; 
            videoResult = USB_HOST_UVC_StreamingInterfaceSet(appData.inStreamHandle, 
                &appData.requestHandle,
                appData.videoStreamInterfaceSettingObjZeroBandwidth);
            if(videoResult == USB_HOST_UVC_RESULT_BUSY)
            {
                appData.state = APP_STATE_ZERO_BANDWIDTH_INTERFACE_SET; 
            }
            else if(videoResult == USB_HOST_UVC_RESULT_SUCCESS)
            {
                appData.state = APP_STATE_WAIT_FOR_VIDEO_STREAM_INTERFACE_SET_ZERO; 
            }
            else
            {
                appData.state = APP_STATE_ERROR; 
            }            
            break;            
            
        case APP_STATE_WAIT_FOR_VIDEO_STREAM_INTERFACE_SET_ZERO:
            if(appData.isStreamInterfaceSetComplete != true)
            {
                break; 
            }
            
            appData.isStreamInterfaceSetZeroComplete=true;
            appData.isControlRequestCompleted = false; 
            videoResult = USB_HOST_UVC_SetVideoProbeControl(appData.uvcObj,
              appData.videoFeatureUnitObj, &appData.requestHandle, 0);
            if(videoResult == USB_HOST_UVC_RESULT_BUSY)
            {
                appData.isControlRequestCompleted = true;
                appData.state = APP_STATE_WAIT_FOR_VIDEO_STREAM_INTERFACE_SET_ZERO; 
            }
            else if(videoResult == USB_HOST_UVC_RESULT_SUCCESS)
            {
                appData.state = APP_STATE_WAIT_FOR_SET_PROBE_VIDEO;
            }
            else
            {
              appData.state = APP_STATE_ERROR; 
            }
            break; 
        
        case APP_STATE_WAIT_FOR_SET_PROBE_VIDEO:
            if(appData.isControlRequestCompleted != true)
            {
                break; 
            }
            appData.isControlRequestCompleted = false;
            videoResult = USB_HOST_UVC_GetVideoControl(appData.uvcObj,
            appData.videoFeatureUnitObj, &appData.requestHandle, 0);
            if(videoResult == USB_HOST_UVC_RESULT_BUSY)
            {
              appData.isControlRequestCompleted = true;
              appData.state = APP_STATE_WAIT_FOR_SET_PROBE_VIDEO; 
            }
            else if(videoResult == USB_HOST_UVC_RESULT_SUCCESS) 
            {
              appData.state = APP_STATE_WAIT_FOR_GET_PROBE_VIDEO;
            }
            else
            {
              appData.state = APP_STATE_ERROR; 
            }
            break;
        
        case APP_STATE_WAIT_FOR_GET_PROBE_VIDEO:
            if(appData.isControlRequestCompleted != true)
            {
              break; 
            }
            appData.isControlRequestCompleted = false;
            videoResult = USB_HOST_UVC_SetVideoCommitControl(appData.uvcObj,
              appData.videoFeatureUnitObj, &appData.requestHandle, 0);
            if(videoResult == USB_HOST_UVC_RESULT_BUSY)
            {
              appData.isControlRequestCompleted = true;
              appData.state = APP_STATE_WAIT_FOR_GET_PROBE_VIDEO; 
            }
            else if(videoResult == USB_HOST_UVC_RESULT_SUCCESS)
            {
              appData.state = APP_STATE_WAIT_FOR_SET_COMMIT_VIDEO;
            }
            else
            {
              appData.state = APP_STATE_ERROR; 
            }
            break;
                
        case APP_STATE_WAIT_FOR_SET_COMMIT_VIDEO:
            if (appData.isControlRequestCompleted == true)
            {
                appData.state = APP_STATE_ENABLE_VIDEO_STREAM; 
            }
            break;
        
        case APP_STATE_ENABLE_VIDEO_STREAM:
            appData.isStreamInterfaceSetComplete = false;
            videoResult = USB_HOST_UVC_StreamingInterfaceSet(appData.inStreamHandle, 
                            &appData.requestHandle, appData.videoStreamInterfaceSettingObj);

            if(videoResult == USB_HOST_UVC_RESULT_BUSY)
            {
                appData.isControlRequestCompleted = true;
                appData.state = APP_STATE_ENABLE_VIDEO_STREAM; 
            }
            else if(videoResult == USB_HOST_UVC_RESULT_SUCCESS)
            {
                appData.state = APP_STATE_WAIT_FOR_VIDEO_STREAM_INTERFACE_SET_ONE;
            }
            else
            {
                appData.state = APP_STATE_ERROR; 
            }     
            break;     
            
        case APP_STATE_WAIT_FOR_VIDEO_STREAM_INTERFACE_SET_ONE:
            if(appData.isStreamInterfaceSetComplete == true)
            {
                appData.isStreamInterfaceSetOneComplete=true;
                appData.state = APP_STATE_START_STREAM_DATA;

#ifdef ENABLE_LCD
                // set position X, Y
                ST7735_SetPosition (lcdStart - 5, 70);  
                // draw string
                ST7735_DrawString (&lcd1, "Streaming Now", WHITE, X2);
#endif
            }
            break; 
            
        case APP_STATE_START_STREAM_DATA:
            
            RUN_LED_Set();
            interruptStatus = __builtin_disable_interrupts(); 
            appData.isVideoReadCompleted = false;
            appData.wLenVideoRead=0;
            __builtin_mtc0(12,0,interruptStatus);
            appData.state = APP_STATE_WAIT_FOR_READ_COMPLETE; 
            
            if(_USB_HOST_UVC_StreamRead(appData.inStreamHandle,
              &appData.transferHandleVideoRead, (void*)&videoSamples, appData.packetSize,
              USB_HOST_UVC_API_VERSION_FLAG_V1) != USB_HOST_UVC_RESULT_SUCCESS)
            {
                CORETIMER_DelayMs(1);
                appData.state = APP_STATE_START_STREAM_DATA;
            }
            else
            {
#ifdef ENABLE_LCD
                // set position X, Y
                ST7735_SetPosition (lcdStart - 5, 90);  
                // draw string
                ST7735_DrawString (&lcd1, "Stream Read Complite", WHITE, X2);
                SPI_CS_LOW();
                ST7735_SetWindow(&lcd1, 0, _TFTWIDTH-1, 0, _TFTHEIGHT-1);
#endif
            }
            break;
      
        case APP_STATE_WAIT_FOR_READ_COMPLETE:
        {    
            uint8_t *streamData;
            static uint16_t row,col;
            static int16_t remainingBytes=0;
            static uint32_t oldFileSize=0;
            static uint8_t fdat=0;
            static uint8_t fdat_old=0;
            static uint8_t rem=0;
            
            if(appData.isVideoReadCompleted)
            {       
                WDT_Clear();
                VP_HeaderInfoType *vphi=(VP_HeaderInfoType *)&videoSamples;                
                streamData=((uint8_t *)&videoSamples)+vphi->HeaderSize;
                remainingBytes=appData.wLenVideoRead -vphi->HeaderSize;
                
                //JPEG
                if(IsBitmapMode==false)
                {
                    if(remainingBytes>0)
                    {
                        int jk=0;
                        
                         for(jk=0;jk<remainingBytes;jk++)
                         {
                            fdat=*(streamData+jk);
                            
                            if( vphi->FID==1 && fdat==0xD8 && fdat_old==0xFF  && (camState == CAMERA_READ_IDLE || camState == CAMERA_READ_EXEC ))
                            {
                                camState=CAMERA_READ_EXEC;
                                ramDataCount=0;
                                rem=jk%2;
                            }
                            else if( vphi->FID==1 && fdat==0xD9 && fdat_old==0xFF && camState==CAMERA_READ_EXEC)
                            {
                                camState=CAMERA_READ_DONE;
                                videoRam[ramDataCount]=(uint16_t)(fdat<<8)+fdat_old;
                                ramDataCount++;
                            }
                            
                            if( camState == CAMERA_READ_EXEC )
                            {
                                if((sizeof(videoRam)/sizeof(videoRam[0])) > ramDataCount)
                                {
                                    if(jk%2==rem)
                                    {
                                        videoRam[ramDataCount]=(uint16_t)(fdat<<8)+fdat_old;
                                        ramDataCount++;
                                    }
                                }
                                else
                                {
                                    camState = CAMERA_READ_IDLE;
                                    ramDataCount=0;
                                }
                            }
                            
                            fdat_old=fdat;
                        }
                    }
                    remainingBytes=0;
                }

                                
#ifdef ENABLE_LCD
                m_LCDDCBit=1;
                SPI_CS_LOW();
                SPI6CONbits.MODE16=1;
#endif
                
                while(remainingBytes>0 && IsBitmapMode==true ) 
                {
                    int Y = *streamData++;
                    int U = *streamData++;
                    int Y2= *streamData++;
                    int V = *streamData++;
                    int r,g,b;

                    int c = Y-16;
                    int d = U-128;
                    int e = V-128;
                    uint16_t buff1,buff2;
#ifdef ENABLE_LCD 
                    uint16_t spiBuff1,spiBuff2;
#endif                   
                    c *= 298;

                    b = (c + 516*d + 128) >> 8;   // blue
                    clip(b);
                    g = (c - 100*d - 208*e + 128) >> 8; // green
                    clip(g);
                    r = (c + 409*e + 128) >> 8;   // red
                    clip(r);
                    buff1 = Color565(r,g,b);

#ifdef ENABLE_LCD 
                    spiBuff1=buff1;
                    JpegSwapForLcd(&spiBuff1);// LCD is swapped color
                    while(SPI6STATbits.SPIBUSY == 1);
                    SPI6STATbits.SPIROV = 0;  // Reset overflow bit                    
                    SPI6BUF=spiBuff1;
#endif

                    c = Y2-16;
                    c *= 298; 

                    b = (c + 516*d + 128) >> 8;   // blue
                    clip(b);
                    g = (c - 100*d - 208*e + 128) >> 8; // green
                    clip(g);
                    r = (c + 409*e + 128) >> 8;   // red
                    clip(r);   
                    buff2 = Color565(r,g,b);     // Write to SPI buffer
 
#ifdef ENABLE_LCD
                    spiBuff2=buff2;
                    JpegSwapForLcd(&spiBuff2);// LCD is swapped color
                    while(SPI6STATbits.SPIBUSY == 1);
                    SPI6STATbits.SPIROV = 0;                  
                    SPI6BUF=spiBuff2;
#endif

                    remainingBytes-=4;
                    col+=2; 

                    if( ramDataReady==false && ramDataCount < (uvcSettings[uvcSelectedSettingIndex].width*uvcSettings[uvcSelectedSettingIndex].height))
                    {
                        Bitmap565SetPixel((uint8_t*)videoRam,col-2, row, buff1 );
                        Bitmap565SetPixel((uint8_t*)videoRam,col-1, row, buff2 );
                        ramDataCount++;
                        ramDataCount++;      
                    }

                    if(col>=uvcSettings[uvcSelectedSettingIndex].width)
                    {
                        col=0; 
                        row++; 
                    }                        
                
                }//while(remainingBytes>0 && IsBitmapMode==true ) 
#ifdef ENABLE_LCD
                while(SPI6STATbits.SPIBUSY == 1);
                SPI6CONbits.MODE16=0;
                SPI_CS_HIGH();
#endif
                               
                if(vphi->isEOF || vphi->ERR )
                {
                   
                    //ERR LED
                    if(vphi->ERR == 1)
                        ERR_LED_Set();
                    else
                        ERR_LED_Clear();
                    
#ifdef ENABLE_LCD     
                    while(SPI6STATbits.SPIBUSY == 1);
                    SPI6STATbits.SPIROV = 0;  // Reset overflow bit
                    SPI_CS_LOW();
                    ST7735_SetWindow(&lcd1, 0, _TFTWIDTH-1, 0, _TFTHEIGHT-1);
#endif               
                    row=col=0; 
                   
                    //Jpeg
                    if(IsBitmapMode==false)
                    {
                   
                        if( camState == CAMERA_READ_DONE)
                        {
                            if(IsSwitchPushed==true)
                            {
                                char filename[25]={'\0'};
                                uint32_t size;
                                bool res=false;
                                getFileName(filename, IsBitmapMode);
                                size = ramDataCount*sizeof(videoRam[0]);
                                if(size>oldFileSize/6)
                                {
                                    res=saveJpeg((uint8_t*)videoRam, size, filename);
                                }
                                else
                                {
                                    SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "SizeRetry\r\n");
                                }
                                
                                if(res==true)
                                {
                                    IsSwitchPushed=false;
                                    oldFileSize= size;  
                                }
                                else
                                {
                                    oldFileSize=size*4;
                                }
                            }
                            else if(IsXmodemMode==true)
                            {
                                uint32_t size;
                                size = ramDataCount*sizeof(videoRam[0]);
                                
                                if(checkJpegFile((uint8_t*)videoRam, &size)==true)
                                {
                                
                                    XmodemInit(size, (uint8_t*)videoRam);

                                    while(XmodemGetState()<XMODEM_STATE_ACK)
                                    {
                                        consoleReadSize = SYS_CONSOLE_Read( consoleHandle, consoleBuffer, sizeof(consoleBuffer) );
                                        XmodemSendProcess(consoleReadSize, (uint8_t*)consoleBuffer);
                                        SYS_Tasks_without_AppTasks();
                                        memset(consoleBuffer,'\0',sizeof(consoleBuffer));
                                        consoleReadSize=0;
                                    }
                                    IsXmodemMode=false;
                                }
                            }
                            else if(IsYmodemMode==true)
                            {
                                char filename[25]={'\0'};
                                uint32_t size;
                                getFileName(filename, IsBitmapMode);
                                size = ramDataCount*sizeof(videoRam[0]);  
                                
                                if(checkJpegFile((uint8_t*)videoRam, &size)==true)
                                {
                                    YmodemSendInit(size, (uint8_t*)videoRam, (uint8_t*)filename);

                                    while(YmodemGetState()<YMODEM_STATE_ACK3)
                                    {
                                        consoleReadSize = SYS_CONSOLE_Read( consoleHandle, consoleBuffer, sizeof(consoleBuffer) );
                                        YmodemSendProcess(consoleReadSize, (uint8_t*)consoleBuffer);
                                        SYS_Tasks_without_AppTasks();
                                        memset(consoleBuffer,'\0',sizeof(consoleBuffer));
                                        consoleReadSize=0;
                                    }                                
                                    IsYmodemMode=false;
                                }
                            }
                            else
                            {

#ifdef ENABLE_JPGDECODE
                                uint32_t jpgSize = ramDataCount*sizeof(videoRam[0]); 
                                uint32_t bmpSize = 0;
                                uint8_t  *bmpData = NULL;
                                uint32_t i = 0;
                                bool jres = false;
                                uint16_t width = uvcSettings[uvcSelectedSettingIndex].width;
                                if(width%LCD_WIDTH_FOR_DECODE==0)
                                {
                                    jres=checkJpegFile((uint8_t*)videoRam, &jpgSize);
                                }
                                if(jres==true)
                                {
                                    uint8_t scale=0;
                                    
                                    
                                    if(width==(LCD_WIDTH_FOR_DECODE*1))
                                        scale=0;
                                    else if(width==(LCD_WIDTH_FOR_DECODE*2))
                                        scale=1;
                                    else if(width==(LCD_WIDTH_FOR_DECODE*4))
                                        scale=2;
                                    else if(width==(LCD_WIDTH_FOR_DECODE*8))
                                        scale=3;
                                    else
                                        scale=255;
                                    
                                    if( scale <4 )
                                        jres= JpegDecodeProcess((uint8_t*)videoRam,jpgSize,scale);
                                    else
                                        jres=false;
                                    //SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "JpegDecodeProcess\r\n");
                                }
                                if(jres==true)
                                {
                                    m_LCDDCBit=1;
                                    SPI_CS_LOW();
                                    SPI6CONbits.MODE16=1;

                                    JpegDecodeBmpData(bmpData, &bmpSize);
                                    //SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "JpegDecodeBmpData\r\n");
                                    uint16_t *bdata=(uint16_t*)TjpgdBmp;
                                    uint16_t cdata=0;
                                    for(i=0; i<bmpSize/2; i++)
                                    {
                                        while(SPI6STATbits.SPIBUSY == 1);
                                        SPI6STATbits.SPIROV = 0;  // Reset overflow bit
                                        cdata=*(bdata+i);
                                        JpegSwapForLcd(&cdata);
                                        SPI6BUF=cdata;
                                    }
                                    //while(SPI6STATbits.SPIBUSY == 1);
                                    SPI6CONbits.MODE16=0;
                                    SPI_CS_HIGH();
                                }
#endif
                              
                            }
                            camState = CAMERA_READ_IDLE;
                        }
                    }
                    //Bmp
                    else if(IsBitmapMode==true)
                    {
                        if( ramDataCount>=(uvcSettings[uvcSelectedSettingIndex].width*uvcSettings[uvcSelectedSettingIndex].height))
                        {
                            ramDataReady=false;
                            ramDataCount=0;
                            char filename[25]={'\0'}; 

                            if(IsSwitchPushed==true )
                            {
                                getFileName(filename, IsBitmapMode);
                                if(IsBitmapMonoMode==true)
                                {
                                    BitmapRGB565toMono((uint8_t*)videoRam, (uint8_t*)videoMono);
                                    saveBitmap((uint8_t*)videoMono,BitmapMonoGetFileSize((uint8_t*)videoMono),filename);
                                }
                                else
                                {
                                     saveBitmap((uint8_t*)videoRam,Bitmap565GetFileSize((uint8_t*)videoRam),filename);
                                }
                                
                                IsSwitchPushed=false;
                            }
                            else if(IsXmodemMode==true)
                            {
                                uint32_t size;
                                
                                if(IsBitmapMonoMode==true)
                                {
                                    BitmapRGB565toMono((uint8_t*)videoRam, (uint8_t*)videoMono);
                                    size = BitmapMonoGetFileSize((uint8_t*)videoMono); 
                                    XmodemInit(size, (uint8_t*)videoMono);
                                }
                                else
                                {
                                    size = Bitmap565GetFileSize((uint8_t*)videoRam); 
                                    XmodemInit(size, (uint8_t*)videoRam);
                                }
                                                                
                                while(XmodemGetState()<XMODEM_STATE_ACK)
                                {
                                    consoleReadSize = SYS_CONSOLE_Read( consoleHandle, consoleBuffer, sizeof(consoleBuffer) );
                                    XmodemSendProcess(consoleReadSize, (uint8_t*)consoleBuffer);
                                    SYS_Tasks_without_AppTasks();
                                    memset(consoleBuffer,'\0',sizeof(consoleBuffer));
                                    consoleReadSize=0;
                                }
                                IsXmodemMode=false;
                            }
                            else if(IsYmodemMode==true)
                            {
                                char filename[25]={'\0'};
                                uint32_t size;
                                getFileName(filename, IsBitmapMode);
                                
                                if(IsBitmapMonoMode==true)
                                {
                                    BitmapRGB565toMono((uint8_t*)videoRam, (uint8_t*)videoMono);
                                    size = BitmapMonoGetFileSize((uint8_t*)videoMono);
                                    YmodemSendInit(size, (uint8_t*)videoMono, (uint8_t*)filename);  
                                }
                                else
                                {
                                    size = Bitmap565GetFileSize((uint8_t*)videoRam); 
                                    YmodemSendInit(size, (uint8_t*)videoRam, (uint8_t*)filename);  
                                }                                                          
                                
                                while(YmodemGetState()<YMODEM_STATE_ACK3)
                                {
                                    consoleReadSize = SYS_CONSOLE_Read( consoleHandle, consoleBuffer, sizeof(consoleBuffer) );
                                    YmodemSendProcess(consoleReadSize, (uint8_t*)consoleBuffer);
                                    SYS_Tasks_without_AppTasks();
                                    memset(consoleBuffer,'\0',sizeof(consoleBuffer));
                                    consoleReadSize=0;
                                }                             
                                IsYmodemMode=false;
                            }
                        }
                        ramDataCount=0;
                    }
                }//if(vph->isEOF || vph->ERR )
                
                if(hasSDcard==false)
                {
                    if(sdMount()==0)
                        hasSDcard=true;
                }
       
                
                RUN_LED_Set();
                interruptStatus = __builtin_disable_interrupts(); 
                appData.isVideoReadCompleted = false;
                appData.wLenVideoRead=0;
                __builtin_mtc0(12,0,interruptStatus);
                _USB_HOST_UVC_StreamRead(appData.inStreamHandle,
                &appData.transferHandleVideoRead, (void*)&videoSamples, appData.packetSize,
                USB_HOST_UVC_API_VERSION_FLAG_V1);                 

            }// if(appData.isVideoReadCompleted)
            appData.state = APP_STATE_WAIT_FOR_READ_COMPLETE; 
            break;
        } 
        case APP_STATE_ERROR:
            
            break;
            
        case APP_STATE_IDLE:
            if(appData.deviceIsAttached == false)
            {
                appData.state = APP_STATE_WAIT_FOR_DEVICE_ATTACH;
            }
            break;
            
        case APP_STATE_PAUSE_SET:
            USB_HOST_Deinitialize(sysObj.drvUSBHSObject);
            DRV_USBHS_Deinitialize( DRV_USBHS_INDEX_0);
            VUSB_Clear();
            appData.state =APP_STATE_PAUSE_DONE;
            break;
            
        case APP_STATE_PAUSE_DONE:
            Nop(); 
            break;
       default:
            Nop(); 
            break;
    }
}

/*******************************************************************************
 End of File
 */
