/********************************************************************************
  USB Host Video Class Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    usb_host_uvc.h

  Summary:
    USB Host Video Class Driver Interface Header

  Description:
    This header file contains the function prototypes and definitions of the
    data types and constants that make up the interface to the USB Host Video
    Class Driver.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
#ifndef USB_HOST_UVC_H
#define	USB_HOST_UVC_H

//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "app.h"
#include "usb_host.h"
#include "usb/src/usb_host_local.h"
#include "usb_host_client_driver.h"
#include "usb/usb_uvc.h"
#include "usb/usb_host_uvc_local.h"

// DOM-IGNORE-BEGIN
#ifdef	__cplusplus
extern "C" {
#endif
// DOM-IGNORE-END

    
#define SYS_DEBUG_INFO                              0
        // Number of Video Function driver instances in the application
#define USB_HOST_UVC_INSTANCES_NUMBER               2

/* Maximum number of Streaming interfaces provided by any Device that will be
 be connected to this Video Host */
#define USB_HOST_UVC_STREAMING_INTERFACES_NUMBER    5

/* Maximum number of Streaming interface alternate settings provided by any 
   Device that will be connected to this Video Host. (This number includes 
   alternate setting 0) */
#define USB_HOST_UVC_STREAMING_INTERFACE_ALTERNATE_SETTINGS_NUMBER  15

// Maximum number of discrete Sampling frequencies supported by the Attached Video Device
#define USB_HOST_UVC_SAMPLING_FREQUENCIES_NUMBER    0


#define USB_HOST_UVC_STREAM_TRANSFER_HANDLE_INVALID ((USB_HOST_UVC_STREAM_TRANSFER_HANDLE)(-1))

    
    
// *****************************************************************************
/* USB Host Video Stream Object 
 
  Summary: 
    Defines the type of the Video Host stream object. 

  Description:
    This type defines the type of the Video Host stream object. This type
    is returned by USB_UVC_StreamGetFirst and USB_UVC_StreamGetNext
    as part of USB_HOST_UVC_STREAM_INFO structure.  

  Remarks:
    None.
*/
typedef uintptr_t USB_HOST_UVC_OBJ;

typedef uintptr_t USB_HOST_UVC_STREAMING_INTERFACE_OBJ;
typedef uintptr_t USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ;
typedef uintptr_t USB_HOST_UVC_REQUEST_HANDLE;
#define USB_HOST_UVC_REQUEST_HANDLE_INVALID ((USB_HOST_UVC_REQUEST_HANDLE)(-1))
typedef uintptr_t USB_HOST_UVC_STREAM_TRANSFER_HANDLE;
#define USB_HOST_UVC_STREAM_TRANSFER_HANDLE_INVALID ((USB_HOST_UVC_STREAM_TRANSFER_HANDLE)(-1))
typedef uintptr_t USB_HOST_UVC_STREAM_HANDLE;
#define USB_HOST_UVC_STREAM_HANDLE_INVALID ((USB_HOST_UVC_STREAM_HANDLE)(-1))
typedef uintptr_t USB_HOST_UVC_STREAM_OBJ;

/*DOM-IGNORE-BEGIN*/
#define USB_HOST_UVC_RESULT_MIN -100

// *****************************************************************************
/* USB Host Video Class Driver Result enumeration.
 
  Summary:
    USB Host Video Class Driver video result enumeration.

  Description:
    This enumeration lists the possible USB Host Video Class Driver operation
    results. These values are returned by Video Class Driver functions.

  Remarks:
    None.
*/
  
