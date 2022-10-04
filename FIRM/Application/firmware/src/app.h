/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"
#include "usb/usb_host_uvc.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/
    
    
#define ENABLE_BMP
#define ENABLE_JPG

#ifdef ENABLE_LCD
    #define FIRMWARE_NAME       "\tUSB Camera Converter with LCD\t"    
#else
    #define FIRMWARE_NAME       "\tUSB Camera Converter\t"    
#endif

#define FIRMWARE_VERSION    "v1.1.0"    
    
#define clip(n) { if(n<0) n=0; else if(n>255) n=255; }
    
#define _TFTWIDTH  		160     //the REAL W resolution of the TFT
#define _TFTHEIGHT 		128     //the REAL H resolution of the TFT
    
#define VERNUMH 1
#define VERNUML 1

#define ClrWdt()    0
#define __delay_ms  CORETIMER_DelayMs
    
#define	SPISDOTris TRISFbits.TRISF8				// SDO
#define	SPISCKTris TRISDbits.TRISD15    		// SCK
#define	SPICSTris  TRISDbits.TRISD14            // CS
#define	LCDDCTris  TRISBbits.TRISB0				// DC
#define	LCDRSTris  TRISBbits.TRISB1				// RST
  
#define	m_SPISCKBit LATDbits.LATD15		// pin 
#define	m_SPISDOBit LATFbits.LATF8		// pin 
#define	m_SPISDIBit 0
#define	m_SPICSBit  LATDbits.LATD14		// pin 
#define	m_LCDDCBit  LATBbits.LATB0 		// pin 
#define	m_LCDRSBit  LATBbits.LATB1 		// pin 

#define SPI_CS_LOW() m_SPICSBit=0
#define SPI_DC_LOW() m_LCDDCBit=0
#define SPI_RS_LOW() m_LCDRSBit=0
#define SPI_CS_HIGH() m_SPICSBit=1
#define SPI_DC_HIGH() m_LCDDCBit=1
#define SPI_RS_HIGH() m_LCDRSBit=1
    
extern void USB_HOST_Deinitialize ( SYS_MODULE_OBJ usbHostObject );
extern void DRV_USBHS_Deinitialize( const SYS_MODULE_INDEX  object);


typedef enum
{
    /* Application's state machine's initial state. */
//    APP_STATE_INIT=0,
//    APP_STATE_SERVICE_TASKS,
    /* TODO: Define states used by the application state machine. */
   
    /* Application enabled the bus*/
    APP_STATE_BUS_ENABLE,
            
    /* Application waits for bus to be enabled */
    APP_STATE_WAIT_FOR_BUS_ENABLE_COMPLETE,

    /* Application waits for CDC Device Attach */
    APP_STATE_WAIT_FOR_DEVICE_ATTACH,

    // Set Interface
    APP_STATE_ZERO_BANDWIDTH_INTERFACE_SET,         
            
    // Video Device is Attached
    APP_STATE_WAIT_FOR_VIDEO_STREAM_INTERFACE_SET_ZERO,
            
        // Waiting for the initial setting of video data
    APP_STATE_WAIT_FOR_SET_PROBE_VIDEO,           // 7
            
    APP_STATE_WAIT_FOR_GET_PROBE_VIDEO,
            
    // Application waits for Set Video parm
    APP_STATE_WAIT_FOR_SET_COMMIT_VIDEO,           // 9
            
    // Start Scheduling Video Stream Enable
    APP_STATE_ENABLE_VIDEO_STREAM,      //
            
    // Application waits for Zero Bandwidth Set to complete
    APP_STATE_WAIT_FOR_VIDEO_STREAM_INTERFACE_SET_ONE,  // 11
            
    // Setting of correct alternate setting for the Video Streaming Interface is completed
    APP_STATE_START_STREAM_DATA,

    // Wait for Write Complete
    APP_STATE_WAIT_FOR_WRITE_COMPLETE,
    APP_STATE_WAIT_FOR_READ_COMPLETE,       // 14        
            
            
    /* CDC Device is Attached */
    APP_STATE_OPEN_DEVICE,
            
    /* Set the Line Coding */
    APP_STATE_SET_LINE_CODING,

    /* Application waits to get the device line coding */
    APP_STATE_WAIT_FOR_GET_LINE_CODING,

    /* Application sets the line coding */
    APP_STATE_SEND_SET_LINE_CODING,

    /* Appliction waits till set line coding is done */
    APP_STATE_WAIT_FOR_SET_LINE_CODING,

    /* Application sets the contol line state */
    APP_STATE_SEND_SET_CONTROL_LINE_STATE,

    /* Application waits for the set control line state to complete */
    APP_STATE_WAIT_FOR_SET_CONTROL_LINE_STATE,

    /* Application sends the prompt to the device */
    APP_STATE_SEND_PROMPT_TO_DEVICE,

    /* Application waits for prompt send complete */
    APP_STATE_WAIT_FOR_PROMPT_SEND_COMPLETE,

    /* Application request to get data from device */
    APP_STATE_GET_DATA_FROM_DEVICE,

    /* Application waits for data from device */
    APP_STATE_WAIT_FOR_DATA_FROM_DEVICE,

    /* Application has received data from the device */
    APP_STATE_DATA_RECEIVED_FROM_DEVICE,

    /* Application is in error state */
    APP_STATE_ERROR,
            
    APP_STATE_IDLE,
            
    APP_STATE_PAUSE_SET,
            
    APP_STATE_PAUSE_DONE

} APP_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* The application's current state */
    APP_STATES state;

    /* TODO: Define any additional data used by the application. */

    USB_HOST_UVC_OBJ uvcObj;
    
    USB_HOST_UVC_STREAMING_INTERFACE_OBJ inStreamObj;
    
    USB_HOST_UVC_STREAM_HANDLE inStreamHandle;
    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ videoStreamInterfaceSettingObj;
    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ videoStreamInterfaceSettingObjZeroBandwidth;
    
    /* True if a device is attached */
    bool deviceIsAttached;
    
    //USB_HOST_UVC_HANDLE cdcHostHandle;
    
    USB_HOST_UVC_REQUEST_HANDLE  requestHandle;
    
    bool isControlRequestCompleted;
    
    //bool deviceWasDetached;
    
    bool isVideoInStreamFound;
    
    USB_HOST_UVC_CONTROL_ENTITY_OBJ videoFeatureUnitObj; 
    
    uint8_t videoTerminalLink; 
    
    bool isStreamInterfaceSetComplete;
    
    bool isStreamInterfaceSetZeroComplete;
    
    // Flag to indicate progress Stream Enable request
    bool isStreamInterfaceSetOneComplete;
    
    bool isVideoReadCompleted;
    uint16_t wLenVideoRead;
    
    USB_HOST_UVC_STREAM_TRANSFER_HANDLE transferHandleVideoRead;
    uint16_t packetSize;

    
} APP_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_Tasks( void );