typedef enum {
    /* An unknown failure has occurred */
    USB_HOST_UVC_RESULT_FAILURE /*DOM-IGNORE-BEGIN*/ = USB_HOST_UVC_RESULT_MIN /*DOM-IGNORE-END*/,

    /* The transfer or request could not be scheduled because internal queues
       are full. The request or transfer should be retried */
    USB_HOST_UVC_RESULT_BUSY,

    /* The request was stalled */
    USB_HOST_UVC_RESULT_REQUEST_STALLED,

    /* A required parameter was invalid */
    USB_HOST_UVC_RESULT_INVALID_PARAMETER,

    /* The associated device does not exist in the system. */
    USB_HOST_UVC_RESULT_DEVICE_UNKNOWN,

    /* The specified handle is not valid */
    USB_HOST_UVC_RESULT_HANDLE_INVALID,

    /* The transfer or requested was aborted */
    USB_HOST_UVC_RESULT_TRANSFER_ABORTED,

    /* The specified Video v1.0 object is invalid */ 
    USB_HOST_UVC_RESULT_OBJ_INVALID,

    /* No more video control entity */ 
    USB_HOST_UVC_RESULT_END_OF_CONTROL_ENTITY, 
    
    /* No more streaming interface settings present in the video device */ 
    USB_HOST_UVC_RESULT_END_OF_STREAMING_INTERFACE, 

    /* No more interface alternate settings are present in the video streaming 
       interface */
    USB_HOST_UVC_RESULT_END_OF_INTERFACE_SETTINGS,

    /* Indicates that the operation succeeded or the request was accepted and
       will be processed. */ 
    USB_HOST_UVC_RESULT_SUCCESS
    
    /*DOM-IGNORE-BEGIN*/ = 1 /*DOM-IGNORE-END*/,

} USB_HOST_UVC_RESULT;
  
// *****************************************************************************
/* USB Host Video Stream Result enumeration.
 
  Summary:
    USB Host Video stream result enumeration.

  Description:
    This enumeration lists the possible USB Host Video stream operation
    results. These values are returned by Video stream functions.

  Remarks:
    None.
*/
typedef enum {
    /* The transfer or request could not be scheduled because internal
     * queues are full. The request or transfer should be retried */
    USB_HOST_UVC_STREAM_RESULT_REQUEST_BUSY =  USB_HOST_RESULT_REQUEST_BUSY,
            
    /* Request was aborted */
    USB_HOST_UVC_STREAM_RESULT_TRANSFER_ABORTED,
            
    /* Request was stalled */
    USB_HOST_UVC_STREAM_RESULT_REQUEST_STALLED,

    /* The specified Stream Handle is not valid */
    USB_HOST_UVC_STREAM_RESULT_HANDLE_INVALID,
 
    /* The end of the device list was reached.*/
    USB_HOST_UVC_STREAM_RESULT_END_OF_DEVICE_LIST,
    
    /* The specified interface is not available */
    USB_HOST_UVC_STREAM_RESULT_INTERFACE_UNKNOWN,
   
    /* A NULL parameter was passed to the function */
    USB_HOST_UVC_STREAM_RESULT_PARAMETER_INVALID,

    /* The specified configuration does not exist on this device.*/
    USB_HOST_UVC_STREAM_RESULT_CONFIGURATION_UNKNOWN, 

    /* A bus operation was requested but the bus was not operated */
    USB_HOST_UVC_STREAM_RESULT_BUS_NOT_ENABLED,

    /* The specified bus does not exist in the system */
    USB_HOST_UVC_STREAM_RESULT_BUS_UNKNOWN,
 
    // The specified video stream does not exist in the system
    USB_HOST_UVC_STREAM_RESULT_UNKNOWN,

    // An unknown failure has occurred
    USB_HOST_UVC_STREAM_RESULT_FAILURE,

    // Indicates a false condition
    USB_HOST_UVC_STREAM_RESULT_FALSE = 0,

    // Indicate a true condition
    USB_HOST_UVC_STREAM_RESULT_TRUE = 1,

    /* Indicates that the operation succeeded or the request was accepted and
       will be processed. */
    USB_HOST_UVC_STREAM_SUCCESS = USB_HOST_RESULT_TRUE
    
} USB_HOST_UVC_STREAM_RESULT;


// *****************************************************************************
/* USB Host Video Stream Event Handler Return Type 
 
  Summary: 
    Returns the type of the USB Host Video stream event handler.

  Description:
    This enumeration lists the possible return values of the USB Host Video
    stream event handler.

  Remarks:
    None.
*/
typedef enum 
{
    USB_HOST_UVC_STREAM_EVENT_RESPONSE_NONE /*DOM-IGNORE-BEGIN*/= 0 /*DOM-IGNORE-END*/
} USB_HOST_UVC_STREAM_EVENT_RESPONSE;
    
// *****************************************************************************
/*  USB Host Video Class Driver Stream Direction
 
  Summary:
    USB Host Video Class Driver stream direction.

  Description:
    This enumeration lists the possible video stream directions.  

  Remarks:
    None.
*/
typedef enum
{
    USB_HOST_UVC_DIRECTION_OUT  /*DOM-IGNORE-BEGIN*/= 0 /*DOM-IGNORE-END*/,
    USB_HOST_UVC_DIRECTION_IN  /*DOM-IGNORE-BEGIN*/= 1 /*DOM-IGNORE-END*/,
} USB_HOST_UVC_STREAM_DIRECTION;

// *****************************************************************************
/* Video Class Driver Events

  Summary:
   Identifies the possible events that the Video Class Driver attach event
   handler can generate.
 
 Description:
   This enumeration identifies the possible events that the Video Class
   Driver attach event handler can generate. The application should register an
   event handler using the USB_HOST_UVC_AttachEventHandlerSet function
   to receive Video Class Driver Attach events.
*/
typedef enum
{
    USB_HOST_UVC_EVENT_ATTACH, 
    USB_HOST_UVC_EVENT_DETACH,
} USB_HOST_UVC_EVENT;

// *****************************************************************************
/* Video Stream Events

  Summary:
    Identifies the possible events that the Video stream can generate.

  Description:
    This enumeration identifies the possible events that the Video stream
    can generate. The application should register an event handler using the
    USB_HOST_UVC_StreamEventHandlerSet function to receive Video
    stream events.
    
    An event may have data associated with it.  Events that are generated due to
    a transfer of data between the Host and Device are accompanied by data
    structures that provide the status of the transfer termination. For example,
    the USB_HOST_UVC_STREAM_EVENT_READ_COMPLETE event is accompanied by a
    pointer to a USB_HOST_UVC_STREAM_EVENT_READ_COMPLETE_DATA data
    structure. The transferStatus member of this data structure indicates the
    success or failure of the transfer. A transfer may fail due to the Device not
    responding on the bus if the Device stalls any stages of the transfer or
    due to NAK time-outs. The event description provides details on the nature of
    the event and the data that is associated with the event.
*/
typedef enum
{
   /* This event occurs when a Video stream read operation has completed
        (i.e., when the data has been received from the connected Video v1.0 stream).
        This event is generated after the application calls the 
        USB_HOST_UVC_StreamRead function. The eventData parameter in the
        event callback function will be of a pointer to a 
        USB_HOST_UVC_STREAM_EVENT_READ_COMPLETE_DATA structure. This
        contains details about the transfer handle associated with this read
        request, the amount of data read and the termination status of the read
        request. */
    USB_HOST_UVC_STREAM_EVENT_READ_COMPLETE,            
            
    /* This event occurs when an Video stream write operation has completed
       (i.e., when the data has been written to the connected Video stream). 
       This event is generated after the application calls the 
       USB_HOST_UVC_StreamWrte function. The eventData parameter in the 
       event callback function will be of a pointer to a 
       USB_HOST_UVC_STREAM_EVENT_WRITE_COMPLETE_DATA structure. This
       contains details about the transfer handle associated with this write
       request, the amount of data written and the termination status of the 
       write request. */
    USB_HOST_UVC_STREAM_EVENT_WRITE_COMPLETE, 
            
    USB_HOST_UVC_STREAM_EVENT_INTERFACE_SET_COMPLETE, 

    /* This event occurs when an Video stream enable request has been 
      completed. This event is generated after the application calls the 
      USB_HOST_UVC_StreamEnable function. The eventData parameter in the
      event callback function will be of a pointer to a 
      USB_HOST_UVC_STREAM_EVENT_ENABLE_COMPLETE_DATA. This contains details
      about the request handle associated with this stream enable request and the
      termination status of the Stream Enable request.*/ 
    USB_HOST_UVC_0_STREAM_EVENT_ENABLE_COMPLETE, 
            
    /*This event occurs when an Video stream disable request has been 
      completed. This event is generated after the application calls the 
      USB_HOST_UVC_StreamDisable function. The eventData parameter in the
      event callback function will be of a pointer to a 
      USB_HOST_UVC_STREAM_EVENT_DISABLE_COMPLETE_DATA. This contains details
      about the request handle associated with this stream disable request and the
      termination status of the Stream Disable request.*/
    USB_HOST_UVC_0_STREAM_EVENT_DISABLE_COMPLETE,
            
    USB_HOST_UVC_STREAM_EVENT_DETACH
} USB_HOST_UVC_STREAM_EVENT;