typedef struct
{
    uint8_t format;
    uint8_t formatidx;
    uint8_t frame;
    uint16_t width;
    uint16_t height;
    uint8_t intervalCount;
    uint32_t interval[10]; 

}APP_USB_HOST_UVC_SETTINGS;

typedef struct
{
    uint8_t index;
    uint16_t size;
}APP_USB_HOST_UVC_ALTERNATE;

extern APP_USB_HOST_UVC_SETTINGS      uvcSettings[50];
extern APP_USB_HOST_UVC_ALTERNATE     uvcAlternalte[16];
extern uint8_t                        uvcSettingsCount;
extern uint8_t                        uvcAlternateCount;
extern uint8_t                        uvcSelectedSettingIndex;
extern uint8_t                        uvcAlternateInterfaceIndex;
extern uint8_t                        uvcDataFrameIntervalIndex;

extern bool IsBitmapMode;
extern bool IsBitmapMonoMode;
extern bool IsSettingMode;

extern uint8_t             videoSamples[5150];
extern uint16_t            videoRam[80000];//23380
extern uint8_t             videoMono[10000];

extern int32_t                        intervalTime;
extern int32_t                       intervalTimeNext;

typedef struct {
    USB_UVC_FORMAT_CODE format; 
    USB_HOST_UVC_STREAM_DIRECTION streamDirection; 
    uint16_t wWidth; 
    uint16_t wHeight; 
    uint8_t bitPerPixel; 
    uint8_t subFrameSize; 
  } APP_USB_HOST_UVC_STREAM_FORMAT;

typedef struct __attribute__((packed)) {
  uint8_t HeaderSize; 
  union {
    struct {
      unsigned int FID:1;   // Frame Identifier
      unsigned int isEOF:1; // End of Frame
      unsigned int PTS:1;   // Presentation Time Stamp
      unsigned int SCR:1;   // Source Clock Reference
      unsigned int RES:1;   // Reserved.
      unsigned int STI:1;   // Still Image
      unsigned int ERR:1;   // Error Bit
      unsigned int EOH:1;   // End of Header
      };
    uint8_t HeaderBitfield;   
    };
  uint16_t TotalSize;   
  uint32_t Frame;       
  uint32_t CurrentBank;
} VP_HeaderInfoType;  
  
  
//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_H */

/*******************************************************************************
 End of File
 */