// *****************************************************************************
/*  USB Host Video Class Stream Data Transfer Event Data.
 
  Summary:
    USB Host Video class stream data transfer event data.

  Description:
    This data type defines the data structure returned by the Video stream
    in conjunction with the following events:
    - USB_HOST_UVC_STREAM_EVENT_READ_COMPLETE_DATA
    - USB_HOST_UVC_STREAM_EVENT_WRITE_COMPLETE_DATA

  Remarks:
    None.
*/

typedef struct
{
    /* Transfer handle of this transfer */
    USB_HOST_UVC_STREAM_TRANSFER_HANDLE transferHandle;

    /* Amount of data transferred */
    size_t length;
    
    /* Transfer termination status */
    USB_HOST_UVC_RESULT result;

}
USB_HOST_UVC_STREAM_EVENT_READ_COMPLETE_DATA,
USB_HOST_UVC_STREAM_EVENT_WRITE_COMPLETE_DATA;


// *****************************************************************************
/* USB Host Video Class Stream Control Event Data.
 
  Summary:
     USB Host Video class stream control event data.

  Description:
    This data type defines the data structure returned by the Video stream
    in conjunction with the following events:
    - USB_HOST_UVC_STREAM_EVENT_INTERFACE_SET_COMPLETE
    - USB_HOST_UVC_STREAM_EVENT_SAMPLING_FREQUENCY_SET_COMPLETE
	- USB_HOST_UVC_STREAM_EVENT_SAMPLING_FREQUENCY_GET_COMPLETE

  Remarks:
    None.
*/
typedef struct
{
    /* Transfer handle of this transfer */
    USB_HOST_UVC_REQUEST_HANDLE  requestHandle; 

    /* Transfer termination status */
    USB_HOST_UVC_RESULT requestStatus;

}
USB_HOST_UVC_STREAM_EVENT_INTERFACE_SET_COMPLETE_DATA,
USB_HOST_UVC_STREAM_EVENT_SAMPLING_RATE_SET_COMPLETE_DATA,
USB_HOST_UVC_STREAM_EVENT_SAMPLING_RATE_GET_COMPLETE_DATA;

// *****************************************************************************
/* USB Host Video Client Driver Attach Event Handler Function Pointer Type.

  Summary:
   USB Host Video Client Driver attach event handler function pointer type.

  Description:
   This data type defines the required function signature of the USB Host Video
   Client Driver attach event handling callback function. The application
   must register a pointer to the Video Client Driver attach events handling
   function whose function signature (parameter and return value types) match
   the types specified by this function pointer to receive attach and
   detach events callbacks from the Video Client Driver. The application
   should use the USB_HOST_UVC_AttachEventHandlerSet function to register an
   attach event handler. The client driver will call this function with the
   relevant event parameters. The descriptions of the event handler function
   parameters are as follows:
   - uvcObj   - VIdeo Device object to which this event is directed
   - event    - Event indicates if it is an Attach or Detach 
   - context  - Value identifying the context of the application that was
                registered with the event handling function

  Remarks:
    None.
*/
typedef void (* USB_HOST_UVC_ATTACH_EVENT_HANDLER)
(
    USB_HOST_UVC_OBJ videoObj, 
    USB_HOST_UVC_EVENT event, 
    uintptr_t context
);

// *****************************************************************************
/* USB Host Video Class Driver Stream Event Handler Function Pointer Type.

  Summary:
   USB Host Video Class Driver stream event handler function pointer type.

  Description:
   This data type defines the required function signature of the USB Host Video
   Class Driver Stream event handling callback function. The application
   must register a pointer to the Video Class Driver stream events handling
   function whose function signature (parameter and return value types) match
   the types specified by this function pointer to receive event callbacks from 
   the Video Class Driver. The application should use the 
   USB_HOST_UVC_StreamEventHandlerSet function to register an video
   stream event handler. The class driver will call this function with the relevant
   event parameters. The descriptions of the stream event handler function
   parameters are as follows:
   - handle           - Handle to the Video stream 
   - event            - Type of event generated
   - eventData        - This parameter should be type casted to an event specific
                        pointer type based on the event that has occurred. Refer 
                        to the USB_HOST_UVC_STREAM_EVENT enumeration 
                        description for more information.
   - context          - Value identifying the context of the application that 
                        was registered with the event handling function

  Remarks:
    None.
*/
typedef USB_HOST_UVC_STREAM_EVENT_RESPONSE (* USB_HOST_UVC_STREAM_EVENT_HANDLER) 
(  
    USB_HOST_UVC_STREAM_HANDLE handle,
    USB_HOST_UVC_STREAM_EVENT event,
    void * eventData, 
    uintptr_t context
);

//*****************************************************************************
/* USB Host Video Class driver Control Transfer Complete Callback Function
   Pointer type

  Summary:
   USB Host Video class driver control transfer complete callback function
   pointer type.

  Description:
   This data type defines the required function signature of the USB Host Video
   class driver control transfer complete callback function. The client
   must provide a pointer to a control transfer complete callback function
   whose function signature (parameter and return value types) must match the
   types specified by this function pointer to receive notification
   when a control transfer has completed. The application should use the 
   USB_HOST_UVC_EntityRequestCallbackSet function to register an entity
   control request callback. The Video client driver will call this
   function with the relevant event parameters. The descriptions of the event
   handler function parameters are as follows:
   - uvcObj        - Video client driver object associated with this event
   - requestHandle - Request handle of the control transfer request that caused
                     this event
   - result        - Completion result of the control transfer. This will be
                     USB_HOST_UVC_RESULT_SUCCESS if the control transfer
		             completed successfully, USB_HOST_UVC_RESULT_FAILURE if 
		             an unknown failure occurred, or 
		             USB_HOST_UVC_RESULT_REQUEST_STALLED if the request was 
		             stalled.
   - size          - Size of the data stage that was transferred
   - context       - Value identifying the context of the application that was
                     provided when the USB_HOST_UVC_ControlRequest 
		             function was called

  Remarks:
    None.
*/
typedef void (* USB_HOST_UVC_ENTITY_REQUEST_CALLBACK) 
(
    USB_HOST_UVC_OBJ videoObj, 
    USB_HOST_UVC_REQUEST_HANDLE requestHandle,
    USB_HOST_UVC_RESULT result,
    size_t size, 
    uintptr_t context
); 

// ****************************************************************************
// ****************************************************************************
// Section: Client Access Functions
// ****************************************************************************
// ****************************************************************************



typedef struct {
    uint32_t event;
    
    USB_HOST_DEVICE_INTERFACE_EVENT_SET_INTERFACE_COMPLETE_DATA eventData; 

    uintptr_t context;
} USB_HOST_UVC_STREAM_STATE_DATA; 



typedef void (* USB_HOST_UVC_CONTROL_CALLBACK) (USB_HOST_UVC_OBJ videoObj, 
    USB_HOST_UVC_REQUEST_HANDLE requestHandle,
    USB_HOST_UVC_RESULT result,
    size_t size, uintptr_t context); 


// *****************************************************************************
/* USB Host Video Control Transfer Object

  Summary:
    USB Host Video Control Transfer Object

  Description:
    This data structure defines a Host Video Control Transfer Object. 
    A Control transfer object tracks a client control transfer request. 

  Remarks:
    None.
*/
typedef struct {
    // True if this object is allocated
    bool inUse;
    
    // Transfer context
    uintptr_t context;

    // Type of control request
    USB_HOST_CONTROL_REQUEST_TYPE requestType;

    // Callback to invoke when control transfer is complete
    USB_HOST_UVC_ENTITY_REQUEST_CALLBACK callback;

} USB_HOST_UVC_CONTROL_TRANSFER_OBJ;





#define USB_HOST_UVC_CONTROL_TRANSFER_OBJ USB_HOST_UVC_CONTROL_TRANSFER_OBJ
typedef struct {         
    // Streaming Interface Number
    uint8_t vsInterfaceId; 

    // Interface Alternate Setting Number
    uint8_t interfaceAlternateSetting;

    // Number of Endpoints
    uint8_t nEndpoints;

    // Video Streaming Interface Class Specific Descriptor
//    uint16_t wFormatTag;
    uint8_t bTerminalLink;

     // The number of bytes occupied by one video subframe.
    uint8_t bSubframeSize[10];
    
    uint16_t wWidth[10],wHeight[10];
    uint8_t bBitsPerPixel[10];
     
    // Flag indicates if Feedback supported
    bool isFeebackSupported;
    
    // Stream Direction
    USB_HOST_UVC_STREAM_DIRECTION direction;
   
    USB_INTERFACE_DESCRIPTOR descVSInterface; 
    
    USB_UVC_VS_INPUT_HEADER_DESCRIPTOR descVSClassSpecific;
    
    USB_ENDPOINT_DESCRIPTOR isoDataEndpointDesc; 
    
    USB_ENDPOINT_DESCRIPTOR isoFeedbackaEndpointDesc;
    
    USB_HOST_UVC_STREAM_EVENT_HANDLER streamEventHandler; 
    
    // Application defined context
    uintptr_t context;
    
} USB_HOST_UVC_STREAM_SETTING;

typedef struct {
    
    // Stream state
    USB_HOST_UVC_STREAM_STATE state; 
    
    USB_HOST_UVC_STREAM_STATE_DATA stateData; 
    
    // Video Streaming Interface Handle
    USB_HOST_DEVICE_INTERFACE_HANDLE vsInterfaceHandle;

    // Interface Type
    USB_UVC_SUBCLASS_CODE infType;
    
    // Interface number
    uint8_t interfaceId;

    // Number of Interface Alternate Settings
    uint8_t nInterfaceSetting; 

    // Interface alternate setting that is active currently
    uint8_t activeInterfaceSetting;

    // Temporary storage of requested Interface Alternate Setting
    uint8_t requestedInterfaceSetting;
    
    // Video Stream Info
    USB_HOST_UVC_STREAM_SETTING videoStreamSetting[USB_HOST_UVC_STREAMING_INTERFACE_ALTERNATE_SETTINGS_NUMBER]; 
    
    // Pipe status
    bool isIsoDataPipeSet;
    
    // Pipe status
    bool isIsoFeedbackPipeSet;

    // Pipe Handle for Isochronous Data Endpoint
    USB_HOST_PIPE_HANDLE isoDataPipeHandle;

    // Pipe Handle for Isochronous Feedback Endpoint
    USB_HOST_PIPE_HANDLE isochronousFeedbackPipeHandle;
    
    USB_SETUP_PACKET setupPacket; 
    
    USB_HOST_UVC_CONTROL_TRANSFER_OBJ videoControlObj;
    
    USB_HOST_UVC_STREAM_EVENT_HANDLER streamEventHandler; 
    
    // Application defined context
    uintptr_t context;

} USB_HOST_UVC_STREAMING_INTERFACE;
    

// *****************************************************************************
/* USB HOST Video Client Driver Instance Data Structure

  Summary:
    USB HOST Video Client Driver Instance Data Structure

  Description:
    This structure has the details about USB HOST Video instance info
    event handler function , states of device , instance , pipes info
    contain all the information about the device driver.

  Remarks:
    None.
 */
typedef struct {

    // Indicates if the instance is in use or not
    bool assigned;
    
    // Index of this Video/UVC Client driver instance
    uint8_t index; 
    
    // Device Object Handle
    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle;

    // Video Control Interface Handle
    USB_HOST_DEVICE_INTERFACE_HANDLE acInterfaceHandle;
    
    // Video Control Descriptor pointer
    uint8_t *pVideoControlDescriptor; 

    // Total number of Video Streaming ecc Interfaces
    uint8_t nVSInterfaces;
    
    /* This is Video Streaming interface counter. This is incremented whenever 
       a new interface is assigned by Host Layer */
    uint8_t countVSInterfaces;
    
    // Pipe for Video Control Endpoint
    USB_HOST_PIPE_HANDLE controlPipeHandle;

    // A collection Video Streaming interfaces
    USB_HOST_UVC_STREAMING_INTERFACE streamInf[USB_HOST_UVC_STREAMING_INTERFACES_NUMBER];
    
    USB_SETUP_PACKET setupPacket; 
    
    USB_HOST_UVC_CONTROL_TRANSFER_OBJ videoControlObj; 
       
} USB_HOST_UVC_INSTANCE;

/* USB Host Video Common object */
typedef struct
{    
    USB_HOST_UVC_ATTACH_EVENT_HANDLER attachEventHandler; 
    USB_HOST_UVC_INSTANCE *prevVideoInstance; 
    uintptr_t context; 
} USB_HOST_UVC_COMMON_OBJ;

typedef uintptr_t USB_HOST_UVC_CONTROL_ENTITY_OBJ; 

  
typedef struct
{
    /* Video Stream Object. Clients need to pass this object when opening this 
       video stream using USB_HOST_UVC_StreamOpen function. */ 
    USB_HOST_UVC_STREAM_OBJ streamObj;

    // Video Format code for this Stream
    //USB_UVC_FORMAT_CODE format;

    // Stream direction
    USB_HOST_UVC_STREAM_DIRECTION streamDirection;

    // Number of bytes occupied by one video sub-frame
    uint8_t subFrameSize;    

} USB_HOST_UVC_STREAM_INFO;

// *****************************************************************************
/* USB HOST Video Client Driver Interface
 
  Summary:
    USB HOST Video Client Driver interface.

  Description:
    This macro should be used by the application in the TPL table while adding
    support for the USB Video Host Client Driver.

  Remarks:
    None.
*/   
/*DOM-IGNORE-BEGIN*/extern const USB_HOST_CLIENT_DRIVER gUSBHostUVCDriver; /*DOM-IGNORE-END*/
#define USB_HOST_UVC_INTERFACE  /*DOM-IGNORE-BEGIN*/&gUSBHostUVCDriver /*DOM-IGNORE-END*/
                    

void APP_VideoDataSetDefault(void);  
void App_USBHostUVCAttachHandler
(
    USB_HOST_UVC_OBJ, 
    USB_HOST_UVC_EVENT, 
    uintptr_t
);

void App_USBVideoControlRequestCallback
(
    USB_HOST_UVC_OBJ, 
    USB_HOST_UVC_REQUEST_HANDLE,
    USB_HOST_UVC_RESULT, 
    size_t, 
    uintptr_t
);

USB_HOST_UVC_STREAM_EVENT_RESPONSE APP_USBHostUVCStreamEventHandler
(
    USB_HOST_UVC_STREAM_HANDLE, 
    USB_HOST_UVC_STREAM_EVENT,
    void *, 
    uintptr_t
);

USB_HOST_UVC_STREAM_HANDLE USB_HOST_UVC_StreamOpen(USB_HOST_UVC_STREAM_OBJ);

USB_HOST_UVC_RESULT USB_HOST_UVC_StreamEventHandlerSet
(
    USB_HOST_UVC_STREAM_HANDLE,
    USB_HOST_UVC_STREAM_EVENT_HANDLER, 
    uintptr_t
);

USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceSet
(
    USB_HOST_UVC_STREAM_HANDLE,
    USB_HOST_UVC_REQUEST_HANDLE *, 
    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ
);


USB_HOST_UVC_STREAM_RESULT USB_HOST_UVC_SetVideoProbeControl
(
    USB_HOST_UVC_OBJ,
    USB_HOST_UVC_CONTROL_ENTITY_OBJ, 
    USB_HOST_UVC_REQUEST_HANDLE *, 
    uint8_t
);

USB_HOST_UVC_STREAM_RESULT USB_HOST_UVC_GetVideoControl
(
    USB_HOST_UVC_OBJ,
    USB_HOST_UVC_CONTROL_ENTITY_OBJ, 
    USB_HOST_UVC_REQUEST_HANDLE *, 
    uint8_t
);

USB_HOST_UVC_STREAM_RESULT USB_HOST_UVC_SetVideoCommitControl
(
    USB_HOST_UVC_OBJ,
    USB_HOST_UVC_CONTROL_ENTITY_OBJ, 
    USB_HOST_UVC_REQUEST_HANDLE *, 
    uint8_t
);

USB_HOST_UVC_RESULT USB_HOST_UVC_EntityRequestCallbackSet
(   
    USB_HOST_UVC_OBJ, 
    USB_HOST_UVC_ENTITY_REQUEST_CALLBACK, 
    uintptr_t
);

USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceGetFirst
(
    USB_HOST_UVC_OBJ,
    USB_HOST_UVC_STREAMING_INTERFACE_OBJ *
);

//USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceSettingGetFirst
//(
//    USB_HOST_UVC_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ *
//);

//USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceSettingGetNext
//(   
//    USB_HOST_UVC_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ *
//);

//uint8_t USB_HOST_UVC_StreamingInterfaceTerminalLinkGet
//(
//    USB_HOST_UVC_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ, 
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ
//);

//USB_UVC_FORMAT_TAG USB_HOST_UVC_StreamingInterfaceFormatTagGet
//(
//    USB_HOST_UVC_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ
//);

//uint8_t USB_HOST_UVC_StreamingInterfaceSubFrameSizeGet
//(
//    USB_HOST_UVC_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ
//);

//USB_HOST_UVC_STREAM_DIRECTION USB_HOST_UVC_StreamingInterfaceDirectionGet
//(
//    USB_HOST_UVC_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ
//);

USB_HOST_UVC_RESULT USB_HOST_UVC_ControlEntityGetNext
(
    USB_HOST_UVC_OBJ,
    USB_HOST_UVC_CONTROL_ENTITY_OBJ, 
    USB_HOST_UVC_CONTROL_ENTITY_OBJ *
);

USB_HOST_UVC_RESULT USB_HOST_UVC_EntityObjectGet
(
    USB_HOST_UVC_OBJ,
    uint8_t, 
    USB_HOST_UVC_CONTROL_ENTITY_OBJ *
);

USB_HOST_UVC_RESULT USB_HOST_UVC_EntityObjectGet
(
    USB_HOST_UVC_OBJ,
    uint8_t, 
    USB_HOST_UVC_CONTROL_ENTITY_OBJ *
);

USB_UVC_ENTITY_TYPE USB_HOST_UVC_EntityTypeGet
(
    USB_HOST_UVC_OBJ, 
    USB_HOST_UVC_CONTROL_ENTITY_OBJ
); 

//USB_HOST_UVC_RESULT _USB_HOST_UVC_StreamWrite
//(
//    USB_HOST_UVC_STREAM_HANDLE,
//    USB_HOST_UVC_STREAM_TRANSFER_HANDLE *, 
//    void *, 
//    size_t , 
//    uint8_t
//);

USB_HOST_UVC_RESULT _USB_HOST_UVC_StreamRead
(
    USB_HOST_UVC_STREAM_HANDLE,
    USB_HOST_UVC_STREAM_TRANSFER_HANDLE *, 
    void *, 
    size_t , 
    uint8_t
);

extern USB_HOST_UVC_COMMON_OBJ gUSBHostUVCCommonObj;


#ifdef	__cplusplus
}
#endif

#endif	/* USB_HOST_UVC_H */

